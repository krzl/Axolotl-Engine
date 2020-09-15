#include "stdafx.h"
#include "VulkanBinding.h"

namespace axlt::vk {
	
	SparseArray<Pair<VulkanBinding*, uint8_t>> VulkanBinding::bindingsToDestroy;
	Map<uint32_t, VulkanBinding*> VulkanBinding::bindingMap;

	void VulkanBinding::Destroy( const uint32_t instanceId ) {
		VulkanBinding** binding = bindingMap.Find( instanceId );
		if( binding != nullptr ) {
			( *binding )->Reset();
			( *binding )->RemoveBinding();
			bindingMap.Remove( instanceId );
		}
	}

	void VulkanBinding::Destroy( const uint32_t instanceId, const uint8_t delay ) {
		VulkanBinding** binding = bindingMap.Find( instanceId );
		if( binding != nullptr ) {
			bindingsToDestroy.Emplace( *binding, delay );
			bindingMap.Remove( instanceId );
		}
	}

	void VulkanBinding::CheckDelayedDestroy() {
		for( uint32_t i = 0; i < bindingsToDestroy.GetDataArraySize(); i++ ) {
			if( bindingsToDestroy.IsFree( i ) ) continue;

			Pair<VulkanBinding*, uint8_t>& pair = bindingsToDestroy[i];
			VulkanBinding* binding = pair.key;
			pair.value--;
			if( pair.value == 0 ) {
				pair.key->Reset();
				pair.key->RemoveBinding();
				bindingsToDestroy.Remove( i );
			}
		}
	}

	void VulkanBinding::DestroyAll() {
		for( uint32_t i = 0; i < bindingsToDestroy.GetDataArraySize(); i++ ) {
			if( bindingsToDestroy.IsFree( i ) ) continue;

			Pair<VulkanBinding*, uint8_t>& pair = bindingsToDestroy[i];
			pair.key->Reset();
			pair.key->RemoveBinding();
			bindingsToDestroy.Remove( i );
		}

		for( auto& binding : bindingMap ) {
			binding.value->Reset();
			binding.value->RemoveBinding();
		}

		bindingMap.Clear();
	}
}
