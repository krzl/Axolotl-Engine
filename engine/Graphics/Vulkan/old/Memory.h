#pragma once
#include "Vulkan.h"

namespace axlt::vk {

	bool AllocateMemory( VkMemoryRequirements memoryRequirements,
						 VkMemoryPropertyFlagBits memoryProperties,
						 VkDeviceMemory& memoryObject );

	bool MapMemory( VkDeviceMemory memoryObject, VkDeviceSize offset, VkDeviceSize size,
					void*& dataPointer, VkMappedMemoryRange& mappedMemoryRange );
	bool FlushMappedMemory( Array<VkMappedMemoryRange>& memoryRanges );
	void UnmapMemory( VkDeviceMemory memoryObject );

	bool MapAndFlushMemory( VkDeviceMemory memoryObject, VkDeviceSize offset, VkDeviceSize size, void* data, bool wholeBuffer = false );

	VkFormatProperties& GetFormatProperties( VkFormat format );
	
	void FreeMemory( VkDeviceMemory& memoryObject );
}