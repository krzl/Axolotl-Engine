#pragma once
#include <Entities/System.h>

#include "Entities/TransformComponent.h"
#include "SceneViewComponent.h"

namespace axlt {
	class CameraComponent;
	class TransformComponent;
}

namespace axlt::editor {
	class SceneViewSystem final : public System<TransformComponent, SceneViewComponent> {
		void OnAdded( Tuple<ComponentHandle<TransformComponent>, ComponentHandle<SceneViewComponent>> tuple ) override;
		void OnRemoved( Tuple<ComponentHandle<TransformComponent>, ComponentHandle<SceneViewComponent>> tuple ) override;
		void Update( Tuple<ComponentHandle<TransformComponent>, ComponentHandle<SceneViewComponent>> tuple ) override;
	};
}
