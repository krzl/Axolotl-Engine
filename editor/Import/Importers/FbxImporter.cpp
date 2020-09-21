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
		
		for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; meshIndex++) {
			aiMesh* importMesh = scene->mMeshes[ meshIndex ];

			Mesh& mesh = model->meshes.Emplace();
			
			mesh.meshLayout = 0;

			mesh.vertexData.AddEmpty( importMesh->mNumVertices * 3 );
			if (importMesh->HasNormals()) {
				mesh.vertexData.AddEmpty( importMesh->mNumVertices * 3 );
				mesh.meshLayout += Mesh::CreateMeshLayoutMask( 1, 1 );
			}

			if (importMesh->HasTangentsAndBitangents()) {
				mesh.vertexData.AddEmpty( importMesh->mNumVertices * 6 );
				mesh.meshLayout += Mesh::CreateMeshLayoutMask( 2, 1 );
				mesh.meshLayout += Mesh::CreateMeshLayoutMask( 3, 1 );
			}

			for (uint32_t i = 0; i < Min( 1u, importMesh->GetNumColorChannels() ); i++) {
				mesh.vertexData.AddEmpty( importMesh->mNumVertices * 4 );
				mesh.meshLayout += Mesh::CreateMeshLayoutMask( 4, 1 );
			}

			for (uint32_t i = 0; i < Min( 4u, importMesh->GetNumUVChannels() ); i++) {
				const uint8_t channelCount = importMesh->mNumUVComponents[ i ];
				mesh.vertexData.AddEmpty( importMesh->mNumVertices * channelCount );
				mesh.meshLayout += Mesh::CreateMeshLayoutMask( 5 + i, Min( (uint8_t)4, channelCount ) );
			}
			

			uint32_t vertexDataIndex = 0;

			for( uint32_t i = 0; i < importMesh->mNumVertices; i++ ) {
				mesh.vertexData[ vertexDataIndex++ ] = (float)importMesh->mVertices[ i ].x;
				mesh.vertexData[ vertexDataIndex++ ] = (float)importMesh->mVertices[ i ].y;
				mesh.vertexData[ vertexDataIndex++ ] = (float)importMesh->mVertices[ i ].z;
				
				if (importMesh->HasNormals()) {
					mesh.vertexData[ vertexDataIndex++ ] = (float)importMesh->mNormals[ i ].x;
					mesh.vertexData[ vertexDataIndex++ ] = (float)importMesh->mNormals[ i ].y;
					mesh.vertexData[ vertexDataIndex++ ] = (float)importMesh->mNormals[ i ].z;
				}
				
				if (importMesh->HasTangentsAndBitangents()) {
					mesh.vertexData[ vertexDataIndex++ ] = (float)importMesh->mTangents[ i ].x;
					mesh.vertexData[ vertexDataIndex++ ] = (float)importMesh->mTangents[ i ].y;
					mesh.vertexData[ vertexDataIndex++ ] = (float)importMesh->mTangents[ i ].z;

					mesh.vertexData[ vertexDataIndex++ ] = (float)importMesh->mBitangents[ i ].x;
					mesh.vertexData[ vertexDataIndex++ ] = (float)importMesh->mBitangents[ i ].y;
					mesh.vertexData[ vertexDataIndex++ ] = (float)importMesh->mBitangents[ i ].z;
				}
				
				for (uint32_t j = 0; j < Min( 1u, importMesh->GetNumColorChannels() ); j++) {
					mesh.vertexData[ vertexDataIndex++ ] = (float)importMesh->mColors[ j ][ i ].r;
					mesh.vertexData[ vertexDataIndex++ ] = (float)importMesh->mColors[ j ][ i ].g;
					mesh.vertexData[ vertexDataIndex++ ] = (float)importMesh->mColors[ j ][ i ].b;
					mesh.vertexData[ vertexDataIndex++ ] = (float)importMesh->mColors[ j ][ i ].a;
				}
				
				for (uint32_t j = 0; j < Min( 4u, importMesh->GetNumUVChannels() ); j++) {
					const uint8_t channelCount = importMesh->mNumUVComponents[ j ];
					for (uint32_t k = 0; k < channelCount; k++) {
						if( k == 1 ) {
							mesh.vertexData[ vertexDataIndex++ ] = 1.0f - (float)importMesh->mTextureCoords[ j ][ i ][ k ];
						} else {
							mesh.vertexData[ vertexDataIndex++ ] = (float)importMesh->mTextureCoords[ j ][ i ][ k ];
						}
					}
				}
			}
			

			mesh.indices.AddEmpty( importMesh->mNumFaces * 3 );
			for (uint32_t i = 0; i < importMesh->mNumFaces; i++) {
				memcpy( &mesh.indices[ i * 3 ], importMesh->mFaces[ i ].mIndices, sizeof( uint32_t ) * 3 );
			}
		}

		aiReleaseImport( scene );

		return model;
	}
	
	// ReSharper disable once CppDeclaratorNeverUsed
	static ImporterRegistrator fbxImporterRegistrator = ImporterRegistrator( FbxImporter, 1, { "fbx" } );
}
