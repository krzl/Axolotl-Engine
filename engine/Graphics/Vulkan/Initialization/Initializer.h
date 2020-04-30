#pragma once
// ReSharper disable once CppUnusedIncludeDirective
#include "../Vulkan.h"
#include "Collections/Array.h"
#include <vulkan/vulkan.h>

namespace axlt::vk::init {

	struct QueueCreateInfo {
		VkQueueFlags flags;
		Array<float> priorities;
	};

	bool LoadGlobalFunctions();
	bool CheckInstanceExtensionAvailability( const Array<const char*>& desiredExtensions );

	bool CreateInstance( const Array<const char*>& desiredExtensions );

	bool LoadInstanceFunctions( const Array<const char*>& enabledExtensions );

	bool SetupPhysicalDevice( const Array<const char*>& desiredExtensions,
							  const VkPhysicalDeviceFeatures& desiredFeatures );
	bool CheckPhysicalDeviceExtensions( const Array<const char*>& desiredExtensions );
	bool GetQueueFamilyIndex( VkQueueFlags desiredCapabilities, uint32_t& queueFamilyIndex );

	bool CreateLogicalDevice( const Array<const char*>& desiredExtensions,
							  const VkPhysicalDeviceFeatures& desiredFeatures,
							  const Array<QueueCreateInfo>& queueCreateInfos );

	bool LoadDeviceFunctions( const Array<const char*>& enabledExtensions );

	bool SetupPresentationSurface( HINSTANCE hInstance, HWND hWnd );
	bool SetupSwapchain( uint32_t width, uint32_t height );
	bool CreateFramebuffers();
	bool CreateCommandPool( uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags, VkCommandPool& commandPool );
	bool CreateRenderPass( const Array<VkAttachmentDescription>& attachmentDescriptions, 
						   const Array<VkSubpassDescription>& subpassDescriptions,
						   const Array<VkSubpassDependency>& subpassDependencies, VkRenderPass& renderPass );
	bool AllocateCommandBuffers( VkCommandPool commandPool, bool primary, uint32_t bufferCount,
								 Array<VkCommandBuffer>& commandBuffers );
	bool QueueFamilyIndexSupportsPresentation( uint32_t queueFamilyIndex );
	bool GetPresentationQueueFamilyIndex( uint32_t& queueFamilyIndex );
}