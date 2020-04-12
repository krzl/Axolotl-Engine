#pragma once
#include "Vulkan.h"
#include "Collections/Array.h"

namespace axlt::vk {

	void ClearColor( VkCommandBuffer commandBuffer, VkImage image, VkImageLayout imageLayout,
					 Array<VkImageSubresourceRange>& imageSubresourceRanges, VkClearColorValue clearColor );
	void ClearDepthStencil( VkCommandBuffer commandBuffer, VkImage image, VkImageLayout imageLayout,
							Array<VkImageSubresourceRange>& imageSubresourceRanges, VkClearDepthStencilValue clearValues );
	void ClearRenderPassAttachments( VkCommandBuffer commandBuffer, Array<VkClearAttachment>& attachments,
									 Array<VkClearRect>& rects );

	void Draw( VkCommandBuffer commandBuffer, uint32_t vertexCount, uint32_t instanceCount, 
			   uint32_t firstVertex, uint32_t firstInstance, uint32_t );
}