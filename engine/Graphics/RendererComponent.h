#pragma once
#include "Resources/ModelResource.h"
#include "Entities/Component.h"

namespace axlt {
	class RendererComponent : public BaseComponent<RendererComponent> {

	public:

		ModelResource* model = nullptr;
	};
}
