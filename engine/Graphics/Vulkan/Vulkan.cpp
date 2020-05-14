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
	VkFormat depthTextureFormat;
	VkImage depthImage;
	VkDeviceMemory depthImageMemory;
	VkImageView depthImageView;
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
}
