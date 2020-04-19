#include "stdafx.h"
#include "MaterialResource.h"

namespace axlt {

	Serializer& operator<<( Serializer& s, MaterialResource& material ) {
		return s << material.technique;
	}

	Serializer& operator>>( Serializer& s, MaterialResource& material ) {
		return s >> material.technique;
	}
}
