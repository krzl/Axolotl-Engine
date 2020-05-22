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

	Map<VkBuffer, uint32_t> buffersToDelete;
	Map<VkDeviceMemory, uint32_t> memoryToFree;

	void BindResources() {
		//TODO: Rework dynamic buffers
		Array<VkBuffer> removedBuffers;
		for( auto& pair : buffersToDelete ) {
			pair.value--;
			if( pair.value == 0 ) {
				removedBuffers.Add( pair.key );
				vkDestroyBuffer( device, pair.key, nullptr );
			}
		}

		for( auto& removed : removedBuffers ) {
			buffersToDelete.Remove( removed );
		}

		Array<VkDeviceMemory> freedMemory;
		for( auto& pair : memoryToFree ) {
			pair.value--;
			if (pair.value == 0) {
				freedMemory.Add( pair.key );
				vkFreeMemory( device, pair.key, nullptr );
			}
		}
		for( auto& freed : freedMemory ) {
			memoryToFree.Remove( freed );
		}
		
		Map<VkDeviceMemory, uint8_t> memoryToFree;
		
		auto& objectsToRender = RenderSystem::GetSystem()->GetTuples();
		for( auto& entityTuplePair : objectsToRender ) {
			auto&[transform, renderer] = entityTuplePair.value;

			DrawBuffers* drawBuffers = meshBuffers.Find( renderer->model.guid );
			if( drawBuffers == nullptr ) {
				CreateDrawBuffers( renderer->model );
				renderer->model->isDirty = false;
			} else if( renderer->model->isDirty ) {
				for (VkBuffer buffer : drawBuffers->buffers) {
					if( buffer != VK_NULL_HANDLE ) {
						buffersToDelete.Add( buffer, swapchainImages.GetSize() );
					}
				}
				if(drawBuffers->memory != VK_NULL_HANDLE ) {
					memoryToFree.Add( drawBuffers->memory, swapchainImages.GetSize() );
				}
				
				meshBuffers.Remove( renderer->model.guid );
				CreateDrawBuffers( renderer->model );
				renderer->model->isDirty = false;
			}

			TechniqueData* techniqueData = techniqueDataArray.Find( renderer->material->GetTechnique().guid );
			if( techniqueData == nullptr ) {
				CreateTechniqueData( renderer->material->GetTechnique() );
			}

			for( const auto& tex : renderer->material->GetTextureParameters() ) {
				const ShaderSampler* sampler = renderer->material->GetTechnique()->GetShaderSampler( tex.key );
				if( sampler == nullptr ) {
					continue;
				}

				TextureData* textureData = textureDataArray.Find( tex.value.guid );
				if( textureData == nullptr ) {
					CreateTextureBuffer( tex.value );
				}
			}
		}

		CopyAllBuffers();
		CreateAllPipelines();

		for( auto& entityTuplePair : objectsToRender ) {
			auto&[transform, renderer] = entityTuplePair.value;

			TechniqueData* techniqueData = techniqueDataArray.Find( renderer->material->GetTechnique().guid );

			MaterialData* materialData = materialDataArray.Find( renderer->material.guid );
			if( materialData == nullptr ) {
				materialData = CreateMaterialData( renderer->material, *techniqueData );
			}
			//TODO: Handle change of technique in material
			const BitArray<> dirtyUniformBlocks = renderer->material->PopDirtyUniforms();
			for( auto& perFrameData : materialData->perFrameData ) {
				for( uint32_t i = 0; i < dirtyUniformBlocks.GetSize(); i++ ) {
					perFrameData.dirtyUniformBuffers[i] = perFrameData.dirtyUniformBuffers[i] | dirtyUniformBlocks[i];
				}
			}
		}

		SetupDescriptorSets();
	}
}
