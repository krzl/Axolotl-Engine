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
			static uint8_t id = 0;
			sprintf_s( t->windowLabel, 32, "%s##%d", T::label, id++ );
		}
		
		virtual ~EditorPanel() = default;

	protected:

		bool isPanelOpened = true;
		char windowLabel[ 32 ];
		
		virtual void Setup() {
			EditorGuiSystem::RegisterPanel( this );
		}
		
		virtual void Update() = 0;
		
		virtual void Shutdown() {
			EditorGuiSystem::UnregisterPanel( this );
		}
	};
}
