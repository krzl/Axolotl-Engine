#include "stdafx.h"
#include "CameraComponent.h"
#include "Game.h"

namespace axlt {
	Array<ComponentHandle<CameraComponent>> CameraComponent::orderedComponents;

	Matrix4 CameraComponent::invertAxisMatrix = Matrix4(
		Vector4( 1.0f, 0.0f, 0.0f, 0.0f ),
		Vector4( 0.0f, 0.0f, -1.0f, 0.0f ),
		Vector4( 0.0f, -1.0f, 0.0f, 0.0f ),
		Vector4( 0.0f, 0.0f, 0.0f, 1.0f )
	);

	void CameraComponent::OnEnabled() {
		orderedComponents.Emplace( this );
		SetOrder( order );
	}

	void CameraComponent::OnDisabled() {
		orderedComponents.Remove( orderedComponents.FindIndex( this ) );
	}

	Matrix4 CameraComponent::ViewMatrix() const {
		return const_cast<CameraComponent&>(*this).ViewMatrixInner();
	}

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

	float CameraComponent::GetOrder() const {
		return order;
	}

	void CameraComponent::SetOrder( const float order ) {
		this->order = order;

		const uint32_t startingIndex = orderedComponents.FindIndex( this );
		if( startingIndex == 0xFFFFFFFF ) return;
		orderedComponents.Remove( startingIndex );

		uint32_t targetIndex = orderedComponents.GetSize() + 1;
		
		for( uint32_t i = 0; i < orderedComponents.GetSize(); i++ ) {
			if( orderedComponents[i]->order > order ) {
				targetIndex = i;
				break;
			}
		}

		if( targetIndex == orderedComponents.GetSize() + 1 ) {
			orderedComponents.Emplace( this );
		} else {
			orderedComponents.Insert( this, targetIndex );
		}
	}
	
	Array<ComponentHandle<CameraComponent>>& CameraComponent::GetAllCameras() {
		return orderedComponents;
	}
	
	const SerializationInfo& CameraComponent::GetSerializationData() const {
		static SerializationInfo serializationInfo = SerializationInfoBuilder<CameraComponent>( "CameraComponent" )
			.AddField( "order", &CameraComponent::order )
			.AddField( "hFov", &CameraComponent::hFov )
			.AddField( "nearClipPlane", &CameraComponent::nearClipPlane )
			.AddField( "farClipPlane", &CameraComponent::farClipPlane )
			.AddField( "viewport", &CameraComponent::viewport )
			.AddField( "scissor", &CameraComponent::scissor )
			.AddField( "renderTexture", &CameraComponent::renderTexture )
			.Build();
		return serializationInfo;
	}
}
