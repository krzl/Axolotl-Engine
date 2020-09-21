#pragma once
#include "EditorPanel.h"
#include <Entities/ComponentHandle.h>
#include <Graphics/CameraComponent.h>
#include <Resources/ModelResource.h>
#include <Graphics/MaterialResource.h>

namespace axlt::editor {
	class SceneViewPanel final : public EditorPanel {

	public:

		inline static const char* label = "Scene View";

		inline static void* sceneViewTextureId = (void*) 0x123456789abcdef;
		
		void Setup() override;
		void Update() override;

	private:
		
		void HandleInput();

		Vector2Int lastViewportSize;

		ResourceHandle<ModelResource> viewportModel;
		ResourceHandle<MaterialResource> viewportMaterial;
		Mesh* viewportMesh;

		ComponentHandle<TransformComponent> transformComponent;
		ComponentHandle<CameraComponent> cameraComponent;
	};
}
