// ReSharper disable CppClangTidyCppcoreguidelinesMacroUsage
#pragma once

#include "Collections/Tuple.h"
#include "Collections/Map.h"
#include "SystemBase.h"
#include "Component.h"

namespace axlt {

	class Entity;

	namespace Hidden_System {
		template<typename T>
		void TryGetComponent( ComponentHandle<T>& compHandle, Entity& entity, bool& success ) {
			compHandle = entity.GetComponent<T>();
			if( !compHandle.IsValid() ) {
				success = false;
			}
		}

		template<typename... Types>
		bool IsTupleEnabled( Tuple<ComponentHandle<Types>...> tuple ) {
			bool isEnabled = true;
			tuple.ForEach( [ &isEnabled ]( auto& t ) { isEnabled |= t.IsEnabled; } );
			return isEnabled;
		}
	}

	template<typename... Types>
	class System : SystemBase {
		
		bool Contains( const Entity& entity ) {
			return m_componentTuples.Find( &entity ) != nullptr && m_disabledTuples.Find( &entity ) != nullptr;
		}

		void TryAddEntity( Entity& entity ) override {
			if( Contains( entity ) ) return;
			Tuple<ComponentHandle<Types>...> tuple;
			bool isSuccess = true;
			tuple.ForEach( [ &isSuccess, &entity ]( auto& t ) { if( isSuccess ) Hidden_System::TryGetComponent( std::forward<decltype( t )>( t ), entity, isSuccess ); } );
			if( isSuccess ) {
				OnAdded( tuple );
				if( Hidden_System::IsTupleEnabled( tuple ) ) {
					OnEnabled( tuple );
					m_componentTuples.Emplace( &entity, tuple );
				} else {
					m_disabledTuples.Emplace( &entity, tuple );
				}
			}
		}

		void TryEnableEntity( Entity& entity ) override {
			Tuple<ComponentHandle<Types>...>* tuple = m_disabledTuples.Find( &entity );
			if( tuple != nullptr && Hidden_System::IsTupleEnabled( *tuple ) ) {
				OnEnabled( *tuple );
				m_componentTuples.Emplace( &entity, *tuple );
				m_disabledTuples.Remove( &entity );
			}
		}

		void TryDisableEntity( Entity& entity ) override {
			Tuple<ComponentHandle<Types>...>* tuple = m_componentTuples.Find( &entity );
			if( tuple != nullptr && !Hidden_System::IsTupleEnabled( *tuple ) ) {
				OnDisabled( *tuple );
				m_disabledTuples.Emplace( &entity, *tuple );
				m_componentTuples.Remove( &entity );
			}
		}

		void TryRemoveEntity( Entity& entity ) override {
			if( !Contains( entity ) ) return;
			Tuple<ComponentHandle<Types>...> tuple;
			bool isSuccess = true;
			tuple.ForEach( [ &isSuccess, &entity ]( auto& t ) { if( isSuccess ) Hidden_System::TryGetComponent( std::forward<decltype( t )>( t ), entity, isSuccess ); } );
			if( !isSuccess ) {
				if( Hidden_System::IsTupleEnabled( tuple ) ) {
					OnDisabled( tuple );
					m_componentTuples.Remove( &entity );
				} else {
					m_disabledTuples.Remove( &entity );
				}
				OnRemoved( tuple );
			}
		}
		
		void RemoveEntity( const Entity& entity ) override {
			if( Contains( entity ) ) {
				m_componentTuples.Remove( &entity );
			}
		}

		virtual void OnAdded( Tuple<ComponentHandle<Types>...> tuple ) {}
		virtual void OnEnabled( Tuple<ComponentHandle<Types>...> tuple ) {}
		virtual void OnDisabled( Tuple<ComponentHandle<Types>...> tuple ) {}
		virtual void OnRemoved( Tuple<ComponentHandle<Types>...> tuple ) {} 
		
		virtual void Update( Tuple<ComponentHandle<Types>...> tuple ) = 0;

		void UpdateSystem() override {
			for( auto& pair : m_componentTuples ) {
				Update( pair.value );
			}
		}

	protected:
		
		Map<Entity*, Tuple<ComponentHandle<Types>...>, SetAllocator<SparseArrayAllocator<HeapArrayAllocator>>> m_componentTuples;
		Map<Entity*, Tuple<ComponentHandle<Types>...>, SetAllocator<SparseArrayAllocator<HeapArrayAllocator>>> m_disabledTuples;
	};

#define DEFINE_SYSTEM( Type ) inline static Type g_##Type##_instance;
	
}