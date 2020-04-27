#include "stdafx.h"
#include "TechniqueResource.h"

namespace axlt {

	Serializer& operator<<( Serializer& s, ShaderUniform& element ) {
		return s << element.name << element.offset << element.type << element.precision <<
			element.rows << element.columns << element.vectorSize << element.arraySize;
	}

	Serializer& operator>>( Serializer& s, ShaderUniform& element ) {
		return s >> element.name >> element.offset >> element.type >> element.precision >>
			element.rows >> element.columns >> element.vectorSize >> element.arraySize;
	}

	Serializer& operator<<( Serializer& s, ShaderUniformBlock& element ) {
		return s << element.size << element.set << element.binding << element.shaderStages <<= element.uniforms;
	}

	Serializer& operator>>( Serializer& s, ShaderUniformBlock& element ) {
		return s >> element.size >> element.set >> element.binding >> element.shaderStages >>= element.uniforms;
	}

	Serializer& operator<<( Serializer& s, TechniqueResource& technique ) {
		s << technique.vertexShader << technique.fragmentShader << technique.inputs;
		s <<= technique.samplers;
		s <<= technique.inputs;
		return s;
	}

	Serializer& operator>>( Serializer& s, TechniqueResource& technique ) {
		s >> technique.vertexShader >> technique.fragmentShader >> technique.inputs;
		s >>= technique.samplers;
		s >>= technique.inputs;
		return s;
	}
}
