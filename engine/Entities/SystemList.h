#pragma once

namespace axlt {
	class SystemBase;
}

namespace axlt::Hidden_System {

	struct SystemListElement {
		SystemBase* system;
		int32_t order;
	};

	void AddToSystemList( SystemBase* system, int32_t order );
	Array<SystemListElement>& GetSystemList();
}
