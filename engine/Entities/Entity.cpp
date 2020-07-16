#include "stdafx.h"
#include "Entity.h"

#include "ComponentHandle.h"
#include "TransformComponent.h"

namespace axlt {
	
	ExactArray<Serializable*> Entity::GetComponentsAsSerializables() const {
		ExactArray<Serializable*> serializables( m_components.GetSize() );
		uint32_t i = 0;
		for (auto& kvp : m_components) {
			serializables[ i++ ] = BaseComponentHelper::GetComponentHelpers()[ kvp.key ]->GetSerializable( kvp.value );
		}
		return serializables;
	}

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