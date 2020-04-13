#include "stdafx.h"
#include "ResourceData.h"

namespace axlt {
	ResourceData::ResourceData( const Guid& guid, const uint32_t version, const uint32_t type, void* data ) :
		guid( guid ),
		version( version ),
		type( type ),
		data( data ) {}
}