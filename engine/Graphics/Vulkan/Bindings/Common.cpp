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
		}
		throw 0;
	}
}
