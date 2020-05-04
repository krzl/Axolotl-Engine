#pragma once
#include "Resources/ResourceHandle.h"
#include "TechniqueResource.h"
#include "TextureResource.h"

namespace axlt {
	class TechniqueResource;
	class MaterialResource;

	namespace editor {
		MaterialResource* ImportMaterial( File& file, Array<Guid>& dependencies );
	}

	class MaterialResource {

		friend MaterialResource* editor::ImportMaterial( File& file, Array<Guid>& dependencies );

		friend Serializer& operator<<( Serializer& s, MaterialResource& material );
		friend Serializer& operator>>( Serializer& s, MaterialResource& material );

	public:

		MaterialResource() = default;
		
		MaterialResource( const MaterialResource& other );
		MaterialResource( MaterialResource&& other ) noexcept;

		MaterialResource& operator=( const MaterialResource& other );
		MaterialResource& operator=( MaterialResource&& other ) noexcept;

		ResourceHandle<TechniqueResource>& GetTechnique();
		const ResourceHandle<TechniqueResource>& GetTechnique() const;
		void SetTechnique( ResourceHandle<TechniqueResource>& technique );

		void SetUniform( const String& name, float value );
		void SetUniform( uint32_t id, float value );
		void SetUniform( const String& name, int value );
		void SetUniform( uint32_t id, int value );
		void SetUniform( const String& name, const Vector4& value );
		void SetUniform( uint32_t id, const Vector4& value );

		const void* GetUniformData( uint32_t index ) const;
		BitArray<> PopDirtyUniforms();

		const Map<uint32_t, ResourceHandle<TextureResource>>& GetTextureParameters() const;
		
		~MaterialResource();

	private:
		
		void UpdateUniform( uint32_t id, const void* data, uint8_t size );

		void ClearUniformData();
		void RecreateUniformData();
		
		ResourceHandle<TechniqueResource> technique;

		BitArray<> dirtyUniformBlocks;
		ExactArray<uint8_t*> uniformData;

		Map<uint32_t, float> floatParameters;
		Map<uint32_t, int32_t> intParameters;
		Map<uint32_t, Vector4> vectorParameters;
		Map<uint32_t, ResourceHandle<TextureResource>> textureParameters;

		DEFINE_TYPE_HASH( MaterialResource );
	};
}
