#include "../stdafx.h"
#include "FileImport.h"

#include <Graphics/TechniqueResource.h>

namespace axlt::editor {
	TechniqueResource* ImportTechnique( File& file ) {

		TechniqueResource* technique = new TechniqueResource();

		const rapidjson::Document importSettings = file.ToJson();

		if( importSettings.HasMember( "vertex" ) ) {
			technique->vertexShader = ResourceHandle<BinaryResource>( importSettings["vertex"].GetString() );
		}

		if( importSettings.HasMember( "fragment" ) ) {
			technique->fragmentShader = ResourceHandle<BinaryResource>( importSettings["fragment"].GetString() );
		}

		return technique;
	}
}