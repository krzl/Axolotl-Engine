#pragma once
#include "FileSystem/Guid.h"
#include "FileSystem/File.h"
#include "Serialization/Serializer.h"
#include "FileSystem/FileSystem.h"

#include "Collections/Map.h"

namespace axlt {
	
	template<typename T>
	class ResourceHandle final : public SharedPtr<T> {

	public:

		ResourceHandle() = default;
		// ReSharper disable once CppNonExplicitConvertingConstructor
		ResourceHandle( T* ptr ) {
			operator=( ptr );
		}

		static ResourceHandle Load( const Guid& guid ) {
			ResourceHandle handle;

			uint32_t* foundInstanceId = guidToInstanceId.Find( guid );
			if( foundInstanceId != nullptr ) {
				WeakPtr<T>* handlePtr = instanceIdToHandle.Find( *foundInstanceId );
				if( handlePtr != nullptr ) {
					handle.SharedPtr<T>::operator=( *handlePtr );
				} else {
					handle.LoadUnused( guid );
					instanceIdToHandle.Add( handle->GetInstanceId(), WeakPtr<T>( handle ) );
				}
			} else {
				handle.LoadUnused( guid );
				guidToInstanceId.Add( guid, handle->GetInstanceId() );
				instanceIdToHandle.Add( handle->GetInstanceId(), WeakPtr<T>( handle ) );
			}

			return handle;
		}

		ResourceHandle& operator=( T* ptr ) {
			SharedPtr<T>::operator=( ptr );
			return *this;
		}

	private:

		void LoadUnused( const Guid& guid ) {
			Serializer serializer( ( String( "../ImportedFiles/" ) + guid.ToString() ).GetData(), "rb" );
			T* ptr = new T();
			serializer >> *ptr >> Serializer::end;
			SharedPtr<T>::operator=( ptr );
		}

		inline static Map<Guid, uint32_t> guidToInstanceId;
		inline static Map<uint32_t, WeakPtr<T>> instanceIdToHandle;
	};

	template<typename T>
	Serializer& operator<<( Serializer& s, ResourceHandle<T>& handle ) {
		s << handle->guid;
		return s;
	}

	template<typename T>
	Serializer& operator>>( Serializer& s, ResourceHandle<T>& handle ) {
		Guid guid(0, 0 );
		s >> guid;
		handle = ResourceHandle<T>::Load( guid );
		return s;
	}
}