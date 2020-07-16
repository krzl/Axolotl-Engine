#pragma once

#include <Resources/Serialization/Serializable.h>

namespace axlt::editor {
	class TechniqueImport final : public Serializable {

	public:

		const SerializationInfo& GetSerializationData() const override {
			static SerializationInfo info = SerializationInfoBuilder<TechniqueImport>( "TechniqueImport" )
				.Build();
			return info;
		}

		uint32_t GetTypeHash() const override {
			return axlt::GetTypeHash<TechniqueImport>();
		}
	};
}