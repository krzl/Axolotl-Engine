#pragma once

#include <Resources/Serialization/Serializable.h>

namespace axlt::editor {
	class TextureImport final : public Serializable {

	public:

		const SerializationInfo& GetSerializationData() const override {
			static SerializationInfo info = SerializationInfoBuilder<TextureImport>( "TextureImport" )
				.Build();
			return info;
		}

		uint32_t GetTypeHash() const override {
			return axlt::GetTypeHash<TextureImport>();
		}
	};
}