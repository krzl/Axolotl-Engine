#pragma once

#include "Component.h"

namespace axlt {
	class TransformComponent : public BaseComponent<TransformComponent> {

	public:

		TransformComponent();

		void SetLocalPosition( const Vector3& position );
		void SetPosition( const Vector3& position );

		const Vector3& GetLocalPosition() const;
		Vector3 GetPosition() const;

		void SetLocalRotation( const Quaternion& rotation );
		void SetRotation( const Quaternion& rotation );

		const Quaternion& GetLocalRotation() const;
		Quaternion GetRotation() const;

		void SetLocalScale( const Vector3& scale );
		void SetScale( const Vector3& scale );

		const Vector3& GetLocalScale() const;
		Vector3 GetScale() const;

		const Matrix4& WorldToLocalMatrix() const;
		const Matrix4& LocalToWorldMatrix() const;

		bool IsChildOf( const ComponentHandle<TransformComponent>& transform, bool recursive = false ) const;
		void SetParent( const ComponentHandle<TransformComponent>& parent, bool worldPositionStays = false );

		const ComponentHandle<TransformComponent>& GetParent() const;
		const Set<ComponentHandle<TransformComponent>>& GetChildren() const;

		Vector3 ForwardDirection() const;
		Vector3 UpDirection() const;
		Vector3 RightDirection() const;

	private:

		void SetDirty();
		void RecalculateMatrices();

		Vector3 localPosition;
		Quaternion localRotation;
		Vector3 localScale;

		Matrix4 localToWorldMatrix;
		Matrix4 worldToLocalMatrix;

		bool isDirty = true;

		ComponentHandle<TransformComponent> parent;
		Set<ComponentHandle<TransformComponent>> childTransforms{};
	};
}