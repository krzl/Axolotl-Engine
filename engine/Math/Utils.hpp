#pragma once

namespace axlt {

	template<typename T>
	T Clamp( const T min, const T max, const T value ) {
		return min > value ? min : ( max < value ? max : value );
	}

	template<typename T>
	T Max( const T a, const T b ) {
		return a > b ? a : b;
	}

	template<typename T>
	T Min( const T a, const T b ) {
		return a < b ? a : b;
	}
}