#include "stdafx.h"
#include "Vulkan.h"
#include "Graphics/RenderSystem.h"

namespace axlt::vk {

	struct BufferDescriptorInfo {
		VkBuffer buffer;
		uint32_t size;
		uint8_t binding;
		VkDescriptorSet descriptorSet;
	};

	struct MaterialBindInfo {
		Array<BufferDescriptorInfo> bufferDescriptorInfos;
	};

	Array<MaterialBindInfo> materialBindInfos;

	MaterialData* CreateMaterialData( ResourceHandle<MaterialResource>& material, TechniqueData& techniqueData ) {
		MaterialData* materialData = &materialDataArray.Add( material.guid, MaterialData() );
		MaterialBindInfo& materialBindInfo = materialBindInfos.Emplace();

		FixedArray<VkDescriptorPoolSize, 2 > poolSizes;
		if( material->GetTechnique()->uniformBlocks.GetSize() > 0 ) {
			VkDescriptorPoolSize& poolSize = poolSizes.Emplace();
			poolSize = {
				VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				material->GetTechnique()->uniformBlocks.GetSize() * commandBuffers.GetSize()
			};
		}
		if( material->GetTechnique()->samplers.GetSize() > 0 ) {
			VkDescriptorPoolSize& poolSize = poolSizes.Emplace();
			poolSize = {
				VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				material->GetTechnique()->samplers.GetSize() * commandBuffers.GetSize()
			};
		}

		VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {
			VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
			nullptr,
			0,
			material->GetTechnique()->uniformBlocks.GetSize() * commandBuffers.GetSize() +
			material->GetTechnique()->samplers.GetSize() * commandBuffers.GetSize(),
			poolSizes.GetSize(),
			poolSizes.GetData()
		};

		vkCreateDescriptorPool( device, &descriptorPoolCreateInfo, nullptr, &materialData->descriptorPool );

		VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {
			VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			nullptr,
			materialData->descriptorPool,
			techniqueData.layouts.GetSize(),
			techniqueData.layouts.GetData()
		};

		materialData->perFrameData.Clear();
		materialData->perFrameData.AddEmpty( commandBuffers.GetSize() );
		for( uint32_t frameId = 0; frameId < materialData->perFrameData.GetSize(); frameId++ ) {
			MaterialData::PerCommandBuffer& perFrameData = materialData->perFrameData[frameId];
			perFrameData.uniformBuffers.Clear();
			perFrameData.uniformBuffers.AddEmpty( material->GetTechnique()->uniformBlocks.GetSize() );
			perFrameData.uniformBuffersMemory.AddEmpty( material->GetTechnique()->uniformBlocks.GetSize() );
			perFrameData.dirtyUniformBuffers.AddEmpty( material->GetTechnique()->uniformBlocks.GetSize() );

			if( techniqueData.layouts.GetSize() > 0 ) {
				perFrameData.descriptorSets.AddEmpty( techniqueData.layouts.GetSize() );

				const VkResult result = vkAllocateDescriptorSets( device, &descriptorSetAllocateInfo, perFrameData.descriptorSets.GetData() );
				if( result != VK_SUCCESS ) {
					printf( "Could not allocate descriptor sets\n" );
					return nullptr;
				}
			}

			for( uint32_t i = 0; i < material->GetTechnique()->uniformBlocks.GetSize(); i++ ) {
				const ShaderUniformBlock& uniformBlock = material->GetTechnique()->uniformBlocks[i];

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
						return nullptr;
					}

					VkMemoryRequirements memoryRequirements;
					vkGetBufferMemoryRequirements( device, perFrameData.uniformBuffers[i], &memoryRequirements );

					if( !AllocateMemory( memoryRequirements, (VkMemoryPropertyFlagBits)
						( VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT ),
										 &perFrameData.uniformBuffersMemory[i] ) ) {
						return nullptr;
					}

					result = vkBindBufferMemory( device, perFrameData.uniformBuffers[i],
												 perFrameData.uniformBuffersMemory[i], 0 );
					if( result != VK_SUCCESS ) {
						printf( "Could not bind buffer memory\n" );
						return nullptr;
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

			//TODO: Samplers
		}

		return materialData;
	}

	void SetupDescriptorSets() {
		if( materialBindInfos.GetSize() == 0 ) return;

		Array<VkWriteDescriptorSet> writeDescriptorSets;
		Array<VkDescriptorBufferInfo> descriptorBufferInfos;

		uint32_t buffersCount = 0;
		
		for( uint32_t i = 0; i < materialBindInfos.GetSize(); i++ ) {
			MaterialBindInfo& materialBindInfo = materialBindInfos[i];
			for( uint32_t j = 0; j < materialBindInfo.bufferDescriptorInfos.GetSize(); j++ ) {
				buffersCount++;
			}
		}

		descriptorBufferInfos.AddEmpty( buffersCount );

		uint32_t currentBufferIndex = 0;

		for( uint32_t i = 0; i < materialBindInfos.GetSize(); i++ ) {
			MaterialBindInfo& materialBindInfo = materialBindInfos[i];

			for( uint32_t j = 0; j < materialBindInfo.bufferDescriptorInfos.GetSize(); j++ ) {
				BufferDescriptorInfo& bufferDescriptorInfo = materialBindInfo.bufferDescriptorInfos[j];

				VkDescriptorBufferInfo& descriptorBufferInfo = descriptorBufferInfos[currentBufferIndex++];
				descriptorBufferInfo = {
					bufferDescriptorInfo.buffer,
					0,
					bufferDescriptorInfo.size
				};

				VkWriteDescriptorSet& writeDescriptorSet = writeDescriptorSets.Emplace();
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

		vkUpdateDescriptorSets( device, writeDescriptorSets.GetSize(), writeDescriptorSets.GetData(), 0, nullptr );

		materialBindInfos.Clear();
	}
}