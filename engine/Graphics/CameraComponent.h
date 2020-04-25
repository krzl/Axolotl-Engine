#pragma once

#include "Entities/Component.h"

namespace axlt {
	class CameraComponent : public BaseComponent<CameraComponent> {

	public:

		Matrix4 ProjectionMatrix() const;

		static SparseArray<CameraComponent>& GetAllCameras();

		float order = 0.0f; //TODO: Use order when drawing
		float hFov = 90.0f;
		float nearClipPlane = 0.1f;
		float farClipPlane = 1000.0f;
	};
}