#pragma once
#include "Vulkan.h"
#include "Collections/Array.h"

namespace axlt::vk {

	bool CreateDescriptorSetLayout( Array<VkDescriptorSetLayoutBinding> bindings, VkDescriptorSetLayout& layout );
	bool CreateDescriptorPool( Array<VkDescriptorPoolSize>& poolSizes, uint32_t maxSets, VkDescriptorPool& descriptorPool );
	bool AllocateDescriptorSets( VkDescriptorPool descriptorPool, Array<VkDescriptorSetLayout>& layouts,
								 Array<VkDescriptorSet>& descriptorSets );

	struct ImageDescriptorInfo {
		VkDescriptorSet					descriptorSet;
		uint32_t						bindingId;
		uint32_t						arrayElement;
		VkDescriptorType				type;
		Array<VkDescriptorImageInfo>	imageInfos;
	};

	struct BufferDescriptorInfo {
		VkDescriptorSet					descriptorSet;
		uint32_t						bindingId;
		uint32_t						arrayElement;
		VkDescriptorType				type;
		Array<VkDescriptorBufferInfo>	bufferInfos;
	};

	struct TexelBufferDescriptorInfo {
		VkDescriptorSet					descriptorSet;
		uint32_t						bindingId;
		uint32_t						arrayElement;
		VkDescriptorType				type;
		Array<VkBufferView>				bufferViews;
	};

	struct CopyDescriptorInfo {
		VkDescriptorSet					targetDescriptorSet;
		uint32_t						targetBindingId;
		uint32_t						targetArrayElement;
		VkDescriptorSet					sourceDescriptorSet;
		uint32_t						sourceBindingId;
		uint32_t						sourceArrayElement;
		uint32_t						descriptorCount;
	};

	void UpdateDescriptorSets( Array<ImageDescriptorInfo>& imageDescriptorInfos,
							   Array<BufferDescriptorInfo>& bufferDescriptorInfos,
							   Array<TexelBufferDescriptorInfo>& texelBufferDescriptorInfos,
							   Array<CopyDescriptorInfo>& copyDescriptorInfos );

	void BindDescriptorSets( VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineType,
							 VkPipelineLayout pipelineLayout, Array<VkDescriptorSet>& descriptorSets,
							 uint32_t firstSetIndex, Array<uint32_t>& dynamicOffsets );


	bool FreeDescriptorSets( VkDescriptorPool descriptorPool, Array<VkDescriptorSet> descriptorSets );
	bool ResetDescriptorPool( VkDescriptorPool descriptorPool );
	void DestroyDescriptorPool( VkDescriptorPool& descriptorPool );
	void DestroyDescriptorSetLayout( VkDescriptorSetLayout& descriptorSetLayout );
}