#pragma once
#include <Entities/System.h>

#include "Entities/TransformComponent.h"
#include "SceneViewComponent.h"

namespace axlt {
	class CameraComponent;

	class TransformComponent;
}

namespace axlt::editor {
	class SceneViewSystem final : public System<TransformComponent, SceneViewComponent> {
		
		void OnAdded(Tuple<ComponentHandle<TransformComponent>, ComponentHandle<SceneViewComponent>> tuple) override {
			auto&[ transform, sceneView ] = tuple;

			Entity* entity = new Entity();
			entity->AddComponent<CameraComponent>();
			sceneView->cameraTransform = entity->AddComponent<TransformComponent>();
			sceneView->cameraTransform->SetParent( transform );
			sceneView->cameraTransform->SetLocalPosition( Vector3::forward  * -5.0f );
		}

		void OnRemoved(Tuple<ComponentHandle<TransformComponent>, ComponentHandle<SceneViewComponent>> tuple) override {
			auto&[ transform, sceneView ] = tuple;

			sceneView->GetEntity().Destroy();
			sceneView->cameraTransform = ComponentHandle< TransformComponent>::CreateInvalidHandle();
		}

		void Update( Tuple<ComponentHandle<TransformComponent>, ComponentHandle<SceneViewComponent>> tuple ) override {

			auto&[ transform, sceneView ] = tuple;

			Vector3 currentPosition = transform->GetLocalPosition();

			if (input::GetKey( Key::MIDDLE_MOUSE_BUTTON )) {
				if (input::GetKey( Key::CTRL )) {
					const Vector3& scale = transform->GetLocalScale();
					const float scaleSpeedCtrl = 0.002f;
					transform->SetLocalScale( scale * (1.0f + input::GetMouseDelta().y * scaleSpeedCtrl) );
				} else if (input::GetKey( Key::SHIFT )) {
					const float speed = 0.01f;
					currentPosition -= transform->RightDirection() * input::GetMouseDelta().x * speed;
					currentPosition += transform->UpDirection() * input::GetMouseDelta().y * speed;

					transform->SetLocalPosition( currentPosition );
				} else {
					Quaternion currentRotation = transform->GetLocalRotation();
					const Vector2Int mouseDelta = input::GetMouseDelta();
					const float mouseRotateSpeed = 0.2f;
					currentRotation *= Quaternion( -mouseDelta.y * mouseRotateSpeed, 0, -mouseDelta.x * mouseRotateSpeed );
					transform->SetLocalRotation( currentRotation );
				}
			}
			else {
				const float speed = 0.02f;
				
				if (input::GetKey( Key::A )) {
					currentPosition -= transform->RightDirection() * speed;
					transform->SetLocalPosition( currentPosition );
				}
				if (input::GetKey( Key::D )) {
					currentPosition += transform->RightDirection() * speed;
					transform->SetLocalPosition( currentPosition );
				}

				if (input::GetKey( Key::W )) {
					currentPosition += transform->ForwardDirection() * speed;
					transform->SetLocalPosition( currentPosition );
				}
				if (input::GetKey( Key::S )) {
					currentPosition -= transform->ForwardDirection() * speed;
					transform->SetLocalPosition( currentPosition );
				}

				if (input::GetKey( Key::Q )) {
					currentPosition += transform->UpDirection() * speed;
					transform->SetLocalPosition( currentPosition );
				}
				if (input::GetKey( Key::E )) {
					currentPosition -= transform->UpDirection() * speed;
					transform->SetLocalPosition( currentPosition );
				}

				const Vector3& scale = transform->GetLocalScale();
				const float scaleSpeed = 0.0002f;
				transform->SetLocalScale( scale * ( 1.0f - input::GetMouseScrollDelta() * scaleSpeed ) );
			}
		};
	};

}
