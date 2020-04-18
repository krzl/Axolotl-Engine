#include "../stdafx.h"
#include "FileImport.h"

#include <glslang/Public/ShaderLang.h>

#include <string>
#include <StandAlone/ResourceLimits.h>
#include <StandAlone/DirStackFileIncluder.h>
#include <FileSystem/Directory.h>
#include <SPIRV/GlslangToSpv.h>

namespace axlt::editor {

	bool glslangInitialized = false;

	const TBuiltInResource builtInResource = {
		/* .MaxLights = */ 32,
		/* .MaxClipPlanes = */ 6,
		/* .MaxTextureUnits = */ 32,
		/* .MaxTextureCoords = */ 32,
		/* .MaxVertexAttribs = */ 64,
		/* .MaxVertexUniformComponents = */ 4096,
		/* .MaxVaryingFloats = */ 64,
		/* .MaxVertexTextureImageUnits = */ 32,
		/* .MaxCombinedTextureImageUnits = */ 80,
		/* .MaxTextureImageUnits = */ 32,
		/* .MaxFragmentUniformComponents = */ 4096,
		/* .MaxDrawBuffers = */ 32,
		/* .MaxVertexUniformVectors = */ 128,
		/* .MaxVaryingVectors = */ 8,
		/* .MaxFragmentUniformVectors = */ 16,
		/* .MaxVertexOutputVectors = */ 16,
		/* .MaxFragmentInputVectors = */ 15,
		/* .MinProgramTexelOffset = */ -8,
		/* .MaxProgramTexelOffset = */ 7,
		/* .MaxClipDistances = */ 8,
		/* .MaxComputeWorkGroupCountX = */ 65535,
		/* .MaxComputeWorkGroupCountY = */ 65535,
		/* .MaxComputeWorkGroupCountZ = */ 65535,
		/* .MaxComputeWorkGroupSizeX = */ 1024,
		/* .MaxComputeWorkGroupSizeY = */ 1024,
		/* .MaxComputeWorkGroupSizeZ = */ 64,
		/* .MaxComputeUniformComponents = */ 1024,
		/* .MaxComputeTextureImageUnits = */ 16,
		/* .MaxComputeImageUniforms = */ 8,
		/* .MaxComputeAtomicCounters = */ 8,
		/* .MaxComputeAtomicCounterBuffers = */ 1,
		/* .MaxVaryingComponents = */ 60,
		/* .MaxVertexOutputComponents = */ 64,
		/* .MaxGeometryInputComponents = */ 64,
		/* .MaxGeometryOutputComponents = */ 128,
		/* .MaxFragmentInputComponents = */ 128,
		/* .MaxImageUnits = */ 8,
		/* .MaxCombinedImageUnitsAndFragmentOutputs = */ 8,
		/* .MaxCombinedShaderOutputResources = */ 8,
		/* .MaxImageSamples = */ 0,
		/* .MaxVertexImageUniforms = */ 0,
		/* .MaxTessControlImageUniforms = */ 0,
		/* .MaxTessEvaluationImageUniforms = */ 0,
		/* .MaxGeometryImageUniforms = */ 0,
		/* .MaxFragmentImageUniforms = */ 8,
		/* .MaxCombinedImageUniforms = */ 8,
		/* .MaxGeometryTextureImageUnits = */ 16,
		/* .MaxGeometryOutputVertices = */ 256,
		/* .MaxGeometryTotalOutputComponents = */ 1024,
		/* .MaxGeometryUniformComponents = */ 1024,
		/* .MaxGeometryVaryingComponents = */ 64,
		/* .MaxTessControlInputComponents = */ 128,
		/* .MaxTessControlOutputComponents = */ 128,
		/* .MaxTessControlTextureImageUnits = */ 16,
		/* .MaxTessControlUniformComponents = */ 1024,
		/* .MaxTessControlTotalOutputComponents = */ 4096,
		/* .MaxTessEvaluationInputComponents = */ 128,
		/* .MaxTessEvaluationOutputComponents = */ 128,
		/* .MaxTessEvaluationTextureImageUnits = */ 16,
		/* .MaxTessEvaluationUniformComponents = */ 1024,
		/* .MaxTessPatchComponents = */ 120,
		/* .MaxPatchVertices = */ 32,
		/* .MaxTessGenLevel = */ 64,
		/* .MaxViewports = */ 16,
		/* .MaxVertexAtomicCounters = */ 0,
		/* .MaxTessControlAtomicCounters = */ 0,
		/* .MaxTessEvaluationAtomicCounters = */ 0,
		/* .MaxGeometryAtomicCounters = */ 0,
		/* .MaxFragmentAtomicCounters = */ 8,
		/* .MaxCombinedAtomicCounters = */ 8,
		/* .MaxAtomicCounterBindings = */ 1,
		/* .MaxVertexAtomicCounterBuffers = */ 0,
		/* .MaxTessControlAtomicCounterBuffers = */ 0,
		/* .MaxTessEvaluationAtomicCounterBuffers = */ 0,
		/* .MaxGeometryAtomicCounterBuffers = */ 0,
		/* .MaxFragmentAtomicCounterBuffers = */ 1,
		/* .MaxCombinedAtomicCounterBuffers = */ 1,
		/* .MaxAtomicCounterBufferSize = */ 16384,
		/* .MaxTransformFeedbackBuffers = */ 4,
		/* .MaxTransformFeedbackInterleavedComponents = */ 64,
		/* .MaxCullDistances = */ 8,
		/* .MaxCombinedClipAndCullDistances = */ 8,
		/* .MaxSamples = */ 4,
		/* .maxMeshOutputVerticesNV = */ 256,
		/* .maxMeshOutputPrimitivesNV = */ 512,
		/* .maxMeshWorkGroupSizeX_NV = */ 32,
		/* .maxMeshWorkGroupSizeY_NV = */ 1,
		/* .maxMeshWorkGroupSizeZ_NV = */ 1,
		/* .maxTaskWorkGroupSizeX_NV = */ 32,
		/* .maxTaskWorkGroupSizeY_NV = */ 1,
		/* .maxTaskWorkGroupSizeZ_NV = */ 1,
		/* .maxMeshViewCountNV = */ 4,

		/* .limits = */ {
		/* .nonInductiveForLoops = */ 1,
		/* .whileLoops = */ 1,
		/* .doWhileLoops = */ 1,
		/* .generalUniformIndexing = */ 1,
		/* .generalAttributeMatrixVectorIndexing = */ 1,
		/* .generalVaryingIndexing = */ 1,
		/* .generalSamplerIndexing = */ 1,
		/* .generalVariableIndexing = */ 1,
		/* .generalConstantMatrixVectorIndexing = */ 1,
	} };

	BinaryResource* ImportShader( File& file ) {

		EShLanguage stage;

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
			return nullptr;
		}

		if( !glslangInitialized ) {
			glslang::InitializeProcess();
			glslangInitialized = true;
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
		EShMessages messages = (EShMessages) ( EShMsgSpvRules | EShMsgVulkanRules );

		DirStackFileIncluder includer;
		includer.pushExternalLocalDirectory( file.ParentDirectory().AbsolutePath().GetData() );

		if( !shader.preprocess( &builtInResource, 450, ENoProfile, false, false, messages, &preprocessedGlsl, includer ) ) {
			printf( "Could not preprocess file %s\n", file.AbsolutePath().GetData() );
			printf( "%s\n", shader.getInfoLog() );
			return nullptr;
		}

		const char* preprocessedPtr = preprocessedGlsl.c_str();
		shader.setStrings( &preprocessedPtr, 1 );

		if( !shader.parse( &builtInResource, 100, false, messages ) ) {
			printf( "Could not parse file %s\n", file.AbsolutePath().GetData() );
			printf( "%s\n", shader.getInfoLog() );
			return nullptr;
		}

		glslang::TProgram program;
		program.addShader( &shader );

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
