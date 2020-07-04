#include "stdafx.h"
#include "ModelResource.h"

namespace axlt {

	void ModelResource::Flush() {
		isDirty = true;
	}

	const SerializationInfo& Mesh::GetSerializationData() const {
		static SerializationInfo info = SerializationInfoBuilder<Mesh>( "Mesh" )
			.AddField( "indices", &Mesh::indices )
			.AddField( "vertices", &Mesh::vertices )
			.AddField( "normals", &Mesh::normals )
			.AddField( "tangents", &Mesh::tangents )
			.AddField( "bitangents", &Mesh::bitangents )
			.AddField( "colorChannels", &Mesh::colorChannels ) // TODO: ADD ARRAY
			.AddField( "texCoordChannels", &Mesh::texCoordChannels ) // TODO: ADD ARRAY
			.Build();
		return info;
	}

	uint32_t Mesh::GetTypeHash() const {
		return axlt::GetTypeHash<Mesh>();
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
