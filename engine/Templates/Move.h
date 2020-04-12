#pragma once

#include "RemoveReference.h"

namespace axlt {
	template<typename T>
	__forceinline typename RemoveReference<T>::Type&& Move( T&& obj ) {
		typedef typename RemoveReference<T>::Type Type;
		return (Type&&) obj;
	}
}