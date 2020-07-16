#include "stdafx.h"
#include "ImportInfo.h"

namespace axlt::editor {

	const SerializationInfo& ImportInfo::GetSerializationData() const {
		static SerializationInfo info = SerializationInfoBuilder<ImportInfo>( "ImportInfo" )
			.AddField( "resourceHash", &ImportInfo::resourceHash )
			.AddField( "importHash", &ImportInfo::importHash )
			.AddField( "importVersion", &ImportInfo::importVersion )
			.AddField( "dependencies", &ImportInfo::dependencies )
			.Build();
		return info;
	}

	uint32_t ImportInfo::GetTypeHash() const {
		return axlt::GetTypeHash<ImportInfo>();
	}
}
