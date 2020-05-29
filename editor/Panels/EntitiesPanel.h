#pragma once
#include "EditorPanel.h"
#include <imgui.h>
#include <Collections/Tuple.h>
#include <Entities/TransformComponent.h>

namespace axlt::editor {
	class EntitiesPanel final : public EditorPanel {

	public:

		inline static const char* label = "Entities";

	private:

		void Update() override;

		void DrawEntity( Entity& entity, ComponentHandle<TransformComponent> transform );

		ImGuiTextFilter filter;

		Entity* selectedEntity = nullptr;
		Entity* entityToExpand = nullptr;
		Entity* renamedEntity = nullptr;

		Tuple<bool, Entity*, Entity*> storedReparentData;
	};
}
