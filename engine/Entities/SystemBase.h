#pragma once

#include "stdafx.h"

namespace axlt {
	
	class SystemBase;
	class Entity;
	
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
		static void UpdateSystems();

	protected:
			
		virtual void TryAddEntity( Entity& entity ) = 0;
		virtual void TryEnableEntity( Entity& entity ) = 0;
		virtual void TryDisableEntity( Entity& entity ) = 0;
		virtual void TryRemoveEntity( Entity& entity ) = 0;
		virtual void RemoveEntity( const Entity& entity ) = 0;
		virtual void UpdateSystem() = 0;
	};
}
