#include "../stdafx.h"
#include "FileImport.h"

#include <Graphics/TechniqueResource.h>
#include <glslang/Public/ShaderLang.h>

#include "../ResourceDatabase.h"
#include "ShaderImporterHelper.h"

namespace axlt::editor {
	TechniqueResource* ImportTechnique( File& file, Array<Guid>& dependencies ) {

		TechniqueResource* technique = new TechniqueResource();

		const rapidjson::Document importSettings = file.ToJson();

		TryInitializeGlslang();

		glslang::TProgram program;
		glslang::TShader vs( EShLangVertex );
		glslang::TShader fs( EShLangFragment );

		if( importSettings.HasMember( "vertex" ) ) {
			const Guid guid = Guid::FromString( importSettings["vertex"].GetString() );
			dependencies.Add( guid );
			technique->vertexShader = ResourceHandle<BinaryResource>( guid );
			const String* filePath = ResourceDatabase::instance->guidToFilepath.Find( guid );
			if( filePath != nullptr ) {
				const File* shaderFile = ResourceDatabase::instance->resourceFileSystem.FindFile( *filePath );
				if( shaderFile != nullptr ) {
					ParseShader( *shaderFile, vs );
					program.addShader( &vs );
				}
			}
		}

		if( importSettings.HasMember( "fragment" ) ) {
			const Guid guid = Guid::FromString( importSettings["fragment"].GetString() );
			dependencies.Add( guid );
			technique->fragmentShader = ResourceHandle<BinaryResource>( guid );
			const String* filePath = ResourceDatabase::instance->guidToFilepath.Find( guid );
			if( filePath != nullptr ) {
				const File* shaderFile = ResourceDatabase::instance->resourceFileSystem.FindFile( *filePath );
				if( shaderFile != nullptr ) {
					ParseShader( *shaderFile, fs );
					program.addShader( &fs );
				}
			}
		}

		program.link( (EShMessages) ( EShMsgSpvRules | EShMsgVulkanRules ) );
		program.buildReflection();

		return technique;
	}
}
