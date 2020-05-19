#include "stdafx.h"
#include "TextureResource.h"

namespace axlt {

	Serializer& operator<<( Serializer& s, TextureResource& texture ) {
		return s << texture.width << texture.height << texture.channelCount << texture.format << texture.textureData;
	}

	Serializer& operator>>( Serializer& s, TextureResource& texture ) {
		return s >> texture.width >> texture.height >> texture.channelCount >> texture.format >> texture.textureData;
	}
}
