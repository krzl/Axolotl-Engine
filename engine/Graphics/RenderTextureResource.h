#pragma once
#include "Resources/Resource.h"

namespace axlt {
	class RenderTextureResource final : public Resource {

	public:

		RenderTextureResource() : RenderTextureResource( 1,1 ) {}
		
		RenderTextureResource( const uint32_t width, const uint32_t height, const bool hasDepth = true ) :
			width( width ),
			height( height ),
			hasDepth( hasDepth ) {
		}

		const SerializationInfo& GetSerializationData() const override;
		uint32_t GetTypeHash() const override;

		uint32_t width;
		uint32_t height;

		bool hasDepth;

	};
}
