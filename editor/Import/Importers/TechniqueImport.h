#pragma once

#include <Resources/Serialization/Serializable.h>
#include <Graphics/TechniqueResource.h>

namespace axlt::editor {
	class TechniqueImport final : public Serializable {

	public:

		const SerializationInfo& GetSerializationData() const override {
			static SerializationInfo info = SerializationInfoBuilder<TechniqueImport>( "TechniqueImport" )
				.AddField( "depthClampEnabled", &TechniqueImport::depthClampEnabled )
				.AddField( "cullMode", &TechniqueImport::cullMode )
				.AddField( "depthBiasEnabled", &TechniqueImport::depthBiasEnabled )
				.AddField( "depthBiasConstantFactor", &TechniqueImport::depthBiasConstantFactor )
				.AddField( "depthBiasClamp", &TechniqueImport::depthBiasClamp )
				.AddField( "depthBiasSlopeFactor", &TechniqueImport::depthBiasSlopeFactor )
				.AddField( "rasterizationSamples", &TechniqueImport::rasterizationSamples )
				.AddField( "alphaToCoverage", &TechniqueImport::alphaToCoverage )
				.AddField( "depthTestEnabled", &TechniqueImport::depthTestEnabled )
				.AddField( "depthWriteEnabled", &TechniqueImport::depthWriteEnabled )
				.AddField( "depthTestOp", &TechniqueImport::depthTestOp )
				.AddField( "stencilTestEnabled", &TechniqueImport::stencilTestEnabled )
				.AddField( "frontStencilOperation", &TechniqueImport::frontStencilOperation )
				.AddField( "backStencilOperation", &TechniqueImport::backStencilOperation )
				.AddField( "blendEnable", &TechniqueImport::blendEnable )
				.AddField( "blendInfo", &TechniqueImport::blendInfo )
				.AddField( "colorMask", &TechniqueImport::colorMask )
				.Build();
			
			return info;
		}

		uint32_t GetTypeHash() const override {
			return axlt::GetTypeHash<TechniqueImport>();
		}

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