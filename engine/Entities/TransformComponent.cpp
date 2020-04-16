#include "stdafx.h"
#include "TransformComponent.h"

namespace axlt {

	TransformComponent::TransformComponent() :
		localPosition( Vector3::zero ),
		localScale( Vector3::one ) {}

	void TransformComponent::SetLocalPosition( const Vector3& position ) {
		localPosition = position;
		SetDirty();
	}

	void TransformComponent::SetPosition( const Vector3& position ) {
		localPosition = position * WorldToLocalMatrix();
		SetDirty();
	}

	const Vector3& TransformComponent::GetLocalPosition() const {
		return localPosition;
	}

	Vector3 TransformComponent::GetPosition() const {
		return Vector3::zero * LocalToWorldMatrix();
	}

	void TransformComponent::SetLocalRotation( const Quaternion& rotation ) {
		localRotation = rotation;
		SetDirty();
	}

	void TransformComponent::SetRotation( const Quaternion& rotation ) {
		if( parent.IsValid() ) {
			localRotation = rotation * parent->GetRotation().Inverse();
		} else {
			localRotation = rotation;
		}
		SetDirty();
	}

	const Quaternion& TransformComponent::GetLocalRotation() const {
		return localRotation;
	}

	Quaternion TransformComponent::GetRotation() const {
		if( parent.IsValid() ) {
			return localRotation * parent->GetRotation();
		}
		return localRotation;
	}

	void TransformComponent::SetLocalScale( const Vector3& scale ) {
		localScale = scale;
		SetDirty();
	}

	void TransformComponent::SetScale( const Vector3& scale ) {
		if( parent.IsValid() ) {
			localScale = scale / parent->GetScale();
		} else {
			localScale = scale;
		}
		SetDirty();
	}

	const Vector3& TransformComponent::GetLocalScale() const {
		return localScale;
	}

	Vector3 TransformComponent::GetScale() const {
		if( parent.IsValid() ) {
			return localScale * parent->GetScale();
		}
		return localScale;
	}

	Matrix4 TransformComponent::WorldToLocalMatrix() const {
		Matrix4 matrix =
			Matrix4::ScaleMatrix( Vector3::one / localScale ) *
			Matrix4::RotationMatrix( localRotation.Inverse() ) *
			Matrix4::TranslationMatrix( -localPosition );

		if( parent.IsValid() ) {
			matrix = parent->WorldToLocalMatrix() * matrix;
		}

		return matrix;
	}

	const Matrix4& TransformComponent::LocalToWorldMatrix() const {
		if( isDirty ) {
			TransformComponent* nonConstThis = const_cast<TransformComponent*>( this );
			nonConstThis->localToWorldMatrix =
				Matrix4::TranslationMatrix( localPosition ) *
				Matrix4::RotationMatrix( localRotation ) *
				Matrix4::ScaleMatrix( localScale );

			if( parent.IsValid() ) {
				nonConstThis->localToWorldMatrix *= parent->LocalToWorldMatrix();
			}
			nonConstThis->isDirty = false;
		}
		return localToWorldMatrix;
	}

	void TransformComponent::SetDirty() {
		for( auto& child : childTransforms ) {
			child->SetDirty();
		}
		isDirty = true;
	}

	void TransformComponent::SetParent( const ComponentHandle<TransformComponent>& parent ) {
		if( this->parent.IsValid() ) {
			this->parent->childTransforms.Remove( *this );
		}
		this->parent = parent;
		SetDirty();
	}

	const ComponentHandle<TransformComponent>& TransformComponent::GetParent() const {
		return parent;
	}

	const Set<ComponentHandle<TransformComponent>>& TransformComponent::GetChildren() const {
		return childTransforms;
	}
}
