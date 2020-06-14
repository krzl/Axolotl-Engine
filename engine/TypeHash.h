#pragma once

namespace axlt {
	namespace getTypeHashHidden {
		extern uint32_t typeHashCounter;
	}

	template<typename T>
	constexpr uint32_t GetTypeHash() {
		static uint32_t typeHash = ++getTypeHashHidden::typeHashCounter;
		return typeHash;
	}
}
