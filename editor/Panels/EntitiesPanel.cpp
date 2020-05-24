#include "stdafx.h"
#include "EntitiesPanel.h"

#include <imgui.h>
#include <Entities/Entity.h>
#include <Entities/TransformComponent.h>

namespace axlt::editor {

	static ImGuiTreeNodeFlags leafFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
	static ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

	static Entity* selectedEntity = nullptr;
	static Entity* entityToExpand = nullptr;
	
	void DrawEntity( Entity& entity, const ComponentHandle<TransformComponent>& transform ) {
		const bool isLeaf = !transform.IsValidHandle() || transform->GetChildren().GetSize() == 0;
		ImGuiTreeNodeFlags flags = isLeaf ? leafFlags : nodeFlags;
		if (selectedEntity == &entity) {
			flags |= ImGuiTreeNodeFlags_Selected;
		}
		if( &entity == entityToExpand ) {
			entityToExpand = nullptr;
			ImGui::SetNextItemOpen( true, ImGuiCond_Always );
		}
		const bool isVisible = ImGui::TreeNodeEx( entity.name.GetData(), flags );
		if (ImGui::BeginDragDropSource()) {
			uint32_t index = entity.GetIndex();
			ImGui::SetDragDropPayload( "Entity", &index, sizeof( uint32_t ) );
			ImGui::Text( entity.name.GetData() );  // NOLINT(clang-diagnostic-format-security)
			ImGui::EndDragDropSource();
		}
		if (ImGui::BeginDragDropTarget()) {
			if ( const ImGuiPayload* payload = ImGui::AcceptDragDropPayload( "Entity" ) ) {
				const uint32_t index = *(const uint32_t*)payload->Data;
				Entity& movedEntity = *Entity::GetAllEntities()[ index ];
				ComponentHandle<TransformComponent> movedTransform = movedEntity.AddOrGetComponent<TransformComponent>();
				movedTransform->SetParent( transform, true );
				entityToExpand = &entity;
			}
		}
		if (ImGui::IsItemClicked()) {
			selectedEntity = &entity;
		}
		if( isVisible && !isLeaf ) {
			auto& children = transform->GetChildren();
			for( const ComponentHandle<TransformComponent>& childTransform : transform->GetChildren() ) {
				DrawEntity( childTransform->GetEntity(), childTransform );
			}
			ImGui::TreePop();
		}
	}

	void EntitiesPanel::Update() {
		if (!ImGui::Begin( "Entities", &isPanelOpened, 0 )) {
			ImGui::End();
			return;
		}
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload( "Entity" )) {
				const uint32_t index = *(const uint32_t*)payload->Data;
				Entity& movedEntity = *Entity::GetAllEntities()[ index ];
				ComponentHandle<TransformComponent> movedTransform = movedEntity.AddOrGetComponent<TransformComponent>();
				movedTransform->SetParent( nullptr, true );
			}
		}
		filter.Draw( "Filter" );
		if (filter.Filters.Size == 0) {
			for (Entity* entity : Entity::GetAllEntities()) {
				ComponentHandle<TransformComponent> transform = entity->GetComponent<TransformComponent>();
				if (transform.IsValidHandle()) {
					if (!transform->GetParent().IsValidHandle()) {
						DrawEntity( *entity, transform );
					}
				}
				else {
					DrawEntity( *entity, transform );
				}
			}
		} else {
			for( Entity* entity : Entity::GetAllEntities() ) {
				if( filter.PassFilter( entity->name.GetData() ) ) {
					ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
					if (selectedEntity == entity) {
						flags |= ImGuiTreeNodeFlags_Selected;
					}
					ImGui::TreeNodeEx( entity->name.GetData(), flags );
					if (ImGui::IsItemClicked()) {
						selectedEntity = entity;
					}
				}
			}
		}
		
		ImGui::End();
	}
}
