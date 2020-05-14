// ReSharper disable CppParameterMayBeConst
#include "stdafx.h"
#include "Initializer.h"
#include "../Vulkan.h"

namespace axlt::vk::init {

	VkFormat GetTextureFormat( const Array<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features ) {
		for (VkFormat format : candidates) {
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties( physicalDevice, format, &props );

			if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
				return format;
			}
			if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
				return format;
			}
		}
		throw 0;
	}

	bool SelectPresentMode( VkPresentModeKHR& targetPresentMode ) {
		uint32_t presentModesCount;
		VkResult result = vkGetPhysicalDeviceSurfacePresentModesKHR( physicalDevice, presentationSurface,
																	 &presentModesCount, nullptr );
		if( VK_SUCCESS != result || presentModesCount == 0 ) {
			printf( "Could not get number of available present modes\n" );
			return false;
		}

		Array<VkPresentModeKHR> availablePresentModes;
		availablePresentModes.AddEmpty( presentModesCount );

		result = vkGetPhysicalDeviceSurfacePresentModesKHR( physicalDevice, presentationSurface,
															&presentModesCount, availablePresentModes.GetData() );
		if( VK_SUCCESS != result ) {
			printf( "Could not get available present modes\n" );
			return false;
		}

		for( VkPresentModeKHR& presentMode : availablePresentModes ) {
			if( presentMode == targetPresentMode ) {
				return true;
			}
		}

		targetPresentMode = VK_PRESENT_MODE_FIFO_KHR;
		return true;
	}

	bool SelectSurfaceFormat( VkSurfaceFormatKHR& targetSurfaceFormat ) {
		uint32_t surfaceFormatCount;
		VkResult result = vkGetPhysicalDeviceSurfaceFormatsKHR( physicalDevice, presentationSurface,
																&surfaceFormatCount, nullptr );
		if( VK_SUCCESS != result || surfaceFormatCount == 0 ) {
			printf( "Could not get number of surface formats\n" );
			return false;
		}

		Array<VkSurfaceFormatKHR> surfaceFormats;
		surfaceFormats.AddEmpty( surfaceFormatCount );

		result = vkGetPhysicalDeviceSurfaceFormatsKHR( physicalDevice, presentationSurface,
													   &surfaceFormatCount, surfaceFormats.GetData() );
		if( VK_SUCCESS != result ) {
			printf( "Could not get number of surface formats\n" );
			return false;
		}

		if( surfaceFormats.GetSize() == 1 && ( VK_FORMAT_UNDEFINED == surfaceFormats[0].format ) ) {
			return true;
		}

		for( VkSurfaceFormatKHR& surfaceFormat : surfaceFormats ) {
			if( surfaceFormat.format == targetSurfaceFormat.format &&
				surfaceFormat.colorSpace == targetSurfaceFormat.colorSpace ) {
				return true;
			}
		}
		for( VkSurfaceFormatKHR& surfaceFormat : surfaceFormats ) {
			if( surfaceFormat.format == targetSurfaceFormat.format ) {
				targetSurfaceFormat.colorSpace = surfaceFormat.colorSpace;
				return true;
			}
		}

		targetSurfaceFormat = surfaceFormats[0];

		return true;
	}

	bool SetupPresentationSurface( HINSTANCE hInstance, HWND hWnd ) {

		VkWin32SurfaceCreateInfoKHR surfaceCreateInfo{
			VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
			nullptr,
			0,
			hInstance,
			hWnd
		};

		presentationSurface = VK_NULL_HANDLE;

		const VkResult result = vkCreateWin32SurfaceKHR( instance, &surfaceCreateInfo, nullptr, &presentationSurface );
		if( VK_SUCCESS != result || VK_NULL_HANDLE == presentationSurface ) {
			printf( "Could not create presentation surface\n" );
			return false;
		}

		return true;
	}

	bool SetupSwapchain( uint32_t width, uint32_t height ) {
		VkPresentModeKHR targetPresentMode = VK_PRESENT_MODE_MAILBOX_KHR; //TODO: Create config

		if( !SelectPresentMode( targetPresentMode ) ) {
			return false;
		}

		VkSurfaceCapabilitiesKHR surfaceCapabilities;
		VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR( physicalDevice,
																	 presentationSurface,
																	 &surfaceCapabilities );

		if( VK_SUCCESS != result ) {
			printf( "Could not get presentation surface capabilities\n" );
			return false;
		}

		uint32_t numberOfImages = surfaceCapabilities.minImageCount + 1;
		if( surfaceCapabilities.maxImageCount > 0 && surfaceCapabilities.maxImageCount < numberOfImages ) {
			numberOfImages = surfaceCapabilities.maxImageCount;
		}

		if( surfaceCapabilities.currentExtent.width == 0xFFFFFFFF ) {
			swapchainExtents.width = Clamp( surfaceCapabilities.minImageExtent.width,
										surfaceCapabilities.maxImageExtent.width,
										width );

			swapchainExtents.height = Clamp( surfaceCapabilities.minImageExtent.height,
										 surfaceCapabilities.maxImageExtent.height,
										 height );
		} else {
			swapchainExtents = surfaceCapabilities.currentExtent;
		}

		const VkImageUsageFlags desiredUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; //TODO: Create config

		if( ( desiredUsage & surfaceCapabilities.supportedUsageFlags ) != desiredUsage ) {
			return false;
		}

		VkSurfaceTransformFlagBitsKHR surfaceTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR; //TODO: Create config
		if( ( surfaceTransform & surfaceCapabilities.supportedTransforms ) == 0 ) {
			surfaceTransform = surfaceCapabilities.currentTransform;
		}

		surfaceFormat = { //TODO: Create config
			VK_FORMAT_R8G8B8A8_UNORM,
			VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
		};

		if( !SelectSurfaceFormat( surfaceFormat ) ) {
			return false;
		}

		const VkSwapchainKHR oldSwapchain = swapchain;

		VkSwapchainCreateInfoKHR swapchainCreateInfo = {
			VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
			nullptr,
			0,
			presentationSurface,
			numberOfImages,
			surfaceFormat.format,
			surfaceFormat.colorSpace,
			swapchainExtents,
			1,
			desiredUsage,
			VK_SHARING_MODE_EXCLUSIVE,
			0,
			nullptr,
			surfaceTransform,
			VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			targetPresentMode,
			VK_TRUE,
			oldSwapchain
		};

		result = vkCreateSwapchainKHR( device, &swapchainCreateInfo, nullptr, &swapchain );
		if( VK_SUCCESS != result ) {
			printf( "Could not create swapchain\n" );
			return false;
		}

		if( oldSwapchain != VK_NULL_HANDLE ) {
			vkDestroySwapchainKHR( device, oldSwapchain, nullptr );
		}

		uint32_t swapchainImagesCount;
		result = vkGetSwapchainImagesKHR( device, swapchain, &swapchainImagesCount, nullptr );
		if( VK_SUCCESS != result || swapchainImagesCount == 0 ) {
			printf( "Could not get the number of swapchain images\n" );
			return false;
		}

		swapchainImages.Clear();
		swapchainImages.AddEmpty( swapchainImagesCount );

		result = vkGetSwapchainImagesKHR( device, swapchain, &swapchainImagesCount, swapchainImages.GetData() );
		if( VK_SUCCESS != result ) {
			printf( "Could not get swapchain images\n" );
			return false;
		}

		swapchainImageViews.Clear();
		swapchainImageViews.AddEmpty( swapchainImagesCount );

		for( uint32_t i = 0; i < swapchainImages.GetSize(); i++ ) {
			VkImageViewCreateInfo imageViewCreateInfo = {
				VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
				nullptr,
				0,
				swapchainImages[i],
				VK_IMAGE_VIEW_TYPE_2D,
				surfaceFormat.format,
				{
					VK_COMPONENT_SWIZZLE_IDENTITY,
					VK_COMPONENT_SWIZZLE_IDENTITY,
					VK_COMPONENT_SWIZZLE_IDENTITY,
					VK_COMPONENT_SWIZZLE_IDENTITY
				},
				{
					VK_IMAGE_ASPECT_COLOR_BIT,
					0,
					1,
					0,
					1
				}
			};
			
			result = vkCreateImageView( device, &imageViewCreateInfo, nullptr, &swapchainImageViews[i] );
			if( VK_SUCCESS != result ) {
				printf( "Could not create swapchain images views\n" );
				return false;
			}
		}

		depthTextureFormat = GetTextureFormat(
			{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
		);
		
		VkImageCreateInfo depthImageCreateInfo = {
			VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
			nullptr,
			0,
			VK_IMAGE_TYPE_2D,
			depthTextureFormat,
			{
				swapchainExtents.width,
				swapchainExtents.height,
				1
			},
			1,
			1,
			VK_SAMPLE_COUNT_1_BIT,
			VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
			VK_SHARING_MODE_EXCLUSIVE,
			0,
			nullptr,
			VK_IMAGE_LAYOUT_UNDEFINED
		};

		result = vkCreateImage( device, &depthImageCreateInfo, nullptr, &depthImage );
		if (VK_SUCCESS != result) {
			printf( "Could not create depth image\n" );
			return false;
		}

		VkMemoryRequirements memoryRequirements;
		vkGetImageMemoryRequirements( device, depthImage, &memoryRequirements );

		if (!AllocateMemory( memoryRequirements, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			&depthImageMemory )) {
			return false;
		}

		result = vkBindImageMemory( device, depthImage, depthImageMemory, 0 );
		if (result != VK_SUCCESS) {
			printf( "Could not bind depth image memory\n" );
			return false;
		}

		VkImageViewCreateInfo imageViewCreateInfo = {
			VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			nullptr,
			0,
			depthImage,
			VK_IMAGE_VIEW_TYPE_2D,
			depthTextureFormat,
			{
				VK_COMPONENT_SWIZZLE_IDENTITY,
				VK_COMPONENT_SWIZZLE_IDENTITY,
				VK_COMPONENT_SWIZZLE_IDENTITY,
				VK_COMPONENT_SWIZZLE_IDENTITY
			},
			{
				VK_IMAGE_ASPECT_DEPTH_BIT,
				0,
				1,
				0,
				1
			}
		};

		result = vkCreateImageView( device, &imageViewCreateInfo, nullptr, &depthImageView );
		if (result != VK_SUCCESS) {
			printf( "Could not create depth image view\n" );
			return false;
		}

		return true;
	}

	bool CreateFramebuffers() {
		framebuffers.Clear();
		framebuffers.AddEmpty( swapchainImages.GetSize() );

		for( uint32_t i = 0; i < swapchainImages.GetSize(); i++ ) {
			FixedArray<VkImageView, 2> attachments = {
				swapchainImageViews[ i ],
				depthImageView
			};

			VkFramebufferCreateInfo framebufferCreateInfo = {
				VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
				nullptr,
				0,
				renderPass,
				attachments.GetSize(),
				attachments.GetData(),
				swapchainExtents.width,
				swapchainExtents.height,
				1
			};

			const VkResult result = vkCreateFramebuffer( device, &framebufferCreateInfo, nullptr, &framebuffers[i] );
			if( VK_SUCCESS != result ) {
				printf( "Could not create framebuffer\n" );
				return false;
			}
		}

		return true;
	}
}