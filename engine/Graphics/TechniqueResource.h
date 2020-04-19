#pragma once
#include "Resources/ResourceHandle.h"

namespace axlt {
	class BinaryResource;

	class TechniqueResource {

	public:

		ResourceHandle<BinaryResource> vertexShader;
		ResourceHandle<BinaryResource> fragmentShader;
	};

	Serializer& operator<<( Serializer& s, TechniqueResource& technique );
	Serializer& operator>>( Serializer& s, TechniqueResource& technique );
}
