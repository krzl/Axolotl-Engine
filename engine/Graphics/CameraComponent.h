#pragma once

#include "Entities/Component.h"

namespace axlt {
	class CameraComponent : public BaseComponent<CameraComponent> {

	public:

		float order = 0.0f;
	};
}