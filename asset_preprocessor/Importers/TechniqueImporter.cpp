#include "../stdafx.h"
#include "FileImport.h"

#include <Graphics/TechniqueResource.h>
#include <glslang/Public/ShaderLang.h>

#include "../ResourceDatabase.h"
#include "ShaderImporterHelper.h"
#include <glslang/Include/Types.h>

namespace axlt::editor {

	ShaderReflectionElement::Type GetNativeType( const glslang::TBasicType type ) {
		switch( type ) {
			case glslang::EbtFloat:
				return ShaderReflectionElement::Float;
			case glslang::EbtDouble:
				return ShaderReflectionElement::Double;
			case glslang::EbtFloat16:
				return ShaderReflectionElement::Float;
			case glslang::EbtInt8:
				return ShaderReflectionElement::Int;
			case glslang::EbtUint8:
				return ShaderReflectionElement::UInt;
			case glslang::EbtInt16:
				return ShaderReflectionElement::Int;
			case glslang::EbtUint16:
				return ShaderReflectionElement::UInt;
			case glslang::EbtInt:
				return ShaderReflectionElement::Int;
			case glslang::EbtUint:
				return ShaderReflectionElement::UInt;
			case glslang::EbtBool:
				return ShaderReflectionElement::Bool;
			case glslang::EbtSampler:
				return ShaderReflectionElement::Sampler;
			default:
				return ShaderReflectionElement::Unsupported;
		}
	}

	ShaderReflectionElement::Precision GetNativePrecision( const glslang::TPrecisionQualifier precision ) {
		switch( precision ) {
			case glslang::EpqLow:
				return ShaderReflectionElement::Low;
			case glslang::EpqMedium:
				return ShaderReflectionElement::Half;
			case glslang::EpqHigh:
				return ShaderReflectionElement::Full;
			case glslang::EpqNone:
			default:
				return ShaderReflectionElement::None;
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

		int32_t uniformCount = program.getNumUniformVariables();
		for( int32_t i = 0; i < uniformCount; i++ ) {
			const glslang::TObjectReflection& uniform = program.getUniform( i );
			uint8_t arraySize = 0;
			if( uniform.getType()->getArraySizes() != nullptr ) {
				arraySize = uniform.getType()->getArraySizes()->getCumulativeSize();
			}
			uint8_t vectorSize = 0;
			if( uniform.getType()->isVector() ) {
				vectorSize = uniform.getType()->getVectorSize();
			}
			if( uniform.index == -1 ) {
				int32_t binding = uniform.getBinding();
				uint32_t layoutSet = uniform.getType()->getQualifier().layoutSet;
				if( layoutSet == 0 ) continue;
				technique->uniforms.Emplace( uniform.name.c_str(), binding, layoutSet, 0,
											 GetNativeType( uniform.getType()->getBasicType() ),
											 GetNativePrecision( uniform.getType()->getQualifier().precision ),
											 uniform.getType()->getMatrixRows(), uniform.getType()->getMatrixCols(),
											 vectorSize, arraySize );
			} else {
				const glslang::TObjectReflection& uniformBlock = program.getUniformBlock( uniform.index );
				int32_t binding = uniformBlock.getBinding();
				uint32_t layoutSet = uniformBlock.getType()->getQualifier().layoutSet;
				if( layoutSet == 0 ) continue;
				technique->uniforms.Emplace( uniform.name.c_str(), binding, layoutSet, uniform.offset,
											 GetNativeType( uniform.getType()->getBasicType() ),
											 GetNativePrecision( uniform.getType()->getQualifier().precision ),
											 uniform.getType()->getMatrixRows(), uniform.getType()->getMatrixCols(),
											 vectorSize, arraySize );
			}
		}

		int32_t inputs = program.getNumPipeInputs();

		for( int32_t i = 0; i < inputs; i++ ) {
			const glslang::TObjectReflection& input = program.getPipeInput( i );
			technique->inputs.Emplace( input.getType()->getQualifier().layoutLocation, input.getType()->getVectorSize() );
		}

		return technique;
	}
}
