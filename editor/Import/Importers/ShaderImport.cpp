#include "stdafx.h"
#include "ShaderImport.h"

#include "../ImporterRegistrator.h"
#include "ShaderImporterHelper.h"

#include <Resources/BinaryResource.h>

#include <glslang/Public/ShaderLang.h>
#include <SPIRV/GlslangToSpv.h>

namespace axlt::editor {

	BinaryResource* ShaderImporter( const File& file, ShaderImport& shaderImport, Array<Guid>& dependencies ) {

		TryInitializeGlslang();

		const EShLanguage stage = GetStageFromExtension( file );
		glslang::TShader shader( stage );
		ParseShader( file, shader );

		glslang::TProgram program;
		program.addShader( &shader );

		const EShMessages messages = (EShMessages)(EShMsgSpvRules | EShMsgVulkanRules);
		if (!program.link( messages ) || !program.mapIO()) {
			printf( "Could not link glsl %s\n", file.AbsolutePath().GetData() );
			printf( "%s\n", program.getInfoLog() );
			return nullptr;
		}

		std::vector<uint32_t> spirvTemp;
		spv::SpvBuildLogger logger;
		glslang::SpvOptions spvOptions;
		glslang::GlslangToSpv( *program.getIntermediate( stage ), spirvTemp, &logger, &spvOptions );

		BinaryResource* resource = new BinaryResource();

		resource->binaryData.AddEmpty( (uint32_t)(sizeof( uint32_t ) * spirvTemp.size()) );
		memcpy( resource->binaryData.GetData(), spirvTemp.data(), resource->binaryData.GetSize() );

		return resource;
	}
	
	static ImporterRegistrator shaderImporterRegistrator = ImporterRegistrator( ShaderImporter, 1, { "vert", "frag" } );
}
