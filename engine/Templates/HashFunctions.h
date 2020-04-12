#pragma once

#include <cstdint>
#include <typeindex>

namespace axlt {

	inline uint32_t GetHash( const void* ptr ) {
		return (uint32_t) ( (uint64_t) ptr >> 4 );
	}

	inline uint32_t GetHash( const uint8_t val ) {
		return val;
	}

	inline uint32_t GetHash( const int8_t val ) {
		return val;
	}

	inline uint32_t GetHash( const uint16_t val ) {
		return val;
	}

	inline uint32_t GetHash( const int16_t val ) {
		return val;
	}

	inline uint32_t GetHash( const uint32_t val ) {
		return val;
	}

	inline uint32_t GetHash( const int32_t val ) {
		return val;
	}

	inline uint32_t GetHash( const uint64_t val ) {
		return (uint32_t) val + ( (uint32_t) ( val >> 32 ) * 23 );
	}

	inline uint32_t GetHash( const int64_t val ) {
		return (uint32_t) val + ( (uint32_t) ( val >> 32 ) * 23 );
	}

	inline uint32_t GetHash( const float val ) {
		return *(uint32_t*) &val;
	}

	inline uint32_t GetHash( const double val ) {
		return GetHash( *(uint64_t*) &val );
	}

	inline void HashCombine( std::uint32_t& seed ) {}

	template <typename T, typename... Rest>
	inline void HashCombine( std::uint32_t& seed, const T& v, Rest... rest ) {
		seed ^= GetHash( v ) + 0x9e3779b9 + ( seed << 6 ) + ( seed >> 2 );
		HashCombine( seed, rest... );
	}
}
