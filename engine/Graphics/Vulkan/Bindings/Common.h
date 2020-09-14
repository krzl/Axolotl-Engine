#pragma once

#include <vulkan/vulkan.h>
#include "../Vulkan.h"

namespace axlt {
	enum class ShaderCompareOperation;
}

namespace axlt::vk {
	VkSampleCountFlagBits GetSampleCount( uint8_t sampleCount );
	VkCompareOp GetCompareOperation( ShaderCompareOperation shaderCompareOperation );
	VkImageType GetImageType( TextureDimension dimension );
	VkFormat GetNativeTextureFormat( TextureFormat format );
	VkImageViewType GetImageViewType( TextureDimension dimension );
	VkFilter GetTextureFilter( TextureFiltering minFilter );
	VkSamplerAddressMode GetTextureAddressMode( TextureAddressMode addressModeU );
}
