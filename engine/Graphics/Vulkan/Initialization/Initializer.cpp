#include "stdafx.h"
#include "Initializer.h"
#include "../Vulkan.h"

namespace axlt::vk::init {

	bool CreateInstance( const Array<const char*>& desiredExtensions ) {

		if( !CheckInstanceExtensionAvailability( desiredExtensions ) ) {
			return false;
		}

		VkApplicationInfo applicationInfo{
			VK_STRUCTURE_TYPE_APPLICATION_INFO,
			nullptr,
			"axlt Test", //TODO: Create config
			VK_MAKE_VERSION( 1, 0, 0 ), //TODO: Create config
			"axlt Engine",
			VK_MAKE_VERSION( 1, 0, 0 ), //TODO: Create config
			VK_MAKE_VERSION( 1, 2, 0 )
		};

		VkInstanceCreateInfo createInfo{
			VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			nullptr,
			0,
			&applicationInfo,
			0,
			nullptr,
			desiredExtensions.GetSize(),
			desiredExtensions.GetData()
		};

		const VkResult result = vkCreateInstance( &createInfo, nullptr, &instance );
		if( VK_SUCCESS != result ) {
			printf( "Could not create vulkan instance\n" );
			return false;
		}

		if( !LoadInstanceFunctions( desiredExtensions ) ) {
			return false;
		}

		return true;
	}

	bool CreateLogicalDevice( const Array<const char*>& desiredExtensions,
							  const VkPhysicalDeviceFeatures& desiredFeatures,
							  const Array<QueueCreateInfo>& queueCreateInfos ) {
		if( !SetupPhysicalDevice( desiredExtensions, desiredFeatures ) ) {
			return false;
		}

		Array<VkDeviceQueueCreateInfo> deviceQueueCreateInfo;
		deviceQueueCreateInfo.AddEmpty( queueCreateInfos.GetSize() );

		uint32_t presentQueueIndex = 0xFFFFFFFF;
		for( uint32_t i = 0; i < queueCreateInfos.GetSize(); i++ ) {
			uint32_t familyIndex;
			if( !GetQueueFamilyIndex( queueCreateInfos[i].flags, familyIndex ) ) {
				return false;
			}

			deviceQueueCreateInfo[i] = VkDeviceQueueCreateInfo{
				VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
				nullptr,
				0,
				familyIndex,
				queueCreateInfos[i].priorities.GetSize(),
				queueCreateInfos[i].priorities.GetData()
			};

			if( presentQueueIndex == 0xFFFFFFFF && ( queueCreateInfos[i].flags & VK_QUEUE_GRAPHICS_BIT ) > 0 ) {
				if( QueueFamilyIndexSupportsPresentation( i ) ) {
					presentQueueIndex = i;
				}
			}
		}

		if( presentQueueIndex == 0xFFFFFFFF ) {
			if( !GetPresentationQueueFamilyIndex( presentQueueIndex ) ) {
				return false;
			}
			float presentQueuePriority = 1.0f;
			deviceQueueCreateInfo.Add(
				VkDeviceQueueCreateInfo{
				VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
				nullptr,
				0,
				presentQueueIndex,
				1,
				&presentQueuePriority
				}
			);
			presentQueueIndex = deviceQueueCreateInfo.GetSize() - 1;
		}

		VkDeviceCreateInfo deviceCreateInfo{
			VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
			nullptr,
			0,
			deviceQueueCreateInfo.GetSize(),
			deviceQueueCreateInfo.GetData(),
			0,
			nullptr,
			desiredExtensions.GetSize(),
			desiredExtensions.GetData(),
			&desiredFeatures
		};

		const VkResult result = vkCreateDevice( physicalDevice, &deviceCreateInfo, nullptr, &device );
		if( VK_SUCCESS != result ) {
			printf( "Could not create logical device\n" );
			return false;
		}

		if( !LoadDeviceFunctions( desiredExtensions ) ) {
			return false;
		}

		queues.AddEmpty( deviceQueueCreateInfo.GetSize() );
		for( uint32_t i = 0; i < deviceQueueCreateInfo.GetSize(); i++ ) {
			queues[i] = Array<VkQueue>();
			queues[i].AddEmpty( deviceQueueCreateInfo[i].queueCount );
			for( uint32_t j = 0; j < deviceQueueCreateInfo[i].queueCount; j++ ) {
				vkGetDeviceQueue( device, deviceQueueCreateInfo[i].queueFamilyIndex, j, &queues[i][j] );
			}
		}

		vkGetDeviceQueue( device, deviceQueueCreateInfo[presentQueueIndex].queueFamilyIndex, 0, &presentationQueue );

		return true;
	}
}
