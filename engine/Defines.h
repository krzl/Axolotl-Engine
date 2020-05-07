#pragma once

#include "Common/String.h"

#define MATH_PI 3.141592653f

#define DEFINE_TYPE_HASH( Type ) public: inline static uint32_t typeHash = GetTypeHash( #Type )
#define GET_TYPE_HASH( Type ) GetTypeHash( #Type )

namespace axlt {
	constexpr uint32_t GetTypeHash( const char* type ) {
		return GetHash( type );
	}
}
