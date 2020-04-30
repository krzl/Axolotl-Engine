#include "stdafx.h"
#include "Vulkan.h"
#include "Graphics/RenderSystem.h"

namespace axlt::vk {

	struct BufferDescriptorInfo {
		VkBuffer buffer;
		uint32_t size;
		uint8_t set;
		uint8_t binding;
	};

	struct MaterialBindInfo {
		ExactArray<VkDescriptorSet> descriptorSets;
		Array<BufferDescriptorInfo> bufferDescriptorInfos;
	};

	Array<MaterialBindInfo> materialBindInfos;

	void CreateMaterialData( ResourceHandle<MaterialResource>& material, TechniqueData& techniqueData ) {
		MaterialData* materialData = &materialDataArray.Add( material.guid, MaterialData() );
		material.isDirty = true;
		MaterialBindInfo& materialBindInfo = materialBindInfos.Emplace();
		
		FixedArray<VkDescriptorPoolSize,2 > poolSizes;
		if( material->technique->uniformBlocks.GetSize() > 0 ) {
			VkDescriptorPoolSize& poolSize = poolSizes.Emplace();
			poolSize = {
				VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				material->technique->uniformBlocks.GetSize() * commandBuffers.GetSize()
			};
		}
		if( material->technique->samplers.GetSize() > 0 ) {
			VkDescriptorPoolSize& poolSize = poolSizes.Emplace();
			poolSize = {
				VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				material->technique->samplers.GetSize() * commandBuffers.GetSize()
			};
		}

		VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {
			VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
			nullptr,
			0,
			material->technique->uniformBlocks.GetSize() * commandBuffers.GetSize() +
			material->technique->samplers.GetSize() * commandBuffers.GetSize(),
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

		if( techniqueData.layouts.GetSize() > 0 ) {
			materialData->descriptorSets.AddEmpty( techniqueData.layouts.GetSize() );

			vkAllocateDescriptorSets( device, &descriptorSetAllocateInfo, materialData->descriptorSets.GetData() );
			
			materialBindInfo.descriptorSets = materialData->descriptorSets;
		}

		materialData->perFrameData.Clear();
		materialData->perFrameData.AddEmpty( commandBuffers.GetSize() );
		for( uint32_t frameId = 0; frameId < materialData->perFrameData.GetSize(); frameId++ ) {
			MaterialData::PerCommandBuffer& perFrameData = materialData->perFrameData[frameId];
			perFrameData.uniformBuffers.Clear();
			perFrameData.uniformBuffers.AddEmpty( material->technique->uniformBlocks.GetSize() );
			perFrameData.uniformBuffersMemory.AddEmpty( material->technique->uniformBlocks.GetSize() );

			for( uint32_t i = 0; i < material->technique->uniformBlocks.GetSize(); i++ ) {
				const ShaderUniformBlock& uniformBlock = material->technique->uniformBlocks[i];

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

					result = vkBindBufferMemory( device, perFrameData.uniformBuffers[i],
												 perFrameData.uniformBuffersMemory[i], 0 );
					if( result != VK_SUCCESS ) {
						printf( "Could not bind buffer memory\n" );
						return;
					}

					BufferDescriptorInfo& bufferDescriptorInfo = materialBindInfo.bufferDescriptorInfos.Emplace();
					bufferDescriptorInfo = {
						perFrameData.uniformBuffers[i],
						uniformBlock.size,
						uniformBlock.set,
						uniformBlock.binding
					};
				} else {
					BufferDescriptorInfo& bufferDescriptorInfo = materialBindInfo.bufferDescriptorInfos.Emplace();
					bufferDescriptorInfo = {
						perCameraBuffers[frameId],
						uniformBlock.size,
						uniformBlock.set,
						uniformBlock.binding
					};
				}
			}

			/*
			VkDescriptorBufferInfo bufferInfo = {
				perFrameData.uniformBuffers[i],
				0,
				uniformBlock.size
			};

			VkWriteDescriptorSet writeDescriptorSet = {
				VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				nullptr,
				materialBindInfo.descriptorSets[uniformBlock.set],
				uniformBlock.binding,
				0,
				1,
				VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				nullptr,
				&bufferInfo,
				nullptr
			};
			*/
			
			//TODO: Samplers
		}
	}

	void SetupDescriptorSets() {
		if( materialBindInfos.GetSize() == 0 ) return;
		
		Array<VkWriteDescriptorSet> writeDescriptorSets;
		Array<VkDescriptorBufferInfo> descriptorBufferInfos;
		
		for( uint32_t i = 0; i < materialBindInfos.GetSize(); i++ ) {
			MaterialBindInfo& materialBindInfo = materialBindInfos[i];

			for( uint32_t j = 0; j < materialBindInfo.bufferDescriptorInfos.GetSize(); j++ ) {
				BufferDescriptorInfo& bufferDescriptorInfo = materialBindInfo.bufferDescriptorInfos[j];

				VkDescriptorBufferInfo& descriptorBufferInfo = descriptorBufferInfos.Emplace();
				descriptorBufferInfo = {
					bufferDescriptorInfo.buffer,
					0,
					bufferDescriptorInfo.size
				};
				
				VkWriteDescriptorSet& writeDescriptorSet = writeDescriptorSets.Emplace();
				writeDescriptorSet = {
					VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
					nullptr,
					materialBindInfo.descriptorSets[ bufferDescriptorInfo.set ],
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