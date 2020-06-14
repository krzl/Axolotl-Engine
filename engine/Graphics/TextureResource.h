#pragma once

namespace axlt {
	enum class TextureFormat {
		RGBA32,
		DXT5,
		DXT1
	};
	
	class TextureResource {

	public:

		uint32_t width;
		uint32_t height;
		uint8_t channelCount;
		Array<uint8_t, ExactHeapArrayAllocator> textureData;

		TextureFormat format;
	};

	Serializer& operator<<( Serializer& s, TextureResource& texture );
	Serializer& operator>>( Serializer& s, TextureResource& texture );
}