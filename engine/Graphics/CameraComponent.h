#pragma once

#include "Entities/Component.h"
#include "Entities/TransformComponent.h"

namespace axlt {
	class CameraComponent final : public BaseComponent<CameraComponent> {

		friend class RenderSystem;
		
	public:

		void OnEnabled() override;
		void OnDisabled() override;

		Matrix4 ViewMatrix() const;
		Matrix4 ProjectionMatrix() const;

		float GetOrder() const;
		void SetOrder( float order );

		static Array<ComponentHandle<CameraComponent>>& GetAllCameras();
		
		const SerializationInfo& GetSerializationData() const override;

		float hFov = 90.0f;
		float nearClipPlane = 0.1f;
		float farClipPlane = 1000.0f;

		Rect2D viewport;
		Rect2D scissor;

	private:
		
		float order = 0.0f;

		Matrix4 ViewMatrixInner();
		
		ComponentHandle<TransformComponent> transform;

		static Matrix4 invertAxisMatrix;
		static Array<ComponentHandle<CameraComponent>> orderedComponents;
	};
}
