#pragma once

#include "Entities/Component.h"

namespace axlt {
	class CameraComponent {
		DEFINE_COMPONENT( CameraComponent )

	public:

		float order = 0.0f;
	};
}