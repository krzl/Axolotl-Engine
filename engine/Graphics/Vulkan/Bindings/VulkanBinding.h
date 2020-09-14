#pragma once

#include "Collections/Map.h"
#include "../Vulkan.h"

namespace axlt::vk {
	class VulkanBinding {

	public:
		
		static void Destroy( uint32_t instanceId );
		static void Destroy( uint32_t instanceId, uint8_t delay );
		static void CheckDelayedDestroy();

		static void DestroyAll();

		template<typename T, typename R>
		static T* GetBinding( const ResourceHandle<R>& resource ) {
			VulkanBinding** ptr = bindingMap.Find( resource->GetInstanceId() );
			if( ptr == nullptr ) {
				return nullptr;
			}
			return (T*) *ptr;
		}

	protected:

		~VulkanBinding() = default;

		virtual void Reset() = 0;
		virtual void RemoveBinding() = 0;

		static Map<uint32_t, VulkanBinding*> bindingMap;
		static SparseArray<Pair<VulkanBinding*, uint8_t>> bindingsToDestroy;
	};
}
