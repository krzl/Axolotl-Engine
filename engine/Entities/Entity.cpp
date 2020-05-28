#include "stdafx.h"
#include "Entity.h"

#include "ComponentHandle.h"
#include "TransformComponent.h"

namespace axlt {

	void Entity::Destroy() const {
		ComponentHandle<TransformComponent> transform = GetComponent<TransformComponent>();
		if (transform.IsValidHandle()) {
			for (const ComponentHandle<TransformComponent>& child : transform->GetChildren()) {
				child->GetEntity().Destroy();
			}
			transform->SetParent( ComponentHandle<TransformComponent>::CreateInvalidHandle() );
		}
		m_entities.Remove( m_index );
		SystemBase::CheckAfterDestroyEntity( *this );
	}
}