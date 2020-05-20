#include "stdafx.h"
#include "MaterialResource.h"

namespace axlt {

	MaterialResource::MaterialResource( const MaterialResource& other ) {
		operator=( other );
	}

	MaterialResource::MaterialResource( MaterialResource&& other ) noexcept {
		operator=( Move( other ) );
	}

	MaterialResource& MaterialResource::operator=( const MaterialResource& other ) {
		if( this == &other ) return *this;
		technique = other.technique;

		floatParameters = other.floatParameters;
		intParameters = other.intParameters;
		vectorParameters = other.vectorParameters;

		RecreateUniformData();

		return *this;
	}

	MaterialResource& MaterialResource::operator=( MaterialResource&& other ) noexcept {
		technique = Move( other.technique );

		floatParameters = Move( other.floatParameters );
		intParameters = Move( other.intParameters );
		vectorParameters = Move( other.vectorParameters );

		dirtyUniformBlocks = Move( other.dirtyUniformBlocks );
		uniformData = Move( other.uniformData );

		return *this;
	}

	void MaterialResource::SetUniform( const String& name, const float value ) {
		SetUniform( GetHash( name ), value );
	}

	void MaterialResource::SetUniform( const uint32_t id, const float value ) {
		float* valuePtr = floatParameters.Find( id );
		if( valuePtr == nullptr ) {
			floatParameters.Add( id, value );
		} else {
			*valuePtr = value;
		}
		UpdateUniform( id, &value, sizeof( value ) );
	}

	void MaterialResource::SetUniform( const String& name, const int value ) {
		SetUniform( GetHash( name ), value );
	}

	void MaterialResource::SetUniform( const uint32_t id, const int value ) {
		int* valuePtr = intParameters.Find( id );
		if( valuePtr == nullptr ) {
			intParameters.Add( id, value );
		} else {
			*valuePtr = value;
		}
		UpdateUniform( id, &value, sizeof( value ) );
	}

	void MaterialResource::SetUniform( const String& name, const Vector4& value ) {
		SetUniform( GetHash( name ), value );
	}

	void MaterialResource::SetUniform( const uint32_t id, const Vector4& value ) {
		Vector4* valuePtr = vectorParameters.Find( id );
		if( valuePtr == nullptr ) {
			vectorParameters.Add( id, value );
		} else {
			*valuePtr = value;
		}
		UpdateUniform( id, &value, sizeof( value ) );
	}

	void MaterialResource::SetTexture( const String& name, const ResourceHandle<TextureResource>& texture ) {
		SetTexture( GetHash( name ), texture );
	}

	void MaterialResource::SetTexture( const uint32_t id, const ResourceHandle<TextureResource>& texture ) {
		auto* valuePtr = textureParameters.Find( id );
		if (valuePtr == nullptr) {
			textureParameters.Add( id, texture );
		}
		else {
			*valuePtr = texture;
		}
	}

	MaterialResource::~MaterialResource() {
		ClearUniformData();
	}

	ResourceHandle<TechniqueResource>& MaterialResource::GetTechnique() {
		return technique;
	}

	const ResourceHandle<TechniqueResource>& MaterialResource::GetTechnique() const {
		return technique;
	}

	void MaterialResource::SetTechnique( ResourceHandle<TechniqueResource>& technique ) {
		this->technique = technique;
		RecreateUniformData();
	}

	void MaterialResource::ClearUniformData() {
		dirtyUniformBlocks.Clear();
		for( uint8_t* data : uniformData ) {
			delete[] data;
		}
		uniformData.Clear();
	}

	void MaterialResource::RecreateUniformData() {
		ClearUniformData();

		const uint32_t uniformBlockCount = technique->GetShaderUniformBlockCount();

		dirtyUniformBlocks.AddEmpty( uniformBlockCount );
		uniformData.AddEmpty( uniformBlockCount );

		for( uint32_t i = 0; i < uniformBlockCount; i++ ) {
			dirtyUniformBlocks[i] = true;
			uniformData[i] = new uint8_t[technique->GetShaderUniformBlock( i ).size];
		}

		for( auto& pair : floatParameters ) {
			UpdateUniform( pair.key, &pair.value, sizeof( pair.value ) );
		}
		for( auto& pair : intParameters ) {
			UpdateUniform( pair.key, &pair.value, sizeof( pair.value ) );
		}
		for( auto& pair : vectorParameters ) {
			UpdateUniform( pair.key, &pair.value, sizeof( pair.value ) );
		}
	}

	void MaterialResource::UpdateUniform( const uint32_t id, const void* data, const uint8_t size ) {
		const uint32_t blockId = technique->GetUniformBlockId( id );
		if( blockId == 0xFFFFFFFF ) return;

		ShaderUniform& shaderUniform = *technique->GetShaderUniformBlock( blockId ).GetShaderUniform( id );

		memcpy( uniformData[blockId] + shaderUniform.offset, data, size );
		dirtyUniformBlocks[blockId] = true;
	}

	const void* MaterialResource::GetUniformData( const uint32_t index ) const {
		return uniformData[index];
	}

	BitArray<> MaterialResource::PopDirtyUniforms() {
		BitArray<> ret = dirtyUniformBlocks;
		for( uint32_t i = 0; i < dirtyUniformBlocks.GetSize(); i++ ) {
			dirtyUniformBlocks[ i ] = false;
		}
		return ret;
	}

	const Map<uint32_t, ResourceHandle<TextureResource>>& MaterialResource::GetTextureParameters() const {
		return textureParameters;
	}

	Serializer& operator<<( Serializer& s, MaterialResource& material ) {
		return s << material.technique << material.floatParameters << material.intParameters << material.vectorParameters << material.textureParameters;
	}

	Serializer& operator>>( Serializer& s, MaterialResource& material ) {
		s >> material.technique >> material.floatParameters >> material.intParameters >> material.vectorParameters >> material.textureParameters;
		material.RecreateUniformData();
		return s;
	}
}
