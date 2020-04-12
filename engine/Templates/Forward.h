#pragma once

#include "RemoveReference.h"

namespace axlt {
	template <typename T>
	__forceinline T&& Forward( typename RemoveReference<T>::Type& obj ) {
		return (T&&) obj;
	}

	template <typename T>
	__forceinline T&& Forward( typename RemoveReference<T>::Type&& obj ) {
		return (T&&) obj;
	}
}