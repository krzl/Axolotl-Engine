#pragma once
#include "Vulkan.h"
#include "Collections/Array.h"

namespace axlt::vk {

	bool CreateShaderModule( const Array<uint8_t, ExactHeapArrayAllocator>& source, VkShaderModule& shaderModule );

	void DestroyShaderModule( VkShaderModule& shaderModule );
}