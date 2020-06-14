#pragma once

#include "Entities/Component.h"
#include "Entities/TransformComponent.h"

namespace axlt {
	class CameraComponent : public BaseComponent<CameraComponent> {

	public:

		Matrix4 ViewMatrix() const;
		Matrix4 ProjectionMatrix() const;

		static SparseArray<CameraComponent>& GetAllCameras();
		
		const SerializationInfo& GetSerializationData() override {
			static SerializationInfo serializationInfo = SerializationInfoBuilder<CameraComponent>( "CameraComponent" )
				.AddField( "order", &CameraComponent::order )
				.AddField( "hFov", &CameraComponent::hFov )
				.AddField( "nearClipPlane", &CameraComponent::nearClipPlane )
				.AddField( "farClipPlane", &CameraComponent::farClipPlane )
				.Build();
			return serializationInfo;
		}

		float order = 0.0f; //TODO: Use order when drawing
		float hFov = 90.0f;
		float nearClipPlane = 0.1f;
		float farClipPlane = 1000.0f;
		
	private:

		Matrix4 ViewMatrixInner();
		
		ComponentHandle<TransformComponent> transform;

		static Matrix4 invertAxisMatrix;
	};
}
