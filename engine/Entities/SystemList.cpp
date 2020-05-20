#include "stdafx.h"
#include "SystemList.h"

namespace axlt::Hidden_System {

	void AddToSystemList( SystemBase* system, const int32_t order ) {
		auto& systems = GetSystemList();
		uint32_t insertIndex = systems.GetSize();
		//TODO: Create ordered array, use that instead
		for (uint32_t i = 0; i < systems.GetSize(); i++) {
			if (systems[ i ].order > order) {
				insertIndex = i;
				break;
			}
		}
		const SystemListElement systemListElement {
			system,
			order
		};
		systems.Insert( systemListElement, insertIndex );
	}

	Array<SystemListElement>& GetSystemList() {
		static Array<SystemListElement> systems;
		return systems;
	}
}
