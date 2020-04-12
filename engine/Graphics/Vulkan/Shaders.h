#pragma once
#include "Vulkan.h"
#include "Collections/Array.h"

namespace axlt::vk {

	bool CreateShaderModule( Array<uint8_t>& source, VkShaderModule& shaderModule );

	void DestroyShaderModule( VkShaderModule& shaderModule );
}