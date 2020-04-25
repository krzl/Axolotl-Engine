#include "stdafx.h"
#include "CameraComponent.h"

namespace axlt {

	Matrix4 CameraComponent::ProjectionMatrix() const {
		const float s = 1.0f / tan( hFov / 2.0f * MATH_PI / 180.0f );
		const float diff = farClipPlane - nearClipPlane;
		return Matrix4{
			Vector4( s, 0, 0, 0 ),
			Vector4( 0, s, 0, 0 ),
			Vector4( 0, 0, -farClipPlane / diff, -farClipPlane * nearClipPlane / diff ),
			Vector4( 0, 0, -1, 0 ),
		};
	}

	SparseArray<CameraComponent>& CameraComponent::GetAllCameras() {
		return helper.storage;
	}
}