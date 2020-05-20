#include "stdafx.h"
#include "ModelResource.h"

namespace axlt {

	void ModelResource::Flush() {
		isDirty = true;
	}

	Serializer& operator<<( Serializer& s, MeshResource& mesh ) {
		s << mesh.indices << mesh.vertices << mesh.normals << mesh.tangents << mesh.bitangents;
		for( auto& colorChannel : mesh.colorChannels ) {
			s << colorChannel;
		}
		for( auto& texCoords : mesh.texCoordChannels ) {
			s << texCoords;
		}
		return s;
	}

	Serializer& operator>>( Serializer& s, MeshResource& mesh ) {
		s >> mesh.indices >> mesh.vertices >> mesh.normals >> mesh.tangents >> mesh.bitangents;
		for( auto& colorChannel : mesh.colorChannels ) {
			s >> colorChannel;
		}
		for( auto& texCoords : mesh.texCoordChannels ) {
			s >> texCoords;
		}
		return s;
	}

	Serializer& operator<<( Serializer& s, ModelResource& model ) {
		s <<= model.meshes;
		return s;
	}

	Serializer& operator>>( Serializer& s, ModelResource& model ) {
		s >>= model.meshes;
		return s;
	}
}
