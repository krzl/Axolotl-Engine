#include "stdafx.h"
#include "Initializer.h"
#include "../Vulkan.h"
#include "Collections/BitArray.h"

namespace axlt::vk::init {

	Array<VkQueueFamilyProperties> queueFamilyProperties;
	BitArray reservedQueueFamilies;

	bool CheckDesiredFeatures( const VkPhysicalDeviceFeatures& desiredFeatures,
							   const VkPhysicalDeviceFeatures& availableFeatures ) {
		const VkBool32* desiredFeaturesArray = reinterpret_cast<const VkBool32*>( &desiredFeatures );
		const VkBool32* deviceFeaturesArray = reinterpret_cast<const VkBool32*>( &availableFeatures );

		constexpr uint32_t arraySize = sizeof( VkPhysicalDeviceFeatures ) / sizeof( VkBool32 );

		for( uint32_t i = 0; i < arraySize; i++ ) {
			if( desiredFeaturesArray[i] == VK_TRUE && deviceFeaturesArray[i] == VK_FALSE ) {
				return false;
			}
		}
		return true;
	}

	bool SetupPhysicalDevice( const Array<const char*>& desiredExtensions, 
							  const VkPhysicalDeviceFeatures& desiredFeatures ) {
		
		uint32_t devicesCount;
		VkResult result = vkEnumeratePhysicalDevices( instance, &devicesCount, nullptr );
		if( VK_SUCCESS != result || devicesCount == 0 ) {
			printf( "Could not get the number of physical devices\n" );
			return false;
		}

		Array<VkPhysicalDevice> physicalDevices;
		physicalDevices.AddEmpty( devicesCount );
		result = vkEnumeratePhysicalDevices( instance, &devicesCount, physicalDevices.GetData() );
		if( VK_SUCCESS != result ) {
			printf( "Could not get physical devices\n" );
			return false;
		}

		// Always take first device, at least for now
		physicalDevice = physicalDevices[0];

		if( !CheckPhysicalDeviceExtensions( desiredExtensions ) ) {
			return false;
		}

		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceFeatures( physicalDevice, &deviceFeatures );
		vkGetPhysicalDeviceProperties( physicalDevice, &deviceProperties );

		if( !CheckDesiredFeatures( desiredFeatures, deviceFeatures ) ) {
			printf( "Desired features are not available on this device\n" );
			return false;
		}

		uint32_t queueFamiliesCount;
		vkGetPhysicalDeviceQueueFamilyProperties( physicalDevice, &queueFamiliesCount, nullptr );
		if( queueFamiliesCount == 0 ) {
			printf( "Could not get queue families count\n" );
			return false;
		}

		queueFamilyProperties.Clear();
		queueFamilyProperties.AddEmpty( queueFamiliesCount );
		reservedQueueFamilies.Clear();
		for( uint32_t i = 0; i < queueFamiliesCount; i++ ) {
			reservedQueueFamilies.Add( false );
		}

		vkGetPhysicalDeviceQueueFamilyProperties( physicalDevice, &queueFamiliesCount, queueFamilyProperties.GetData() );

		return true;
	}

	bool GetQueueFamilyIndex( const VkQueueFlags desiredCapabilities, uint32_t& queueFamilyIndex ) {
		for( uint32_t i = 0; i < queueFamilyProperties.GetSize(); i++ ) {
			if( queueFamilyProperties[i].queueCount > 0 &&
				queueFamilyProperties[i].queueFlags & desiredCapabilities &&
				!reservedQueueFamilies[i] ) {

				reservedQueueFamilies[i] = true;
				queueFamilyIndex = i;
				return true;
			}
		}
		printf( "Could not get queue family index\n" );
		return false;
	}

	bool GetPresentationQueueFamilyIndex( uint32_t& queueFamilyIndex ) {
		VkBool32 presentationSupported = VK_FALSE;
		for( uint32_t i = 0; i < queueFamilyProperties.GetSize(); i++ ) {
			const VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR( physicalDevice, i, presentationSurface, &presentationSupported );
			if( result == VK_SUCCESS &&
				presentationSupported == VK_TRUE &&
				!reservedQueueFamilies[i] ) {

				reservedQueueFamilies[i] = true;
				queueFamilyIndex = i;
				return true;
			}
		}
		printf( "Could not get presentation queue family index\n" );
		return false;
	}
}
