#pragma once
#include "Serialization/Serializer.h"
#include "Serialization/Serializable.h"
#include "Resource.h"

namespace axlt {

	class BinaryResource final : public Resource {

	public:

		const SerializationInfo& GetSerializationData() const override;
		uint32_t GetTypeHash() const override;

		Array<uint8_t, ExactHeapArrayAllocator> binaryData;
	};
}
