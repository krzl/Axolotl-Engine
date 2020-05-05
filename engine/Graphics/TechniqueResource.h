#pragma once
#include "Resources/ResourceHandle.h"
#include "Resources/BinaryResource.h"
#include "Collections/Map.h"

namespace axlt {
	class BinaryResource;

	enum ShaderType {
		Float,
		Double,
		Int,
		UInt,
		Bool,
		Unsupported
	};

	enum ShaderPrecision {
		Full,
		Half,
		Low,
		None
	};

	enum ShaderStage {
		VERTEX = 1,
		TESS_CONTROL = 2,
		TESS_EVAL = 4,
		GEOMETRY = 8,
		FRAGMENT = 16,
		COMPUTE = 32,
		ALL = 63
	};

	class ShaderSampler {

	public:

		uint32_t id;
		uint8_t set;
		uint8_t binding;
		uint8_t count;
		ShaderStage shaderStages;
	};

	class ShaderUniform {

		friend Serializer& operator<<( Serializer& s, ShaderUniform& element );
		friend Serializer& operator>>( Serializer& s, ShaderUniform& element );

	public:

		uint32_t id;
		String name;
		uint32_t offset;
		ShaderType type;
		ShaderPrecision precision;
		uint8_t rows;
		uint8_t columns;
		uint8_t vectorSize;
		uint8_t arraySize;
	};

	class ShaderUniformBlock {

		friend Serializer& operator<<( Serializer& s, ShaderUniformBlock& element );
		friend Serializer& operator>>( Serializer& s, ShaderUniformBlock& element );

	public:

		uint32_t size;
		uint8_t set;
		uint8_t binding;
		ShaderStage shaderStages;
		ExactArray<ShaderUniform> uniforms{};

		ShaderUniformBlock( const uint32_t size, const uint8_t set, const uint8_t binding, const ShaderStage shaderStages ) :
			size( size ),
			set( set ),
			binding( binding ),
			shaderStages( shaderStages ) {}

		ShaderUniform* GetShaderUniform( uint32_t uniformId );
		const ShaderUniform* GetShaderUniform( uint32_t uniformId ) const;

	private:

		Map<uint32_t, uint32_t> uniformIdToBlockId{};
	};

	class ShaderInputElement {

	public:

		uint8_t location;
		uint8_t vectorSize;
		uint32_t stride;
		ShaderType type;
		ShaderPrecision precision;
	};

	class TechniqueResource;
	class MaterialResource;

	namespace editor {
		TechniqueResource* ImportTechnique( File& file, Array<Guid>& dependencies );
	}

	namespace vk {
		struct MaterialData;
		struct TechniqueData;
		
		void CreateTechniqueData( const ResourceHandle<TechniqueResource>& technique );
		MaterialData* CreateMaterialData( ResourceHandle<MaterialResource>& material, TechniqueData& techniqueData );
	}

	class TechniqueResource {

		friend TechniqueResource* editor::ImportTechnique( File& file, Array<Guid>& dependencies );
		friend void vk::CreateTechniqueData( const ResourceHandle<TechniqueResource>& technique );
		friend vk::MaterialData* vk::CreateMaterialData( ResourceHandle<MaterialResource>& material, vk::TechniqueData& techniqueData );

		friend Serializer& operator<<( Serializer& s, TechniqueResource& technique );
		friend Serializer& operator>>( Serializer& s, TechniqueResource& technique );

	public:

		ShaderUniform* GetShaderUniform( uint32_t uniformId );
		const ShaderUniform* GetShaderUniform( uint32_t uniformId ) const;
		ShaderUniform* GetShaderUniform( const String& uniformName );
		const ShaderUniform* GetShaderUniform( const String& uniformName ) const;

		uint32_t GetUniformBlockId( uint32_t uniformId ) const;
		uint32_t GetUniformBlockId( const String& uniformName ) const;

		ShaderUniformBlock& GetShaderUniformBlock( uint32_t index );
		const ShaderUniformBlock& GetShaderUniformBlock( uint32_t index ) const;
		uint32_t GetShaderUniformBlockCount() const;

		ShaderSampler* GetShaderSampler( uint32_t textureId );
		const ShaderSampler* GetShaderSampler( uint32_t textureId ) const;
		uint32_t GetShaderSamplerCount() const;

	private:

		ResourceHandle<BinaryResource> vertexShader;
		ResourceHandle<BinaryResource> fragmentShader;

		ExactArray<ShaderUniformBlock> uniformBlocks;
		ExactArray<ShaderSampler> samplers;
		ExactArray<ShaderInputElement> inputs;

		Map<uint32_t, uint32_t> uniformIdToBlockId;
		Map<uint32_t, uint32_t> textureIdToSamplerId;

		DEFINE_TYPE_HASH( TechniqueResource );
	};
}
