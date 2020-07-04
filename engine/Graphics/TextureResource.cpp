#include "stdafx.h"
#include "TextureResource.h"

namespace axlt {

	const SerializationInfo& TextureResource::GetSerializationData() const {
		static SerializationInfo info = SerializationInfoBuilder<TextureResource>( "TextureResource" )
			.AddField( "width", &TextureResource::width )
			.AddField( "height", &TextureResource::height )
			.AddField( "channelCount", &TextureResource::channelCount )
			.AddField( "textureData", &TextureResource::textureData )
			.AddField( "format", &TextureResource::format )
			.Build();
		return info;
	}

	uint32_t TextureResource::GetTypeHash() const {
		return axlt::GetTypeHash<TextureResource>();
	}
}