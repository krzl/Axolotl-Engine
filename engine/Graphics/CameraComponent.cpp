#include "stdafx.h"
#include "CameraComponent.h"
#include "Game.h"

namespace axlt {
	
	Matrix4 CameraComponent::ViewMatrix() const {
		return const_cast<CameraComponent&>( *this ).ViewMatrixInner();
	}

	Matrix4 CameraComponent::invertAxisMatrix = Matrix4(
		Vector4( 1.0f, 0.0f, 0.0f, 0.0f ),
		Vector4( 0.0f, 0.0f, -1.0f, 0.0f ),
		Vector4( 0.0f, -1.0f, 0.0f, 0.0f ),
		Vector4( 0.0f, 0.0f, 0.0f, 1.0f )
	);

	Matrix4 CameraComponent::ViewMatrixInner() {
		if( !transform.IsValidHandle() ) {
			transform = GetEntity().GetComponent<TransformComponent>();
		}
		return
			invertAxisMatrix *
			Matrix4::RotationMatrix( transform->GetRotation().Inverse() ) *
			Matrix4::TranslationMatrix( -transform->GetPosition() );
	}

	Matrix4 CameraComponent::ProjectionMatrix() const {
		const float s = 1.0f / tan( hFov / 2.0f * MATH_PI / 180.0f );
		const Window& window = GameInstance.GetWindow();
		const float aspectRatio = (float) window.GetWidth() / window.GetHeight();
		const float diff = farClipPlane - nearClipPlane;
		return Matrix4{
			Vector4( s, 0, 0, 0 ),
			Vector4( 0, s*aspectRatio, 0, 0 ),
			Vector4( 0, 0, -farClipPlane / diff, -1 ),
			Vector4( 0, 0, -farClipPlane * nearClipPlane / diff , 0 ),
		};
	}

	SparseArray<CameraComponent>& CameraComponent::GetAllCameras() {
		return helper.storage;
	}
	
	const SerializationInfo& CameraComponent::GetSerializationData() const {
		static SerializationInfo serializationInfo = SerializationInfoBuilder<CameraComponent>( "CameraComponent" )
			.AddField( "order", &CameraComponent::order )
			.AddField( "hFov", &CameraComponent::hFov )
			.AddField( "nearClipPlane", &CameraComponent::nearClipPlane )
			.AddField( "farClipPlane", &CameraComponent::farClipPlane )
			.Build();
		return serializationInfo;
	}
}
