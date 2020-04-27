#pragma once
// ReSharper disable once CppUnusedIncludeDirective
#include "Collections/Array.h"
#include "../Vulkan.h"
#include <vulkan/vulkan.h>

namespace axlt { namespace vk { namespace init {

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
	bool QueueFamilyIndexSupportsPresentation( uint32_t queueFamilyIndex );
	bool GetPresentationQueueFamilyIndex( uint32_t& queueFamilyIndex );

	bool AcquireNextImage( VkSemaphore semaphore, VkFence fence, uint32_t& imageIndex );
	bool PresentImage( VkQueue queue,
					   Array<VkSemaphore> renderingSemaphores,
					   Array<VkSwapchainKHR>& swapchains );
} } }
