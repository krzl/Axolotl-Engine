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
	VkQueue presentationQueue = VK_NULL_HANDLE;
	VkSwapchainKHR swapchain = VK_NULL_HANDLE;
	VkSurfaceFormatKHR surfaceFormat;
	Array<VkImage> swapchainImages;
	Array<VkImageView> swapchainImageViews;
	VkCommandPool commandPool;
	Array<VkCommandBuffer> commandBuffers;
	VkRenderPass renderPass;
	VkDescriptorPool descriptorPool;

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

		if( !init::SetupPresentationSurface( hInstance, hWnd ) ) {
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

		if( !init::CreateLogicalDevice( desiredDeviceExtensions, desiredFeatures, queueCreateInfos ) ) {
			return false;
		}

		if( !CreateCommandPool( 0, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, commandPool ) ) {
			return false;
		}

		if( !init::SetupSwapchain( width, height ) ) {
			return false;
		}

		Array<VkAttachmentDescription> attachments;
		attachments.Add(
			VkAttachmentDescription{
				0,
				surfaceFormat.format,
				VK_SAMPLE_COUNT_1_BIT,
				VK_ATTACHMENT_LOAD_OP_CLEAR,
				VK_ATTACHMENT_STORE_OP_STORE,
				VK_ATTACHMENT_LOAD_OP_CLEAR,
				VK_ATTACHMENT_STORE_OP_DONT_CARE,
				VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
				VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
			}
		);

		Array<SubpassParameters> subpassParameters;
		subpassParameters.AddEmpty( 1 );
		subpassParameters[0].pipelineType = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassParameters[0].colorAttachments = Array<VkAttachmentReference> {
			VkAttachmentReference {
				0,
				VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
			}
		};
		
		Array<VkSubpassDependency> subpassDependencies;

		if( !CreateRenderPass( attachments, subpassParameters, subpassDependencies, renderPass ) ) {
			return false;
		}

		Array<VkDescriptorPoolSize> poolSizes;
		poolSizes.Add(
			VkDescriptorPoolSize{
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			64
			}
		);
		if( !CreateDescriptorPool( poolSizes, 64, descriptorPool ) ) {
			return false;
		}

		if( !AllocateCommandBuffers( commandPool, true, 3, commandBuffers ) ) {
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
