#include "stdafx.h"
#include "TechniqueImport.h"

#include "ShaderImporterHelper.h"

#include <Graphics/TechniqueResource.h>

#include "Import/ImporterRegistrator.h"

namespace axlt::editor {

	static ShaderType GetNativeType( const glslang::TBasicType type ) {
		switch( type ) {
			case glslang::EbtFloat:
				return ShaderType::Float;
			case glslang::EbtDouble:
				return ShaderType::Double;
			case glslang::EbtFloat16:
				return ShaderType::Float;
			case glslang::EbtInt8:
				return ShaderType::Int;
			case glslang::EbtUint8:
				return ShaderType::UInt;
			case glslang::EbtInt16:
				return ShaderType::Int;
			case glslang::EbtUint16:
				return ShaderType::UInt;
			case glslang::EbtInt:
				return ShaderType::Int;
			case glslang::EbtUint:
				return ShaderType::UInt;
			case glslang::EbtBool:
				return ShaderType::Bool;
			default:
				return ShaderType::Unsupported;
		}
	}

	static ShaderPrecision GetNativePrecision( const glslang::TPrecisionQualifier precision ) {
		switch( precision ) {
			case glslang::EpqLow:
				return ShaderPrecision::Low;
			case glslang::EpqMedium:
				return ShaderPrecision::Half;
			case glslang::EpqHigh:
				return ShaderPrecision::Full;
			case glslang::EpqNone:
			default:
				return ShaderPrecision::None;
		}
	}
	
	static TechniqueResource* TechniqueImporter( const File& file, TechniqueImport& import, Array<Guid>& dependencies ) {

		TechniqueResource* technique = new TechniqueResource();

		const rapidjson::Document importSettings = file.ToJson();

		TryInitializeGlslang();

		glslang::TProgram program;
		glslang::TShader vs( EShLangVertex );
		glslang::TShader fs( EShLangFragment );

		if( importSettings.HasMember( "vertex" ) ) {
			const Guid guid = Guid::FromString( importSettings["vertex"].GetString() );
			dependencies.Add( guid );
			technique->vertexShader = new BinaryResource();
			technique->vertexShader->guid = guid;
			const String* filePath = ImportManager::guidToFilepath.Find( guid );
			if( filePath != nullptr ) {
				const File* shaderFile = EditorFileManager::assetsFileSystem.FindFile( *filePath );
				if( shaderFile != nullptr ) {
					ParseShader( *shaderFile, vs );
					program.addShader( &vs );
				}
			}
		}

		if( importSettings.HasMember( "fragment" ) ) {
			const Guid guid = Guid::FromString( importSettings["fragment"].GetString() );
			dependencies.Add( guid );
			technique->fragmentShader = new BinaryResource();
			technique->fragmentShader->guid = guid;
			const String* filePath = ImportManager::guidToFilepath.Find( guid );
			if( filePath != nullptr ) {
				const File* shaderFile = EditorFileManager::assetsFileSystem.FindFile( *filePath );
				if( shaderFile != nullptr ) {
					ParseShader( *shaderFile, fs );
					program.addShader( &fs );
				}
			}
		}

		program.link( (EShMessages) ( EShMsgSpvRules | EShMsgVulkanRules ) );
		program.buildReflection();

		int32_t uniformBlockCount = program.getNumUniformBlocks();
		for( int32_t i = 0; i < uniformBlockCount; i++ ) {
			const glslang::TObjectReflection& uniformBlockRef = program.getUniformBlock( i );
			if( uniformBlockRef.getType()->getQualifier().isPushConstant() ) continue;
			ShaderUniformBlock& uniformBlock =
				technique->uniformBlocks.Emplace( (uint32_t) uniformBlockRef.size,
												  (uint8_t) uniformBlockRef.getType()->getQualifier().layoutSet,
												  (uint8_t) uniformBlockRef.getBinding(),
												  (ShaderStage) ( uniformBlockRef.stages & (uint32_t) ShaderStage::ALL ) );

			const auto& structure = *uniformBlockRef.getType()->getStruct();
			for( uint32_t j = 0; j < structure.size(); j++ ) {
				const auto& uniformRef = structure[j];
				ShaderUniform& uniform = uniformBlock.uniforms.Emplace();
				uniform.id = GetHash( String( uniformRef.type->getFieldName().c_str() ) );
				uniform.name = uniformRef.type->getFieldName().c_str();
				uniform.offset = (uint32_t)uniformRef.type->getQualifier().layoutOffset;
				uniform.type = GetNativeType( uniformRef.type->getBasicType() );
				uniform.precision = GetNativePrecision( uniformRef.type->getQualifier().precision );
				uniform.rows = (uint8_t)uniformRef.type->getMatrixRows();
				uniform.columns = (uint8_t)uniformRef.type->getMatrixCols();
				uniform.vectorSize = (uint8_t)(uniformRef.type->isVector() ? uniformRef.type->getVectorSize() : 1);
				uniform.arraySize = uniformRef.type->getArraySizes() != nullptr ?
					(uint8_t) uniformRef.type->getArraySizes()->getCumulativeSize() : 1;
				
				technique->uniformIdToBlockId.Add( uniform.id, technique->uniformBlocks.GetSize() - 1 );
			}
		}

		int32_t uniformCount = program.getNumUniformVariables();
		for( int32_t i = 0; i < uniformCount; i++ ) {
			const glslang::TObjectReflection& uniformRef = program.getUniform( i );
			if( uniformRef.getType()->getBasicType() == glslang::EbtSampler ) {
				ShaderSampler& sampler = technique->samplers.Emplace();
				sampler = {
					GetHash( String( uniformRef.name.c_str() ) ),
					(uint8_t) uniformRef.getType()->getQualifier().layoutSet,
					(uint8_t) uniformRef.getBinding(),
					uniformRef.getType()->getArraySizes() != nullptr ?
						(uint8_t) uniformRef.getType()->getArraySizes()->getCumulativeSize() : (uint8_t) 1,
					(ShaderStage) ( uniformRef.stages & ( uint32_t ) ShaderStage::ALL )
				};

				technique->textureIdToSamplerId.Add( sampler.id, technique->samplers.GetSize() - 1 );
			} else {
				const uint32_t uniformId = GetHash( String( uniformRef.getType()->getFieldName().c_str() ) );
				const uint32_t blockId = technique->GetUniformBlockId( uniformId );
				if( blockId == 0xFFFFFFFF ) continue;

				technique->GetShaderUniformBlock( blockId ).GetShaderUniform( uniformId )->offset = uniformRef.offset;
			}
		}

		int32_t inputs = program.getNumPipeInputs();

		for( int32_t i = 0; i < inputs; i++ ) {
			const glslang::TObjectReflection& inputRef = program.getPipeInput( i );
			ShaderType type = GetNativeType( inputRef.getType()->getBasicType() );
			ShaderPrecision precision = GetNativePrecision( inputRef.getType()->getQualifier().precision );
			ShaderInputElement& input = technique->inputs.Emplace( ShaderInputElement {
				(uint8_t) inputRef.getType()->getQualifier().layoutLocation,
				(uint8_t) inputRef.getType()->getVectorSize(),
				type,
				precision
			} );
		}

		technique->depthClampEnabled		= import.depthClampEnabled;
		technique->cullMode					= import.cullMode;
		technique->depthBiasEnabled			= import.depthBiasEnabled;
		technique->depthBiasConstantFactor	= import.depthBiasConstantFactor;
		technique->depthBiasClamp			= import.depthBiasClamp;
		technique->depthBiasSlopeFactor		= import.depthBiasSlopeFactor;
		technique->rasterizationSamples		= import.rasterizationSamples;
		technique->alphaToCoverage			= import.alphaToCoverage;
		technique->depthTestEnabled			= import.depthTestEnabled;
		technique->depthWriteEnabled		= import.depthWriteEnabled;
		technique->depthTestOp				= import.depthTestOp;
		technique->stencilTestEnabled		= import.stencilTestEnabled;
		technique->frontStencilOperation	= import.frontStencilOperation;
		technique->backStencilOperation		= import.backStencilOperation;
		technique->blendEnable				= import.blendEnable;
		technique->blendInfo				= import.blendInfo;
		technique->colorMask				= import.colorMask;
			
		return technique;
	}

	// ReSharper disable once CppDeclaratorNeverUsed
	static ImporterRegistrator techniqueImporterRegistrator = ImporterRegistrator( TechniqueImporter, 1, { "tnq" } );
}
