#include "stdafx.h"
#include "Initializer.h"
#include "../Vulkan.h"

namespace axlt::vk::init {
	bool IsExtensionSupported( const char* extension, const Array<VkExtensionProperties>& availableExtensions ) {
		for( const VkExtensionProperties& property : availableExtensions ) {
			if( strcmp( extension, property.extensionName ) == 0 ) {
				return true;
			}
		}
		return false;
	}

	bool CheckInstanceExtensionAvailability( const Array<const char*>& desiredExtensions ) {
		if( desiredExtensions.GetSize() == 0 ) {
			return true;
		}
		
		uint32_t extensionsCount;
		VkResult result = vkEnumerateInstanceExtensionProperties( nullptr, &extensionsCount, nullptr );
		if( VK_SUCCESS != result || extensionsCount == 0 ) {
			printf("Could not get the number of instance extensions\n" );
			return false;
		}

		Array<VkExtensionProperties> availableExtensions;
		availableExtensions.AddEmpty( extensionsCount );

		result = vkEnumerateInstanceExtensionProperties( nullptr, &extensionsCount, availableExtensions.GetData() );
		if( VK_SUCCESS != result ) {
			printf("Could not get instance extensions\n" );
			return false;
		}

		for( const char* extension : desiredExtensions ) {
			if( !IsExtensionSupported( extension, availableExtensions ) ) {
				printf("Unsupported extension %s\n", extension );
				return false;
			}
		}

		return true;
	}
	
	bool CheckPhysicalDeviceExtensions( const Array<const char*>& desiredExtensions ) {
		if( desiredExtensions.GetSize() == 0 ) {
			return true;
		}

		uint32_t extensionCount;
		VkResult result = vkEnumerateDeviceExtensionProperties( physicalDevice, nullptr, &extensionCount, nullptr );
		if( VK_SUCCESS != result || extensionCount == 0 ) {
			printf( "Could not get the number of physical device extensions\n" );
			return false;
		}

		Array<VkExtensionProperties> physicalDeviceExtensions;
		physicalDeviceExtensions.AddEmpty( extensionCount );
		
		result = vkEnumerateDeviceExtensionProperties( physicalDevice, nullptr, &extensionCount, physicalDeviceExtensions.GetData() );
		if( VK_SUCCESS != result ) {
			printf( "Could not get physical device extensions\n" );
			return false;
		}
		
		for( const char* extension : desiredExtensions ) {
			if( !IsExtensionSupported( extension, physicalDeviceExtensions ) ) {
				printf("Unsupported extension %s\n", extension );
				return false;
			}
		}

		return true;
	}
}
