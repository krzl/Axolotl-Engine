// ReSharper disable CppZeroConstantCanBeReplacedWithNullptr
#include "stdafx.h"
#include "Vulkan.h"
#include "Initialization/Initializer.h"

namespace axlt::vk {

	HMODULE library;
	VkInstance instance;
	VkPhysicalDevice physicalDevice;
	VkPhysicalDeviceProperties deviceProperties;
	VkDevice device;
	Array<Array<VkQueue>> queues;
	VkSurfaceKHR presentationSurface;
	VkSwapchainKHR swapchain = VK_NULL_HANDLE;
	Array<VkImage> swapchainImages;

	// ReSharper disable CppParameterMayBeConst
	bool Initialize( HINSTANCE hInstance, HWND hWnd, uint32_t width, uint32_t height ) {
		// ReSharper restore CppParameterMayBeConst
		library = LoadLibrary( "vulkan-1.dll" );

		if( nullptr == library ) {
			return false;
		}

		if( !init::LoadGlobalFunctions() ) {
			return false;
		}

		Array<const char*> desiredExtensions; //TODO: Create config
		desiredExtensions.Add( VK_KHR_WIN32_SURFACE_EXTENSION_NAME );
		desiredExtensions.Add( VK_KHR_SURFACE_EXTENSION_NAME );

		if( !init::CreateInstance( desiredExtensions ) ) {
			return false;
		}

		Array<const char*> desiredDeviceExtensions; //TODO: Create config
		desiredDeviceExtensions.Add( VK_KHR_SWAPCHAIN_EXTENSION_NAME );

		VkPhysicalDeviceFeatures desiredFeatures = {}; //TODO: Create config
		desiredFeatures.geometryShader = VK_TRUE;

		Array<init::QueueCreateInfo> queueCreateInfos; // TODO: Create config
		
		queueCreateInfos.Emplace(
			init::QueueCreateInfo{
			VK_QUEUE_GRAPHICS_BIT,
			Array<float> { 1.0f }
			}
		);
		queueCreateInfos.Emplace(
			init::QueueCreateInfo{
				VK_QUEUE_COMPUTE_BIT,
				Array<float> { 1.0f }
			}
		);

		if( !init::CreateLogicalDevice( desiredDeviceExtensions, desiredFeatures, queueCreateInfos ) ) {
			return false;
		}

		if( !init::SetupPresentationSurface( hInstance, hWnd ) ) {
			return false;
		}

		if( !init::SetupSwapchain( width, height ) ) {
			return false;
		}

		return true;
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
