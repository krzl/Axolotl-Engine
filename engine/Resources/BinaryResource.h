#pragma once
#include "Serialization/Serializer.h"

namespace axlt {

	class BinaryResource {

	public:

		Array<uint8_t, ExactHeapArrayAllocator> data;

		DEFINE_TYPE_HASH( BinaryResource );
	};

	Serializer& operator<<( Serializer& s, BinaryResource& resource );
	Serializer& operator>>( Serializer& s, BinaryResource& resource );
}