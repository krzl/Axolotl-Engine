// ReSharper disable CppClangTidyCppcoreguidelinesMacroUsage
#pragma once

#include <cstdint>
#include "Collections/SparseArray.h"
#include "Collections/Map.h"

namespace axlt {
	class Entity;

	template<typename ComponentType>
	struct ComponentHandle {

		friend class Entity;
		template<typename T>
		friend uint32_t GetHash( const ComponentHandle<T>& handle );

	private:

		uint32_t m_componentIndex;

		explicit ComponentHandle( const uint32_t index ) : m_componentIndex( index ) {}

	public:

		ComponentHandle() {
			m_componentIndex = 0xFFFFFFFF;
		}

		ComponentHandle( const ComponentType& component ) {
			m_componentIndex = component.m_componentIndex;
		}

		ComponentHandle( ComponentType* component ) {
			if( component == nullptr ) {
				m_componentIndex = 0xFFFFFFFF;
			} else {
				m_componentIndex = component->m_componentIndex;
			}
		}

		ComponentHandle( const ComponentHandle& handle ) {
			m_componentIndex = handle.m_componentIndex;
		}

		ComponentHandle& operator=( const ComponentHandle& handle ) {  // NOLINT(cert-oop54-cpp)
			if( this == &handle ) return *this;
			m_componentIndex = handle.m_componentIndex;
			return *this;
		}

		ComponentType& operator*() {
			return ComponentType::helper.GetComponentByIndex( m_componentIndex );
		}

		const ComponentType& operator*() const {
			return ComponentType::helper.GetComponentByIndex( m_componentIndex );
		}

		ComponentType* operator->() {
			return &ComponentType::helper.GetComponentByIndex( m_componentIndex );
		}

		const ComponentType* operator->() const {
			return &ComponentType::helper.GetComponentByIndex( m_componentIndex );
		}

		bool IsValid() const {
			return m_componentIndex != 0xFFFFFFFF && ComponentType::helper.lookupMap.Find( m_componentIndex ) != nullptr;
		}

		static ComponentHandle<ComponentType> CreateInvalidHandle() {
			return ComponentHandle<ComponentType>( 0xFFFFFFFF );
		}
	};

	template<typename ComponentType>
	uint32_t GetHash( const ComponentHandle<ComponentType>& handle ) {
		return handle.m_componentIndex;
	}

	template<typename ComponentType>
	struct ComponentHelper {
		inline static uint16_t currentIdx = 0;
		SparseArray<ComponentType, SparseArrayAllocator<HeapArrayAllocator, HeapArrayAllocator>> storage;
		Map<uint32_t, uint32_t> lookupMap = {};

		uint16_t id;

		ComponentHelper() {
			id = currentIdx++;
		}

		ComponentType& GetComponentByIndex( uint32_t componentIndex ) {
			return storage[lookupMap[componentIndex]];
		}

		const ComponentType& GetComponentByIndex( uint32_t componentIndex ) const {
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

#define DEFINE_COMPONENT( Type )							\
	friend class axlt::Entity;								\
	friend struct axlt::ComponentHandle<Type>;				\
	friend struct axlt::ComponentHelper<Type>;				\
	inline static axlt::ComponentHelper<Type> helper;		\
	inline static uint32_t m_componentCounter = 0;			\
	uint32_t m_componentIndex = 0;		

}