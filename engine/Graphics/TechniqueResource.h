#pragma once
#include "Resources/ResourceHandle.h"
#include "Resources/BinaryResource.h"

namespace axlt {
	class BinaryResource;

	class ShaderReflectionElement {

	public:

		enum Type {
			Float,
			Double,
			Int,
			UInt,
			Bool,
			Sampler,
			Unsupported
		};

		enum Precision {
			Full,
			Half,
			Low,
			None
		};

		String name;
		uint8_t set;
		uint8_t binding;
		uint32_t offset;
		Type type;
		Precision precision;
		uint8_t rows;
		uint8_t columns;
		uint8_t vectorSize;
		uint8_t arraySize;

		ShaderReflectionElement( const String& name, const uint8_t set, const uint8_t binding, const uint32_t offset,
								 const Type type, const Precision precision, const uint8_t rows, const uint8_t columns,
								 const uint8_t vectorSize, const uint8_t arraySize ) :
			name( name ),
			set( set ),
			binding( binding ),
			offset( offset ),
			type( type ),
			precision( precision ),
			rows( rows ),
			columns( columns ),
			vectorSize( vectorSize ),
			arraySize( arraySize ) {}
	};

	class ShaderInputElement {

	public:

		uint8_t location;
		uint8_t vectorSize;

		ShaderInputElement( const uint8_t location, const uint8_t vectorSize ) :
			location( location ),
			vectorSize( vectorSize ) {}
	};

	Serializer& operator<<( Serializer& s, ShaderReflectionElement& element );
	Serializer& operator>>( Serializer& s, ShaderReflectionElement& element );

	class TechniqueResource {

	public:

		ResourceHandle<BinaryResource> vertexShader;
		ResourceHandle<BinaryResource> fragmentShader;

		Array<ShaderReflectionElement> uniforms;
		Array<ShaderInputElement> inputs;

		DEFINE_TYPE_HASH( TechniqueResource );
	};

	Serializer& operator<<( Serializer& s, TechniqueResource& technique );
	Serializer& operator>>( Serializer& s, TechniqueResource& technique );
}
