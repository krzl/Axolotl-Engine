#include "../stdafx.h"
#include "FileImport.h"

#include "ShaderImporterHelper.h"

#include <glslang/Public/ShaderLang.h>
#include <SPIRV/GlslangToSpv.h>

namespace axlt::editor {

	BinaryResource* ImportShader( File& file, Array<Guid>& dependencies ) {

		TryInitializeGlslang();

		const EShLanguage stage = GetStageFromExtension( file );
		glslang::TShader shader( stage );
		ParseShader( file, shader );

		glslang::TProgram program;
		program.addShader( &shader );

		const EShMessages messages = (EShMessages) ( EShMsgSpvRules | EShMsgVulkanRules );
		if( !program.link( messages ) || !program.mapIO() ) {
			printf( "Could not link glsl %s\n", file.AbsolutePath().GetData() );
			printf( "%s\n", program.getInfoLog() );
			return nullptr;
		}
		
		std::vector<uint32_t> spirvTemp;
		spv::SpvBuildLogger logger;
		glslang::SpvOptions spvOptions;
		glslang::GlslangToSpv( *program.getIntermediate( stage ), spirvTemp, &logger, &spvOptions );

		BinaryResource* resource = new BinaryResource();

		resource->data.AddEmpty( (uint32_t) ( sizeof( uint32_t ) * spirvTemp.size() ) );
		memcpy( resource->data.GetData(), spirvTemp.data(), resource->data.GetSize() );

		return resource;
	}
}
