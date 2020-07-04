#pragma once
#include <Resources/ResourceHandle.h>

namespace axlt {
	class Directory;
}

namespace axlt::editor {
	class Selection {

	public:

		static void ClearSelection() {
			selectedEntity = nullptr;
			selectedDirectory = nullptr;
			selectedResource = nullptr;
			selectionTypeHash = 0;
		}

		static void SetSelection( Entity& entity ) {
			ClearSelection();
			selectedEntity = &entity;
			selectionTypeHash = GetTypeHash<Entity>();
		}

		static void SetSelection( Directory& directory ) {
			ClearSelection();
			selectedDirectory = &directory;
			selectionTypeHash = GetTypeHash<Directory>();
		}

		static void SetSelection( File& file ) {
			ClearSelection();
			selectionTypeHash = GetTypeHash<File>();
			//TOOD: Load resource from file path
			//TODO: use SetSelection( ResourceHandle<T> ) after resource is loaded
		}

		template<typename T>
		static void SetSelection( ResourceHandle<T>& directory ) {
			ClearSelection();
			selectedResource = reinterpret_cast<void*>( directory );
			selectionTypeHash = GetTypeHash<T>();
		}
		
		inline static Entity* selectedEntity;
		inline static Directory* selectedDirectory;
		inline static void* selectedResource;
		inline static uint32_t selectionTypeHash;
	};
}
