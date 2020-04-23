#pragma once
#include "Resources/ResourceHandle.h"
#include "Resources/BinaryResource.h"

namespace axlt {
	class BinaryResource;

	class TechniqueResource {

	public:

		ResourceHandle<BinaryResource> vertexShader;
		ResourceHandle<BinaryResource> fragmentShader;

		DEFINE_TYPE_HASH( TechniqueResource );
	};

	Serializer& operator<<( Serializer& s, TechniqueResource& technique );
	Serializer& operator>>( Serializer& s, TechniqueResource& technique );
}
