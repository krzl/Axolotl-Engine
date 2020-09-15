#include "stdafx.h"
#include "ModelResource.h"

namespace axlt {

	ModelResource::ModelResource( const bool isDynamic ) {
		this->isDynamic = isDynamic;
	}

	void ModelResource::Flush() {
		isDirty = true;
	}

	Mesh::Mesh( const uint16_t meshLayout ) :
		meshLayout( meshLayout ) {}

	const SerializationInfo& Mesh::GetSerializationData() const {
		static SerializationInfo info = SerializationInfoBuilder<Mesh>( "Mesh" )
			.AddField( "vertexData", &Mesh::vertexData )
			.AddField( "indices", &Mesh::indices )
			.AddField( "meshLayout", &Mesh::meshLayout )
			.Build();
		return info;
	}

	uint32_t Mesh::GetTypeHash() const {
		return axlt::GetTypeHash<Mesh>();
	}

	uint16_t Mesh::GetMeshLayout() const {
		return meshLayout;
	}

	uint16_t Mesh::GetAttributeOffset( const uint8_t location, const uint16_t meshLayout, const uint8_t startingLocation ) {
		uint8_t offset = 0;
		for( uint8_t i = startingLocation; i < location; i++ ) {
			offset += GetAttributeByteSize( i ) * GetAttributeVectorSize( i, meshLayout );
		}
		return offset;
	}

	uint16_t Mesh::GetAttributeByteSize( const uint16_t index ) {
		if( index < 4 ) {
			return 12;
		}
		if( index == 4 ) {
			return 16;
		}
		return 4;
	}

	uint8_t Mesh::GetAttributeVectorSize( const uint8_t location, const uint16_t meshLayout ) {
		if( location == 0 ) {
			return 1;
		}
		const uint8_t byteMaskSize = location < 5 ? 1 : 7;
		const uint16_t offset = location >= 5 ? 4 + (location - 4) * 3 : location;
		const uint16_t bitShift = meshLayout >> (16 - offset);
		const uint8_t maskVal = bitShift & byteMaskSize;
		return maskVal;
	}

	uint16_t Mesh::CreateMeshLayoutMask( const uint8_t location, const uint8_t vectorSize ) {
		if( location == 0 ) return 0;
		const uint16_t offset = location >= 5 ? 4 + ( location - 4 ) * 3 : location;
		return ( location > 4 ? vectorSize : 1 ) << ( 16 - offset );
	}

	uint16_t Mesh::GetMeshStride( const uint16_t meshLayout ) {
		return GetAttributeOffset( 9, meshLayout );
	}

	const SerializationInfo& ModelResource::GetSerializationData() const {
		static SerializationInfo info = SerializationInfoBuilder<ModelResource>( "ModelResource" )
			.AddField( "meshes", &ModelResource::meshes )
			.Build();
		return info;
	}

	uint32_t ModelResource::GetTypeHash() const {
		return axlt::GetTypeHash<ModelResource>();
	}
}
