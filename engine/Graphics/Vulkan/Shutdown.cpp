#include "stdafx.h"
#include "Vulkan.h"

#include "Bindings/VulkanBinding.h"

namespace axlt::vk {

	void Shutdown() {
		vkDeviceWaitIdle( device );

		VulkanBinding::DestroyAll();

		vkDestroyRenderPass( device, renderPass, nullptr );

		vkDestroySemaphore( device, imageAvailableSemaphore, nullptr );
		vkDestroySemaphore( device,  renderingFinishedSemaphore, nullptr );

		vkDestroyCommandPool( device, copyCommandPool, nullptr );
		vkDestroyCommandPool( device, commandPool, nullptr );

		for( uint32_t i = 0; i < swapchainImages.GetSize(); i++ ) {
			vkDestroyBuffer( device, perCameraBuffers[i], nullptr );
			vkFreeMemory( device, perCameraBuffersMemory[i], nullptr );
			vkDestroyFence( device, renderFences[i], nullptr );
			vkDestroyFramebuffer( device, framebuffers[i], nullptr );
			vkDestroyImageView( device, swapchainImageViews[i], nullptr );
		}

		vkDestroyImageView( device, depthImageView, nullptr );
		vkFreeMemory( device, depthImageMemory, nullptr );
		vkDestroyImage( device, depthImage, nullptr );
		
		if( swapchain ) {
			vkDestroySwapchainKHR( device, swapchain, nullptr );
			swapchain = VK_NULL_HANDLE;
		}

		if( presentationSurface ) {
			vkDestroySurfaceKHR( instance, presentationSurface, nullptr );
			presentationSurface = VK_NULL_HANDLE;
		}

		if( device ) {
			vkDestroyDevice( device, nullptr );
			device = VK_NULL_HANDLE;
		}

		if( instance ) {
			vkDestroyInstance( instance, nullptr );
			instance = VK_NULL_HANDLE;
		}

		FreeLibrary( library );
		library = nullptr;
	}
}
