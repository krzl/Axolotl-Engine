// ReSharper disable CppZeroConstantCanBeReplacedWithNullptr
#include "stdafx.h"
#include "Vulkan.h"

namespace axlt::vk {

	HMODULE library;
	VkInstance instance;
	VkPhysicalDevice physicalDevice;
	VkPhysicalDeviceProperties deviceProperties;
	VkDevice device;
	Array<Array<VkQueue>> queues;
	Array<uint32_t> queueFamilyIndices;
	VkSurfaceKHR presentationSurface;
	VkQueue presentationQueue = VK_NULL_HANDLE;
	uint32_t presentationQueueFamilyIndex;
	VkSwapchainKHR swapchain = VK_NULL_HANDLE;
	VkExtent2D swapchainExtents;
	VkSurfaceFormatKHR surfaceFormat;
	Array<VkImage> swapchainImages;
	Array<VkImageView> swapchainImageViews;
	Array<VkFramebuffer> framebuffers;
	VkCommandPool commandPool;
	Array<VkCommandBuffer> commandBuffers;
	Array<VkFence> renderFences;
	VkRenderPass renderPass;
	VkPhysicalDeviceMemoryProperties deviceMemoryProperties;

	Array<VkBuffer> perCameraBuffers;
	Array<VkDeviceMemory> perCameraBuffersMemory;

	VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderingFinishedSemaphore;
	
	void Update() {
		BindResources();
		Draw();
	}

	void Shutdown() {
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
