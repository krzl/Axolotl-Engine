// ReSharper disable CppClangTidyCppcoreguidelinesMacroUsage
#pragma once

#include <cstdint>
#include "Collections/SparseArray.h"
#include "Collections/Map.h"
#include "SystemBase.h"
#include "Entity.h"
#include "Resources/Serialization/Serializable.h"

namespace axlt {

	class BaseComponentHelper {

	public:

		static uint16_t currentIdx;
		virtual Serializable* GetSerializable( uint32_t index ) = 0;
	};

	Array<BaseComponentHelper*>& GetComponentHelpers();

	template<typename ComponentType>
	class ComponentHelper final : public BaseComponentHelper {
		
	public:

		SparseArray<ComponentType, SparseArrayAllocator<HeapArrayAllocator, HeapArrayAllocator>> storage;
		Map<uint32_t, uint32_t> lookupMap = {};

		uint16_t id;
		uint32_t typeHash;

		ComponentHelper() {
			id = currentIdx++;
			typeHash = GetTypeHash<ComponentType>();
			GetComponentHelpers().Add( this );
		}
		
		Serializable* GetSerializable( const uint32_t index ) override {
			return static_cast<Serializable*>( &storage[ lookupMap[ index ] ] );
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
	class BaseComponent : Serializable {

		template<typename Comp>
		friend struct ComponentHandle;

		protected:

		friend class Entity;
		friend class ComponentHelper<T>;
		
		static ComponentHelper<T> helper;
		static uint32_t m_componentCounter;

	public:

		virtual ~BaseComponent() = default;
		
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
			return *Entity::m_entities[ m_entityIndex ];
		}
		
		uint32_t GetTypeHash() override {
			return axlt::GetTypeHash<T>();
		}
		
	private:

		uint32_t m_entityIndex = 0;
		uint32_t m_componentIndex = 0;
		bool m_enabled = true;
	};

	template<typename T>
	ComponentHelper<T> BaseComponent<T>::helper = ComponentHelper<T>();

	template<typename T>
	uint32_t BaseComponent<T>::m_componentCounter = 0;
	
#define DEFINE_COMPONENT( Type )								\
	friend class axlt::Entity;									\
	friend struct axlt::ComponentHandle<Type>;					\
	friend struct axlt::ComponentHelper<Type>;					\
	inline static axlt::ComponentHelper<Type> helper;			\
	inline static uint32_t m_componentCounter = 0;				
}
