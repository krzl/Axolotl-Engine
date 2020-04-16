#pragma once
#include "Resources/ModelResource.h"
#include "Entities/Component.h"

namespace axlt {
	class RendererComponent {
		DEFINE_COMPONENT( RendererComponent )

	public:

		ModelResource* model = nullptr;
	};
}
