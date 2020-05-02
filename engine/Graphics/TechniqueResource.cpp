#include "stdafx.h"
#include "TechniqueResource.h"

namespace axlt {

	ShaderUniform* ShaderUniformBlock::GetShaderUniform( const uint32_t uniformId ) {
		for( ShaderUniform& uniform : uniforms ) {
			if( uniform.id == uniformId ) {
				return &uniform;
			}
		}
		return nullptr;
	}

	const ShaderUniform* ShaderUniformBlock::GetShaderUniform( const uint32_t uniformId ) const {
		for( const ShaderUniform& uniform : uniforms ) {
			if( uniform.id == uniformId ) {
				return &uniform;
			}
		}
		return nullptr;
	}

	ShaderUniform* TechniqueResource::GetShaderUniform( const uint32_t uniformId ) {
		const uint32_t blockId = GetUniformBlockId( uniformId );
		if( blockId == 0xFFFFFFFF ) {
			return nullptr;
		}
		return uniformBlocks[blockId].GetShaderUniform( uniformId );
	}

	const ShaderUniform* TechniqueResource::GetShaderUniform( const uint32_t uniformId ) const {
		const uint32_t blockId = GetUniformBlockId( uniformId );
		if( blockId == 0xFFFFFFFF ) {
			return nullptr;
		}
		return uniformBlocks[blockId].GetShaderUniform( uniformId );
	}

	ShaderUniform* TechniqueResource::GetShaderUniform( const String& uniformName ) {
		return GetShaderUniform( GetHash( uniformName ) );
	}

	const ShaderUniform* TechniqueResource::GetShaderUniform( const String& uniformName ) const {
		return GetShaderUniform( GetHash( uniformName ) );
	}

	uint32_t TechniqueResource::GetUniformBlockId( const uint32_t uniformId ) const {
		const uint32_t* ptr = uniformIdToBlockId.Find( uniformId );
		if( ptr != nullptr ) {
			return *ptr;
		}
		return 0xFFFFFFFF;
	}

	uint32_t TechniqueResource::GetUniformBlockId( const String& uniformName ) const {
		return GetUniformBlockId( GetHash( uniformName ) );
	}

	Serializer& operator<<( Serializer& s, ShaderUniform& element ) {
		return s << element.name << element.offset << element.type << element.precision <<
			element.rows << element.columns << element.vectorSize << element.arraySize;
	}

	Serializer& operator>>( Serializer& s, ShaderUniform& element ) {
		return s >> element.name >> element.offset >> element.type >> element.precision >>
			element.rows >> element.columns >> element.vectorSize >> element.arraySize;
	}

	Serializer& operator<<( Serializer& s, ShaderUniformBlock& element ) {
		return s << element.size << element.set << element.binding << element.shaderStages <<= element.uniforms;
	}

	Serializer& operator>>( Serializer& s, ShaderUniformBlock& element ) {
		return s >> element.size >> element.set >> element.binding >> element.shaderStages >>= element.uniforms;
	}

	Serializer& operator<<( Serializer& s, TechniqueResource& technique ) {
		s << technique.vertexShader << technique.fragmentShader;
		s <<= technique.uniformBlocks;
		s <<= technique.samplers;
		s <<= technique.inputs;
		s << technique.uniformIdToBlockId;
		return s;
	}

	Serializer& operator>>( Serializer& s, TechniqueResource& technique ) {
		s >> technique.vertexShader >> technique.fragmentShader;
		s >>= technique.uniformBlocks;
		s >>= technique.samplers;
		s >>= technique.inputs;
		s >> technique.uniformIdToBlockId;
		return s;
	}
}
