#include "stdafx.h"
#include "Vulkan.h"
#include "Graphics/RenderSystem.h"

namespace axlt::vk {

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

	struct MaterialCreateInfo {
		Array<VkDescriptorSetLayout> layouts;
		
		Array<BufferDescriptorInfo> bufferDescriptorInfos;
		Array<ImageDescriptorInfo> imageDescriptorInfos;

		Array<VkDescriptorSet>& descriptorSets;
		VkBuffer& uniformBuffer;
	};

	void CreateMaterialData( ResourceHandle<MaterialResource>& material, TechniqueData& techniqueData ) {
		MaterialData& materialData = materialDataArray.Add( material.guid, MaterialData() );
		material.isDirty = false;
		if( material->technique.guid != materialData.techniqueGuid ) {
			Array<VkDescriptorSetLayout> layouts( 2  );
			layouts[0] = sharedSetLayout;
			layouts[1] = techniqueData.layout;

			AllocateDescriptorSets( descriptorPool, layouts, materialData.descriptorSets );

			materialData.perFrameData.Clear();
			materialData.perFrameData.AddEmpty( commandBuffers.GetSize() );
			for( MaterialData::PerCommandBuffer& perFrameData : materialData.perFrameData ) {
				perFrameData.uniformBuffers.Clear();
				perFrameData.uniformBuffers.AddEmpty( material->technique->uniformBlocks.GetSize() );
				perFrameData.uniformBuffersMemory.Clear();
				perFrameData.uniformBuffersMemory.AddEmpty( material->technique->uniformBlocks.GetSize() );

				Array<BufferDescriptorInfo> bufferDescriptorInfos( material->technique->uniformBlocks.GetSize() );
				Array<ImageDescriptorInfo> imageDescriptorInfos( 0 );// material->technique->samplers.GetSize() );


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
				CreateTechniqueData( renderer->material->technique );
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
