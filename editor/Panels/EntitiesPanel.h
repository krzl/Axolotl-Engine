#pragma once
#include "EditorPanel.h"
#include <imgui.h>

namespace axlt::editor {
	class EntitiesPanel final : public EditorPanel {

		void Update() override;

		ImGuiTextFilter filter;
	};
}
