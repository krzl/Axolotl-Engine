#pragma once

#include "Collections/Map.h"
#include "Collections/SparseArray.h"
#include "SystemBase.h"
#include "Component.h"

namespace axlt {
	class Entity {

	public:

		Entity() {
			m_index = m_entities.Add( this ).index;
		}

		template<typename ComponentType, typename... ArgsType>
		ComponentHandle<ComponentType> AddComponent( ArgsType&&... args ) {
			if( GetComponent<ComponentType>().IsValid() ) return nullptr;
			ComponentHandle<ComponentType> component = ComponentType::helper.CreateComponent( Forward<ArgsType>( args )... );
			m_components.Add( ComponentType::helper.id, component.m_componentIndex );
			SystemBase::CheckAfterAddComponent( *this );
			return component;
		}

		template<typename ComponentType>
		ComponentHandle<ComponentType> GetComponent() const {
			const uint32_t* componentIndex = m_components.Find( ComponentType::helper.id );
			if( nullptr == componentIndex ) {
				return ComponentHandle<ComponentType>::CreateInvalidHandle();
			}
			return ComponentHandle<ComponentType>( *componentIndex );
		}

		template<typename ComponentType>
		bool RemoveComponent() {
			uint32_t* componentIndex = m_components.Find( ComponentType::helper.id );
			if( componentIndex != nullptr ) {
				ComponentType::helper.DestroyComponent( *componentIndex );
				m_components.Remove( ComponentType::helper.id );
				SystemBase::CheckAfterRemoveComponent( *this );
			}
			return componentIndex != nullptr;
		}

		void Destroy() const {
			m_entities.Remove( m_index );
			SystemBase::CheckAfterDestroyEntity( *this );
		}

	private:

		uint32_t m_index;
		Map<uint32_t, uint32_t> m_components;

		inline static SparseArray<Entity*> m_entities;
	};
}