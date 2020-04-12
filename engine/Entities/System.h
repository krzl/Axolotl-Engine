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
		static void TryGetComponent( ComponentHandle<T>& compHandle, Entity& entity, bool& success ) {
			compHandle = entity.GetComponent<T>();
			if( !compHandle.IsValid() ) {
				success = false;
			}
		}
	}

	template<typename... Types>
	class System : SystemBase {
		
		bool Contains( const Entity& entity ) {
			return m_componentTuples.Find( &entity ) != nullptr;
		}

		void TryAddEntity( Entity& entity ) override {
			if( Contains( entity ) ) return;
			Tuple<ComponentHandle<Types>...> tuple;
			bool isSuccess = true;
			tuple.ForEach( [ &isSuccess, &entity ]( auto& t ) { if( isSuccess ) Hidden_System::TryGetComponent( std::forward<decltype( t )>( t ), entity, isSuccess ); } );
			if( isSuccess ) {
				m_componentTuples.Emplace( &entity, tuple );
			}
		}

		void TryRemoveEntity( Entity& entity ) override {
			if( !Contains( entity ) ) return;
			Tuple<ComponentHandle<Types>...> tuple;
			bool isSuccess = true;
			tuple.ForEach( [ &isSuccess, &entity ]( auto& t ) { if( isSuccess ) Hidden_System::TryGetComponent( std::forward<decltype( t )>( t ), entity, isSuccess ); } );
			if( !isSuccess ) {
				m_componentTuples.Remove( &entity );
			}
		}
		
		void RemoveEntity( const Entity& entity ) override {
			if( Contains( entity ) ) {
				m_componentTuples.Remove( &entity );
			}
		}

		virtual void Update( Tuple<ComponentHandle<Types>...> tuple ) = 0;

		void UpdateSystem() override {
			for( auto& pair : m_componentTuples ) {
				Update( pair.value );
			}
		}
		
		Map<Entity*, Tuple<ComponentHandle<Types>...>, SetAllocator<SparseArrayAllocator<HeapArrayAllocator>>> m_componentTuples;
	};

#define DEFINE_SYSTEM( Type ) inline static Type g_##Type##_instance;
	
}