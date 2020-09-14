#pragma once

#include <vulkan/vulkan.h>

namespace axlt {
	enum class ShaderCompareOperation;
}

namespace axlt::vk {
	VkSampleCountFlagBits GetSampleCount( uint8_t sampleCount );
	VkCompareOp GetCompareOperation( ShaderCompareOperation shaderCompareOperation );
}
