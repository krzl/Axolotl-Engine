#include "../stdafx.h"
#include "FileImport.h"

#include <Graphics/TechniqueResource.h>
#include <glslang/Public/ShaderLang.h>

#include "../ResourceDatabase.h"
#include "ShaderImporterHelper.h"
#include <glslang/Include/Types.h>

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


		int32_t uniformBlockCount = program.getNumUniformBlocks();
		for( int32_t i = 0; i < uniformBlockCount; i++ ) {
			const glslang::TObjectReflection& uniformBlockRef = program.getUniformBlock( i );
			if( uniformBlockRef.getType()->getQualifier().layoutSet == 0 ) {
				continue;
			}
			ShaderUniformBlock& uniformBlock = technique->uniformBlocks.Emplace();
			uniformBlock = {
				(uint32_t) uniformBlockRef.size,
				(uint8_t) uniformBlockRef.getType()->getQualifier().layoutSet,
				(uint8_t) uniformBlockRef.getBinding(),
				(ShaderStage) ( uniformBlockRef.stages & ShaderStage::ALL ),
				ExactArray<ShaderUniform>()
			};

			const auto& structure = *uniformBlockRef.getType()->getStruct();
			for( uint32_t j = 0; j < structure.size(); j++ ) {
				const auto& uniformRef = structure[j];
				ShaderUniform& uniform = uniformBlock.uniforms.Emplace();
				uniform = {
					uniformRef.type->getFieldName().c_str(),
					(uint32_t) uniformRef.type->getQualifier().layoutOffset,
					GetNativeType( uniformRef.type->getBasicType() ),
					GetNativePrecision( uniformRef.type->getQualifier().precision ),
					(uint8_t) uniformRef.type->getMatrixRows(),
					(uint8_t) uniformRef.type->getMatrixCols(),
					(uint8_t) ( uniformRef.type->isVector() ? uniformRef.type->getVectorSize() : 1 ),
					uniformRef.type->getArraySizes() != nullptr ? 
						(uint8_t) uniformRef.type->getArraySizes()->getCumulativeSize() : (uint8_t) 1
				};
			}
		}

		int32_t uniformCount = program.getNumUniformVariables();
		for( int32_t i = 0; i < uniformCount; i++ ) {
			const glslang::TObjectReflection& uniformRef = program.getUniform( i );
			if( uniformRef.getType()->getBasicType() == glslang::EbtSampler ) {
				ShaderSampler& sampler = technique->samplers.Emplace();
				sampler = {
					(uint8_t) uniformRef.getType()->getQualifier().layoutSet,
					(uint8_t) uniformRef.getBinding(),
					uniformRef.getType()->getArraySizes() != nullptr ? 
						(uint8_t) uniformRef.getType()->getArraySizes()->getCumulativeSize() : (uint8_t) 1,
					(ShaderStage) ( uniformRef.stages & ShaderStage::ALL )
				};
			}
		}

		int32_t inputs = program.getNumPipeInputs();

		for( int32_t i = 0; i < inputs; i++ ) {
			const glslang::TObjectReflection& inputRef = program.getPipeInput( i );
			ShaderInputElement& input = technique->inputs.Emplace();
			input = {
				(uint8_t) inputRef.getType()->getQualifier().layoutLocation,
				(uint8_t) inputRef.getType()->getVectorSize(),
				GetNativeType( inputRef.getType()->getBasicType() ),
				GetNativePrecision( inputRef.getType()->getQualifier().precision )
			};
		}

		return technique;
	}
}
