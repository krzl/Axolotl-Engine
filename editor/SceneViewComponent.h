#pragma once

#include "Entities/Component.h"
#include "Entities/TransformComponent.h"

namespace axlt::editor {
	class SceneViewComponent : public BaseComponent<SceneViewComponent> {

	public:

		ComponentHandle<TransformComponent> cameraTransform;
	};
}