#pragma once

#include <cstdint>
#include "Templates/Relocatable.h"

namespace axlt {
	template<typename DestType, typename SrcType>
	void MoveElements( DestType* dest, const SrcType* src, uint32_t count ) {
		if constexpr( Relocatable<DestType, SrcType>::Value ) {
			memmove( (void*) dest, (void*) const_cast<SrcType*>( src ), sizeof( SrcType ) * count );
		} else {
			while( count ) {
				// ReSharper disable once CppNonReclaimedResourceAcquisition
				new ( dest++ ) DestType( *src );
				( src++ )->~SrcType();
				--count;
			}
		}
	}

	template<typename DestType, typename SrcType>
	void CopyElements( DestType* dest, const SrcType* src, uint32_t count ) {
		if constexpr( Relocatable<DestType, SrcType>::Value ) {
			memcpy( (void*) dest, (void*) const_cast<SrcType*>( src ), sizeof( SrcType ) * count );
		} else {
			while( count ) {
				// ReSharper disable once CppNonReclaimedResourceAcquisition
				new ( dest++ ) DestType( *src++ );
				--count;
			}
		}
	}
}