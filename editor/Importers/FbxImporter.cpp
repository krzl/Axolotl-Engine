#include "../stdafx.h"
#include "FileImport.h"

#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace axlt::editor {
	ModelResource* ImportModel( File& file, Array<Guid>& dependencies ) {

		const aiScene* scene = aiImportFile(
			file.AbsolutePath().GetData(),
			aiProcessPreset_TargetRealtime_MaxQuality
		);

		ModelResource* model = new ModelResource();

		for( uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; meshIndex++ ) {
			aiMesh* importMesh = scene->mMeshes[meshIndex];

			MeshResource& mesh = model->meshes.Emplace();

			mesh.indices.AddEmpty( importMesh->mNumFaces * 3 );
			for( uint32_t i = 0; i < importMesh->mNumFaces; i++ ) {
				memcpy( &mesh.indices[i * 3], importMesh->mFaces[i].mIndices, sizeof( uint32_t ) * 3 );
			}

			mesh.vertices.AddEmpty( importMesh->mNumVertices );
			for( uint32_t i = 0; i < importMesh->mNumVertices; i++ ) {
				mesh.vertices[i].x = (float) importMesh->mVertices[i].x;
				mesh.vertices[i].y = (float) importMesh->mVertices[i].y;
				mesh.vertices[i].z = (float) importMesh->mVertices[i].z;
			}

			if( importMesh->HasNormals() ) {
				mesh.normals.AddEmpty( importMesh->mNumVertices );
				for( uint32_t i = 0; i < importMesh->mNumVertices; i++ ) {
					mesh.normals[i].x = (float) importMesh->mNormals[i].x;
					mesh.normals[i].y = (float) importMesh->mNormals[i].y;
					mesh.normals[i].z = (float) importMesh->mNormals[i].z;
				}
			}

			if( importMesh->HasTangentsAndBitangents() ) {
				mesh.tangents.AddEmpty( importMesh->mNumVertices );
				mesh.bitangents.AddEmpty( importMesh->mNumVertices );
				for( uint32_t i = 0; i < importMesh->mNumVertices; i++ ) {
					mesh.tangents[i].x = (float) importMesh->mNormals[i].x;
					mesh.tangents[i].y = (float) importMesh->mNormals[i].y;
					mesh.tangents[i].z = (float) importMesh->mNormals[i].z;

					mesh.bitangents[i].x = (float) importMesh->mNormals[i].x;
					mesh.bitangents[i].y = (float) importMesh->mNormals[i].y;
					mesh.bitangents[i].z = (float) importMesh->mNormals[i].z;
				}
			}

			for( uint32_t i = 0; i < importMesh->GetNumColorChannels(); i++ ) {
				mesh.colorChannels[i].AddEmpty( importMesh->mNumVertices );
				for( uint32_t j = 0; j < importMesh->mNumVertices; j++ ) {
					mesh.colorChannels[i][j].r = (float) importMesh->mColors[i][j].r;
					mesh.colorChannels[i][j].g = (float) importMesh->mColors[i][j].g;
					mesh.colorChannels[i][j].b = (float) importMesh->mColors[i][j].b;
					mesh.colorChannels[i][j].a = (float) importMesh->mColors[i][j].a;
				}
			}

			for( uint32_t i = 0; i < importMesh->GetNumUVChannels(); i++ ) {
				const uint8_t channelCount = importMesh->mNumUVComponents[i];
				mesh.texCoordChannels[i].AddEmpty( importMesh->mNumVertices * channelCount );
				for( uint32_t j = 0; j < importMesh->mNumVertices; j++ ) {
					for( uint32_t k = 0; k < channelCount; k++ ) {
						mesh.texCoordChannels[i][j * channelCount + k] = (float) importMesh->mTextureCoords[i][j][k];
					}
				}
			}
		}

		aiReleaseImport( scene );

		return model;
	}
}
