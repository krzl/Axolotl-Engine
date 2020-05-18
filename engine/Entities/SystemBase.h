#pragma once

#include "SystemList.h"

namespace axlt {
	
	class SystemBase;
	class Entity;

	namespace Hidden_System {
		template<typename T>
		static T* CreateSystem( int32_t order ) {
			T* system = new T();
			AddToSystemList( (SystemBase*)system, order );
			return system;
		}
	}
	
	class SystemBase {

		friend class Game;
		friend class Entity;
		template<typename T>
		friend class BaseComponent;

	public:

		virtual ~SystemBase() = default;
		
		static void CheckAfterAddComponent( Entity& entity );
		static void CheckAfterEnableComponent( Entity& entity );
		static void CheckAfterDisableComponent( Entity& entity );
		static void CheckAfterRemoveComponent( Entity& entity );
		static void CheckAfterDestroyEntity( const Entity& entity );
		static void InitializeSystems();
		static void UpdateSystems();

	protected:

		virtual void TryAddEntity( Entity& entity ) {}
		virtual void TryEnableEntity( Entity& entity ) {}
		virtual void TryDisableEntity( Entity& entity ) {}
		virtual void TryRemoveEntity( Entity& entity ) {}
		virtual void RemoveEntity( const Entity& entity ) {}
		virtual void OnInitialize() {}
		virtual void Update() = 0;
	};

#define DEFINE_SYSTEM( Type, Order ) Type* g_##Type##_instance = (Type*) axlt::Hidden_System::CreateSystem<Type>( Order );
}
