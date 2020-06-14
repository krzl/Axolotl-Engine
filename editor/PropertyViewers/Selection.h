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
			selectedFile = nullptr;
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

		static void SetSelection( File& directory ) {
			ClearSelection();
			selectedFile = &directory;
			selectionTypeHash = GetTypeHash<File>();
		}

		template<typename T>
		static void SetSelection( ResourceHandle<T>& directory ) {
			ClearSelection();
			selectedResource = reinterpret_cast<void*>( directory );
			selectionTypeHash = GetTypeHash<T>();
		}
		
		inline static Entity* selectedEntity;
		inline static Directory* selectedDirectory;
		inline static File* selectedFile;
		inline static void* selectedResource;
		inline static uint32_t selectionTypeHash;
	};
}
