#include "stdafx.h"
#include "TechniqueResource.h"

namespace axlt {

	Serializer& operator<<( Serializer& s, TechniqueResource& technique ) {
		return s << technique.vertexShader << technique.fragmentShader;
	}

	Serializer& operator>>( Serializer& s, TechniqueResource& technique ) {
		return s >> technique.vertexShader >> technique.fragmentShader;
	}
}
