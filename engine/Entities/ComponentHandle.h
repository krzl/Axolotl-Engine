#pragma once

namespace axlt {
	class Entity;

	template<typename ComponentType>
	struct ComponentHandle {

		friend class Entity;
		template<typename T>
		friend uint32_t GetHash( const ComponentHandle<T>& handle ) { 
			return handle.m_componentIndex;
		}

	private:

		uint32_t m_componentIndex;

		explicit ComponentHandle( const uint32_t index ) : m_componentIndex( index ) {}

	public:

		ComponentHandle() {
			m_componentIndex = 0xFFFFFFFF;
		}

		// ReSharper disable once CppNonExplicitConvertingConstructor
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

		bool IsValidHandle() const {
			return m_componentIndex != 0xFFFFFFFF && ComponentType::helper.lookupMap.Find( m_componentIndex ) != nullptr;
		}

		static ComponentHandle<ComponentType> CreateInvalidHandle() {
			return ComponentHandle<ComponentType>( 0xFFFFFFFF );
		}
	};
}