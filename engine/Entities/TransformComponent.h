#pragma once

#include "Component.h"

namespace axlt {
	class TransformComponent {
		DEFINE_COMPONENT( TransformComponent );

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

		Matrix4 WorldToLocalMatrix() const;
		const Matrix4& LocalToWorldMatrix() const;

		void SetParent( const ComponentHandle<TransformComponent>& parent );

		const ComponentHandle<TransformComponent>& GetParent() const;
		const Set<ComponentHandle<TransformComponent>>& GetChildren() const;

	private:

		void SetDirty();

		Vector3 localPosition;
		Quaternion localRotation;
		Vector3 localScale;

		Matrix4 localToWorldMatrix;

		bool isDirty = true;

		ComponentHandle<TransformComponent> parent;
		Set<ComponentHandle<TransformComponent>> childTransforms;
	};
}