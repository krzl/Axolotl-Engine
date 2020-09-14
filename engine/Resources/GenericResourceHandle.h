#pragma once
#include "FileSystem/Guid.h"
#include "Collections/Map.h"

namespace axlt {
	class GenericResourceHandle {

		friend class ResourceLoader;

	public:

		GenericResourceHandle() = default;

		explicit GenericResourceHandle( void* ptr ) {
			operator=( ptr );
		}

		GenericResourceHandle& operator=( void* ptr ) {
			//SharedPtr<void>::operator=( ptr );
			return *this;
		}

	private:

		void* resourceHandlePtr = nullptr;

		inline static Map<uint32_t, ResourceLoader*> resourceLoaders;
	};
}
