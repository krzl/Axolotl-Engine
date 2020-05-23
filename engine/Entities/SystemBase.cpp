#include "stdafx.h"
#include "SystemBase.h"
#include "SystemList.h"

namespace axlt {

	void SystemBase::CheckAfterAddComponent( Entity& entity ) {
		for (Hidden_System::SystemListElement& elem : Hidden_System::GetSystemList()) {
			elem.system->TryAddEntity( entity );
		}
	}

	void SystemBase::CheckAfterEnableComponent( Entity& entity ) {
		for (Hidden_System::SystemListElement& elem : Hidden_System::GetSystemList()) {
			elem.system->TryEnableEntity( entity );
		}
	}

	void SystemBase::CheckAfterDisableComponent( Entity& entity ) {
		for (Hidden_System::SystemListElement& elem : Hidden_System::GetSystemList()) {
			elem.system->TryDisableEntity( entity );
		}
	}

	void SystemBase::CheckAfterRemoveComponent( Entity& entity ) {
		for (Hidden_System::SystemListElement& elem : Hidden_System::GetSystemList()) {
			elem.system->TryRemoveEntity( entity );
		}
	}

	void SystemBase::CheckAfterDestroyEntity( const Entity& entity ) {
		for (Hidden_System::SystemListElement& elem : Hidden_System::GetSystemList()) {
			elem.system->RemoveEntity( entity );
		}
	}

	void SystemBase::InitializeSystems() {
		for (Hidden_System::SystemListElement& elem : Hidden_System::GetSystemList()) {
			elem.system->Setup();
		}
	}

	void SystemBase::UpdateSystems() {
		for (Hidden_System::SystemListElement& elem : Hidden_System::GetSystemList()) {
			elem.system->Update();
		}
	}
}
