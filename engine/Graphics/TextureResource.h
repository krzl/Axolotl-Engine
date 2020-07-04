#pragma once
#include "Resources/Serialization/Serializable.h"
#include "Resources/Resource.h"

namespace axlt {
	enum class TextureFormat {
		RGBA32,
		DXT5,
		DXT1
	};
	
	class TextureResource final : public Resource {

	public:

		const SerializationInfo& GetSerializationData() const override;
		uint32_t GetTypeHash() const override;

		uint32_t width;
		uint32_t height;
		uint8_t channelCount;
		Array<uint8_t, ExactHeapArrayAllocator> textureData;

		TextureFormat format;
	};
}
