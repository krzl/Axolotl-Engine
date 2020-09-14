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

	const SerializationInfo& ShaderStencilInfo::GetSerializationData() const {
		static SerializationInfo info = SerializationInfoBuilder<ShaderStencilInfo>( "ShaderStencilInfo" )
			.AddField( "failOp", &ShaderStencilInfo::failOp )
			.AddField( "passOp", &ShaderStencilInfo::passOp )
			.AddField( "depthFailOp", &ShaderStencilInfo::depthFailOp )
			.AddField( "compareOp", &ShaderStencilInfo::compareOp )
			.AddField( "compareMask", &ShaderStencilInfo::compareMask )
			.AddField( "writeMask", &ShaderStencilInfo::writeMask )
			.AddField( "reference", &ShaderStencilInfo::reference )
			.Build();
		return info;
	}

	uint32_t ShaderStencilInfo::GetTypeHash() const {
		return axlt::GetTypeHash<ShaderStencilInfo>();
	}

	const SerializationInfo& ShaderBlendInfo::GetSerializationData() const {
		static SerializationInfo info = SerializationInfoBuilder<ShaderBlendInfo>( "ShaderBlendInfo" )
			.AddField( "colorSrcBlendFactor", &ShaderBlendInfo::colorSrcBlendFactor )
			.AddField( "colorDstBlendFactor", &ShaderBlendInfo::colorDstBlendFactor )
			.AddField( "colorBlendOperation", &ShaderBlendInfo::colorBlendOperation )
			.AddField( "alphaSrcBlendFactor", &ShaderBlendInfo::alphaSrcBlendFactor )
			.AddField( "alphaDstBlendFactor", &ShaderBlendInfo::alphaDstBlendFactor )
			.AddField( "alphaBlendOperation", &ShaderBlendInfo::alphaBlendOperation )
			.Build();
		return info;
	}

	uint32_t ShaderBlendInfo::GetTypeHash() const {
		return axlt::GetTypeHash<ShaderBlendInfo>();
	}

	const SerializationInfo& ShaderUniform::GetSerializationData() const {
		static SerializationInfo info = SerializationInfoBuilder<ShaderUniform>( "ShaderUniform" )
			.AddField( "id", &ShaderUniform::id )
			.AddField( "name", &ShaderUniform::name )
			.AddField( "offset", &ShaderUniform::offset )
			.AddField( "type", &ShaderUniform::type )
			.AddField( "precision", &ShaderUniform::precision )
			.AddField( "rows", &ShaderUniform::rows )
			.AddField( "columns", &ShaderUniform::columns )
			.AddField( "vectorSize", &ShaderUniform::vectorSize )
			.AddField( "arraySize", &ShaderUniform::arraySize )
			.Build();
		return info;
	}

	uint32_t ShaderUniform::GetTypeHash() const {
		return axlt::GetTypeHash<ShaderUniform>();
	}

	const SerializationInfo& ShaderUniformBlock::GetSerializationData() const {
		static SerializationInfo info = SerializationInfoBuilder<ShaderUniformBlock>( "ShaderUniformBlock" )
			.AddField( "size", &ShaderUniformBlock::size )
			.AddField( "set", &ShaderUniformBlock::set )
			.AddField( "binding", &ShaderUniformBlock::binding )
			.AddField( "shaderStages", &ShaderUniformBlock::shaderStages )
			.AddField( "uniforms", &ShaderUniformBlock::uniforms )
			.Build();
		return info;
	}

	uint32_t ShaderUniformBlock::GetTypeHash() const {
		return axlt::GetTypeHash<ShaderUniformBlock>();
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

	ShaderUniformBlock& TechniqueResource::GetShaderUniformBlock( const uint32_t index ) {
		return uniformBlocks[index];
	}

	const ShaderUniformBlock& TechniqueResource::GetShaderUniformBlock( const uint32_t index ) const {
		return uniformBlocks[index];
	}

	uint32_t TechniqueResource::GetShaderUniformBlockCount() const {
		return uniformBlocks.GetSize();
	}

	ShaderSampler* TechniqueResource::GetShaderSampler( const uint32_t textureId ) {
		const uint32_t* ptr = textureIdToSamplerId.Find( textureId );
		if( ptr != nullptr ) {
			return &samplers[*ptr];
		}
		return nullptr;
	}

	const ShaderSampler* TechniqueResource::GetShaderSampler( const uint32_t textureId ) const {
		const uint32_t* ptr = textureIdToSamplerId.Find( textureId );
		if( ptr != nullptr ) {
			return &samplers[*ptr];
		}
		return nullptr;
	}

	uint32_t TechniqueResource::GetShaderSamplerCount() const {
		return samplers.GetSize();
	}

	uint32_t TechniqueResource::GetShaderInputsCount() const {
		return inputs.GetSize();
	}

	const ShaderInputElement& TechniqueResource::GetShaderInput( const uint32_t index ) const {
		return inputs[ index ];
	}

	const SerializationInfo& TechniqueResource::GetSerializationData() const {
		static SerializationInfo info = SerializationInfoBuilder<TechniqueResource>( "TechniqueResource" )
			.AddField( "vertexShader", &TechniqueResource::vertexShader )
			.AddField( "fragmentShader", &TechniqueResource::fragmentShader )
			.AddField( "uniformBlocks", &TechniqueResource::uniformBlocks )
			.AddField( "samplers", &TechniqueResource::samplers )
			.AddField( "inputs", &TechniqueResource::inputs )
			.AddField( "uniformIdToBlockId", &TechniqueResource::uniformIdToBlockId )
			.AddField( "textureIdToSamplerId", &TechniqueResource::textureIdToSamplerId )
			.AddField( "depthClampEnabled", &TechniqueResource::depthClampEnabled )
			.AddField( "cullMode", &TechniqueResource::cullMode )
			.AddField( "depthBiasEnabled", &TechniqueResource::depthBiasEnabled )
			.AddField( "depthBiasConstantFactor", &TechniqueResource::depthBiasConstantFactor )
			.AddField( "depthBiasClamp", &TechniqueResource::depthBiasClamp )
			.AddField( "depthBiasSlopeFactor", &TechniqueResource::depthBiasSlopeFactor )
			.AddField( "rasterizationSamples", &TechniqueResource::rasterizationSamples )
			.AddField( "alphaToCoverage", &TechniqueResource::alphaToCoverage )
			.AddField( "depthTestEnabled", &TechniqueResource::depthTestEnabled )
			.AddField( "depthWriteEnabled", &TechniqueResource::depthWriteEnabled )
			.AddField( "depthTestOp", &TechniqueResource::depthTestOp )
			.AddField( "stencilTestEnabled", &TechniqueResource::stencilTestEnabled )
			.AddField( "frontStencilOperation", &TechniqueResource::frontStencilOperation )
			.AddField( "backStencilOperation", &TechniqueResource::backStencilOperation )
			.AddField( "blendEnable", &TechniqueResource::blendEnable )
			.AddField( "blendInfo", &TechniqueResource::blendInfo )
			.AddField( "colorMask", &TechniqueResource::colorMask )
			.Build();
		
		return info;
	}

	uint32_t TechniqueResource::GetTypeHash() const {
		return axlt::GetTypeHash<TechniqueResource>();
	}
}
