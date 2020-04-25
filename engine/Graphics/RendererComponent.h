#pragma once
#include "Resources/ModelResource.h"
#include "Entities/Component.h"
#include "Resources/ResourceHandle.h"
#include "MaterialResource.h"

namespace axlt {
	class RendererComponent : public BaseComponent<RendererComponent> {

	public:

		ResourceHandle<ModelResource> model;
		ResourceHandle<MaterialResource> material;
	};
}
