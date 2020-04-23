#include "../stdafx.h"
#include "ShaderImporterHelper.h"

#include <FileSystem/File.h>
#include <FileSystem/Directory.h>
#include <glslang/MachineIndependent/Initialize.h>
#include <StandAlone/DirStackFileIncluder.h>

namespace axlt::editor {

	void TryInitializeGlslang() {
		static bool glslangInitialized = false;
		if( !glslangInitialized ) {
			glslang::InitializeProcess();
			glslangInitialized = true;
		}
	}

	glslang::TShader CompileShader( const File& file, EShLanguage& stage ) {

		const String extension = file.Extension();
		if( extension == "vert" ) {
			stage = EShLangVertex;
		} else if( extension == "tesc" ) {
			stage = EShLangTessControl;
		} else if( extension == "tese" ) {
			stage = EShLangTessEvaluation;
		} else if( extension == "geom" ) {
			stage = EShLangGeometry;
		} else if( extension == "frag" ) {
			stage = EShLangFragment;
		} else if( extension == "comp" ) {
			stage = EShLangCompute;
		} else {
			printf( "Unknown shader extension %s\n", file.Extension().GetData() );
			return glslang::TShader( EShLangMiss );
		}

		glslang::TShader shader( stage );

		FILE* fp;
		fopen_s( &fp, file.AbsolutePath().GetData(), "r" );

		fseek( fp, 0L, SEEK_END );
		const uint32_t fileSize = ftell( fp );
		rewind( fp );

		ExactHeapArrayAllocator::AllocatorInstance<char> fileData;
		fileData.ResizeAllocation( 0, fileSize );

		char* ptr = fileData.GetAllocation();
		fread( ptr, sizeof( uint8_t ), fileSize, fp );
		fclose( fp );

		shader.setStrings( &ptr, 1 );
		shader.setEnvInput( glslang::EShSourceGlsl, stage, glslang::EShClientVulkan, 100 );
		shader.setEnvClient( glslang::EShClientVulkan, glslang::EShTargetVulkan_1_2 );
		shader.setEnvTarget( glslang::EShTargetSpv, glslang::EShTargetSpv_1_5 );

		std::string preprocessedGlsl;
		const EShMessages messages = (EShMessages) ( EShMsgSpvRules | EShMsgVulkanRules );

		DirStackFileIncluder includer;
		includer.pushExternalLocalDirectory( file.ParentDirectory().AbsolutePath().GetData() );

		if( !shader.preprocess( &builtInResource, 450, ENoProfile, false, false, messages, &preprocessedGlsl, includer ) ) {
			printf( "Could not preprocess file %s\n", file.AbsolutePath().GetData() );
			printf( "%s\n", shader.getInfoLog() );
			return shader;
		}

		const char* preprocessedPtr = preprocessedGlsl.c_str();
		shader.setStrings( &preprocessedPtr, 1 );

		if( !shader.parse( &builtInResource, 100, false, messages ) ) {
			printf( "Could not parse file %s\n", file.AbsolutePath().GetData() );
			printf( "%s\n", shader.getInfoLog() );
			return shader;
		}

		return shader;
	}
}
