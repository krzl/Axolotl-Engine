#pragma once
#include "Vulkan.h"
#include "Collections/Array.h"

namespace axlt::vk {

	bool CreateBuffer( VkDeviceSize size, VkBufferUsageFlags usage, VkBuffer& buffer );

	bool BindMemoryToBuffer( VkBuffer buffer, VkDeviceMemory& memoryObject,
							 VkMemoryPropertyFlagBits memoryProperties = (VkMemoryPropertyFlagBits) 0,
							 uint32_t memoryOffset = 0 );
	
	struct BufferTransition {
		VkBuffer buffer{};
		VkAccessFlags oldAccess{};
		VkAccessFlags newAccess{};
		uint32_t oldQueueFamily = VK_QUEUE_FAMILY_IGNORED;
		uint32_t newQueueFamily = VK_QUEUE_FAMILY_IGNORED;
	};

	void SetBufferBarriers( Array<BufferTransition>& bufferTransitions, VkCommandBuffer commandBuffer,
							VkPipelineStageFlags generatingStages, VkPipelineStageFlags consumingStages );

	bool CreateBufferView( VkBuffer buffer, VkFormat format,
						   VkDeviceSize memoryOffset, VkDeviceSize memoryRange,
						   VkBufferView& bufferView );
	
	void CopyBuffersData( VkCommandBuffer commandBuffer, VkBuffer source, 
						  VkBuffer destination, Array<VkBufferCopy>& bufferCopyInfo );
	
	void CopyDataFromImage( VkCommandBuffer commandBuffer, VkImage source,
							VkImageLayout layout, VkBuffer destination,
							Array<VkBufferImageCopy>& bufferCopyInfo );
	
	void DestroyBufferView( VkBufferView& bufferView );
}