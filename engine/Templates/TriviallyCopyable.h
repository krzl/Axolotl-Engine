#pragma once

#include <cstdint>

namespace axlt {
	template<typename T1, typename T2>
	struct TriviallyCopyable {
		enum { Value = false };
	};

	template<typename T>
	struct TriviallyCopyable<T, T> {
		enum { Value = __has_trivial_copy( T ) };
	};

	template<typename T, typename U>
	struct TriviallyCopyable<const T, U> : TriviallyCopyable<T, U> {};

	template<> struct TriviallyCopyable< uint8_t,	int8_t>		{ enum { Value = true }; };
	template<> struct TriviallyCopyable<  int8_t,	uint8_t>	{ enum { Value = true }; };
	template<> struct TriviallyCopyable<uint16_t,	int16_t>	{ enum { Value = true }; };
	template<> struct TriviallyCopyable< int16_t,	uint16_t>	{ enum { Value = true }; };
	template<> struct TriviallyCopyable<uint32_t,	int32_t>	{ enum { Value = true }; };
	template<> struct TriviallyCopyable< int32_t,	uint32_t>	{ enum { Value = true }; };
	template<> struct TriviallyCopyable<uint64_t,	int64_t>	{ enum { Value = true }; };
	template<> struct TriviallyCopyable< int64_t,	uint64_t>	{ enum { Value = true }; };
}