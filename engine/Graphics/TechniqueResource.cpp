#include "stdafx.h"
#include "TechniqueResource.h"

namespace axlt {

	Serializer& operator<<( Serializer& s, ShaderReflectionElement& element ) {
		return s << element.name << element.set << element.binding << element.offset;
	}

	Serializer& operator>>( Serializer& s, ShaderReflectionElement& element ) {
		return s >> element.name >> element.set >> element.binding >> element.offset;
	}
	
	Serializer& operator<<( Serializer& s, TechniqueResource& technique ) {
		return s << technique.vertexShader << technique.fragmentShader << technique.inputs <<= technique.uniforms;
	}

	Serializer& operator>>( Serializer& s, TechniqueResource& technique ) {
		return s >> technique.vertexShader >> technique.fragmentShader >> technique.inputs >>= technique.uniforms;
	}
}
