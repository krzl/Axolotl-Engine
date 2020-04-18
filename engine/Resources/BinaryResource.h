#pragma once
#include "Serialization/Serializer.h"

namespace axlt {

	class BinaryResource {

	public:

		Array<uint8_t, ExactHeapArrayAllocator> data;
	};

	Serializer& operator<<( Serializer& s, BinaryResource& resource );
	Serializer& operator>>( Serializer& s, BinaryResource& resource );
}