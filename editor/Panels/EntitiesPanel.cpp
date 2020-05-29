// ReSharper disable CppClangTidyClangDiagnosticFormatSecurity
#include "stdafx.h"
#include "EntitiesPanel.h"

#include <imgui.h>
#include <Entities/Entity.h>

namespace axlt::editor {

	static ImGuiTreeNodeFlags leafFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap;
	static ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap;
	
	static char inputStorage[ 512 ];
	static char entityNameId[ 512 ];
	
	void EntitiesPanel::DrawEntity( Entity& entity, ComponentHandle<TransformComponent> transform ) {
		const bool isLeaf = !transform.IsValidHandle() || transform->GetChildren().GetSize() == 0;
		ImGuiTreeNodeFlags flags = isLeaf ? leafFlags : nodeFlags;
		if (selectedEntity == &entity) {
			if( ImGui::IsWindowFocused() && input::GetKeyDown( Key::F2 ) ) {
				renamedEntity = &entity;
				memset( inputStorage, 0, IM_ARRAYSIZE( inputStorage ) );
				memcpy( inputStorage, entity.name.GetData(), entity.name.Length() );
			}
			if( renamedEntity != &entity ) {
				flags |= ImGuiTreeNodeFlags_Selected;
			}
		}
		if( &entity == entityToExpand ) {
			entityToExpand = nullptr;
			ImGui::SetNextItemOpen( true, ImGuiCond_Always );
		}
		bool isVisible;
		if( renamedEntity == &entity ) {
			sprintf_s( entityNameId, "##%d", entity.GetIndex() );
			isVisible = ImGui::TreeNodeEx( entityNameId, flags );
			ImGui::SameLine();
			if( ImGui::InputText( "##EntityNameInput", inputStorage, IM_ARRAYSIZE( inputStorage ), 
				ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue ) || 
				!ImGui::IsWindowFocused() ) {
				entity.name = inputStorage;
				renamedEntity = nullptr;
			}
		} else {
			sprintf_s( entityNameId, "##%d", entity.GetIndex() );
			isVisible = ImGui::TreeNodeEx( entityNameId, flags );
			if (ImGui::BeginDragDropSource()) {
				uint32_t index = entity.GetIndex();
				ImGui::SetDragDropPayload( "Entity", &index, sizeof( uint32_t ) );
				ImGui::Text( entity.name.GetData() );
				ImGui::EndDragDropSource();
			}
			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload( "Entity" )) {
					const uint32_t index = *(const uint32_t*)payload->Data;
					Entity& movedEntity = *Entity::GetAllEntities()[ index ];
					storedReparentData = Tuple<bool, Entity*, Entity*>( true, &entity, &movedEntity );
				}
			}
			if (ImGui::IsItemClicked()) {
				selectedEntity = &entity;
				if (selectedEntity != renamedEntity && renamedEntity != nullptr) {
					renamedEntity->name = inputStorage;
					renamedEntity = nullptr;
				}
			}
			ImGui::SameLine();
			ImGui::Text( entity.name.GetData() );
		}
		if( selectedEntity == &entity && input::GetKeyDown( Key::DELETE ) && ImGui::IsWindowFocused() ) {
			entity.Destroy();
		} else {
			if( isVisible && !isLeaf ) {
				for( const ComponentHandle<TransformComponent>& childTransform : transform->GetChildren() ) {
					DrawEntity( childTransform->GetEntity(), childTransform );
				}
			}
		}
		if( isVisible && !isLeaf ) {
			ImGui::TreePop();
		}
	}

	void EntitiesPanel::Update() {
		if (!ImGui::Begin( windowLabel, &isPanelOpened, ImGuiWindowFlags_NoCollapse )) {
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
		filter.Draw( "Filter", ImGui::GetWindowWidth() - 100 );
		ImGui::SameLine( ImGui::GetWindowWidth() - 30 );
		if( ImGui::Button( "+" ) ) {
			// ReSharper disable once CppNonReclaimedResourceAcquisition
			new Entity();
		}
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

		if( storedReparentData.Get<0>() ) {
			Entity* entity = storedReparentData.Get<1>();
			ComponentHandle<TransformComponent> transform = entity->GetComponent<TransformComponent>();
			ComponentHandle<TransformComponent> movedTransform = storedReparentData.Get<2>()->AddOrGetComponent<TransformComponent>();
			if (!transform.IsValidHandle()) {
				transform = entity->AddComponent<TransformComponent>();
			}
			movedTransform->SetParent( transform, true );
			entityToExpand = entity;
			storedReparentData.Get<0>() = false;
		}
	}
}
