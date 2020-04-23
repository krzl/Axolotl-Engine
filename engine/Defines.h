#pragma once

#include "Common/String.h"

#define DEFINE_TYPE_HASH( Type ) inline static uint32_t typeHash = GetTypeHash( #Type )
#define GET_TYPE_HASH( Type ) GetTypeHash( #Type )

namespace axlt {
	constexpr uint32_t GetTypeHash( const char* type ) {
		return GetHash( type );
	}
}
