#pragma once
#include "GenericResourceHandle.h"

namespace axlt {
	class Guid;

	class ResourceLoader {

	public:

		ResourceLoader( void*( *loadFunction )(const Guid&), void( *unloadFunction )(void*), const uint32_t resourceTypeHash ) :
			loadFunction( loadFunction ),
			unloadFunction( unloadFunction ){
			GenericResourceHandle::resourceLoaders.Add( resourceTypeHash, this );
		}

		void*( *loadFunction )(const Guid&) = nullptr;
		void( *unloadFunction )(void *) = nullptr;
	};

	template<typename T>
	class ResourceLoaderHelper {

	public:
		
		ResourceLoaderHelper() : loader( 
			(void*(*)(const Guid&)) ResourceHandle<T>::LoadHeap, 
			(void( *)(void *)) ResourceHandle<T>::UnloadHeap,
			GetTypeHash<T>()
		) {}

	private:

		ResourceLoader loader;
	};
}
