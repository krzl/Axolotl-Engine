#include "stdafx.h"
#include "Vulkan.h"
#include "Graphics/RenderSystem.h"

namespace axlt::vk {

	struct BufferCreateInfo {
		uint32_t size;
		VkBufferUsageFlags usage;
		VkBuffer& buffer;
		VkDeviceMemory& memory;

		BufferCreateInfo( const uint32_t size, const VkBufferUsageFlags usage, VkBuffer& buffer, VkDeviceMemory& memory ) :
			size( size ),
			usage( usage ),
			buffer( buffer ),
			memory( memory ) {}
	};

	Array<BufferCreateInfo> buffersToCreate;

	void CreateDrawBuffers( const ResourceHandle<ModelResource>& model ) {
		DrawBuffers& drawBuffers = meshBuffers.Add( model.guid, DrawBuffers() );
		drawBuffers.buffers.AddEmpty( buffersPerMesh * model->meshes.GetSize() );
		memset( drawBuffers.buffers.GetData(), VK_NULL_HANDLE, sizeof( VkBuffer ) * drawBuffers.buffers.GetSize() );

		Array<VkBuffer> nonEmptyBuffers;
		for( uint32_t i = 0; i < model->meshes.GetSize(); i++ ) {
			auto& mesh = model->meshes[i];
			if( mesh.vertices.GetSize() != 0 ) {
				buffersToCreate.Emplace( mesh.vertices.GetSize() * 3 * 4, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
										 drawBuffers.buffers[0], drawBuffers.memory );
			}
			if( mesh.normals.GetSize() != 0 ) {
				buffersToCreate.Emplace( mesh.normals.GetSize() * 3 * 4, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
										 drawBuffers.buffers[1], drawBuffers.memory );
			}
			if( mesh.tangents.GetSize() != 0 ) {
				buffersToCreate.Emplace( mesh.tangents.GetSize() * 3 * 4, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
										 drawBuffers.buffers[2], drawBuffers.memory );
			}
			if( mesh.bitangents.GetSize() != 0 ) {
				buffersToCreate.Emplace( mesh.bitangents.GetSize() * 3 * 4, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
										 drawBuffers.buffers[3], drawBuffers.memory );
			}
			for( uint8_t j = 0; j < MAX_COLOR_CHANNELS; j++ ) {
				if( mesh.colorChannels[j].GetSize() != 0 ) {
					buffersToCreate.Emplace( mesh.colorChannels[j].GetSize() * 4 * 4, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
											 drawBuffers.buffers[4 + j], drawBuffers.memory );
				}
			}
			for( uint8_t j = 0; j < MAX_UV_CHANNELS; j++ ) {
				if( mesh.texCoordChannels[j].GetSize() != 0 ) {
					buffersToCreate.Emplace( mesh.texCoordChannels[j].GetSize() * 4, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
											 drawBuffers.buffers[4 + MAX_COLOR_CHANNELS + j], drawBuffers.memory );
				}
			}
			buffersToCreate.Emplace( mesh.indices.GetSize() * 4, VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
									 drawBuffers.buffers[4 + MAX_COLOR_CHANNELS + MAX_UV_CHANNELS], drawBuffers.memory );
		}

		// BindMemoryToBuffers( nonEmptyBuffers, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, drawBuffers.memory ) )
	}

	struct ShaderModule {
		VkShaderStageFlags stage;
		VkShaderModule module;
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

		const VkResult result = vkCreateDescriptorSetLayout( device, &descriptorSetLayoutCreateInfo, nullptr, &techniqueData.layout );
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

		CreatePipelineLayout( layouts, pushConstants, &techniqueData.pipelineLayout );

		Array<VkVertexInputBindingDescription> inputBindings( technique->inputs.GetSize() );
		Array<VkVertexInputAttributeDescription> inputAttributes( technique->inputs.GetSize() );
		for( uint32_t i = 0; i < technique->inputs.GetSize(); i++ ) {
			const ShaderInputElement& input = technique->inputs[i];
			VkVertexInputBindingDescription& binding = inputBindings[i];
			VkVertexInputAttributeDescription& attribute = inputAttributes[i];
			techniqueData->usedBuffers |= 1 << input.location;
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

		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo = {
			VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
			nullptr,
			0,
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
			false
		};

		Array<VkViewport> viewports( 1 );
		viewports[0] = {
			0,
			0,
			(float) swapchainExtents.width,
			(float) swapchainExtents.height,
			0,
			1
		};

		Array<VkRect2D> scissorRects( 1 );
		scissorRects[0] = {
			{
				0,
				0
			},
			{
				swapchainExtents.width,
				swapchainExtents.height
			}
		};

		VkPipelineRasterizationStateCreateInfo rasterizationInfo = {
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

		VkPipelineMultisampleStateCreateInfo multisampleInfo = {
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

		VkPipelineDepthStencilStateCreateInfo depthStencilInfo = {
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

		Array<VkPipelineColorBlendAttachmentState> blendAttachmentState( 1 );
		blendAttachmentState[0].colorWriteMask = 0xf;
		blendAttachmentState[0].blendEnable = VK_FALSE;

		VkPipelineColorBlendStateCreateInfo colorBlendInfo = {
			VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
			nullptr,
			0,
			VK_FALSE,
			VK_LOGIC_OP_CLEAR,
			blendAttachmentState.GetSize(),
			blendAttachmentState.GetData(),
		{
			0.0f,
			0.0f,
			0.0f,
			0.0f
		}
		};

		Array<VkDynamicState> dynamicStates( 2 );
		dynamicStates[0] = VK_DYNAMIC_STATE_VIEWPORT;
		dynamicStates[1] = VK_DYNAMIC_STATE_SCISSOR;

		if( !CreateGraphicsPipeline( shaderStages, inputBindings, inputAttributes,
									 inputAssemblyInfo, nullptr,
									 viewports, scissorRects, rasterizationInfo,
									 multisampleInfo, depthStencilInfo, colorBlendInfo,
									 dynamicStates, techniqueData->pipelineLayout,
									 renderPass, 0, 0,
									 VK_NULL_HANDLE, 0,
									 VK_NULL_HANDLE, techniqueData->pipeline ) ) {
			return false;
		}

		return true;
	}








	void BindResources() {

		auto& objectsToRender = g_RenderSystem_instance.m_componentTuples;
		for( auto& entityTuplePair : objectsToRender ) {
			auto&[transform, renderer] = entityTuplePair.value;

			DrawBuffers* drawBuffers = meshBuffers.Find( renderer->model.guid );
			if( drawBuffers == nullptr ) {
				CreateDrawBuffers( renderer->model );
			}

			TechniqueData* techniqueData = techniqueDataArray.Find( renderer->material->technique.guid );
			if( techniqueData == nullptr ) {
				if( !CreateTechniqueData( renderer->material->technique ) ) {
					continue;
				}
			}

			MaterialData* materialData = materialDataArray.Find( renderer->material.guid );
			if( materialData == nullptr || renderer->material.isDirty ) {
				if( !CreateMaterialData( renderer->material, *techniqueData ) ) {
					continue;
				}
			}
		}
	}
}
