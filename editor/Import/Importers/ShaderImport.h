#pragma once

#include <Resources/Serialization/Serializable.h>

namespace axlt::editor {
	class ShaderImport final : public Serializable {

	public:

		const SerializationInfo& GetSerializationData() const override {
			static SerializationInfo info = SerializationInfoBuilder<ShaderImport>( "ShaderImport" )
				.Build();
			return info;
		}

		uint32_t GetTypeHash() const override {
			return axlt::GetTypeHash<ShaderImport>();
		}
	};
}
