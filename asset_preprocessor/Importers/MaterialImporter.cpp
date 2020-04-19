#include "../stdafx.h"
#include "FileImport.h"

namespace axlt::editor {
	MaterialResource* ImportMaterial( File& file ) {

		MaterialResource* material = new MaterialResource();

		const rapidjson::Document importSettings = file.ToJson();

		if( importSettings.HasMember( "technique" ) ) {
			material->technique = ResourceHandle<TechniqueResource>( importSettings["technique"].GetString() );
		}

		return material;		
	}
}