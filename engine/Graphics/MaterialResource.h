#pragma once
#include "Resources/ResourceHandle.h"
#include "TechniqueResource.h"
#include "TextureResource.h"
#include "Resources/Serialization/Serializable.h"

namespace axlt {
	class TechniqueResource;
	class MaterialResource;

	namespace editor {
		class MaterialImport;
		MaterialResource* MaterialImporter( const File& file, MaterialImport& import, Array<Guid>& dependencies );
	}

	class MaterialResource final : public Resource {

		friend MaterialResource* editor::MaterialImporter( const File& file, editor::MaterialImport& import, Array<Guid>& dependencies );

	public:

		MaterialResource() = default;
		
		MaterialResource( const MaterialResource& other );
		MaterialResource( MaterialResource&& other ) noexcept;

		~MaterialResource();

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

		void SetTexture( const String& name, const ResourceHandle<TextureResource>& texture );
		void SetTexture( uint32_t id, const ResourceHandle<TextureResource>& texture );
		
		const void* GetUniformData( uint32_t index ) const;
		BitArray<> PopDirtyUniforms();

		const Map<uint32_t, ResourceHandle<TextureResource>>& GetTextureParameters() const;

		const SerializationInfo& GetSerializationData() const override;
		void OnPostDeserialization() override;
		uint32_t GetTypeHash() const override;

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
	};
}
