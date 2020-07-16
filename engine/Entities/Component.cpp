#include "stdafx.h"
#include "Component.h"

namespace axlt {
	Array<BaseComponentHelper*>& BaseComponentHelper::GetComponentHelpers() {
		static Array<BaseComponentHelper*> helpers;
		return helpers;
	}
	
	uint16_t BaseComponentHelper::currentIdx = 0;
}