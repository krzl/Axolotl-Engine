#include "stdafx.h"
#include "ShaderImporterHelper.h"

#include <FileSystem/File.h>
#include <FileSystem/Directory.h>

#include <string>
#include <StandAlone/ResourceLimits.h>
#include <StandAlone/DirStackFileIncluder.h>

namespace axlt::editor {

	TBuiltInResource builtInResource = {
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

	bool glslangInitialized = false;

	void TryInitializeGlslang() {
		if( !glslangInitialized ) {
			glslang::InitializeProcess();
			glslangInitialized = true;
		}
	}

	EShLanguage GetStageFromExtension( const File& file ) {
		const String extension = file.Extension();
		if( extension == "vert" ) {
			return EShLangVertex;
		}
		if( extension == "tesc" ) {
			return EShLangTessControl;
		}
		if( extension == "tese" ) {
			return EShLangTessEvaluation;
		}
		if( extension == "geom" ) {
			return EShLangGeometry;
		}
		if( extension == "frag" ) {
			return EShLangFragment;
		}
		if( extension == "comp" ) {
			return EShLangCompute;
		}
		printf( "Unknown shader extension %s\n", file.Extension().GetData() );
		throw 0;
	}

	void ParseShader( const File& file, glslang::TShader& shader ) {

		std::ifstream shaderFile( file.AbsolutePath().GetData() );
		const std::string shaderCode( (std::istreambuf_iterator<char>( shaderFile )), std::istreambuf_iterator<char>() );
		const char* shaderCodeChar = shaderCode.c_str();

		shader.setStrings( &shaderCodeChar, 1 );
		shader.setEnvInput( glslang::EShSourceGlsl, shader.getStage(), glslang::EShClientVulkan, 100 );
		shader.setEnvClient( glslang::EShClientVulkan, glslang::EShTargetVulkan_1_1 );
		shader.setEnvTarget( glslang::EShTargetSpv, glslang::EShTargetSpv_1_3 );

		std::string preprocessedGlsl;
		const EShMessages messages = (EShMessages) ( EShMsgSpvRules | EShMsgVulkanRules );

		DirStackFileIncluder includer;
		includer.pushExternalLocalDirectory( file.ParentDirectory().AbsolutePath().GetData() );
		includer.pushExternalLocalDirectory( "../Engine/Shaders/" );

		if( !shader.preprocess( &builtInResource, 450, ENoProfile, false, false, messages, &preprocessedGlsl, includer ) ) {
			printf( "Could not preprocess file %s\n", file.AbsolutePath().GetData() );
			printf( "%s\n", shader.getInfoLog() );
			return;
		}

		const char* preprocessedPtr = preprocessedGlsl.c_str();
		shader.setStrings( &preprocessedPtr, 1 );

		if( !shader.parse( &builtInResource, 100, false, messages ) ) {
			printf( "Could not parse file %s\n", file.AbsolutePath().GetData() );
			printf( "%s\n", shader.getInfoLog() );
		}
	}
}
