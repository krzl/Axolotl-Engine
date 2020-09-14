#pragma once
#include "Resources/ResourceHandle.h"
#include "Resources/BinaryResource.h"
#include "Collections/Map.h"
#include "Resources/Resource.h"

namespace axlt {
	class BinaryResource;

	enum class ShaderType {
		Float,
		Double,
		Int,
		UInt,
		Bool,
		Unsupported
	};

	enum class ShaderPrecision {
		Full,
		Half,
		Low,
		None
	};

	enum class ShaderStage {
		VERTEX = 1,
		TESS_CONTROL = 2,
		TESS_EVAL = 4,
		GEOMETRY = 8,
		FRAGMENT = 16,
		COMPUTE = 32,
		ALL = 63
	};

	enum class ShaderCullMode {
		NONE = 0,
		FRONT = 1,
		BACK = 2,
		FRONT_AND_BACK = 3
	};

	enum class ShaderCompareOperation {
		NEVER,
		LESS,
		EQUAL,
		L_EQUAL,
		GREATER,
		NOT_EQUAL,
		G_EQUAL,
		ALWAYS
	};

	enum class ShaderStencilOperation {
		KEEP,
		ZERO,
		REPLACE,
		INCREMENT_CLAMP,
		DECREMENT_CLAMP,
		INVERT,
		INCREMENT_WRAP,
		DECREMENT_WRAP
	};

	class ShaderStencilInfo final : public Serializable {

	public:
		
		const SerializationInfo& GetSerializationData() const override;
		uint32_t GetTypeHash() const override;

		ShaderStencilOperation failOp = ShaderStencilOperation::KEEP;
		ShaderStencilOperation passOp = ShaderStencilOperation::KEEP;
		ShaderStencilOperation depthFailOp = ShaderStencilOperation::KEEP;
		ShaderCompareOperation compareOp = ShaderCompareOperation::NEVER;
		uint8_t compareMask = 0;
		uint8_t writeMask = 0;
		uint8_t reference = 0;
	};

	enum class ShaderBlendFactor {
		ZERO,
		ONE,
		SRC_COLOR,
		ONE_MINUS_SRC_COLOR,
		DST_COLOR,
		ONE_MINUS_DST_COLOR,
		SRC_ALPHA,
		ONE_MINUS_SRC_ALPHA,
		DST_ALPHA,
		ONE_MINUS_DST_ALPHA
	};

	enum class ShaderBlendOperation {
		ADD,
		SUBTRACT,
		REVERSE_SUBTRACT,
		MIN,
		MAX
	};

	class ShaderBlendInfo final : public Serializable {

	public:
		const SerializationInfo& GetSerializationData() const override;
		uint32_t GetTypeHash() const override;

		ShaderBlendFactor colorSrcBlendFactor = ShaderBlendFactor::SRC_ALPHA;
		ShaderBlendFactor colorDstBlendFactor = ShaderBlendFactor::ONE_MINUS_SRC_ALPHA;
		ShaderBlendOperation colorBlendOperation = ShaderBlendOperation::ADD;
		ShaderBlendFactor alphaSrcBlendFactor = ShaderBlendFactor::SRC_ALPHA;
		ShaderBlendFactor alphaDstBlendFactor = ShaderBlendFactor::ONE_MINUS_SRC_ALPHA;
		ShaderBlendOperation alphaBlendOperation = ShaderBlendOperation::ADD;
	};

	class ShaderSampler {

	public:

		uint32_t id;
		uint8_t set;
		uint8_t binding;
		uint8_t count;
		ShaderStage shaderStages;
	};

	class ShaderUniform final : public Serializable {

	public:
		
		const SerializationInfo& GetSerializationData() const override;
		uint32_t GetTypeHash() const override;

		uint32_t id = 0;
		String name;
		uint32_t offset = 0;
		ShaderType type = ShaderType::Unsupported;
		ShaderPrecision precision = ShaderPrecision::None;
		uint8_t rows = 0;
		uint8_t columns = 0;
		uint8_t vectorSize = 0;
		uint8_t arraySize = 0;
	};

	class ShaderUniformBlock final : public Serializable {

	public:

		ShaderUniformBlock() = default;

		ShaderUniformBlock( const uint32_t size, const uint8_t set, const uint8_t binding, const ShaderStage shaderStages ) :
			size( size ),
			set( set ),
			binding( binding ),
			shaderStages( shaderStages ) {}

		ShaderUniform* GetShaderUniform( uint32_t uniformId );
		const ShaderUniform* GetShaderUniform( uint32_t uniformId ) const;

		const SerializationInfo& GetSerializationData() const override;
		uint32_t GetTypeHash() const override;

		uint32_t size = 0;
		uint8_t set = 0;
		uint8_t binding = 0;
		ShaderStage shaderStages = ShaderStage::ALL;
		ExactArray<ShaderUniform> uniforms{};
		
	private:

		Map<uint32_t, uint32_t> uniformIdToBlockId;
	};

	class ShaderInputElement {

	public:

		uint8_t location;
		uint8_t vectorSize;
		ShaderType type;
		ShaderPrecision precision;
	};

	class TechniqueResource;
	class MaterialResource;

	namespace vk {
		class TechniqueBinding;
	}

	namespace editor {
		class TechniqueImport;
		TechniqueResource* TechniqueImporter( const File&, TechniqueImport&, Array<Guid>&);
	}

	class TechniqueResource final : public Resource {

		friend class vk::TechniqueBinding;
		friend TechniqueResource* editor::TechniqueImporter( const File&, editor::TechniqueImport&, Array<Guid>& );
		
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

		uint32_t GetShaderInputsCount() const;
		const ShaderInputElement& GetShaderInput( uint32_t index ) const;

		const SerializationInfo& GetSerializationData() const override;
		uint32_t GetTypeHash() const override;

		bool isDirty = true;

	private:

		ResourceHandle<BinaryResource> vertexShader;
		ResourceHandle<BinaryResource> fragmentShader;

		ExactArray<ShaderUniformBlock> uniformBlocks;
		ExactArray<ShaderSampler> samplers;
		ExactArray<ShaderInputElement> inputs;

		Map<uint32_t, uint32_t> uniformIdToBlockId;
		Map<uint32_t, uint32_t> textureIdToSamplerId;

		bool depthClampEnabled = false;
		ShaderCullMode cullMode = ShaderCullMode::BACK;
		bool depthBiasEnabled = false;
		float depthBiasConstantFactor = 0.0f;
		float depthBiasClamp = 0.0f;
		float depthBiasSlopeFactor = 0.0f;

		uint8_t rasterizationSamples = 1;
		bool alphaToCoverage = false;

		bool depthTestEnabled = true;
		bool depthWriteEnabled = true;
		ShaderCompareOperation depthTestOp = ShaderCompareOperation::L_EQUAL;

		bool stencilTestEnabled = false;
		ShaderStencilInfo frontStencilOperation{};
		ShaderStencilInfo backStencilOperation{};

		bool blendEnable = false;
		ShaderBlendInfo blendInfo{};
		uint8_t colorMask = 15;
	};
}