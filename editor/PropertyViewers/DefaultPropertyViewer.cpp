#include "stdafx.h"
#include "DefaultPropertyViewer.h"
#include "Panels/PropertiesPanel.h"
#include "PropertyViewerRegistrator.h"

#include <Resources/Serialization/Serializable.h>
#include <imgui.h>

namespace axlt::editor {
	void SerializablePropertyDrawer( Serializable* property ) {
		for( auto& field : property->GetSerializationData().GetAllFields() ) {
			PropertyViewerRegistrator** customPropertyRegistrator = PropertiesPanel::customPropertyViewers.Find( field.value.GetStoredTypeHash() );
			if(customPropertyRegistrator != nullptr ) {
				void* data = field.value.GetVoidPtr<Serializable>( *property );
				const auto propertyViewer = (*customPropertyRegistrator)->GetPropertyViewer();
				propertyViewer( field.key, data );
			} else if( field.value.IsDerivedFromSerializable() ) {
				if (ImGui::TreeNode( field.key.GetData() ) ) {
					Serializable& data = *(Serializable*)field.value.GetVoidPtr<Serializable>( *property );
					SerializablePropertyDrawer( &data );
					ImGui::TreePop();
				}
			}
		}
	}
}
