#include "stdafx.h"

#include "../ImporterRegistrator.h"
#include <Resources/ModelResource.h>
#include "ModelImport.h"

#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace axlt::editor {

	ModelResource* FbxImporter( const File& file, ModelImport& modelImport, Array<Guid>& dependencies ) {
		const aiScene* scene = aiImportFile(
			file.AbsolutePath().GetData(),
			aiProcessPreset_TargetRealtime_MaxQuality
		);

		ModelResource* model = new ModelResource();

		model->storeMeshData = modelImport.storeMeshData;
		
		for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; meshIndex++) {
			aiMesh* importMesh = scene->mMeshes[ meshIndex ];

			Mesh& mesh = model->meshes.Emplace();

			mesh.meshLayout = 0;

			mesh.indices.AddEmpty( importMesh->mNumFaces * 3 );
			for (uint32_t i = 0; i < importMesh->mNumFaces; i++) {
				memcpy( &mesh.indices[ i * 3 ], importMesh->mFaces[ i ].mIndices, sizeof( uint32_t ) * 3 );
			}

			uint32_t vertexDataIndex = 0;
			
			if( !model->storeMeshData ) {
				mesh.vertices.AddEmpty( importMesh->mNumVertices );
			}
			mesh.vertexData.AddEmpty( importMesh->mNumVertices * 3 );
			for (uint32_t i = 0; i < importMesh->mNumVertices; i++) {
				if( !model->storeMeshData ) {
					mesh.vertices[ i ].x = (float)importMesh->mVertices[ i ].x;
					mesh.vertices[ i ].y = (float)importMesh->mVertices[ i ].y;
					mesh.vertices[ i ].z = (float)importMesh->mVertices[ i ].z;
				}
				mesh.vertexData[ vertexDataIndex++ ] = (float)importMesh->mVertices[ i ].x;
				mesh.vertexData[ vertexDataIndex++ ] = (float)importMesh->mVertices[ i ].y;
				mesh.vertexData[ vertexDataIndex++ ] = (float)importMesh->mVertices[ i ].z;
			}

			if (importMesh->HasNormals()) {
				if (model->storeMeshData) {
					mesh.normals.AddEmpty( importMesh->mNumVertices );
				}
				mesh.vertexData.AddEmpty( importMesh->mNumVertices * 3 );
				for (uint32_t i = 0; i < importMesh->mNumVertices; i++) {
					if (model->storeMeshData) {
						mesh.normals[ i ].x = (float)importMesh->mNormals[ i ].x;
						mesh.normals[ i ].y = (float)importMesh->mNormals[ i ].y;
						mesh.normals[ i ].z = (float)importMesh->mNormals[ i ].z;
					}
					mesh.vertexData[ vertexDataIndex++ ] = (float)importMesh->mNormals[ i ].x;
					mesh.vertexData[ vertexDataIndex++ ] = (float)importMesh->mNormals[ i ].y;
					mesh.vertexData[ vertexDataIndex++ ] = (float)importMesh->mNormals[ i ].z;
				}
				mesh.meshLayout += Mesh::CreateMeshLayoutMask( 1, 1 );
			}

			if (importMesh->HasTangentsAndBitangents()) {
				if (model->storeMeshData) {
					mesh.tangents.AddEmpty( importMesh->mNumVertices );
					mesh.bitangents.AddEmpty( importMesh->mNumVertices );
				}
				mesh.vertexData.AddEmpty( importMesh->mNumVertices * 6 );
				for (uint32_t i = 0; i < importMesh->mNumVertices; i++) {
					if (model->storeMeshData) {
						mesh.tangents[ i ].x = (float)importMesh->mTangents[ i ].x;
						mesh.tangents[ i ].y = (float)importMesh->mTangents[ i ].y;
						mesh.tangents[ i ].z = (float)importMesh->mTangents[ i ].z;

						mesh.bitangents[ i ].x = (float)importMesh->mBitangents[ i ].x;
						mesh.bitangents[ i ].y = (float)importMesh->mBitangents[ i ].y;
						mesh.bitangents[ i ].z = (float)importMesh->mBitangents[ i ].z;
					}

					
					mesh.vertexData[ vertexDataIndex++ ] = (float)importMesh->mTangents[ i ].x;
					mesh.vertexData[ vertexDataIndex++ ] = (float)importMesh->mTangents[ i ].y;
					mesh.vertexData[ vertexDataIndex++ ] = (float)importMesh->mTangents[ i ].z;

					mesh.vertexData[ vertexDataIndex++ ] = (float)importMesh->mBitangents[ i ].x;
					mesh.vertexData[ vertexDataIndex++ ] = (float)importMesh->mBitangents[ i ].y;
					mesh.vertexData[ vertexDataIndex++ ] = (float)importMesh->mBitangents[ i ].z;
				}
				
				mesh.meshLayout += Mesh::CreateMeshLayoutMask( 2, 1 );
				mesh.meshLayout += Mesh::CreateMeshLayoutMask( 3, 1 );
			}

			for (uint32_t i = 0; i < Min( 1u, importMesh->GetNumColorChannels() ); i++) {
				if (model->storeMeshData) {
					mesh.colorChannels[ i ].AddEmpty( importMesh->mNumVertices );
				}
				mesh.vertexData.AddEmpty( importMesh->mNumVertices * 4 );
				for (uint32_t j = 0; j < importMesh->mNumVertices; j++) {
					if (model->storeMeshData) {
						mesh.colorChannels[ i ][ j ].r = (float)importMesh->mColors[ i ][ j ].r;
						mesh.colorChannels[ i ][ j ].g = (float)importMesh->mColors[ i ][ j ].g;
						mesh.colorChannels[ i ][ j ].b = (float)importMesh->mColors[ i ][ j ].b;
						mesh.colorChannels[ i ][ j ].a = (float)importMesh->mColors[ i ][ j ].a;
					}
					
					mesh.vertexData[ vertexDataIndex++ ] = (float)importMesh->mColors[ i ][ j ].r;
					mesh.vertexData[ vertexDataIndex++ ] = (float)importMesh->mColors[ i ][ j ].g;
					mesh.vertexData[ vertexDataIndex++ ] = (float)importMesh->mColors[ i ][ j ].b;
					mesh.vertexData[ vertexDataIndex++ ] = (float)importMesh->mColors[ i ][ j ].a;
				}
				mesh.meshLayout += Mesh::CreateMeshLayoutMask( 4, 1 );
			}

			for (uint32_t i = 0; i < Min( 4u, importMesh->GetNumUVChannels() ); i++) {
				const uint8_t channelCount = importMesh->mNumUVComponents[ i ];
				if (model->storeMeshData) {
					mesh.texCoordChannels[ i ].AddEmpty( importMesh->mNumVertices * channelCount );
				}
				mesh.vertexData.AddEmpty( importMesh->mNumVertices * channelCount );
				for (uint32_t j = 0; j < importMesh->mNumVertices; j++) {
					for (uint32_t k = 0; k < channelCount; k++) {
						if (model->storeMeshData) {
							mesh.texCoordChannels[ i ][ j * channelCount + k ] = (float)importMesh->mTextureCoords[ i ][ j ][ k ];
						}
						mesh.vertexData[ vertexDataIndex++ ] = (float)importMesh->mTextureCoords[ i ][ j ][ k ];
					}
				}
				mesh.meshLayout += Mesh::CreateMeshLayoutMask( 5 + i, Min( (uint8_t) 4, channelCount ) );
			}
		}

		aiReleaseImport( scene );

		return model;
	}
	
	static ImporterRegistrator fbxImporterRegistrator = ImporterRegistrator( FbxImporter, 1, { "fbx" } );
}
