#include "stdafx.h"
#include "Synchronization.h"

namespace axlt::vk {

	bool CreateSemaphoreVk( VkSemaphore& semaphore ) {

		VkSemaphoreCreateInfo semaphoreCreateInfo = {
			VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
			nullptr,
			0
		};

		const VkResult result = vkCreateSemaphore( device, &semaphoreCreateInfo, nullptr, &semaphore );
		if( VK_SUCCESS != result ) {
			printf( "Could not create semaphore\n" );
			return false;
		}

		return true;
	}

	bool CreateFence( VkFence& fence, const bool isSignaled ) {

		VkFenceCreateInfo fenceCreateInfo = {
			VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
			nullptr,
			isSignaled ? VK_FENCE_CREATE_SIGNALED_BIT : (VkFenceCreateFlags) 0
		};

		const VkResult result = vkCreateFence( device, &fenceCreateInfo, nullptr, &fence );
		if( VK_SUCCESS != result ) {
			printf( "Could not create fence\n" );
			return false;
		}

		return true;
	}

	bool WaitForFences( Array<VkFence>& fences, const bool waitForAll, const uint64_t timeout ) {

		if( fences.GetSize() == 0 ) {
			return true;
		}

		const VkBool32 waitFlag = waitForAll ? VK_TRUE : VK_FALSE;

		const VkResult result = vkWaitForFences( device, fences.GetSize(), fences.GetData(),
												 waitFlag, timeout );
		if( VK_SUCCESS != result ) {
			printf( "Waiting on fences failed\n" );
			return false;
		}

		return true;
	}

	bool ResetFences( Array<VkFence>& fences ) {

		if( fences.GetSize() == 0 ) {
			return true;
		}

		const VkResult result = vkResetFences( device, fences.GetSize(), fences.GetData() );
		if( VK_SUCCESS != result ) {
			printf( "Could not reset fences\n" );
			return false;
		}

		return true;
	}
	
	void DestroyFence( VkFence& fence ) {
		if( fence != VK_NULL_HANDLE ) {
			vkDestroyFence( device, fence, nullptr );
			fence = VK_NULL_HANDLE;
		}
	}
	
	void DestroySemaphore( VkSemaphore& semaphore ) {
		if( semaphore != VK_NULL_HANDLE ) {
			vkDestroySemaphore( device, semaphore, nullptr );
			semaphore = VK_NULL_HANDLE;
		}
	}
}