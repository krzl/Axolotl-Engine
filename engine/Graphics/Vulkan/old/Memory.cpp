// ReSharper disable CppParameterMayBeConst
#include "stdafx.h"
#include "Memory.h"
#include "Collections/Map.h"

namespace axlt::vk {

	VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
	bool memoryPropertiesInitialized = false;

	bool AllocateMemory( VkMemoryRequirements memoryRequirements, 
						 VkMemoryPropertyFlagBits memoryProperties, 
						 VkDeviceMemory& memoryObject ) {

		if( !memoryPropertiesInitialized ) {
			vkGetPhysicalDeviceMemoryProperties( physicalDevice, &deviceMemoryProperties );
		}
		for( uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; i++ ) {
			if( memoryRequirements.memoryTypeBits & ( 1 << i ) &&
				( deviceMemoryProperties.memoryTypes[i].propertyFlags & memoryProperties ) == memoryProperties ) {

				VkMemoryAllocateInfo memoryAllocateInfo = {
					VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
					nullptr,
					memoryRequirements.size,
					i
				};

				const VkResult result = vkAllocateMemory( device, &memoryAllocateInfo, nullptr, &memoryObject );
				if( VK_SUCCESS == result && memoryObject != VK_NULL_HANDLE ) {
					return true;
				}
			}
		}

		printf( "Could not allocate memory\n" );
		return false;
	}

	bool MapMemory( VkDeviceMemory memoryObject, VkDeviceSize offset, VkDeviceSize size, 
					void*& dataPointer, VkMappedMemoryRange& mappedMemoryRange ) {
		const VkResult result = vkMapMemory( device, memoryObject, offset, size, 0, &dataPointer );
		if( VK_SUCCESS != result ) {
			printf( "Could not map memory\n" );
			return false;
		}

		mappedMemoryRange = VkMappedMemoryRange {
			VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
			nullptr,
			memoryObject,
			offset,
			size
		};

		return true;
	}

	bool FlushMappedMemory( Array<VkMappedMemoryRange>& memoryRanges ) {
		if( memoryRanges.GetSize() == 0 ) {
			return true;
		}
		
		const VkResult result = vkFlushMappedMemoryRanges( device, memoryRanges.GetSize(), memoryRanges.GetData() );
		if( VK_SUCCESS != result ) {
			printf( "Could not flush mapped memory\n" );
			return false;
		}

		return true;
	}

	void UnmapMemory( VkDeviceMemory memoryObject ) {
		vkUnmapMemory(device, memoryObject );
	}

	bool MapAndFlushMemory( VkDeviceMemory memoryObject, VkDeviceSize offset, VkDeviceSize size, void* data, bool wholeBuffer ) {

		void* mappedData;
		Array<VkMappedMemoryRange> memoryRanges;
		memoryRanges.AddEmpty( 1 );

		if( !MapMemory( memoryObject, offset, wholeBuffer ? VK_WHOLE_SIZE : size, mappedData, memoryRanges[0] ) ) {
			return false;
		}
		
		memcpy( mappedData, data, size );

		if( !FlushMappedMemory( memoryRanges ) ) {
			return false;
		}

		UnmapMemory( memoryObject );

		return true;
	}

	VkFormatProperties& GetFormatProperties( VkFormat format ) {
		static Map<VkFormat, VkFormatProperties> formatProperties;

		VkFormatProperties* properties = formatProperties.Find( format );
		if( nullptr == properties  ) {
			vkGetPhysicalDeviceFormatProperties( physicalDevice, format, properties );
			properties = &formatProperties.Add( format, *properties );
		}

		return *properties;
	}
	
	void FreeMemory( VkDeviceMemory& memoryObject ) {
		if( VK_NULL_HANDLE != memoryObject ) {
			vkFreeMemory( device, memoryObject, nullptr );
			memoryObject = VK_NULL_HANDLE;
		}
	}
}
