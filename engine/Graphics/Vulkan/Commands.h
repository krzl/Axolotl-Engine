#pragma once
#include "Vulkan.h"
#include "Collections/Array.h"

namespace axlt::vk {

	bool CreateCommandPool( uint32_t queueFamilyIndex,
							VkCommandPoolCreateFlags flags,
							VkCommandPool& commandPool );

	bool AllocateCommandBuffers( VkCommandPool commandPool, bool primary, uint32_t bufferCount,
								 Array<VkCommandBuffer>& commandBuffers );

	bool BeginRecordingCommands( VkCommandBuffer commandBuffer,
								 VkCommandBufferUsageFlags usageFlags,
								 VkRenderPass renderPass,
								 uint32_t subpassIndex,
								 VkFramebuffer framebuffer,
								 VkBool32 occlusionQueryEnabled,
								 VkQueryControlFlags queryFlags,
								 VkQueryPipelineStatisticFlags pipelineStatistics );

	bool BeginRecordingCommands( VkCommandBuffer commandBuffer,
								 VkCommandBufferUsageFlags usageFlags,
								 VkCommandBufferInheritanceInfo* inheritanceInfo = nullptr );

	bool EndRecordingCommands( VkCommandBuffer commandBuffer );

	bool ResetCommandBuffer( VkCommandBuffer commandBuffer, bool releaseResources );

	struct WaitSemaphoreInfo {
		VkSemaphore semaphore;
		VkPipelineStageFlags waitingStage;
	};

	bool SubmitCommandBuffers( VkQueue queue, Array<WaitSemaphoreInfo>& waitSemaphoreInfos,
							   Array<VkCommandBuffer>& commandBuffers,
							   Array<VkSemaphore>& signalSemaphores,
							   VkFence fence );
	
	bool WaitForQueueIdle( VkQueue queue );
	bool WaitForDeviceIdle();

	void FreeCommandBuffers( VkCommandPool commandPool, Array<VkCommandBuffer>& commandBuffers );
	void DestroyCommandPool( VkCommandPool& commandPool );
}