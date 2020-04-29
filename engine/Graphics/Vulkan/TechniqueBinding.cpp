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
	};


	bool CrateShaderModule( const ResourceHandle<BinaryResource>& source, const VkShaderStageFlagBits stage,
							Array<ShaderModule>& shaderModules ) {
		ShaderModule& shaderModule = shaderModules.Emplace();
		shaderModule.stage = stage;

		VkShaderModuleCreateInfo shaderModuleCreateInfo = {
			VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
			nullptr,
			0,
			source->binaryData.GetSize(),
			reinterpret_cast<const uint32_t*>( source->binaryData.GetData() )
		};

		const VkResult result = vkCreateShaderModule( device, &shaderModuleCreateInfo, nullptr, &shaderModule.module );
		if( VK_SUCCESS != result ) {
			printf( "Could not create shader module\n" );
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
		Array<VkViewport> viewports;
		Array<VkRect2D> scissorRects;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		VkPipelineMultisampleStateCreateInfo multisampleInfo;
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
		VkPipelineColorBlendStateCreateInfo colorBlendInfo;
		Array<VkPipelineColorBlendAttachmentState> blendAttachmentState;
		Array<VkDynamicState> dynamicStates;
		VkPipelineLayout& pipelineLayout;
		VkRenderPass& renderPass;

		VkPipeline& pipeline;


		PipelineCreateInfo( VkPipelineLayout& pipelineLayout, VkRenderPass& renderPass, VkPipeline& pipeline ) :
			inputAssemblyInfo(),
			rasterizationInfo(),
			multisampleInfo(),
			depthStencilInfo(),
			colorBlendInfo(),
			pipelineLayout( pipelineLayout ),
			renderPass( renderPass ),
			pipeline( pipeline ) {}

		explicit operator VkGraphicsPipelineCreateInfo() const {

			VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = {
				VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
				nullptr,
				0,
				inputBindings.GetSize(),
				inputBindings.GetData(),
				inputAttributes.GetSize(),
				inputAttributes.GetData()
			};

			Array<VkViewport> viewports =
			{
				{
					0,
					0,
					(float) swapchainExtents.width,
					(float) swapchainExtents.height,
					0,
					1
				}
			};

			Array<VkRect2D> scissorRects = {
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

			VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {
				VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
				nullptr,
				0,
				viewports.GetSize(),
				viewports.GetData(),
				scissorRects.GetSize(),
				scissorRects.GetData()
			};

			Array<VkDynamicState> dynamicStates = {
				VK_DYNAMIC_STATE_VIEWPORT,
				VK_DYNAMIC_STATE_SCISSOR
			};

			VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {
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
	};

	Array<PipelineCreateInfo> pipelinesToCreate;

	void CreateTechniqueData( const ResourceHandle<TechniqueResource>& technique ) {
		TechniqueData& techniqueData = techniqueDataArray.Add( technique.guid, TechniqueData() );

		Array<ShaderModule> shaderStages;
		if( !CrateShaderModule( technique->vertexShader, VK_SHADER_STAGE_VERTEX_BIT, shaderStages ) ) {
			return;
		}
		if( !CrateShaderModule( technique->fragmentShader, VK_SHADER_STAGE_FRAGMENT_BIT, shaderStages ) ) {
			return;
		}

		ExactArray<VkDescriptorSetLayoutBinding> layoutBindings;
		layoutBindings.AddEmpty( technique->uniformBlocks.GetSize() + technique->samplers.GetSize() );

		for( uint32_t i = 0; i < technique->uniformBlocks.GetSize(); i++ ) {
			layoutBindings[i] = {
				technique->uniformBlocks[i].binding,
				VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				1,
				(uint32_t) technique->uniformBlocks[i].shaderStages,
				nullptr
			};
		}

		for( uint32_t i = 0; i < technique->samplers.GetSize(); i++ ) {
			layoutBindings[technique->uniformBlocks.GetSize() + i] = {
				technique->samplers[i].binding,
				VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				technique->samplers[i].count,
				(uint32_t) technique->samplers[i].shaderStages,
				nullptr
			};
		}

		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
			VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			nullptr,
			0,
			layoutBindings.GetSize(),
			layoutBindings.GetData()
		};

		VkResult result = vkCreateDescriptorSetLayout( device, &descriptorSetLayoutCreateInfo, nullptr, &techniqueData.layout );
		if( VK_SUCCESS != result ) {
			printf( "Could not create descriptor set layout\n" );
			return;
		}

		Array<VkDescriptorSetLayout> layouts( 2 );
		layouts[0] = sharedSetLayout;
		layouts[1] = techniqueData.layout;

		Array<VkPushConstantRange> pushConstants{
			{
				VK_SHADER_STAGE_VERTEX_BIT,
				0,
				sizeof( PerDrawUniformBuffer )
			}
		};

		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
			VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			nullptr,
			0,
			layouts.GetSize(),
			layouts.GetData(),
			pushConstants.GetSize(),
			pushConstants.GetData()
		};

		result = vkCreatePipelineLayout( device, &pipelineLayoutCreateInfo, nullptr, &techniqueData.pipelineLayout );
		if( VK_SUCCESS != result ) {
			printf( "Could not create pipeline layout\n" );
			return;
		}

		PipelineCreateInfo& pipelineCreateInfo = pipelinesToCreate.Emplace( techniqueData.pipelineLayout, renderPass, techniqueData.pipeline );

		pipelineCreateInfo.inputBindings.AddEmpty( technique->inputs.GetSize() );
		pipelineCreateInfo.inputAttributes.AddEmpty( technique->inputs.GetSize() );
		for( uint32_t i = 0; i < technique->inputs.GetSize(); i++ ) {
			const ShaderInputElement& input = technique->inputs[i];
			VkVertexInputBindingDescription& binding = pipelineCreateInfo.inputBindings[i];
			VkVertexInputAttributeDescription& attribute = pipelineCreateInfo.inputAttributes[i];
			techniqueData.usedBuffers |= 1 << input.location;
			binding = {
				i,
				0,
				VK_VERTEX_INPUT_RATE_VERTEX
			};
			attribute = {
				input.location,
				i,
				GetInputFormat( input.type, input.precision, input.vectorSize ),
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
			VK_CULL_MODE_BACK_BIT,
			VK_FRONT_FACE_CLOCKWISE,
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

		pipelineCreateInfo.blendAttachmentState.AddEmpty( 1 );
		pipelineCreateInfo.blendAttachmentState[0].colorWriteMask = 0xf;
		pipelineCreateInfo.blendAttachmentState[0].blendEnable = VK_FALSE;

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
}