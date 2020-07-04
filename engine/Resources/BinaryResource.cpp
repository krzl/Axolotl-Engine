#include "stdafx.h"
#include "BinaryResource.h"

namespace axlt {

	const SerializationInfo& BinaryResource::GetSerializationData() const {
		static SerializationInfo info = SerializationInfoBuilder<BinaryResource>( "BinaryResource" )
			.AddField( "binaryData", &BinaryResource::binaryData )
			.Build();
		return info;
	}

	uint32_t BinaryResource::GetTypeHash() const {
		return axlt::GetTypeHash<BinaryResource>();
	}
}