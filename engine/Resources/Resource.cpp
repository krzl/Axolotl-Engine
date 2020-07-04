#include "stdafx.h"
#include "Resource.h"

namespace axlt {

	Resource::Resource() :
		guid( Guid::invalidGuid ) {}

	Resource::Resource( const Guid& guid ) :
		guid( guid ) {}

}