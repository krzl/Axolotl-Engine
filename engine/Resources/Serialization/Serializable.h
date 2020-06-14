#pragma once
#include "SerializationInfo.h"
#include "Resources/Serialization/SerializationinfoBuilder.h"

namespace axlt {
	class Serializable {

	public:

		virtual ~Serializable() = default;
		
		virtual const SerializationInfo& GetSerializationData() {
			static SerializationInfo info  = SerializationInfoBuilder<Serializable>( String( "Unknown class" ) ).Build();
			return info;
		}

		virtual uint32_t GetTypeHash() = 0;
	};
}
 