#include "stdafx.h"
#include "Common.h"

#include "Graphics/TechniqueResource.h"

namespace axlt::vk {
	VkSampleCountFlagBits GetSampleCount( const uint8_t sampleCount ) {
		switch (sampleCount) {
			case 1:
				return VK_SAMPLE_COUNT_1_BIT;
			case 2:
				return VK_SAMPLE_COUNT_2_BIT;
			case 4:
				return VK_SAMPLE_COUNT_4_BIT;
			case 8:
				return VK_SAMPLE_COUNT_8_BIT;
			case 16:
				return VK_SAMPLE_COUNT_16_BIT;
			case 32:
				return VK_SAMPLE_COUNT_32_BIT;
			case 64:
				return VK_SAMPLE_COUNT_64_BIT;
			default:
				return VK_SAMPLE_COUNT_1_BIT;
		}
	}

	VkCompareOp GetCompareOperation( const ShaderCompareOperation shaderCompareOperation ) {
		switch( shaderCompareOperation ) {
			case ShaderCompareOperation::NEVER:
				return VK_COMPARE_OP_NEVER;
			case ShaderCompareOperation::LESS:
				return VK_COMPARE_OP_LESS;
			case ShaderCompareOperation::EQUAL:
				return VK_COMPARE_OP_EQUAL;
			case ShaderCompareOperation::L_EQUAL:
				return VK_COMPARE_OP_LESS_OR_EQUAL;
			case ShaderCompareOperation::GREATER:
				return VK_COMPARE_OP_GREATER;
			case ShaderCompareOperation::NOT_EQUAL:
				return VK_COMPARE_OP_NOT_EQUAL;
			case ShaderCompareOperation::G_EQUAL:
				return VK_COMPARE_OP_GREATER_OR_EQUAL;
			case ShaderCompareOperation::ALWAYS:
				return VK_COMPARE_OP_ALWAYS;
			default:
				throw 0;
		}
	}

	VkImageType GetImageType( const TextureDimension dimension ) {
		switch (dimension) {
			case TextureDimension::TEX_1D:
				return VK_IMAGE_TYPE_1D;
			case TextureDimension::TEX_2D:
				return VK_IMAGE_TYPE_2D;
			case TextureDimension::TEX_3D:
				return VK_IMAGE_TYPE_3D;
			default:
				throw 0;
		}
	}

	VkFormat GetNativeTextureFormat( const TextureFormat format ) {
		switch (format) {
			case TextureFormat::DXT1:
				return VK_FORMAT_BC1_RGB_UNORM_BLOCK;
			case TextureFormat::DXT5:
				return VK_FORMAT_BC3_SRGB_BLOCK;
			case TextureFormat::RGBA32:
				return VK_FORMAT_R8G8B8A8_SRGB;
			default:
				return VK_FORMAT_R8G8B8A8_SRGB;
		}
	}

	VkImageViewType GetImageViewType( const TextureDimension dimension ) {
		switch (dimension) {
			case TextureDimension::TEX_1D:
				return VK_IMAGE_VIEW_TYPE_1D;
			case TextureDimension::TEX_2D:
				return VK_IMAGE_VIEW_TYPE_2D;
			case TextureDimension::TEX_3D:
				return VK_IMAGE_VIEW_TYPE_3D;
			default:
				throw 0;
		}
	}

	VkFilter GetTextureFilter( const TextureFiltering minFilter ) {
		switch (minFilter) {
			case TextureFiltering::POINT:
				return VK_FILTER_NEAREST;
			case TextureFiltering::LINEAR:
				return VK_FILTER_LINEAR;
			case TextureFiltering::CUBIC:
				return VK_FILTER_CUBIC_EXT;
			default:
				throw 0;
		}
	}

	VkSamplerAddressMode GetTextureAddressMode( const TextureAddressMode addressModeU ) {
		switch (addressModeU) {
			case TextureAddressMode::REPEAT:
				return VK_SAMPLER_ADDRESS_MODE_REPEAT;
			case TextureAddressMode::CLAMP_BORDER:
				return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
			case TextureAddressMode::CLAMP_EDGE:
				return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			case TextureAddressMode::MIRROR_CLAMP_EDGE:
				return VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
			case TextureAddressMode::MIRROR_REPEAT:
				return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
			default:
				throw 0;
		}
	}
}
