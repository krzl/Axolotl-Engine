#include "stdafx.h"
#include "Vulkan.h"
#include "../RenderSystem.h"
#include "Graphics/CameraComponent.h"
#include "FileSystem/Guid.h"
#include "Initialization/Initializer.h"

namespace axlt::vk {

	const uint32_t buffersPerMesh = 5 + MAX_COLOR_CHANNELS + MAX_COLOR_CHANNELS;

	struct PerCameraUniformBuffer {
		Matrix4 projectionMatrix;
		Matrix4 viewMatrix;
		Matrix4 pvMatrix;
		Vector3 cameraPosition;
	} perCameraUniformBuffer;

	Array<VkBuffer> perCameraBuffers;
	Array<VkDeviceMemory> perCameraBuffersMemory;

	struct PerDrawUniformBuffer {
		Matrix4 modelMatrix;
		Matrix4 pvmMatrix;
	} perDrawUniformBuffer;
	
	struct DrawBuffers {
		// size is 10 times the amount of meshes in model
		// last buffer buffer in each 10 is index buffer
		ExactArray<VkBuffer> buffers;
		VkDeviceMemory memory = VK_NULL_HANDLE;
	};

	Map<Guid, DrawBuffers> meshBuffers;

	bool CreateDrawBuffers( const ResourceHandle<ModelResource>& model, DrawBuffers* drawBuffers ) {
		drawBuffers = &meshBuffers.Add( model.guid, DrawBuffers() );
		drawBuffers->buffers.AddEmpty( buffersPerMesh * model->meshes.GetSize() );
		memset( drawBuffers->buffers.GetData(), VK_NULL_HANDLE, sizeof( VkBuffer ) * drawBuffers->buffers.GetSize() );

		for( uint32_t i = 0; i < model->meshes.GetSize(); i++ ) {
			auto& mesh = model->meshes[i];
			if( mesh.vertices.GetSize() != 0 ) {
				VkBuffer& buffer = drawBuffers->buffers[0];
				CreateBuffer( mesh.vertices.GetSize() * 3 * 4, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, buffer );
			}
			if( mesh.normals.GetSize() != 0 ) {
				VkBuffer& buffer = drawBuffers->buffers[1];
				CreateBuffer( mesh.normals.GetSize() * 3 * 4, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, buffer );
			}
			if( mesh.tangents.GetSize() != 0 ) {
				VkBuffer& buffer = drawBuffers->buffers[2];
				CreateBuffer( mesh.tangents.GetSize() * 3 * 4, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, buffer );
			}
			if( mesh.bitangents.GetSize() != 0 ) {
				VkBuffer& buffer = drawBuffers->buffers[3];
				CreateBuffer( mesh.bitangents.GetSize() * 3 * 4, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, buffer );
			}
			for( uint8_t j = 0; j < MAX_COLOR_CHANNELS; j++ ) {
				if( mesh.colorChannels[j].GetSize() != 0 ) {
					VkBuffer& buffer = drawBuffers->buffers[4 + j];
					CreateBuffer( mesh.colorChannels[j].GetSize() * 4 * 4, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, buffer );
				}
			}
			for( uint8_t j = 0; j < MAX_UV_CHANNELS; j++ ) {
				if( mesh.texCoordChannels[j].GetSize() != 0 ) {
					VkBuffer& buffer = drawBuffers->buffers[4 + MAX_COLOR_CHANNELS + j];
					CreateBuffer( mesh.texCoordChannels[j].GetSize() * 4, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, buffer );
				}
			}
			VkBuffer& buffer = drawBuffers->buffers[4 + MAX_COLOR_CHANNELS + MAX_UV_CHANNELS];
			CreateBuffer( mesh.indices.GetSize() * 4, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, buffer );
		}

		if( !BindMemoryToBuffers( drawBuffers->buffers, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, drawBuffers->memory ) ) {
			return false;
		}

		return true;
	}

	VkFormat GetInputFormat( const ShaderType type, const ShaderPrecision precision, const uint8_t vectorSize ) {
		if( vectorSize == 1 ) {
			switch( type ) {
				case Float:
					switch( precision ) {
						case Full:
							return VK_FORMAT_R32_SFLOAT;
						case Half:
							return VK_FORMAT_R16_SFLOAT;
						case Low:
							return VK_FORMAT_R8_SSCALED;
						default:
							return VK_FORMAT_UNDEFINED;
					}
				case Double:
					return VK_FORMAT_R64_SFLOAT;
				case Int:
					switch( precision ) {
						case Full:
							return VK_FORMAT_R32_SINT;
						case Half:
							return VK_FORMAT_R16_SINT;
						case Low:
							return VK_FORMAT_R8_SINT;
						default:
							return VK_FORMAT_UNDEFINED;
					}
				case UInt:
					switch( precision ) {
						case Full:
							return VK_FORMAT_R32_UINT;
						case Half:
							return VK_FORMAT_R16_UINT;
						case Low:
							return VK_FORMAT_R8_UINT;
						default:
							return VK_FORMAT_UNDEFINED;
					}
				default:
					return VK_FORMAT_UNDEFINED;
			}
		} else if( vectorSize == 2 ) {
			switch( type ) {
				case Float:
					switch( precision ) {
						case Full:
							return VK_FORMAT_R32G32_SFLOAT;
						case Half:
							return VK_FORMAT_R16G16_SFLOAT;
						case Low:
							return VK_FORMAT_R8G8_SSCALED;
						default:
							return VK_FORMAT_UNDEFINED;
					}
				case Double:
					return VK_FORMAT_R64G64_SFLOAT;
				case Int:
					switch( precision ) {
						case Full:
							return VK_FORMAT_R32G32_SINT;
						case Half:
							return VK_FORMAT_R16G16_SINT;
						case Low:
							return VK_FORMAT_R8G8_SINT;
						default:
							return VK_FORMAT_UNDEFINED;
					}
				case UInt:
					switch( precision ) {
						case Full:
							return VK_FORMAT_R32G32_UINT;
						case Half:
							return VK_FORMAT_R16G16_UINT;
						case Low:
							return VK_FORMAT_R8G8_UINT;
						default:
							return VK_FORMAT_UNDEFINED;
					}
				default:
					return VK_FORMAT_UNDEFINED;
			}
		} else if( vectorSize == 3 ) {
			switch( type ) {
				case Float:
					switch( precision ) {
						case Full:
							return VK_FORMAT_R32G32B32_SFLOAT;
						case Half:
							return VK_FORMAT_R16G16B16_SFLOAT;
						case Low:
							return VK_FORMAT_R8G8B8_SSCALED;
						default:
							return VK_FORMAT_UNDEFINED;
					}
				case Double:
					return VK_FORMAT_R64G64B64_SFLOAT;
				case Int:
					switch( precision ) {
						case Full:
							return VK_FORMAT_R32G32B32_SINT;
						case Half:
							return VK_FORMAT_R16G16B16_SINT;
						case Low:
							return VK_FORMAT_R8G8B8_SINT;
						default:
							return VK_FORMAT_UNDEFINED;
					}
				case UInt:
					switch( precision ) {
						case Full:
							return VK_FORMAT_R32G32B32_UINT;
						case Half:
							return VK_FORMAT_R16G16B16_UINT;
						case Low:
							return VK_FORMAT_R8G8B8_UINT;
						default:
							return VK_FORMAT_UNDEFINED;
					}
				default:
					return VK_FORMAT_UNDEFINED;
			}
		} else if( vectorSize == 4 ) {
			switch( type ) {
				case Float:
					switch( precision ) {
						case Full:
							return VK_FORMAT_R32G32B32A32_SFLOAT;
						case Half:
							return VK_FORMAT_R16G16B16A16_SFLOAT;
						case Low:
							return VK_FORMAT_R8G8B8A8_SSCALED;
						default:
							return VK_FORMAT_UNDEFINED;
					}
				case Double:
					return VK_FORMAT_R64G64B64A64_SFLOAT;
				case Int:
					switch( precision ) {
						case Full:
							return VK_FORMAT_R32G32B32A32_SINT;
						case Half:
							return VK_FORMAT_R16G16B16A16_SINT;
						case Low:
							return VK_FORMAT_R8G8B8A8_SINT;
						default:
							return VK_FORMAT_UNDEFINED;
					}
				case UInt:
					switch( precision ) {
						case Full:
							return VK_FORMAT_R32G32B32A32_UINT;
						case Half:
							return VK_FORMAT_R16G16B16A16_UINT;
						case Low:
							return VK_FORMAT_R8G8B8A8_UINT;
						default:
							return VK_FORMAT_UNDEFINED;
					}
				default:
					return VK_FORMAT_UNDEFINED;
			}
		}

		return VK_FORMAT_UNDEFINED;
	}

	struct TechniqueData {
		VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
		VkPipeline pipeline = VK_NULL_HANDLE;
		VkDescriptorSetLayout layout = VK_NULL_HANDLE;
		uint16_t usedBuffers = 0;
	};

	Map<Guid, TechniqueData> techniqueDataArray;

	bool CreateShaderStageParameters( const ResourceHandle<BinaryResource>& vertexShader,
									  const VkShaderStageFlagBits stage,
									  Array<ShaderStageParameters>& shaderParamsArray ) {
		ShaderStageParameters& shaderParams = shaderParamsArray.Emplace();
		shaderParams.shaderStage = stage;
		return CreateShaderModule( vertexShader->binaryData, shaderParams.shaderModule );
	}

	bool CreateTechniqueData( const ResourceHandle<TechniqueResource>& technique, TechniqueData& techniqueData ) {
		techniqueData = techniqueDataArray.Add( technique.guid, TechniqueData() );

		Array<ShaderStageParameters> shaderStages;
		if( !CreateShaderStageParameters( technique->vertexShader, VK_SHADER_STAGE_VERTEX_BIT, shaderStages ) ) {
			return false;
		}
		if( !CreateShaderStageParameters( technique->fragmentShader, VK_SHADER_STAGE_FRAGMENT_BIT, shaderStages ) ) {
			return false;
		}

		ExactArray<VkDescriptorSetLayoutBinding> layoutBindings;
		layoutBindings.AddEmpty( technique->uniformBlocks.GetSize() + technique->samplers.GetSize() );

		for( uint32_t i = 0; i < technique->uniformBlocks.GetSize(); i++ ) {
			layoutBindings[i] = {
				technique->uniformBlocks[i].binding,
				VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,//VK_DESCRIPTOR_TYPE_SAMPLER
				1,
				(uint32_t) technique->uniformBlocks[i].shaderStages,
				nullptr
			};
		}

		for( uint32_t i = 0; i < technique->samplers.GetSize(); i++ ) {
			layoutBindings[technique->uniformBlocks.GetSize() + i] = {
				technique->samplers[i].binding,
				VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,//VK_DESCRIPTOR_TYPE_SAMPLER
				technique->samplers[i].count,
				(uint32_t) technique->samplers[i].shaderStages,
				nullptr
			};
		}

		CreateDescriptorSetLayout( layoutBindings, techniqueData.layout );


		Array<VkDescriptorSetLayout> layouts( 2 );
		//layouts[0] = sharedSetLayout;
		layouts[1] = techniqueData.layout;
		
		Array<VkPushConstantRange> pushConstants {
			{
				VK_SHADER_STAGE_VERTEX_BIT,
				0,
				sizeof( PerDrawUniformBuffer )
			}
		};
		CreatePipelineLayout( layouts, pushConstants, techniqueData.pipelineLayout );

		Array<VkVertexInputBindingDescription> inputBindings( technique->inputs.GetSize() );
		Array<VkVertexInputAttributeDescription> inputAttributes( technique->inputs.GetSize() );
		for( uint32_t i = 0; i < technique->inputs.GetSize(); i++ ) {
			const ShaderInputElement& input = technique->inputs[i];
			VkVertexInputBindingDescription& binding = inputBindings[i];
			VkVertexInputAttributeDescription& attribute = inputAttributes[i];
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
			true,
			true,
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

		auto pipelineInfo = GetGraphicsPipelineCreateInfo( shaderStages, inputBindings, inputAttributes,
														   inputAssemblyInfo, nullptr,
														   viewports, scissorRects, rasterizationInfo,
														   multisampleInfo, depthStencilInfo, colorBlendInfo,
														   dynamicStates, techniqueData.pipelineLayout,
														   renderPass, 0, 0,
														   VK_NULL_HANDLE, 0 );

		VkPipelineCache cache;
		if( !CreateGraphicsPipeline( pipelineInfo, cache, techniqueData.pipeline ) ) {
			return false;
		}

		return true;
	}

	struct MaterialData {
		struct PerCommandBuffer {
			Array<VkBuffer> uniformBuffers;
			Array<VkDeviceMemory> uniformBuffersMemory;
			Array<VkSampler> samplers;
		};
		Guid techniqueGuid = Guid::invalidGuid;
		Array<VkDescriptorSet> descriptorSets;

		Array<PerCommandBuffer> perFrameData;
	};

	Map<Guid, MaterialData> materialDataArray;
	
	bool CreateMaterialData( ResourceHandle<MaterialResource>& material, TechniqueData& techniqueData, MaterialData& materialData ) {
		material.isDirty = false;
		if( material->technique.guid != materialData.techniqueGuid ) {
			Array<VkDescriptorSetLayout> layouts( 2  );
			//layouts[0] = sharedSetLayout;
			layouts[1] = techniqueData.layout;
			
			AllocateDescriptorSets( descriptorPool, layouts, materialData.descriptorSets );

			materialData.perFrameData.Clear();
			materialData.perFrameData.AddEmpty( commandBuffers.GetSize() );
			for( MaterialData::PerCommandBuffer& perFrameData : materialData.perFrameData ) {
				perFrameData.uniformBuffers.Clear();
				perFrameData.uniformBuffers.AddEmpty( material->technique->uniformBlocks.GetSize() );
				
				Array<BufferDescriptorInfo> bufferDescriptorInfos( material->technique->uniformBlocks.GetSize() );
				Array<ImageDescriptorInfo> imageDescriptorInfos( material->technique->samplers.GetSize() );
				Array<TexelBufferDescriptorInfo> texelBufferDescriptorInfos;
				Array<CopyDescriptorInfo> copyDescriptorInfos;

				for( uint32_t i = 0; i < bufferDescriptorInfos.GetSize(); i++ ) {
					const ShaderUniformBlock& uniformBlock = material->technique->uniformBlocks[i];

					CreateBuffer( uniformBlock.size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, perFrameData.uniformBuffers[i] );
					
					BindMemoryToBuffer( perFrameData.uniformBuffers[i], perFrameData.uniformBuffersMemory[i],
										(VkMemoryPropertyFlagBits) ( VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | 
										VK_MEMORY_PROPERTY_HOST_COHERENT_BIT ) );

					bufferDescriptorInfos[i] = BufferDescriptorInfo {
						materialData.descriptorSets[1],
						uniformBlock.binding,
						0,
						VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
						 {
							VkDescriptorBufferInfo {
								perFrameData.uniformBuffers[i],
								0,
								uniformBlock.size
							}
						}
					};
				}

				/*
				for( uint32_t i = 0; i < imageDescriptorInfos.GetSize(); i++ ) {
					const ShaderSampler& sampler = material->technique->samplers[i];

					CreateImage( VK_IMAGE_TYPE_)
					CreateSampler

					imageDescriptorInfos[i] = ImageDescriptorInfo {
						materialData.descriptorSets[1],
						sampler.binding,
						0,
						VK_DESCRIPTOR_TYPE_SAMPLER,
						{
							{
								
							}
						}
					};
				}*/

				UpdateDescriptorSets( imageDescriptorInfos, bufferDescriptorInfos, 
									  texelBufferDescriptorInfos, copyDescriptorInfos );
			}
		}

		return true;
	}

	VkDeviceSize emptyOffsets[10] = { 9, 0, 0, 0, 0, 9, 0, 0, 0, 0 };

	VkSemaphore imageAvailableSemaphore = VK_NULL_HANDLE;
	VkSemaphore renderingFinishedSemaphore = VK_NULL_HANDLE;

	void Update() {
		if( perCameraBuffers.GetSize() == 0 ) {
			perCameraBuffers.AddEmpty( commandBuffers.GetSize() );
			perCameraBuffersMemory.AddEmpty( commandBuffers.GetSize() );

			for( uint32_t i = 0; i < commandBuffers.GetSize(); i++ ) {
				CreateBuffer( sizeof( PerCameraUniformBuffer ), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
							  perCameraBuffers[i] );
				BindMemoryToBuffer( perCameraBuffers[i], perCameraBuffersMemory[i], (VkMemoryPropertyFlagBits) 
									( VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT ) );
			}

			CreateSemaphoreVk( imageAvailableSemaphore );
			CreateSemaphoreVk( renderingFinishedSemaphore );
		}
		uint32_t currentCommandBufferIndex;
		init::AcquireNextImage( imageAvailableSemaphore, VK_NULL_HANDLE, currentCommandBufferIndex );
		
		VkCommandBuffer& currentCommandBuffer = commandBuffers[currentCommandBufferIndex];
		ResetCommandBuffer( currentCommandBuffer, true );
		BeginRecordingCommands( currentCommandBuffer, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT );
		auto& cameras = CameraComponent::GetAllCameras();
		for( CameraComponent& camera : cameras ) {
			if( !camera.IsEnabled() ) continue;

			ComponentHandle<TransformComponent> cameraTransform = camera.GetEntity().GetComponent<TransformComponent>();
			if( !cameraTransform.IsValidHandle() ) continue;
			perCameraUniformBuffer.viewMatrix = cameraTransform->WorldToLocalMatrix();
			perCameraUniformBuffer.projectionMatrix = camera.ProjectionMatrix();
			perCameraUniformBuffer.pvMatrix = perCameraUniformBuffer.projectionMatrix * perCameraUniformBuffer.viewMatrix;
			perCameraUniformBuffer.cameraPosition = cameraTransform->GetPosition();

			MapAndFlushMemory( perCameraBuffersMemory[currentCommandBufferIndex], 0,
							   sizeof( PerCameraUniformBuffer ), &perCameraUniformBuffer );

			auto& objectsToRender = g_RenderSystem_instance.m_componentTuples;
			for( auto& entityTuplePair : objectsToRender ) {
				auto&[transform, renderer] = entityTuplePair.value;

				perDrawUniformBuffer.modelMatrix = transform->LocalToWorldMatrix();
				perDrawUniformBuffer.pvmMatrix = perCameraUniformBuffer.pvMatrix * perDrawUniformBuffer.modelMatrix;

				
				DrawBuffers* drawBuffers = meshBuffers.Find( renderer->model.guid );
				if( drawBuffers == nullptr ) {
					if( !CreateDrawBuffers( renderer->model, drawBuffers ) ) {
						continue;
					}
				}

				TechniqueData* techniqueData = techniqueDataArray.Find( renderer->material->technique.guid );
				if( techniqueData == nullptr ) {
					if( !CreateTechniqueData( renderer->material->technique, *techniqueData ) ) {
						continue;
					}
				}

				MaterialData* materialData = materialDataArray.Find( renderer->material.guid );
				if( materialData == nullptr || renderer->material.isDirty ) {
					if( !CreateMaterialData( renderer->material, *techniqueData, *materialData ) ) {
						continue;
					}
				}
				
				UpdatePushConstants( currentCommandBuffer, techniqueData->pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 
									 0, sizeof( PerDrawUniformBuffer ), &perDrawUniformBuffer );

				const uint32_t meshCount = buffersPerMesh / drawBuffers->buffers.GetSize();
				for( uint32_t i = 0; i < meshCount; i++ ) {
					uint32_t startAdjacentBuffersIndex = 0xFFFFFFFF;
					uint32_t currentBinding = 0;
					for( uint32_t j = 0; j < buffersPerMesh - 1; j++ ) {
						if( drawBuffers->buffers[i * buffersPerMesh + j] != VK_NULL_HANDLE &&
							( techniqueData->usedBuffers & ( 1 << j ) ) != 0 ) {
							if( startAdjacentBuffersIndex == 0xFFFFFFFF ) {
								startAdjacentBuffersIndex = j;
							}
						} else {
							if( startAdjacentBuffersIndex == 0xFFFFFFFF ) {
								vkCmdBindVertexBuffers( currentCommandBuffer, currentBinding,
														j - startAdjacentBuffersIndex,
														&drawBuffers->buffers[i * buffersPerMesh + j],
														emptyOffsets );

								currentBinding += j - startAdjacentBuffersIndex;
								startAdjacentBuffersIndex = 0xFFFFFFFF;
							}
						}
					}

					BindIndexBuffers( currentCommandBuffer, drawBuffers->buffers[( i + 1 ) * buffersPerMesh - 1],
									  0, VK_INDEX_TYPE_UINT32 );

					Draw( currentCommandBuffer, renderer->model->meshes[i].vertices.GetSize(),
						  1, 0, 0 );
				}
			}
		}
		EndRecordingCommands( currentCommandBuffer );

		Array<WaitSemaphoreInfo> waitSemaphores = {
			{
				imageAvailableSemaphore,
				VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT
			}
		};
		Array<VkCommandBuffer> commandBuffers = {
			currentCommandBuffer
		};
		Array<VkSemaphore> renderingSemaphores = {
			renderingFinishedSemaphore
		};
		SubmitCommandBuffers( queues[0][0], waitSemaphores, commandBuffers, renderingSemaphores, VK_NULL_HANDLE );

		Array<VkSwapchainKHR> swapchains = {
			swapchain
		};
		init::PresentImage( presentationQueue, renderingSemaphores, swapchains );
	}
}
