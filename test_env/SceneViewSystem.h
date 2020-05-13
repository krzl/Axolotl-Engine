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

			const float speed = 0.02f;
			const float scaleSpeed = 0.0002f;



			if (input::GetKey( Key::MIDDLE_MOUSE_BUTTON )) {

			}
			else {
				if (input::GetKey( Key::A )) {
					currentPosition -= Vector3::right * speed;
					transform->SetLocalPosition( currentPosition );
				}
				if (input::GetKey( Key::D )) {
					currentPosition += Vector3::right * speed;
					transform->SetLocalPosition( currentPosition );
				}

				if (input::GetKey( Key::W )) {
					currentPosition += Vector3::forward * speed;
					transform->SetLocalPosition( currentPosition );
				}
				if (input::GetKey( Key::S )) {
					currentPosition -= Vector3::forward * speed;
					transform->SetLocalPosition( currentPosition );
				}

				if (input::GetKey( Key::Q )) {
					currentPosition += Vector3::up * speed;
					transform->SetLocalPosition( currentPosition );
				}
				if (input::GetKey( Key::E )) {
					currentPosition -= Vector3::up * speed;
					transform->SetLocalPosition( currentPosition );
				}

				const Vector3& scale = transform->GetLocalScale();
				transform->SetLocalScale( scale * ( 1.0f - input::GetMouseScrollDelta() * scaleSpeed ) );
				printf( "%f\n", sceneView->cameraTransform->GetPosition().y );
			}
		};
	};

}
