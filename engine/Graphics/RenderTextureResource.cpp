#include "stdafx.h"
#include "RenderTextureResource.h"

namespace axlt {
	const SerializationInfo& RenderTextureResource::GetSerializationData() const {
		static SerializationInfo info = SerializationInfoBuilder<RenderTextureResource>( "RenderTextureResource" )
			.AddField( "width", &RenderTextureResource::width )
			.AddField( "height", &RenderTextureResource::height )
			.AddField( "hasDepth", &RenderTextureResource::hasDepth )
			.Build();

		return info;
	}

	uint32_t RenderTextureResource::GetTypeHash() const {
		return axlt::GetTypeHash<RenderTextureResource>();
	}
}