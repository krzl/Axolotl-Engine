#include "../stdafx.h"
#include "FileImport.h"

namespace axlt::editor {
	MaterialResource* ImportMaterial( File& file, Array<Guid>& dependencies ) {

		MaterialResource* material = new MaterialResource();

		const rapidjson::Document importSettings = file.ToJson();

		if( importSettings.HasMember( "technique" ) ) {
			material->technique = new TechniqueResource();
			material->technique->guid = Guid::FromString( importSettings[ "technique" ].GetString() );
		}

		if( importSettings.HasMember( "floatParameters" ) ) {
			for( auto& param : importSettings["floatParameters"].GetObject() ) {
				uint32_t nameHash = GetHash( String( param.name.GetString() ) );
				material->floatParameters.Add( nameHash, param.value.GetFloat() );
			}
		}

		if( importSettings.HasMember( "intParameters" ) ) {
			for( auto& param : importSettings["intParameters"].GetObject() ) {
				uint32_t nameHash = GetHash( String( param.name.GetString() ) );
				material->intParameters.Add( nameHash, param.value.GetInt() );
			}
		}

		if( importSettings.HasMember( "vectorParameters" ) ) {
			for( auto& param : importSettings["vectorParameters"].GetObject() ) {
				uint32_t nameHash = GetHash( String( param.name.GetString() ) );
				Vector4 value{};
				uint32_t i = 0;
				for( auto& f : param.value.GetArray() ) {
					if( i == 4 ) break;
					value[i++] = f.GetFloat();
				}
				material->vectorParameters.Add( nameHash, value );
			}
		}

		if( importSettings.HasMember( "textureParameters" ) ) {
			for( auto& param : importSettings["textureParameters"].GetObject() ) {
				uint32_t nameHash = GetHash( String( param.name.GetString() ) );
				TextureResource* texture = new TextureResource();
				texture->guid = Guid::FromString( param.value.GetString() );
				material->textureParameters.Add( nameHash, texture );
			}
		}

		return material;
	}
}