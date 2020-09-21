#include "stdafx.h"
#include "Drawing.h"

namespace axlt::vk {
	int8_t BeginDrawCommand() {
		uint32_t currentCommandBufferIndex;
		VkResult result = vkAcquireNextImageKHR( device, swapchain, 1000000000u, imageAvailableSemaphore, VK_NULL_HANDLE, &currentCommandBufferIndex );
		if( result != VK_SUCCESS ) {
			printf( "Could not acquire next image\n" );
			return -1;
		}
		
		result = vkWaitForFences( device, 1, &renderFences[currentCommandBufferIndex], VK_TRUE, 1000000000 );
		if( result != VK_SUCCESS ) {
			printf( "Could not wait for current render fence\n" );
			return -1;
		}

		result = vkResetFences( device, 1, &renderFences[currentCommandBufferIndex] );
		if( result != VK_SUCCESS ) {
			printf( "Could not reset current render fence\n" );
			return -1;
		}
	
		VkCommandBuffer& currentCommandBuffer = commandBuffers[currentCommandBufferIndex];
		result = vkResetCommandBuffer( currentCommandBuffer, 0 );
		if( result != VK_SUCCESS ) {
			printf( "Could not reset command buffer\n" );
			return -1;
		}

		VkCommandBufferBeginInfo commandBufferBeginInfo = {
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			nullptr,
			VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
			nullptr
		};

		result = vkBeginCommandBuffer( currentCommandBuffer, &commandBufferBeginInfo );
		if( result != VK_SUCCESS ) {
			printf( "Could not begin command buffer\n" );
			return -1;
		}

		VkImageMemoryBarrier presentToDrawBarrier = {
			VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
			nullptr,
			0,
			VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
			queueFamilyIndices[0] == presentationQueueFamilyIndex ? VK_QUEUE_FAMILY_IGNORED : presentationQueueFamilyIndex,
			queueFamilyIndices[0] == presentationQueueFamilyIndex ? VK_QUEUE_FAMILY_IGNORED : queueFamilyIndices[0],
			swapchainImages[currentCommandBufferIndex],
			{
				VK_IMAGE_ASPECT_COLOR_BIT,
				0,
				1,
				0,
				1
			}
		};

		vkCmdPipelineBarrier( currentCommandBuffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, 0, nullptr, 0, nullptr, 1, &presentToDrawBarrier );

		return currentCommandBufferIndex;
	}
	
	void SubmitDrawCommand( const int8_t commandBufferIndex ) {

		VkCommandBuffer& currentCommandBuffer = commandBuffers[commandBufferIndex];
		
		if( queueFamilyIndices[0] != presentationQueueFamilyIndex ) {
			VkImageMemoryBarrier drawToPresentBarrier = {
				VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
				nullptr,
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				VK_ACCESS_MEMORY_READ_BIT,
				VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
				VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
				queueFamilyIndices[0],
				presentationQueueFamilyIndex,
				swapchainImages[commandBufferIndex],
				{
					VK_IMAGE_ASPECT_COLOR_BIT,
					0,
					1,
					0,
					1
				}
			};

			vkCmdPipelineBarrier( currentCommandBuffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &drawToPresentBarrier );
		}

		VkResult result = vkEndCommandBuffer( currentCommandBuffer );
		if( result != VK_SUCCESS ) {
			printf( "Could not end command buffer\n" );
			return;
		}

		VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

		VkSubmitInfo queueSubmitInfo = {
			VK_STRUCTURE_TYPE_SUBMIT_INFO,
			nullptr,
			1,
			&imageAvailableSemaphore,
			&waitDstStageMask,
			1,
			&currentCommandBuffer,
			1,
			&renderingFinishedSemaphore
		};
		
		result = vkQueueSubmit( queues[0][0], 1, &queueSubmitInfo, renderFences[commandBufferIndex] );
		if( result != VK_SUCCESS ) {
			printf( "Could not submit queue\n" );
			return;
		}

		uint32_t swapchainImageIndex = commandBufferIndex;
		
		VkPresentInfoKHR presentInfo = {
			VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
			nullptr,
			1,
			&renderingFinishedSemaphore,
			1,
			&swapchain,
			(const uint32_t*) &swapchainImageIndex,
			nullptr
		};

		result = vkQueuePresentKHR( presentationQueue, &presentInfo );
		if( result != VK_SUCCESS ) {
			printf( "Could not present queue\n" );
			return;
		}
	}
	
	VkCommandBuffer CreateCopyCommandBuffer() {
		
		VkCommandBufferAllocateInfo commandBufferAllocateInfo = {
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			nullptr,
			copyCommandPool,
			VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			1
		};

		VkCommandBufferBeginInfo beginInfo = {
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			nullptr,
			VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
			nullptr
		};

		VkCommandBuffer copyCommandBuffer;

		VkResult result = vkAllocateCommandBuffers( device, &commandBufferAllocateInfo, &copyCommandBuffer );
		if( result != VK_SUCCESS ) {
			printf( "Could not allocate copy command buffer\n" );
			return VK_NULL_HANDLE;
		}
		
		result = vkBeginCommandBuffer( copyCommandBuffer, &beginInfo );
		if( result != VK_SUCCESS ) {
			printf( "Could not begin copy command buffer\n" );
			return VK_NULL_HANDLE;
		}

		return copyCommandBuffer;
	}

	void SubmitCopyCommandBuffer( const VkCommandBuffer copyCommandBuffer ) {
		
		vkEndCommandBuffer( copyCommandBuffer );

		VkSubmitInfo submitInfo = {
			VK_STRUCTURE_TYPE_SUBMIT_INFO,
			nullptr,
			0,
			nullptr,
			nullptr,
			1,
			&copyCommandBuffer,
			0,
			nullptr
		};

		VkResult result = vkQueueSubmit( queues[0][1], 1, &submitInfo, VK_NULL_HANDLE );
		if( result != VK_SUCCESS ) {
			printf( "Could not submit queue\n" );
			return;
		}
		result = vkQueueWaitIdle( queues[0][1] );
		if( result != VK_SUCCESS ) {
			printf( "Could not wait idle for queue\n" );
			return;
		}

		vkFreeCommandBuffers( device, copyCommandPool, 1, &copyCommandBuffer );
	}

}