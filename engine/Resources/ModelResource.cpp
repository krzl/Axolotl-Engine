#include "stdafx.h"
#include "ModelResource.h"

namespace axlt {

	template<typename T>
	void WriteSingle( FILE* fp, T val ) {
		fwrite( &val, sizeof val, 1, fp );
	}

	template<typename ElementType, typename AllocatorType>
	void WriteArray( FILE* fp, Array<ElementType, AllocatorType> array ) {
		WriteSingle( fp, array.GetSize() );
		fwrite( array.GetData(), sizeof ElementType, array.GetSize(), fp );
	}

	void ModelResource::Serialize( FILE* fp ) {
		WriteArray( fp, indices );
		WriteArray( fp, vertices );
		WriteArray( fp, normals );
		WriteArray( fp, tangents );
		WriteArray( fp, bitangents );
		for( const auto& colors : colorChannels ) {
			WriteArray( fp, colors );
		}
		for( const auto& texCoords : texCoordChannels ) {
			WriteArray( fp, texCoords );
		}
	}
}
