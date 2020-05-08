// ReSharper disable CppZeroConstantCanBeReplacedWithNullptr
#include "stdafx.h"
#include "Vulkan.h"
#include "Initialization/Initializer.h"
#include "Graphics/TechniqueResource.h"

namespace axlt::vk {

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
		desiredFeatures.depthClamp = VK_TRUE;

		Array<init::QueueCreateInfo> queueCreateInfos = { // TODO: Create config
			init::QueueCreateInfo{
				VK_QUEUE_GRAPHICS_BIT,
				Array<float> { 1.0f }
			}
		};

		if( !CreateLogicalDevice( desiredDeviceExtensions, desiredFeatures, queueCreateInfos ) ) {
			return false;
		}

		if( !init::CreateCommandPool( 0, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, commandPool ) ) {
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

		VkAttachmentReference subpassColorAttachment = {
			0,
			VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
		};
		
		Array<VkSubpassDescription> subpassDescriptions = {
			VkSubpassDescription {
				0,
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				0,
				nullptr,
				1,
				&subpassColorAttachment,
				nullptr,
				nullptr,
				0,
				nullptr
			}
		};

		Array<VkSubpassDependency> subpassDependencies;

		if( !init::CreateRenderPass( attachments, subpassDescriptions, subpassDependencies, renderPass ) ) {
			return false;
		}

		if( !init::AllocateCommandBuffers( commandPool, true, 3, commandBuffers ) ) {
			return false;
		}

		if( !init::CreateFramebuffers() ) {
			return false;
		}

		perCameraBuffers.AddEmpty( commandBuffers.GetSize() );
		perCameraBuffersMemory.AddEmpty( commandBuffers.GetSize() );

		vkGetPhysicalDeviceMemoryProperties( physicalDevice, &deviceMemoryProperties );

		for( uint32_t i = 0; i < commandBuffers.GetSize(); i++ ) {
			VkBufferCreateInfo bufferCreateInfo = {
				VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
				nullptr,
				0,
				sizeof( PerCameraUniformBuffer ),
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_SHARING_MODE_EXCLUSIVE,
				0,
				nullptr
			};

			VkResult result = vkCreateBuffer( device, &bufferCreateInfo, nullptr, &perCameraBuffers[i] );
			if( VK_SUCCESS != result ) {
				printf( "Could not create buffer\n" );
				return false;
			}


			VkMemoryRequirements memoryRequirements;
			vkGetBufferMemoryRequirements( device, perCameraBuffers[i], &memoryRequirements );

			if( !AllocateMemory( memoryRequirements, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &perCameraBuffersMemory[i] ) ) {
				return false;
			}

			result = vkBindBufferMemory( device, perCameraBuffers[i], perCameraBuffersMemory[i], 0 );
			if( VK_SUCCESS != result ) {
				printf( "Could not bind memory to buffer\n" );
				return false;
			}
		}

		VkSemaphoreCreateInfo semaphoreCreateInfo = {
			VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
			nullptr,
			0
		};

		VkResult result = vkCreateSemaphore( device, &semaphoreCreateInfo, nullptr, &imageAvailableSemaphore );
		if( VK_SUCCESS != result ) {
			printf( "Could not create semaphore\n" );
			return false;
		}
		result = vkCreateSemaphore( device, &semaphoreCreateInfo, nullptr, &renderingFinishedSemaphore );
		if( VK_SUCCESS != result ) {
			printf( "Could not create semaphore\n" );
			return false;
		}

		VkFenceCreateInfo fenceCreateInfo = {
			VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
			nullptr,
			VK_FENCE_CREATE_SIGNALED_BIT
		};

		renderFences.AddEmpty( commandBuffers.GetSize() );

		for( uint32_t i = 0; i < commandBuffers.GetSize(); i++ ) {
			result = vkCreateFence( device, &fenceCreateInfo, nullptr, &renderFences[i] );
			if( VK_SUCCESS != result ) {
				printf( "Could not create fence\n" );
				return false;
			}
		}

		return true;
	}

	bool AllocateMemory( const VkMemoryRequirements& memoryRequirements, const VkMemoryPropertyFlagBits memoryProperties,
						 VkDeviceMemory* memoryObject ) {

		for( uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; i++ ) {
			if( memoryRequirements.memoryTypeBits & ( 1 << i ) &&
				( deviceMemoryProperties.memoryTypes[i].propertyFlags & memoryProperties ) == memoryProperties ) {

				VkMemoryAllocateInfo memoryAllocateInfo = {
					VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
					nullptr,
					memoryRequirements.size,
					i
				};

				const VkResult result = vkAllocateMemory( device, &memoryAllocateInfo, nullptr, memoryObject );
				if( VK_SUCCESS == result && memoryObject != VK_NULL_HANDLE ) {
					return true;
				}
			}
		}

		printf( "Could not allocate memory\n" );
		return false;
	}
}
