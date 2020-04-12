#include "stdafx.h"
#include "Random.h"
#include <time.h>

namespace axlt {

	uint8_t RandMaxBytesCount() {

		srand( (unsigned int) time( 0 ) );  // NOLINT(cert-msc32-c, cert-msc51-cpp)

		uint64_t byteCheck = RAND_MAX;
		uint64_t bytes;
		for( bytes = 0; byteCheck != 0; bytes++ ) {
			byteCheck = byteCheck >> 1;
		}

		return (uint8_t) bytes;
	}

	template<typename T>
	T RandomNumber() {

		static uint8_t maxBytesPerRand = RandMaxBytesCount();

		T result = 0;

		for( uint32_t i = 0; i < sizeof( T ) * 8; i += maxBytesPerRand ) {
			result <<= maxBytesPerRand;
			result |= rand();
		}

		return result;
	}
}
