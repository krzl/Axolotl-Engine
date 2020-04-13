#include "../stdafx.h"
#include "FileImport.h"

#include <assimp/cimport.h>
#include "../../external/assimp/code/PostProcessing/ProcessHelper.h"

namespace axlt::editor {
	ModelResource* ImportFbx( File& file ) {

		const aiScene* scene = aiImportFile( 
			file.AbsolutePath().GetData(), 
			aiProcessPreset_TargetRealtime_MaxQuality
		);

		for( uint32_t i = 0; i < (uint32_t) scene->mNumMeshes; i++ ) {
			ModelResource* model = new ModelResource();
			aiMesh* mesh = scene->mMeshes[i];
			model->vertexData.AddEmpty( mesh->mNumVertices );
			for( int i = 0 ;)
		}

		aiReleaseImport( scene );
		return nullptr;
	}
}
