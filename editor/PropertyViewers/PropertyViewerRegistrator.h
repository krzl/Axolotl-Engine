#pragma once
#include "Panels/PropertiesPanel.h"

namespace axlt::editor {
	class PropertyViewerRegistrator {

		typedef void( *PropertyViewer )(const String&, void*);

	public:

		template<typename T>
		explicit PropertyViewerRegistrator( void( *func )(const String&, T&) ) {
			this->func = (PropertyViewer) func;
			PropertiesPanel::RegisterCustomPropertyViewer<T>( *this );
		}

		PropertyViewer GetPropertyViewer() const {
			return func;
		}

	private:

		PropertyViewer func;
	};
}
