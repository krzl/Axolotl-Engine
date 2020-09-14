#pragma once
#include "Resources/Serialization/Serializable.h"
#include "Resources/Resource.h"

namespace axlt {
	enum class TextureFormat {
		RGBA32,
		DXT5,
		DXT1,
		UNKNOWN
	};

	enum class TextureDimension {
		TEX_1D,
		TEX_2D,
		TEX_3D,
		UNKNOWN
	};

	enum class TextureFiltering {
		POINT,
		LINEAR,
		CUBIC
	};

	enum class TextureAddressMode {
		REPEAT,
		MIRROR_REPEAT,
		CLAMP_EDGE,
		CLAMP_BORDER,
		MIRROR_CLAMP_EDGE
	};

	class TextureResource final : public Resource {

	public:

		const SerializationInfo& GetSerializationData() const override;
		uint32_t GetTypeHash() const override;

		uint32_t width = 1;
		uint32_t height = 1;
		uint32_t depth = 1;
		
		uint8_t channelCount = 1;
		
		Array<uint8_t, ExactHeapArrayAllocator> textureData{};

		TextureFormat format = TextureFormat::RGBA32;
		TextureDimension dimension = TextureDimension::TEX_2D;

		uint8_t mipmapCount = 1;
		uint8_t arrayCount = 1;

		uint8_t sampleCount = 1;

		TextureFiltering minFilter = TextureFiltering::LINEAR;
		TextureFiltering magFilter = TextureFiltering::LINEAR;

		bool mipmapFilterLinear = true;

		TextureAddressMode addressModeU = TextureAddressMode::REPEAT;
		TextureAddressMode addressModeV = TextureAddressMode::REPEAT;
		TextureAddressMode addressModeW = TextureAddressMode::REPEAT;

		float mipLodBias = 0.0f;
		float anisotropicFiltering = 1.0f;
	};
}
