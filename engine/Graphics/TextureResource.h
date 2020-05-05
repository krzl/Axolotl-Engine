#pragma once

namespace axlt {
	class TextureResource {

	public:

		uint32_t width;
		uint32_t height;
		uint8_t channelCount;
		Array<uint8_t, ExactHeapArrayAllocator> textureData;

		DEFINE_TYPE_HASH( TextureResource );
	};

	Serializer& operator<<( Serializer& s, TextureResource& texture );
	Serializer& operator>>( Serializer& s, TextureResource& texture );
}