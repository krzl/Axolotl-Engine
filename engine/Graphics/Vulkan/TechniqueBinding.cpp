#include "stdafx.h"
#include "Vulkan.h"

namespace axlt::vk {
	struct ShaderModule {
		VkShaderStageFlagBits stage;
		VkShaderModule module;

		explicit operator VkPipelineShaderStageCreateInfo() const {
			return VkPipelineShaderStageCreateInfo{
				VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
				nullptr,
				0,
				stage,
				module,
				"main",
				nullptr
			};
		}

		explicit operator VkShaderModule() const {
			return module;
		}
	};


	bool CreateShaderModule(const ResourceHandle<BinaryResource>& source, const VkShaderStageFlagBits stage,
		Array<ShaderModule>& shaderModules) {
		ShaderModule& shaderModule = shaderModules.Emplace();
		shaderModule.stage = stage;

		VkShaderModuleCreateInfo shaderModuleCreateInfo = {
			VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
			nullptr,
			0,
			source->binaryData.GetSize(),
			reinterpret_cast<const uint32_t*>(source->binaryData.GetData())
		};

		const VkResult result = vkCreateShaderModule(device, &shaderModuleCreateInfo, nullptr, &shaderModule.module);
		if (VK_SUCCESS != result) {
			printf("Could not create shader module\n");
			return false;
		}

		return true;
	}

	struct PipelineCreateInfo {
		Array<VkPipelineShaderStageCreateInfo> shaderStages;
		Array<VkVertexInputBindingDescription> inputBindings;
		Array<VkVertexInputAttributeDescription> inputAttributes;
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		//VkPipelineTessellationStateCreateInfo* tessellationInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		VkPipelineMultisampleStateCreateInfo multisampleInfo;
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
		VkPipelineColorBlendStateCreateInfo colorBlendInfo;
		Array<VkPipelineColorBlendAttachmentState> blendAttachmentState;
		VkPipelineLayout& pipelineLayout;
		VkRenderPass& renderPass;

		uint32_t instanceId;

		PipelineCreateInfo(VkPipelineLayout& pipelineLayout, VkRenderPass& renderPass, const uint32_t instanceId ) :
			inputAssemblyInfo(),
			rasterizationInfo(),
			multisampleInfo(),
			depthStencilInfo(),
			colorBlendInfo(),
			pipelineLayout(pipelineLayout),
			renderPass(renderPass),
			instanceId( instanceId ) {}

	private:

		VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo{};
		Array<VkViewport> viewports;
		Array<VkRect2D> scissorRects;
		VkPipelineViewportStateCreateInfo viewportStateCreateInfo{};
		Array<VkDynamicState> dynamicStates;
		VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{};

	public:

		explicit operator VkGraphicsPipelineCreateInfo() {

			vertexInputStateCreateInfo = {
				VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
				nullptr,
				0,
				inputBindings.GetSize(),
				inputBindings.GetData(),
				inputAttributes.GetSize(),
				inputAttributes.GetData()
			};

			viewports = Array<VkViewport> {
				{
					0,
					0,
					(float)swapchainExtents.width,
					(float)swapchainExtents.height,
					0,
					1
				}
			};

			scissorRects = Array<VkRect2D>{
				{
					{
						0,
						0
					},
					{
						swapchainExtents.width,
						swapchainExtents.height
					}
				}
			};

			viewportStateCreateInfo = {
			   VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
			   nullptr,
			   0,
			   viewports.GetSize(),
			   viewports.GetData(),
			   scissorRects.GetSize(),
			   scissorRects.GetData()
			};

			dynamicStates = Array<VkDynamicState>{
				VK_DYNAMIC_STATE_VIEWPORT,
				VK_DYNAMIC_STATE_SCISSOR
			};

			dynamicStateCreateInfo = {
				VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
				nullptr,
				0,
				dynamicStates.GetSize(),
				dynamicStates.GetData()
			};

			return VkGraphicsPipelineCreateInfo{
				VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
				nullptr,
				0,
				shaderStages.GetSize(),
				shaderStages.GetData(),
				&vertexInputStateCreateInfo,
				&inputAssemblyInfo,
				nullptr,
				&viewportStateCreateInfo,
				&rasterizationInfo,
				&multisampleInfo,
				&depthStencilInfo,
				&colorBlendInfo,
				&dynamicStateCreateInfo,
				pipelineLayout,
				renderPass,
				0,
				VK_NULL_HANDLE,
				0
			};
		};

		explicit operator VkGraphicsPipelineCreateInfo() const {
			PipelineCreateInfo& nonConst = const_cast<PipelineCreateInfo&>(*this);
			return (VkGraphicsPipelineCreateInfo)nonConst;
		}
	};

	Array<PipelineCreateInfo> pipelinesToCreate;

	void CreateTechniqueData(const ResourceHandle<TechniqueResource>& technique) {
		TechniqueData& techniqueData = techniqueDataArray.Add( technique->GetInstanceId(), TechniqueData() );

		Array<ShaderModule> shaderStages;
		if (!CreateShaderModule(technique->vertexShader, VK_SHADER_STAGE_VERTEX_BIT, shaderStages)) {
			return;
		}
		if (!CreateShaderModule(technique->fragmentShader, VK_SHADER_STAGE_FRAGMENT_BIT, shaderStages)) {
			return;
		}

		int16_t lastLayoutSetIndex = -1;

		for (uint32_t i = 0; i < technique->uniformBlocks.GetSize(); i++) {
			if (technique->uniformBlocks[i].set > 7) continue;
			lastLayoutSetIndex = max(lastLayoutSetIndex, technique->uniformBlocks[i].set);
		}
		for (uint32_t i = 0; i < technique->samplers.GetSize(); i++) {
			if (technique->samplers[i].set > 7) continue;
			lastLayoutSetIndex = max(lastLayoutSetIndex, technique->samplers[i].set);
		}

		techniqueData.layouts.AddEmpty(lastLayoutSetIndex + 1);

		for (uint32_t layoutId = 0; layoutId < techniqueData.layouts.GetSize(); layoutId++) {
			Array<VkDescriptorSetLayoutBinding> layoutBindings;

			for (uint32_t i = 0; i < technique->uniformBlocks.GetSize(); i++) {
				if (technique->uniformBlocks[i].set == layoutId) {
					VkDescriptorSetLayoutBinding& binding = layoutBindings.Emplace();
					binding = {
						technique->uniformBlocks[i].binding,
						VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
						1,
						(uint32_t)technique->uniformBlocks[i].shaderStages,
						nullptr
					};
				}
			}

			for (uint32_t i = 0; i < technique->samplers.GetSize(); i++) {
				if (technique->samplers[i].set == layoutId) {
					VkDescriptorSetLayoutBinding& binding = layoutBindings.Emplace();
					binding = {
						technique->samplers[i].binding,
						VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
						technique->samplers[i].count,
						(uint32_t)technique->samplers[i].shaderStages,
						nullptr
					};
				}
			}

			VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
				VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
				nullptr,
				0,
				layoutBindings.GetSize(),
				layoutBindings.GetData()
			};

			const VkResult result = vkCreateDescriptorSetLayout(device, &descriptorSetLayoutCreateInfo, nullptr, &techniqueData.layouts[layoutId]);
			if (VK_SUCCESS != result) {
				printf("Could not create descriptor set layout\n");
				return;
			}
		}

		Array<VkPushConstantRange> pushConstants = {
			{
				VK_SHADER_STAGE_ALL,
				0,
				sizeof(PerDrawUniformBuffer)
			}
		};

		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
			VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			nullptr,
			0,
			techniqueData.layouts.GetSize(),
			techniqueData.layouts.GetData(),
			pushConstants.GetSize(),
			pushConstants.GetData()
		};

		const VkResult result = vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &techniqueData.pipelineLayout);
		if (VK_SUCCESS != result) {
			printf("Could not create pipeline layout\n");
			return;
		}

		PipelineCreateInfo& pipelineCreateInfo = pipelinesToCreate.Emplace(techniqueData.pipelineLayout, renderPass, technique->GetInstanceId() );

		pipelineCreateInfo.shaderStages = shaderStages;
		techniqueData.shaderModules = shaderStages;

		pipelineCreateInfo.inputBindings.AddEmpty(technique->inputs.GetSize());
		pipelineCreateInfo.inputAttributes.AddEmpty(technique->inputs.GetSize());
		for (uint32_t i = 0; i < technique->inputs.GetSize(); i++) {
			const ShaderInputElement& input = technique->inputs[i];
			VkVertexInputBindingDescription& binding = pipelineCreateInfo.inputBindings[i];
			VkVertexInputAttributeDescription& attribute = pipelineCreateInfo.inputAttributes[i];
			techniqueData.usedBuffers |= 1 << input.location;
			binding = {
				i,
				input.stride,
				VK_VERTEX_INPUT_RATE_VERTEX
			};
			attribute = {
				input.location,
				i,
				GetInputFormat(input.type, input.precision, input.vectorSize),
				0
			};
		}

		pipelineCreateInfo.inputAssemblyInfo = {
			VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
			nullptr,
			0,
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
			false
		};

		pipelineCreateInfo.rasterizationInfo = {
			VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
			nullptr,
			0,
			VK_FALSE,
			VK_FALSE,
			VK_POLYGON_MODE_FILL,
			VK_CULL_MODE_NONE,
			VK_FRONT_FACE_COUNTER_CLOCKWISE,
			VK_FALSE,
			0.0f,
			0.0f,
			0.0f,
			1.0f
		};

		pipelineCreateInfo.multisampleInfo = {
			VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
			nullptr,
			0,
			VK_SAMPLE_COUNT_1_BIT,
			VK_FALSE,
			0.0f,
			nullptr,
			VK_FALSE,
			VK_FALSE
		};

		pipelineCreateInfo.depthStencilInfo = {
			VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
			nullptr,
			0,
			VK_TRUE,
			VK_TRUE,
			VK_COMPARE_OP_LESS_OR_EQUAL,
			VK_FALSE,
			VK_FALSE,
			{
				VK_STENCIL_OP_KEEP,
				VK_STENCIL_OP_KEEP,
				VK_STENCIL_OP_KEEP,
				VK_COMPARE_OP_NEVER,
				0,
				0,
				0
			},
			{
				VK_STENCIL_OP_KEEP,
				VK_STENCIL_OP_KEEP,
				VK_STENCIL_OP_KEEP,
				VK_COMPARE_OP_NEVER,
				0,
				0,
				0
			},
			0.0f,
			1.0f
		};

		pipelineCreateInfo.blendAttachmentState.AddEmpty(1);
		pipelineCreateInfo.blendAttachmentState[0] = {
			VK_TRUE,
			VK_BLEND_FACTOR_SRC_ALPHA,
			VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
			VK_BLEND_OP_ADD,
			VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
			VK_BLEND_FACTOR_ZERO,
			VK_BLEND_OP_ADD,
			VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
		};

		pipelineCreateInfo.colorBlendInfo = {
			VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
			nullptr,
			0,
			VK_FALSE,
			VK_LOGIC_OP_CLEAR,
			pipelineCreateInfo.blendAttachmentState.GetSize(),
			pipelineCreateInfo.blendAttachmentState.GetData(),
			{
				0.0f,
				0.0f,
				0.0f,
				0.0f
			}
		};
	}

	void CreateAllPipelines() {
		if (pipelinesToCreate.GetSize() == 0) return;

		ExactArray<VkGraphicsPipelineCreateInfo> pipelineCreateInfos(pipelinesToCreate);
		Array<VkPipeline> pipelines(pipelinesToCreate.GetSize());

		const VkResult result = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, pipelineCreateInfos.GetSize(), pipelineCreateInfos.GetData(),
			nullptr, pipelines.GetData());
		if (result != VK_SUCCESS) {
			printf("Could not create graphics pipelines\n");
			return;
		}

		for (uint32_t i = 0; i < pipelinesToCreate.GetSize(); i++) {
			techniqueDataArray.Find( pipelinesToCreate[i].instanceId )->pipeline = pipelines[i];
		}

		pipelinesToCreate.Clear();
	}
}