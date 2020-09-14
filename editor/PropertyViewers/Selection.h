#pragma once
#include <Resources/ResourceHandle.h>
#include "Import/ImportManager.h"

namespace axlt {
	class Directory;
	class Entity;
	class Resource;
}

namespace axlt::editor {
	class Selection {

	public:

		static void ClearSelection() {
			selectedEntity = nullptr;
			selectedResource = nullptr;
			selectionTypeHash = 0;
		}

		static void SetSelection( Entity& entity ) {
			ClearSelection();
			selectedEntity = &entity;
			selectionTypeHash = GetTypeHash<Entity>();
		}

		static void SetSelection( File& file ) {
			if( Guid* guid = ImportManager::filepathToGuid.Find( file.AbsolutePath().GetData() ) ) {
				//ResourceHandle<void> resource = ResourceHandle<void>::Load( *guid );
				//SetSelection( resource );
			}
		}

		template<typename T>
		static void SetSelection( ResourceHandle<T>& resource ) {
			ClearSelection();
			selectedResource = (Resource*) resource.GetData();
			selectedResourceHandle = reinterpret_cast<void*>( &resource );
			selectionTypeHash = GetTypeHash<T>();
		}

		static Entity* GetSelectedEntity() {
			return selectedEntity;
		}

		static void* GetSelectedResource() {
			return selectedResource;
		}

		static uint32_t GetSelectionTypeHash() {
			return selectionTypeHash;
		}

	private:

		inline static Entity* selectedEntity;
		inline static Resource* selectedResource;
		inline static void* selectedResourceHandle;
		inline static uint32_t selectionTypeHash;
	};
}
