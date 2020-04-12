#pragma once

#include "AlignedBytes.h"
#include "vcruntime_string.h"

namespace axlt {
	template<typename T>
	struct TypeCompatibleBytes : AlignedBytes<sizeof( T ), alignof( T )> {

		TypeCompatibleBytes() {
			memset( this, 0, sizeof( T ) );
		}

		T& operator*() {
			return (T&) *this;
		}

		const T& operator*() const {
			return (const T&) *this;
		}
	};
}