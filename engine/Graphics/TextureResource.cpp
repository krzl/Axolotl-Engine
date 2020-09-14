#include "stdafx.h"
#include "TextureResource.h"

namespace axlt {

	const SerializationInfo& TextureResource::GetSerializationData() const {
		static SerializationInfo info = SerializationInfoBuilder<TextureResource>( "TextureResource" )
			.AddField( "width", &TextureResource::width )
			.AddField( "height", &TextureResource::height )
			.AddField( "depth", &TextureResource::depth )
			.AddField( "channelCount", &TextureResource::channelCount )
			.AddField( "textureData", &TextureResource::textureData )
			.AddField( "format", &TextureResource::format )
			.AddField( "dimension", &TextureResource::dimension )
			.AddField( "mipmapCount", &TextureResource::mipmapCount )
			.AddField( "arrayCount", &TextureResource::arrayCount )
			.AddField( "sampleCount", &TextureResource::sampleCount )
			.AddField( "minFilter", &TextureResource::minFilter )
			.AddField( "magFilter", &TextureResource::magFilter )
			.AddField( "mipmapFilterLinear", &TextureResource::mipmapFilterLinear )
			.AddField( "addressModeU", &TextureResource::addressModeU )
			.AddField( "addressModeV", &TextureResource::addressModeV )
			.AddField( "addressModeW", &TextureResource::addressModeW )
			.AddField( "mipLodBias", &TextureResource::mipLodBias )
			.AddField( "anisotropicFiltering", &TextureResource::anisotropicFiltering )
			.Build();
		
		return info;
	}

	uint32_t TextureResource::GetTypeHash() const {
		return axlt::GetTypeHash<TextureResource>();
	}
}