#include "stdafx.h"
#include "Guid.h"
#include "Common/Random.h"

namespace axlt {

	const Guid Guid::invalidGuid = Guid( 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF );

	Guid::Guid( const uint64_t high, const uint64_t low ) :
		high( high ),
		low( low ) {}

	Guid Guid::GenerateGuid() {
		Guid generated( RandomNumber<uint64_t>(), RandomNumber<uint64_t>() );
		while( generated.high == 0xFFFFFFFFFFFFFFFF && generated.low == 0xFFFFFFFFFFFFFFFF ) {
			generated.high = RandomNumber<uint64_t>();
			generated.low = RandomNumber<uint64_t>();
		}
		return generated;
	}

	Guid Guid::FromString( const String& string ) {
		char* endPtr;
		const uint64_t high = strtoull( string.Substring( 0, 16 ).GetData(), &endPtr, 16 );
		const uint64_t low = strtoull( string.GetData() + 16, &endPtr, 16 );
		return Guid( high, low );
	}

	String Guid::ToString() const {
		String guidString = "00000000000000000000000000000000";

		snprintf(
			&( guidString[16 * 0] ),
			17,
			"%016llx",
			high
		);
		snprintf(
			&( guidString[16 * 1] ),
			17,
			"%016llx",
			low
		);

		return guidString;
	}

	uint32_t GetHash( const Guid& guid ) {
		uint32_t resultHash;
		HashCombine(
			resultHash,
			(uint32_t) ( guid.high & 0xFFFFFFFF00000000 >> 32 ),
			(uint32_t) ( guid.high & 0xFFFFFFFF ),
			(uint32_t) ( guid.low & 0xFFFFFFFF00000000 >> 32 ),
			(uint32_t) ( guid.low & 0xFFFFFFFF )
		);
		return resultHash;
	}

	bool Guid::operator==( const Guid& other ) const {
		return high == other.high && low == other.low;
	}
	
	bool Guid::operator!=( const Guid& other ) const {
		return high != other.high || low != other.low;
	}
}
