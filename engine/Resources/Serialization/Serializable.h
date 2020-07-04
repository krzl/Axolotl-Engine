#pragma once

#include "SerializationInfo.h"
#include "Resources/Serialization/SerializationinfoBuilder.h"

namespace axlt {
	class Serializable {

	public:

		virtual ~Serializable() = default;
		
		virtual const SerializationInfo& GetSerializationData() const = 0;

		virtual void OnPostDeserialization() {}

		virtual uint32_t GetTypeHash() const = 0;
	};
}
 