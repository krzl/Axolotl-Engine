#pragma once
#include "Resources/ResourceHandle.h"
#include "Resources/BinaryResource.h"

namespace axlt {
	class BinaryResource;

	enum ShaderType {
		Float,
		Double,
		Int,
		UInt,
		Bool,
		Unsupported
	};

	enum ShaderPrecision {
		Full,
		Half,
		Low,
		None
	};

	enum ShaderStage {
		VERTEX = 1,
		TESS_CONTROL = 2,
		TESS_EVAL = 4,
		GEOMETRY = 8,
		FRAGMENT = 16,
		COMPUTE = 32,
		ALL = 63
	};

	class ShaderSampler {

	public:

		uint8_t set;
		uint8_t binding;
		uint8_t count;
		ShaderStage shaderStages;
	};

	class ShaderUniform {

	public:

		String name;
		uint32_t offset;
		ShaderType type;
		ShaderPrecision precision;
		uint8_t rows;
		uint8_t columns;
		uint8_t vectorSize;
		uint8_t arraySize;
	};

	Serializer& operator<<( Serializer& s, ShaderUniform& element );
	Serializer& operator>>( Serializer& s, ShaderUniform& element );

	class ShaderUniformBlock {

	public:

		uint32_t size;
		uint8_t set;
		uint8_t binding;
		ShaderStage shaderStages;
		ExactArray<ShaderUniform> uniforms;
	};

	Serializer& operator<<( Serializer& s, ShaderUniformBlock& element );
	Serializer& operator>>( Serializer& s, ShaderUniformBlock& element );

	class ShaderInputElement {

	public:

		uint8_t location;
		uint8_t vectorSize;
		uint32_t stride;
		ShaderType type;
		ShaderPrecision precision;
	};

	class TechniqueResource {

	public:

		ResourceHandle<BinaryResource> vertexShader;
		ResourceHandle<BinaryResource> fragmentShader;

		ExactArray<ShaderUniformBlock> uniformBlocks;
		ExactArray<ShaderSampler> samplers;
		ExactArray<ShaderInputElement> inputs;

		DEFINE_TYPE_HASH( TechniqueResource );
	};

	Serializer& operator<<( Serializer& s, TechniqueResource& technique );
	Serializer& operator>>( Serializer& s, TechniqueResource& technique );
}
