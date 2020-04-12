#pragma once

#include <cstdint>

namespace axlt {
	template<typename T, uint32_t... Indices>
	struct IntegerSequence {};

	template<typename T, T N>
	using MakeIntegerSequence = __make_integer_seq<IntegerSequence, T, N>;
}