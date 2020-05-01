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
		localPosition = Vector4( position, 1.0f ) * WorldToLocalMatrix();
		SetDirty();
	}

	const Vector3& TransformComponent::GetLocalPosition() const {
		return localPosition;
	}

	Vector3 TransformComponent::GetPosition() const {
		return Vector4( Vector3::zero, 1.0f ) * LocalToWorldMatrix();
	}

	void TransformComponent::SetLocalRotation( const Quaternion& rotation ) {
		localRotation = rotation;
		SetDirty();
	}

	void TransformComponent::SetRotation( const Quaternion& rotation ) {
		if( parent.IsValidHandle() ) {
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
		if( parent.IsValidHandle() ) {
			return localRotation * parent->GetRotation();
		}
		return localRotation;
	}

	void TransformComponent::SetLocalScale( const Vector3& scale ) {
		localScale = scale;
		SetDirty();
	}

	void TransformComponent::SetScale( const Vector3& scale ) {
		if( parent.IsValidHandle() ) {
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
		if( parent.IsValidHandle() ) {
			return localScale * parent->GetScale();
		}
		return localScale;
	}

	const Matrix4& TransformComponent::WorldToLocalMatrix() const {
		if( isDirty ) {
			const_cast<TransformComponent*>( this )->RecalculateMatrices();
		}
		return worldToLocalMatrix;
	}

	const Matrix4& TransformComponent::LocalToWorldMatrix() const {
		if( isDirty ) {
			const_cast<TransformComponent*>( this )->RecalculateMatrices();
		}
		return localToWorldMatrix;
	}

	void TransformComponent::SetDirty() {
		for( auto& child : childTransforms ) {
			child->SetDirty();
		}
		isDirty = true;
	}

	void TransformComponent::RecalculateMatrices() {
		localToWorldMatrix =
			Matrix4::TranslationMatrix( localPosition ) *
			Matrix4::RotationMatrix( localRotation ) *
			Matrix4::ScaleMatrix( localScale );

		worldToLocalMatrix =
			Matrix4::ScaleMatrix( Vector3::one / localScale ) *
			Matrix4::RotationMatrix( localRotation.Inverse() ) *
			Matrix4::TranslationMatrix( -localPosition );

		if( parent.IsValidHandle() ) {
			localToWorldMatrix *= parent->LocalToWorldMatrix();
			worldToLocalMatrix = parent->WorldToLocalMatrix() * worldToLocalMatrix;
		}

		isDirty = false;
	}

	void TransformComponent::SetParent( const ComponentHandle<TransformComponent>& parent ) {
		if( this->parent.IsValidHandle() ) {
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
