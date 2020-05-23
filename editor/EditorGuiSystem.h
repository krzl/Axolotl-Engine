#pragma once
#include <Entities/SystemBase.h>

namespace axlt::editor {
	class EditorPanel;

	class EditorGuiSystem final : SystemBase {

		friend class EditorPanel;

		void Setup() override;
		void Update() override;
		
		static void RegisterPanel( EditorPanel* editorPanel );
		static void UnregisterPanel( EditorPanel* editorPanel );
	};
}
