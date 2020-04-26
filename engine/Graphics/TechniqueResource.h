#pragma once
#include "Resources/ResourceHandle.h"
#include "Resources/BinaryResource.h"
#include "Vulkan/Initialization/Initializer.h"

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

	class ShaderReflectionElement {

	public:

		String name;
		uint32_t offset;
		ShaderType type;
		ShaderPrecision precision;
		uint8_t rows;
		uint8_t columns;
		uint8_t vectorSize;
		uint8_t arraySize;

		ShaderReflectionElement( String name, const uint32_t offset, const ShaderType type, 
								 const ShaderPrecision precision, const uint8_t rows, const uint8_t columns,
								 const uint8_t vectorSize, const uint8_t arraySize ) :
			name( Move( name ) ),
			offset( offset ),
			type( type ),
			precision( precision ),
			rows( rows ),
			columns( columns ),
			vectorSize( vectorSize ),
			arraySize( arraySize ) {}
	};

	class UniformBlockElement {

	public:

		uint32_t size;
		uint8_t set;
		uint8_t binding;
		uint8_t count;
		ShaderStage shaderStages;
		ExactArray<ShaderReflectionElement> uniforms;
	};

	class ShaderInputElement {

	public:

		uint8_t location;
		uint8_t vectorSize;
		ShaderType type;
		ShaderPrecision precision;

		ShaderInputElement( const uint8_t location, const uint8_t vectorSize,
							const ShaderType type, const ShaderPrecision precision ) :
			location( location ),
			vectorSize( vectorSize ),
			type( type ),
			precision( precision ) {}
	};

	Serializer& operator<<( Serializer& s, ShaderReflectionElement& element );
	Serializer& operator>>( Serializer& s, ShaderReflectionElement& element );

	class TechniqueResource {

	public:

		ResourceHandle<BinaryResource> vertexShader;
		ResourceHandle<BinaryResource> fragmentShader;

		ExactArray<UniformBlockElement> uniformBlocks;
		ExactArray<ShaderSampler> samplers;
		ExactArray<ShaderInputElement> inputs;

		DEFINE_TYPE_HASH( TechniqueResource );
	};

	Serializer& operator<<( Serializer& s, TechniqueResource& technique );
	Serializer& operator>>( Serializer& s, TechniqueResource& technique );
}
