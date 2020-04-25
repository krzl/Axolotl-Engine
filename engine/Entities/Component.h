// ReSharper disable CppClangTidyCppcoreguidelinesMacroUsage
#pragma once

#include <cstdint>
#include "Collections/SparseArray.h"
#include "Collections/Map.h"
#include "SystemBase.h"
#include "Entity.h"

namespace axlt {

	template<typename ComponentType>
	struct ComponentHelper {
		inline static uint16_t currentIdx = 0;
		SparseArray<ComponentType, SparseArrayAllocator<HeapArrayAllocator, HeapArrayAllocator>> storage;
		Map<uint32_t, uint32_t> lookupMap = {};

		uint16_t id;

		ComponentHelper() {
			id = currentIdx++;
		}

		ComponentType& GetComponentByIndex( const uint32_t componentIndex ) {
			return storage[lookupMap[componentIndex]];
		}

		const ComponentType& GetComponentByIndex( const uint32_t componentIndex ) const {
			return storage[lookupMap[componentIndex]];
		}

		template<typename... ArgsType>
		ComponentHandle<ComponentType> CreateComponent( ArgsType&&... args ) {
			SparseArrayAllocationInfo allocation = storage.Emplace( Forward<ArgsType>( args )... );
			ComponentType* component = (ComponentType*) allocation.element;
			component->m_componentIndex = ComponentType::m_componentCounter++;
			lookupMap.Add( component->m_componentIndex, allocation.index );
			return ComponentHandle( component );
		}

		void DestroyComponent( const uint32_t componentIndex ) {
			storage.Remove( lookupMap[componentIndex] );
			lookupMap.Remove( componentIndex );
		}
	};

	template<typename T>
	class BaseComponent {

		template<typename Comp>
		friend struct ComponentHandle;

		protected:
		
		friend struct ComponentHelper<T>;
		inline static ComponentHelper<T> helper;
		inline static uint32_t m_componentCounter = 0;

		friend class Entity;
		friend struct ComponentHelper<T>;

	public:
		
		bool IsEnabled() const {
			return m_enabled;
		}
		
		void SetEnabled( const bool value ) {
			if( value != m_enabled ) {
				if ( value ) {
					SystemBase::CheckAfterEnableComponent( GetEntity() );
				} else {
					SystemBase::CheckAfterDisableComponent( GetEntity() );
				}
				m_enabled = value;
			}
		}

		Entity& GetEntity() const {
			Entity::m_entities[ m_entityIndex ];
		}
		
	private:

		uint32_t m_entityIndex = 0;
		uint32_t m_componentIndex = 0;
		bool m_enabled = true;
	};

#define DEFINE_COMPONENT( Type )								\
	friend class axlt::Entity;									\
	friend struct axlt::ComponentHandle<Type>;					\
	friend struct axlt::ComponentHelper<Type>;					\
	inline static axlt::ComponentHelper<Type> helper;			\
	inline static uint32_t m_componentCounter = 0;				
}
