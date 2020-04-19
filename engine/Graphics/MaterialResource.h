#pragma once
#include "Resources/ResourceHandle.h"

namespace axlt {
	class TechniqueResource;

	class MaterialResource {

	public:

		ResourceHandle<TechniqueResource> technique;
	};

	Serializer& operator<<( Serializer& s, MaterialResource& material );
	Serializer& operator>>( Serializer& s, MaterialResource& material );
}
