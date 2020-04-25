// ReSharper disable CppUnusedIncludeDirective
#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
// ReSharper disable once CppUnusedIncludeDirective
#include "vulkan/vulkan.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "VulkanFunctions.h"
#include "Buffers.h"
#include "Commands.h"
#include "Descriptors.h"
#include "Drawing.h"
#include "Images.h"
#include "Memory.h"
#include "Pipelines.h"
#include "PipelineUpdates.h"
#include "RenderPasses.h"
#include "Shaders.h"
#include "Synchronization.h"
#include "Collections/Array.h"

namespace axlt::vk {

	extern HMODULE library;
	extern VkInstance instance;
	extern VkPhysicalDevice physicalDevice;
	extern VkPhysicalDeviceProperties deviceProperties;
	extern Array<VkQueueFamilyProperties> queueFamilyProperties;
	extern VkDevice device;
	extern Array<Array<VkQueue>> queues;
	extern VkSurfaceKHR presentationSurface;
	extern VkQueue presentationQueue;
	extern VkSwapchainKHR swapchain;
	extern VkSurfaceFormatKHR surfaceFormat;
	extern Array<VkImage> swapchainImages;
	extern Array<VkImageView> swapchainImageViews;
	extern VkCommandPool commandPool;
	extern Array<VkCommandBuffer> commandBuffers;
	extern VkRenderPass renderPass;
	extern VkDescriptorPool descriptorPool;

	bool Initialize( HINSTANCE hInstance, HWND hWnd, uint32_t width, uint32_t height );
	void Update();
	void Shutdown();
}
