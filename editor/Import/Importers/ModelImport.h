#pragma once

#include <Resources/Serialization/Serializable.h>

namespace axlt::editor {
	class ModelImport final : public Serializable {

	public:
		
		const SerializationInfo& GetSerializationData() const override {
			static SerializationInfo info = SerializationInfoBuilder<ModelImport>( "ModelImport" )
				.AddField( "storeMeshData", &ModelImport::storeMeshData )
				.Build();
			
			return info;
		}
		
		uint32_t GetTypeHash() const override {
			return axlt::GetTypeHash<ModelImport>();
		}

		bool storeMeshData = false;
	};
}
