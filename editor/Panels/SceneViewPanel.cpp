#include "stdafx.h"
#include "SceneViewPanel.h"

#include <imgui.h>
#include <Graphics/RendererComponent.h>

namespace axlt::editor {
	
	void SceneViewPanel::Setup() {
		EditorPanel::Setup();
		Entity* root = new Entity( "Scene View Pivot" );
		transformComponent = root->AddComponent<TransformComponent>();
		
		Entity* cameraEntity = new Entity( "Scene View Camera" );
		cameraComponent = cameraEntity->AddComponent<CameraComponent>();
		ComponentHandle<TransformComponent> cameraTransform = cameraEntity->AddComponent<TransformComponent>();
		cameraTransform->SetParent( transformComponent );
		cameraTransform->SetLocalPosition( Vector3::forward * -5.0f );

		/*
		ComponentHandle<RendererComponent> renderer = cameraEntity->AddComponent<RendererComponent>();
		const uint16_t meshLayout = Mesh::CreateMeshLayoutMask( 4, 4 ) +
			Mesh::CreateMeshLayoutMask( 5, 2 );
		
		viewportModel = new ModelResource( true );
		viewportModel->meshes.Emplace( meshLayout );
		viewportMesh = &viewportModel->meshes[ 0 ];
		
		renderer->model = viewportModel;
		renderer->material = ResourceHandle<MaterialResource>::Load( Guid::FromString( "ffffffffffffffffffffffffffffff09" ) );
		viewportMaterial = renderer->material;
		*/
	}

	void SceneViewPanel::Update() {
		if (!ImGui::Begin( windowLabel, &isPanelOpened, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse )) {
			ImGui::End();
			return;
		}

		HandleInput();

		const ImVec2 viewportSize = ImGui::GetContentRegionAvail();
		
		/*
		if( lastViewportSize.x != viewportSize.x || lastViewportSize.y != viewportSize.y ) {
			lastViewportSize.x = viewportSize.x;
			lastViewportSize.y = viewportSize.y;

			cameraComponent->renderTexture = new RenderTextureResource( viewportSize.x, viewportSize.y, true );
			
		}
		*/
		
		ImGui::Image( sceneViewTextureId, viewportSize, { 0, 0 }, { 1, 1 } );
		
		ImGui::End();
	}

	void SceneViewPanel::HandleInput() {
		Vector3 currentPosition = transformComponent->GetLocalPosition();

		if (input::GetKey( Key::MIDDLE_MOUSE_BUTTON )) {
			if (input::GetKey( Key::CTRL )) {
				const Vector3& scale = transformComponent->GetLocalScale();
				const float scaleSpeedCtrl = 0.002f;
				transformComponent->SetLocalScale( scale * (1.0f + input::GetMouseDelta().y * scaleSpeedCtrl) );
			}
			else if (input::GetKey( Key::SHIFT )) {
				const float speed = 0.01f;
				currentPosition -= transformComponent->RightDirection() * (float)input::GetMouseDelta().x * speed;
				currentPosition += transformComponent->UpDirection() * (float)input::GetMouseDelta().y * speed;

				transformComponent->SetLocalPosition( currentPosition );
			}
			else {
				Quaternion currentRotation = transformComponent->GetLocalRotation();
				const Vector2Int mouseDelta = input::GetMouseDelta();
				const float mouseRotateSpeed = 0.2f;
				currentRotation *= Quaternion( -mouseDelta.y * mouseRotateSpeed, 0, -mouseDelta.x * mouseRotateSpeed );
				transformComponent->SetLocalRotation( currentRotation );
			}
		}
		else {
			const float speed = 0.02f;

			if (input::GetKey( Key::A )) {
				currentPosition -= transformComponent->RightDirection() * speed;
				transformComponent->SetLocalPosition( currentPosition );
			}
			if (input::GetKey( Key::D )) {
				currentPosition += transformComponent->RightDirection() * speed;
				transformComponent->SetLocalPosition( currentPosition );
			}

			if (input::GetKey( Key::W )) {
				currentPosition += transformComponent->ForwardDirection() * speed;
				transformComponent->SetLocalPosition( currentPosition );
			}
			if (input::GetKey( Key::S )) {
				currentPosition -= transformComponent->ForwardDirection() * speed;
				transformComponent->SetLocalPosition( currentPosition );
			}

			if (input::GetKey( Key::Q )) {
				currentPosition += transformComponent->UpDirection() * speed;
				transformComponent->SetLocalPosition( currentPosition );
			}
			if (input::GetKey( Key::E )) {
				currentPosition -= transformComponent->UpDirection() * speed;
				transformComponent->SetLocalPosition( currentPosition );
			}

			const Vector3& scale = transformComponent->GetLocalScale();
			const float scaleSpeed = 0.0002f;
			transformComponent->SetLocalScale( scale * (1.0f - input::GetMouseScrollDelta() * scaleSpeed) );
		}
	}
}
