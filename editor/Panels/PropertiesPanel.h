#pragma once
#include "EditorPanel.h"
#include <Collections/Map.h>

namespace axlt {
	class Serializable;
}

namespace axlt::editor {
	class PropertyViewerRegistrator;
	
	class PropertiesPanel final : public EditorPanel {

		friend void SerializablePropertyDrawer( Serializable* );

	public:

		inline static const char* label = "Properties";

		template<typename T>
		static void RegisterCustomPropertyViewer( PropertyViewerRegistrator& registrator ) {
			customPropertyViewers.Add( GetTypeHash<T>(), &registrator );
		}
		
	private:

		void Update() override;

		static Map<uint32_t, PropertyViewerRegistrator*> customPropertyViewers;
	};
}
