#pragma once

#include "Templates/IsEnum.h"

namespace axlt {
	namespace getTypeHashHidden {
		extern uint32_t typeHashCounter;
	}

	template<typename T>
	constexpr uint32_t GetTypeHash() {
		if constexpr ( IsEnum<T>::Value ) {
			return GetTypeHash<uint32_t>();
		} else {
			static uint32_t typeHash = ++getTypeHashHidden::typeHashCounter;
			return typeHash;
		}
	}
}
