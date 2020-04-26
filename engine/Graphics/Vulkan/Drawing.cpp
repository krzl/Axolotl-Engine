// ReSharper disable CppParameterMayBeConst
#include "stdafx.h"
#include "Drawing.h"

namespace axlt::vk {

	void ClearColor( VkCommandBuffer commandBuffer, VkImage image, VkImageLayout imageLayout,
					 Array<VkImageSubresourceRange>& imageSubresourceRanges, VkClearColorValue clearColor ) {
		vkCmdClearColorImage( commandBuffer, image, imageLayout, &clearColor,
							  imageSubresourceRanges.GetSize(), imageSubresourceRanges.GetData() );
	}

	void ClearDepthStencil( VkCommandBuffer commandBuffer, VkImage image, VkImageLayout imageLayout,
							Array<VkImageSubresourceRange>& imageSubresourceRanges, VkClearDepthStencilValue clearValues ) {
		vkCmdClearDepthStencilImage( commandBuffer, image, imageLayout, &clearValues,
									 imageSubresourceRanges.GetSize(), imageSubresourceRanges.GetData() );
	}

	void ClearRenderPassAttachments( VkCommandBuffer commandBuffer, Array<VkClearAttachment>& attachments,
									 Array<VkClearRect>& rects ) {
		vkCmdClearAttachments( commandBuffer, attachments.GetSize(), attachments.GetData(), rects.GetSize(), rects.GetData() );
	}

	void Draw( VkCommandBuffer commandBuffer, uint32_t vertexCount, uint32_t instanceCount,
			   uint32_t firstVertex, uint32_t firstInstance ) {
		vkCmdDraw( commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance );
	}

	void DrawIndexed( VkCommandBuffer commandBuffer, uint32_t indexCount, uint32_t instanceCount,
					  uint32_t firstIndex, uint32_t vertexOffset, uint32_t firstInstance ) {
		vkCmdDrawIndexed( commandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance );
	}

	void ComputeDispatch( VkCommandBuffer commandBuffer, uint32_t xSize, uint32_t ySize, uint32_t zSize ) {
		vkCmdDispatch( commandBuffer, xSize, ySize, zSize );
	}

	void ExecuteCommands( VkCommandBuffer commandBuffer, Array<VkCommandBuffer>& secondaryBuffers ) {
		vkCmdExecuteCommands( commandBuffer, secondaryBuffers.GetSize(), secondaryBuffers.GetData() );
	}
}