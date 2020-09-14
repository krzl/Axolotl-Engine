#include "stdafx.h"
#include "MaterialBinding.h"
#include "TechniqueBinding.h"
#include "TextureBinding.h"

namespace axlt::vk {

	Array<MaterialBinding::BufferDescriptorCreateInfo> MaterialBinding::awaitingBufferDescriptorSets;
	Array<MaterialBinding::TextureDescriptorCreateInfo> MaterialBinding::awaitingTextureDescriptorSets;
	SparseArray<MaterialBinding> MaterialBinding::bindings;
	
	MaterialBinding& MaterialBinding::CreateBinding( const ResourceHandle<MaterialResource>& material ) {
		MaterialBinding& binding = *(MaterialBinding*) bindings.Emplace().element;
		bindingMap.Add( material->GetInstanceId(), &binding );

		return binding;
	}

	void MaterialBinding::UpdateBinding( const ResourceHandle<MaterialResource>& material ) {

		techniqueId = material->GetTechnique()->GetInstanceId();
		const TechniqueBinding& techniqueBinding = *(TechniqueBinding*) bindingMap[techniqueId];
		BufferDescriptorCreateInfo& materialBindInfo = awaitingBufferDescriptorSets.Emplace();
		
		FixedArray<VkDescriptorPoolSize, 2> poolSizes;
		if( material->GetTechnique()->GetShaderUniformBlockCount() > 0 ) {
			VkDescriptorPoolSize& poolSize = poolSizes.Emplace();
			poolSize = {
				VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				material->GetTechnique()->GetShaderUniformBlockCount() * commandBuffers.GetSize()
			};
		}
		if( material->GetTechnique()->GetShaderSamplerCount() > 0 ) {
			VkDescriptorPoolSize& poolSize = poolSizes.Emplace();
			poolSize = {
				VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				material->GetTechnique()->GetShaderSamplerCount() * commandBuffers.GetSize()
			};
		}

		VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {
			VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
			nullptr,
			0,
			material->GetTechnique()->GetShaderUniformBlockCount() * commandBuffers.GetSize() +
			material->GetTechnique()->GetShaderSamplerCount() * commandBuffers.GetSize(),
			poolSizes.GetSize(),
			poolSizes.GetData()
		};

		vkCreateDescriptorPool( device, &descriptorPoolCreateInfo, nullptr, &descriptorPool );
		
		VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {
			VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			nullptr,
			descriptorPool,
			techniqueBinding.layouts.GetSize(),
			techniqueBinding.layouts.GetData()
		};
		
		perFrameDataArray.AddEmpty( commandBuffers.GetSize() );
		for( uint32_t frameId = 0; frameId < perFrameDataArray.GetSize(); frameId++ ) {
			PerCommandBuffer& perFrameData = perFrameDataArray[frameId];

			perFrameData.uniformBuffers.AddEmpty( material->GetTechnique()->GetShaderUniformBlockCount() );
			perFrameData.uniformBuffersMemory.AddEmpty( material->GetTechnique()->GetShaderUniformBlockCount() );
			perFrameData.dirtyUniformBuffers.AddEmpty( material->GetTechnique()->GetShaderUniformBlockCount() );
			
			if( techniqueBinding.layouts.GetSize() > 0 ) {
				perFrameData.descriptorSets.AddEmpty( techniqueBinding.layouts.GetSize() );

				const VkResult result = vkAllocateDescriptorSets( device, &descriptorSetAllocateInfo, perFrameData.descriptorSets.GetData() );
				if( result != VK_SUCCESS ) {
					printf( "Could not allocate descriptor sets\n" );
					return;
				}
			}
			
			for( uint32_t i = 0; i < material->GetTechnique()->GetShaderUniformBlockCount(); i++ ) {
				const ShaderUniformBlock& uniformBlock = material->GetTechnique()->GetShaderUniformBlock( i );

				if( uniformBlock.set != 0 ) {
					VkBufferCreateInfo createInfo = {
						VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
						nullptr,
						0,
						uniformBlock.size,
						VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
						VK_SHARING_MODE_EXCLUSIVE,
						0,
						nullptr
					};

					VkResult result = vkCreateBuffer( device, &createInfo, nullptr, &perFrameData.uniformBuffers[i] );
					if( result != VK_SUCCESS ) {
						printf( "Could not create buffer\n" );
						return;
					}

					VkMemoryRequirements memoryRequirements;
					vkGetBufferMemoryRequirements( device, perFrameData.uniformBuffers[i], &memoryRequirements );

					if( !AllocateMemory( memoryRequirements, (VkMemoryPropertyFlagBits)
						( VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT ),
						&perFrameData.uniformBuffersMemory[i] ) ) {
						return;
					}

					result = vkBindBufferMemory( device, perFrameData.uniformBuffers[i], perFrameData.uniformBuffersMemory[i], 0 );
					if( result != VK_SUCCESS ) {
						printf( "Could not bind buffer memory\n" );
						return;
					}

					BufferDescriptorInfo& bufferDescriptorInfo = materialBindInfo.bufferDescriptorInfos.Emplace();
					bufferDescriptorInfo = {
						perFrameData.uniformBuffers[i],
						uniformBlock.size,
						uniformBlock.binding,
						perFrameData.descriptorSets[uniformBlock.set]
					};
				} else {
					perFrameData.uniformBuffersMemory[i] = VK_NULL_HANDLE;
					BufferDescriptorInfo& bufferDescriptorInfo = materialBindInfo.bufferDescriptorInfos.Emplace();
					bufferDescriptorInfo = {
						perCameraBuffers[frameId],
						uniformBlock.size,
						uniformBlock.binding,
						perFrameData.descriptorSets[uniformBlock.set]
					};
				}
			}
			
			for( const auto& tex : material->GetTextureParameters() ) {
				const ShaderSampler* sampler = material->GetTechnique()->GetShaderSampler( tex.key );
				if( sampler == nullptr ) {
					continue;
				}

				TextureBinding* textureBinding = (TextureBinding*)bindingMap[tex.value->GetInstanceId()];
				if( textureBinding != nullptr ) {
					TextureDescriptorCreateInfo& imageInfo = awaitingTextureDescriptorSets.Emplace();
					imageInfo = {
						{
							textureBinding->sampler,
							textureBinding->imageView,
							VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
						},
						perFrameData.descriptorSets[sampler->set],
						sampler->binding
					};
				}
			}
		}
	}

	void MaterialBinding::CreateAwaitingDescriptorSets() {
		if( awaitingBufferDescriptorSets.GetSize() == 0 && awaitingTextureDescriptorSets.GetSize() == 0 ) {
			return;
		}

		uint32_t buffersCount = 0;

		for( uint32_t i = 0; i < awaitingBufferDescriptorSets.GetSize(); i++ ) {
			BufferDescriptorCreateInfo& bufferDescriptorCreateInfo = awaitingBufferDescriptorSets[i];
			for( uint32_t j = 0; j < bufferDescriptorCreateInfo.bufferDescriptorInfos.GetSize(); j++ ) {
				buffersCount++;
			}
		}
		
		Array<VkWriteDescriptorSet> writeDescriptorSets( buffersCount + awaitingTextureDescriptorSets.GetSize() );
		Array<VkDescriptorBufferInfo> descriptorBufferInfos( buffersCount + awaitingTextureDescriptorSets.GetSize() );

		uint32_t currentBufferIndex = 0;

		for( uint32_t i = 0; i < awaitingBufferDescriptorSets.GetSize(); i++ ) {
			BufferDescriptorCreateInfo& bufferDescriptorCreateInfo = awaitingBufferDescriptorSets[i];

			for( uint32_t j = 0; j < bufferDescriptorCreateInfo.bufferDescriptorInfos.GetSize(); j++ ) {
				BufferDescriptorInfo& bufferDescriptorInfo = bufferDescriptorCreateInfo.bufferDescriptorInfos[j];

				VkDescriptorBufferInfo& descriptorBufferInfo = descriptorBufferInfos[currentBufferIndex];
				descriptorBufferInfo = {
					bufferDescriptorInfo.buffer,
					0,
					bufferDescriptorInfo.size
				};

				VkWriteDescriptorSet& writeDescriptorSet = writeDescriptorSets[currentBufferIndex++];
				writeDescriptorSet = {
					VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
					nullptr,
					bufferDescriptorInfo.descriptorSet,
					bufferDescriptorInfo.binding,
					0,
					1,
					VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
					nullptr,
					&descriptorBufferInfo,
					nullptr
				};
			}
		}

		for( uint32_t i = 0; i < awaitingTextureDescriptorSets.GetSize(); i++ ) {
			TextureDescriptorCreateInfo& textureDescriptorCreateInfo = awaitingTextureDescriptorSets[i];

			VkWriteDescriptorSet& writeDescriptorSet = writeDescriptorSets[currentBufferIndex++];
			writeDescriptorSet = {
				VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				nullptr,
				textureDescriptorCreateInfo.descriptorSet,
				textureDescriptorCreateInfo.binding,
				0,
				1,
				VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				&textureDescriptorCreateInfo.imageInfo,
				nullptr,
				nullptr
			};
		}

		vkUpdateDescriptorSets( device, writeDescriptorSets.GetSize(), writeDescriptorSets.GetData(), 0, nullptr );

		awaitingBufferDescriptorSets.Clear();
		awaitingTextureDescriptorSets.Clear();
	}

	void MaterialBinding::Reset() {
		vkDestroyDescriptorPool( device, descriptorPool, nullptr );
		for( PerCommandBuffer& perCommandBuffer : perFrameDataArray ) {
			for( uint32_t i = 0; i < perCommandBuffer.uniformBuffers.GetSize(); i++ ) {
				vkFreeMemory( device, perCommandBuffer.uniformBuffersMemory[i], nullptr );
				vkDestroyBuffer( device, perCommandBuffer.uniformBuffers[i], nullptr );
			}
		}
	}

	void MaterialBinding::RemoveBinding() {
		const uint64_t ptrDiff = (uint64_t) this - (uint64_t) bindings.GetData();
		bindings.Remove( (uint32_t) ( ptrDiff / sizeof( MaterialBinding ) ) );
	}
}
