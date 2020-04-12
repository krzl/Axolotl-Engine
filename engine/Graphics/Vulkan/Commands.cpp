// ReSharper disable CppParameterMayBeConst
#include "stdafx.h"
#include "Commands.h"

namespace axlt::vk {

	bool CreateCommandPool( const uint32_t queueFamilyIndex,
							const VkCommandPoolCreateFlags flags,
							VkCommandPool& commandPool ) {

		VkCommandPoolCreateInfo commandPoolCreateInfo{
			VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			nullptr,
			flags,
			queueFamilyIndex
		};

		const VkResult result = vkCreateCommandPool( device, &commandPoolCreateInfo, nullptr, &commandPool );
		if( VK_SUCCESS != result ) {
			printf( "Could not create command pool\n" );
			return false;
		}

		return true;
	}

	bool AllocateCommandBuffers( const VkCommandPool commandPool, const bool primary,
								 const uint32_t bufferCount, Array<VkCommandBuffer>& commandBuffers ) {

		VkCommandBufferAllocateInfo commandBufferAllocateInfo = {
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			nullptr,
			commandPool,
			primary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY,
			bufferCount
		};

		commandBuffers.Clear();
		commandBuffers.AddEmpty( bufferCount );

		const VkResult result = vkAllocateCommandBuffers( device,
														  &commandBufferAllocateInfo,
														  commandBuffers.GetData() );
		if( VK_SUCCESS != result ) {
			printf( "Could not allocate command buffers\n" );
			return false;
		}

		return true;
	}

	bool BeginRecordingCommands( VkCommandBuffer commandBuffer,
								 const VkCommandBufferUsageFlags usageFlags,
								 const VkRenderPass renderPass,
								 const uint32_t subpassIndex,
								 const VkFramebuffer framebuffer,
								 const VkBool32 occlusionQueryEnabled,
								 const VkQueryControlFlags queryFlags,
								 const VkQueryPipelineStatisticFlags pipelineStatistics ) {

		VkCommandBufferInheritanceInfo inheritanceInfo = {
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO,
			nullptr,
			renderPass,
			subpassIndex,
			framebuffer,
			occlusionQueryEnabled,
			queryFlags,pipelineStatistics
		};

		return BeginRecordingCommands( commandBuffer, usageFlags, &inheritanceInfo );
	}

	bool BeginRecordingCommands( VkCommandBuffer commandBuffer,
								 const VkCommandBufferUsageFlags usageFlags,
								 VkCommandBufferInheritanceInfo* inheritanceInfo ) {

		VkCommandBufferBeginInfo commandBufferBeginInfo = {
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			nullptr,
			usageFlags,
			inheritanceInfo
		};

		const VkResult result = vkBeginCommandBuffer( commandBuffer, &commandBufferBeginInfo );
		if( VK_SUCCESS != result ) {
			printf( "Could not begin recording commands\n" );
			return false;
		}

		return true;
	}

	bool EndRecordingCommands( VkCommandBuffer commandBuffer ) {

		const VkResult result = vkEndCommandBuffer( commandBuffer );
		if( VK_SUCCESS != result ) {
			printf( "Could not end recording commands\n" );
			return false;
		}

		return true;
	}

	bool ResetCommandBuffer( VkCommandBuffer commandBuffer, const bool releaseResources ) {
		const VkCommandBufferResetFlags resetFlags =
			releaseResources ? VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT : 0;

		const VkResult result = vkResetCommandBuffer( commandBuffer, resetFlags );
		if( VK_SUCCESS != result ) {
			printf( "Could not reset command buffer\n" );
			return false;
		}

		return true;
	}

	bool ResetCommandPool( VkCommandPool commandPool, const bool releaseResources ) {
		const VkCommandPoolResetFlags resetFlags =
			releaseResources ? VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT : 0;

		const VkResult result = vkResetCommandPool( device, commandPool, resetFlags );
		if( VK_SUCCESS != result ) {
			printf( "Could not reset command pool\n" );
			return false;
		}

		return true;
	}

	bool SubmitCommandBuffers( VkQueue queue, const Array<WaitSemaphoreInfo>& waitSemaphoreInfos,
							   const Array<VkCommandBuffer>& commandBuffers,
							   const Array<VkSemaphore>& signalSemaphores,
							   VkFence fence ) {

		Array<VkSemaphore> waitSemaphoreHandles;
		waitSemaphoreHandles.AddEmpty( waitSemaphoreInfos.GetSize() );
		Array<VkPipelineStageFlags> waitSemaphoreStages;
		waitSemaphoreStages.AddEmpty( waitSemaphoreInfos.GetSize() );

		for( uint32_t i = 0; i < waitSemaphoreInfos.GetSize(); i++ ) {
			waitSemaphoreHandles[i] = waitSemaphoreInfos[i].semaphore;
			waitSemaphoreStages[i] = waitSemaphoreInfos[i].waitingStage;
		}

		VkSubmitInfo submitInfo = {
			VK_STRUCTURE_TYPE_SUBMIT_INFO,
			nullptr,
			waitSemaphoreHandles.GetSize(),
			waitSemaphoreHandles.GetData(),
			waitSemaphoreStages.GetData(),
			commandBuffers.GetSize(),
			commandBuffers.GetData(),
			signalSemaphores.GetSize(),
			signalSemaphores.GetData(),
		};

		const VkResult result = vkQueueSubmit( queue, 1, &submitInfo, fence );
		if( VK_SUCCESS != result ) {
			printf( "Could not submit command buffers\n" );
			return false;
		}

		return true;
	}

	bool WaitForQueueIdle( VkQueue queue ) {
		const VkResult result = vkQueueWaitIdle( queue );
		if( VK_SUCCESS != result ) {
			printf( "Error while waiting for queue to finish commands\n" );
			return false;
		}

		return true;
	}

	bool WaitForDeviceIdle() {
		const VkResult result = vkDeviceWaitIdle( device );
		if( VK_SUCCESS != result ) {
			printf( "Error while waiting for device to finish commands\n" );
			return false;
		}

		return true;
	}

	void FreeCommandBuffers( VkCommandPool commandPool, Array<VkCommandBuffer>& commandBuffers ) {
		if( commandBuffers.GetSize() == 0 ) {
			return;
		}

		vkFreeCommandBuffers( device, commandPool, commandBuffers.GetSize(), commandBuffers.GetData() );
		commandBuffers.Clear();
	}

	void DestroyCommandPool( VkCommandPool& commandPool ) {
		if( commandPool != VK_NULL_HANDLE ) {
			vkDestroyCommandPool( device, commandPool, nullptr );
			commandPool = VK_NULL_HANDLE;
		}
	}
}