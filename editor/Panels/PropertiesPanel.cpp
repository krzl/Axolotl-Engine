#include "stdafx.h"
#include "PropertiesPanel.h"
#include <imgui.h>

#include "PropertyViewers//Selection.h"
#include "PropertyViewers/DefaultPropertyViewer.h"
#include <Resources/Serialization/Serializable.h>
#include <Entities/Entity.h>
#include "PropertyViewers/BasicTypesPropertyViewers.h"
#include <PropertyViewers/PropertyViewerRegistrator.h>

namespace axlt::editor {
	Map<uint32_t, PropertyViewerRegistrator*> PropertiesPanel::customPropertyViewers = Map<uint32_t, PropertyViewerRegistrator*>();
	
	void PropertiesPanel::Update() {
		if (!ImGui::Begin( windowLabel, &isPanelOpened, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse )) {
			ImGui::End();
			return;
		}
		
		if ( Entity* entity = Selection::GetSelectedEntity() ) {
			ExactArray<Serializable*> serializables = entity->GetComponentsAsSerializables();
			DrawPropertyLabel( "Entity name" );
			static char inputStorage[ 512 ];
			memset( inputStorage, 0, 512 );
			memcpy( inputStorage, entity->name.GetData(), entity->name.Length() );
			if (ImGui::InputText( "##EntityName", inputStorage, IM_ARRAYSIZE( inputStorage ),
				ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue ) || !ImGui::IsWindowFocused()) {
				entity->name = inputStorage;
			}
			for( auto serializable : serializables ) {
				if( !ImGui::CollapsingHeader( serializable->GetSerializationData().GetName().GetData(), ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_DefaultOpen ) ) {
					continue;
				}
				PropertyViewerRegistrator** customPropertyRegistrator = customPropertyViewers.Find( serializable->GetTypeHash() );
				if (customPropertyRegistrator != nullptr) {
					void* data = serializable;
					const auto propertyViewer = (*customPropertyRegistrator)->GetPropertyViewer();
					propertyViewer( serializable->GetSerializationData().GetName(), data );
				} else {
					SerializablePropertyDrawer( serializable );
				}
			}
		} else if ( void* resource = Selection::GetSelectedResource() ) {

		}
		
		ImGui::End();
	}
}
