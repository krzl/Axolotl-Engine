#include "stdafx.h"
#include "BinaryResource.h"

namespace axlt {

	Serializer& operator<<( Serializer& s, BinaryResource& resource ) {
		s <<= resource.data;
		return s;
	}

	Serializer& operator>>( Serializer& s, BinaryResource& resource ) {
		s >>= resource.data;
		return s;
	}
}