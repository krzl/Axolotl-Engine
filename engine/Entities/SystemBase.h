#pragma once

#include "Collections/Array.h"
#include "stdafx.h"

namespace axlt {

	class SystemBase;

	namespace Hidden_SystemBase {
		extern Array<SystemBase*> systems;
	}

	class Entity;

	class SystemBase {

		friend class Game;
		friend class Entity;
		template<typename T>
		friend class BaseComponent;

	public:

		virtual ~SystemBase() = default;

		SystemBase() {
			Hidden_SystemBase::systems.Add( this );
		}

		static void CheckAfterAddComponent( Entity& entity ) {
			for( SystemBase* system : Hidden_SystemBase::systems ) {
				system->TryAddEntity( entity );
			}
		}

		static void CheckAfterEnableComponent( Entity& entity ) {
			for( SystemBase* system : Hidden_SystemBase::systems ) {
				system->TryEnableEntity( entity );
			}
		}

		static void CheckAfterDisableComponent( Entity& entity ) {
			for( SystemBase* system : Hidden_SystemBase::systems ) {
				system->TryDisableEntity( entity );
			}
		}

		static void CheckAfterRemoveComponent( Entity& entity ) {
			for( SystemBase* system : Hidden_SystemBase::systems ) {
				system->TryRemoveEntity( entity );
			}
		}

		static void CheckAfterDestroyEntity( const Entity& entity ) {
			for( SystemBase* system : Hidden_SystemBase::systems ) {
				system->RemoveEntity( entity );
			}
		}

		static void UpdateSystems() {
			for( SystemBase* system : Hidden_SystemBase::systems ) {
				system->UpdateSystem();
			}
		}

	protected:

		virtual void TryAddEntity( Entity& entity ) = 0;
		virtual void TryEnableEntity( Entity& entity ) = 0;
		virtual void TryDisableEntity( Entity& entity ) = 0;
		virtual void TryRemoveEntity( Entity& entity ) = 0;
		virtual void RemoveEntity( const Entity& entity ) = 0;
		virtual void UpdateSystem() = 0;
	};
}
