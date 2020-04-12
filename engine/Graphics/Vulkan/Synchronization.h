#pragma once
#include "Vulkan.h"
#include "Collections/Array.h"

namespace axlt::vk {

	bool CreateSemaphoreVk( VkSemaphore& semaphore );
	bool CreateFence( VkFence& fence, bool isSignaled );
	bool WaitForFences( Array<VkFence>& fences, bool waitForAll, uint64_t timeout );
	bool ResetFences( Array<VkFence>& fences );
	
	void DestroyFence( VkFence& fence );
	void DestroySemaphore( VkSemaphore& semaphore );
}