#pragma once

#include <Resources/Serialization/Serializable.h>
#include <Graphics/TextureResource.h>

namespace axlt::editor {
	class TextureImport final : public Serializable {

	public:

		const SerializationInfo& GetSerializationData() const override {
			static SerializationInfo info = SerializationInfoBuilder<TextureImport>( "TextureImport" )
				.AddField( "format", &TextureImport::format )
				.AddField( "sampleCount", &TextureImport::sampleCount )
				.AddField( "generateMipmaps", &TextureImport::generateMipmaps )
				.AddField( "magFilter", &TextureImport::magFilter )
				.AddField( "mipmapFilterLinear", &TextureImport::mipmapFilterLinear )
				.AddField( "addressModeU", &TextureImport::addressModeU )
				.AddField( "addressModeV", &TextureImport::addressModeV )
				.AddField( "addressModeW", &TextureImport::addressModeW )
				.AddField( "mipLodBias", &TextureImport::mipLodBias )
				.AddField( "anisotropicFiltering", &TextureImport::anisotropicFiltering )
				.Build();
			
			return info;
		}

		uint32_t GetTypeHash() const override {
			return axlt::GetTypeHash<TextureImport>();
		}
		
		TextureFormat format = TextureFormat::UNKNOWN;

		bool generateMipmaps = false;
		uint8_t sampleCount = 1;

		TextureFiltering minFilter = TextureFiltering::LINEAR;
		TextureFiltering magFilter = TextureFiltering::LINEAR;

		bool mipmapFilterLinear = true;

		TextureAddressMode addressModeU = TextureAddressMode::REPEAT;
		TextureAddressMode addressModeV = TextureAddressMode::REPEAT;
		TextureAddressMode addressModeW = TextureAddressMode::REPEAT;

		float mipLodBias = 0.0f;
		float anisotropicFiltering = 1.0f;
	};
}
