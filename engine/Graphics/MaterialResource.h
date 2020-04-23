#pragma once
#include "Resources/ResourceHandle.h"
#include "TechniqueResource.h"

namespace axlt {
	class TechniqueResource;

	class MaterialResource {

	public:

		ResourceHandle<TechniqueResource> technique;

		DEFINE_TYPE_HASH( MaterialResource );
	};

	Serializer& operator<<( Serializer& s, MaterialResource& material );
	Serializer& operator>>( Serializer& s, MaterialResource& material );
}
