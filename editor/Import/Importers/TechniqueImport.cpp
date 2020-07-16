#include "stdafx.h"
#include "TechniqueImport.h"

#include "ShaderImporterHelper.h"

#include <Graphics/TechniqueResource.h>

#include "Import/ImporterRegistrator.h"

namespace axlt::editor {

	ShaderType GetNativeType( const glslang::TBasicType type ) {
		switch( type ) {
			case glslang::EbtFloat:
				return Float;
			case glslang::EbtDouble:
				return Double;
			case glslang::EbtFloat16:
				return Float;
			case glslang::EbtInt8:
				return Int;
			case glslang::EbtUint8:
				return UInt;
			case glslang::EbtInt16:
				return Int;
			case glslang::EbtUint16:
				return UInt;
			case glslang::EbtInt:
				return Int;
			case glslang::EbtUint:
				return UInt;
			case glslang::EbtBool:
				return Bool;
			default:
				return Unsupported;
		}
	}

	ShaderPrecision GetNativePrecision( const glslang::TPrecisionQualifier precision ) {
		switch( precision ) {
			case glslang::EpqLow:
				return Low;
			case glslang::EpqMedium:
				return Half;
			case glslang::EpqHigh:
				return Full;
			case glslang::EpqNone:
			default:
				return None;
		}
	}

	uint32_t GetStride( const uint32_t vectorSize, const ShaderType type, const ShaderPrecision precision ) {

		uint32_t stride;

		switch( type ) {
			case Float:
			case Int:
			case UInt:
				stride = 4;
				break;
			case Double:
				stride = 8;
				break;
			case Bool:
				stride = 1;
				break;
			default:
				stride = 0;
		}


		switch( precision ) {
			case Low:
				stride /= 4;
				break;
			case Half:
				stride /= 2;
				break;
			default:
				break;
		}

		return stride * vectorSize;
	}

	TechniqueResource* TechniqueImporter( const File& file, TechniqueImport& import, Array<Guid>& dependencies ) {

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
												  (ShaderStage) ( uniformBlockRef.stages & ShaderStage::ALL ) );

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
					(ShaderStage) ( uniformRef.stages & ShaderStage::ALL )
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
			ShaderInputElement& input = technique->inputs.Emplace();
			ShaderType type = GetNativeType( inputRef.getType()->getBasicType() );
			ShaderPrecision precision = GetNativePrecision( inputRef.getType()->getQualifier().precision );
			input = {
				(uint8_t) inputRef.getType()->getQualifier().layoutLocation,
				(uint8_t) inputRef.getType()->getVectorSize(),
				GetStride( inputRef.getType()->getVectorSize(), type, precision ),
				type,
				precision
			};
		}

		return technique;
	}

	static ImporterRegistrator techniqueImporterRegistrator = ImporterRegistrator( TechniqueImporter, 1, { "tnq" } );
}
