#pragma once

namespace axlt {

	template<typename T>
	T Clamp( const T& min, const T& max, const T& value ) {
		return min > value ? min : ( max < value ? max : value );
	}
}