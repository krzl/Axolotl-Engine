#pragma once
#include "EditorGuiSystem.h"

namespace axlt::editor {
	class EditorPanel {

		friend class EditorGuiSystem;
		
	public:

		template< typename T>
		static void CreatePanel() {
			T* t = new T();
			t->Setup();
		}

		virtual ~EditorPanel() = default;

	protected:

		bool isPanelOpened;

		virtual void Setup() {
			EditorGuiSystem::RegisterPanel( this );
		}
		
		virtual void Update() = 0;
		virtual void Shutdown() {
			EditorGuiSystem::UnregisterPanel( this );
		}
	};
}
