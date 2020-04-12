// ReSharper disable CppParameterMayBeConst
#include "stdafx.h"
#include "Descriptors.h"

namespace axlt::vk {

	bool CreateDescriptorSetLayout( Array<VkDescriptorSetLayoutBinding> bindings, VkDescriptorSetLayout& layout ) {

		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
			VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			nullptr,
			0,
			bindings.GetSize(),
			bindings.GetData()
		};

		const VkResult result = vkCreateDescriptorSetLayout( device, &descriptorSetLayoutCreateInfo, nullptr, &layout );
		if( VK_SUCCESS != result ) {
			printf( "Could not create descriptor set layout\n" );
			return false;
		}

		return true;
	}

	bool CreateDescriptorPool( Array<VkDescriptorPoolSize>& poolSizes, uint32_t maxSets, VkDescriptorPool& descriptorPool ) {

		VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {
			VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
			nullptr,
			0,
			maxSets,
			poolSizes.GetSize(),
			poolSizes.GetData()
		};

		const VkResult result = vkCreateDescriptorPool( device, &descriptorPoolCreateInfo, nullptr, &descriptorPool );
		if( VK_SUCCESS != result ) {
			printf( "Could not create descriptor pool\n" );
			return false;
		}

		return true;
	}

	bool AllocateDescriptorSets( VkDescriptorPool descriptorPool, Array<VkDescriptorSetLayout>& layouts,
								 Array<VkDescriptorSet>& descriptorSets ) {

		VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {
			VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			nullptr,
			descriptorPool,
			layouts.GetSize(),
			layouts.GetData()
		};

		descriptorSets.Clear();
		descriptorSets.AddEmpty( layouts.GetSize() );

		const VkResult result = vkAllocateDescriptorSets( device, &descriptorSetAllocateInfo, descriptorSets.GetData() );
		if( VK_SUCCESS != result ) {
			printf( "Could not allocate descriptor sets\n" );
			return false;
		}

		return true;
	}

	void UpdateDescriptorSets( Array<ImageDescriptorInfo>& imageDescriptorInfos,
							   Array<BufferDescriptorInfo>& bufferDescriptorInfos,
							   Array<TexelBufferDescriptorInfo>& texelBufferDescriptorInfos,
							   Array<CopyDescriptorInfo>& copyDescriptorInfos ) {

		Array<VkWriteDescriptorSet> writeDescriptors;

		writeDescriptors.AddEmpty( imageDescriptorInfos.GetSize() + bufferDescriptorInfos.GetSize() +
								   texelBufferDescriptorInfos.GetSize() );

		uint32_t currentId = 0;

		for( ImageDescriptorInfo& imageDescriptorInfo : imageDescriptorInfos ) {
			writeDescriptors[currentId++] = VkWriteDescriptorSet{
				VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				nullptr,
				imageDescriptorInfo.descriptorSet,
				imageDescriptorInfo.bindingId,
				imageDescriptorInfo.arrayElement,
				imageDescriptorInfo.imageInfos.GetSize(),
				imageDescriptorInfo.type,
				imageDescriptorInfo.imageInfos.GetData(),
				nullptr,
				nullptr
			};
		}

		for( BufferDescriptorInfo& bufferDescriptorInfo : bufferDescriptorInfos ) {
			writeDescriptors[currentId++] = VkWriteDescriptorSet{
				VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				nullptr,
				bufferDescriptorInfo.descriptorSet,
				bufferDescriptorInfo.bindingId,
				bufferDescriptorInfo.arrayElement,
				bufferDescriptorInfo.bufferInfos.GetSize(),
				bufferDescriptorInfo.type,
				nullptr,
				bufferDescriptorInfo.bufferInfos.GetData(),
				nullptr
			};
		}

		for( TexelBufferDescriptorInfo& texelBufferDescriptorInfo : texelBufferDescriptorInfos ) {
			writeDescriptors[currentId++] = VkWriteDescriptorSet{
				VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				nullptr,
				texelBufferDescriptorInfo.descriptorSet,
				texelBufferDescriptorInfo.bindingId,
				texelBufferDescriptorInfo.arrayElement,
				texelBufferDescriptorInfo.bufferViews.GetSize(),
				texelBufferDescriptorInfo.type,
				nullptr,
				nullptr,
				texelBufferDescriptorInfo.bufferViews.GetData()
			};
		}

		Array<VkCopyDescriptorSet> copyDescriptors;
		copyDescriptors.AddEmpty( copyDescriptors.GetSize() );

		for( uint32_t i = 0; i < copyDescriptorInfos.GetSize(); i++ ) {
			const CopyDescriptorInfo copyDescriptorInfo = copyDescriptorInfos[i];

			copyDescriptors[i] = VkCopyDescriptorSet{
				VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET,
				nullptr,
				copyDescriptorInfo.sourceDescriptorSet,
				copyDescriptorInfo.sourceBindingId,
				copyDescriptorInfo.sourceArrayElement,
				copyDescriptorInfo.targetDescriptorSet,
				copyDescriptorInfo.sourceBindingId,
				copyDescriptorInfo.sourceArrayElement,
				copyDescriptorInfo.descriptorCount
			};
		}

		vkUpdateDescriptorSets( device, writeDescriptors.GetSize(), writeDescriptors.GetData(),
								copyDescriptors.GetSize(), copyDescriptors.GetData() );
	}

	void BindDescriptorSets( VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineType,
							 VkPipelineLayout pipelineLayout, Array<VkDescriptorSet>& descriptorSets,
							 uint32_t firstSetIndex, Array<uint32_t>& dynamicOffsets ) {

		vkCmdBindDescriptorSets( commandBuffer, pipelineType, pipelineLayout, firstSetIndex,
								 descriptorSets.GetSize(), descriptorSets.GetData(),
								 dynamicOffsets.GetSize(), dynamicOffsets.GetData() );
	}

	bool FreeDescriptorSets( VkDescriptorPool descriptorPool, Array<VkDescriptorSet> descriptorSets ) {
		const VkResult result = vkFreeDescriptorSets( device, descriptorPool, descriptorSets.GetSize(),
													  descriptorSets.GetData() );
		if( VK_SUCCESS != result ) {
			printf( "Could not free descriptor sets\n" );
			return false;
		}

		return true;
	}

	bool ResetDescriptorPool( VkDescriptorPool descriptorPool ) {
		const VkResult result = vkResetDescriptorPool( device, descriptorPool, 0 );
		if( VK_SUCCESS != result ) {
			printf( "Could not reset descriptor pool\n" );
			return false;
		}

		return true;
	}

	void DestroyDescriptorPool( VkDescriptorPool& descriptorPool ) {
		if( VK_NULL_HANDLE != descriptorPool ) {
			vkDestroyDescriptorPool( device, descriptorPool, nullptr );
			descriptorPool = VK_NULL_HANDLE;
		}
	}

	void DestroyDescriptorSetLayout( VkDescriptorSetLayout& descriptorSetLayout ) {
		if( VK_NULL_HANDLE != descriptorSetLayout ) {
			vkDestroyDescriptorSetLayout( device, descriptorSetLayout, nullptr );
			descriptorSetLayout = VK_NULL_HANDLE;
		}
	}
}