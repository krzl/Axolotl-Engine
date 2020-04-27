#include "stdafx.h"
#include "BinaryResource.h"

namespace axlt {

	Serializer& operator<<( Serializer& s, BinaryResource& resource ) {
		s <<= resource.binaryData;
		return s;
	}

	Serializer& operator>>( Serializer& s, BinaryResource& resource ) {
		s >>= resource.binaryData;
		return s;
	}
}