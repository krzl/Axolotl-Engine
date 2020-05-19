#pragma once
#include "FileSystem/Guid.h"
#include "FileSystem/File.h"
#include "Serialization/Serializer.h"
#include "FileSystem/FileSystem.h"

#include "Collections/Map.h"

namespace axlt {

	namespace resourceHandleInner {
		template<typename T>
		void SerializeResource( Serializer& serializer, const uint32_t type, T* data ) {
			if( data == nullptr ) return;
			if( T::typeHash == type ) {
				serializer << (*(T*) data);
			} else {
				printf( "Trying to serialize wrong resource type\n" );
			}
		}

		template<typename T>
		T* DeserializeResource( Serializer& serializer, const uint32_t type ) {
			if( T::typeHash == type ) {
				T* ptr = new T();
				serializer >> *ptr;
				return (T*) ptr;
			} else {
				printf( "Trying to serialize wrong resource type\n" );
				return nullptr;
			}
		}
	}

	inline static FileSystem* g_importFilesystem;

	template<typename T>
	class ResourceHandle {

	public:

		ResourceHandle() : ResourceHandle( nullptr, Guid( 0, 0 ) ) {}
		explicit ResourceHandle( const String& guid ) : ResourceHandle( nullptr, Guid::FromString( guid ) ) {}
		explicit ResourceHandle( const Guid& guid ) : ResourceHandle( nullptr, guid ) {}
		explicit ResourceHandle( T* data ) : ResourceHandle( data, Guid( 0, 0 ) ) {}

		ResourceHandle( T* data, const Guid& guid ) :
			data( data ),
			guid( guid ) {}

		void Serialize( const File& file, uint32_t type ) {
			Serializer serializer( file, "wb" );
			serializer << type;
			resourceHandleInner::SerializeResource( serializer, type, data.GetData() );
			serializer << Serializer::end;
		}

		static ResourceHandle Load( const Guid& guid ) {
			ResourceHandle handle;
			handle.guid = guid;
			
			WeakPtr<T>* foundPtr = resourceMap.Find( guid );
			if( foundPtr != nullptr ) {
				WeakPtr<T>& weakPtr = *foundPtr;
				handle.data = weakPtr;
			} else {
				handle.Deserialize( guid.ToString().GetData() );
				resourceMap.Add( guid, WeakPtr<T>( handle.data ) );
			}
			
			return handle;
		}

		
		static ResourceHandle CreateEmptyResource( const Guid& guid ) { //TODO: Remove guid from ResourceHandle
			ResourceHandle handle;
			handle.guid = guid;
			T* resource = new T();
			handle.data = resource;
			resourceMap.Add( guid, WeakPtr<T>( handle.data ) );
			return handle;
		}

		void Deserialize( const char* filePath ) {
			String path;
			if( g_importFilesystem != nullptr ) {
				path = g_importFilesystem->RootDirectory().AbsolutePath() + '/' + filePath;
			} else {
				path = filePath;
			}

			Serializer serializer( path.GetData(), "rb" );
			uint32_t type;
			serializer >> type;
			data = (T*) resourceHandleInner::DeserializeResource<T>( serializer, type );
			serializer >> Serializer::end;
		}

		T& operator*() {
			return *data.GetData();
		}

		const T& operator*() const {
			return *data.GetData();
		}

		T* operator->() {
			return data.GetData();
		}

		const T* operator->() const {
			return data.GetData();
		}

		T* GetData() {
			return data.GetData();
		}

		const T* GetData() const {
			return data.GetData();
		}

		WeakPtr<T> GetWeakPtr() const {
			return WeakPtr<T>( data );
		}
		
		SharedPtr<T> data;
		Guid guid;

	private:

		inline static Map<Guid, WeakPtr<T> > resourceMap;
	};

	template<typename T>
	uint64_t GetHash( const ResourceHandle<T>& resourceData ) {
		return GetHash( resourceData.guid );
	}

	template<typename T>
	Serializer& operator<<( Serializer& s, ResourceHandle<T>& handle ) {
		return s << handle.guid;
	}

	template<typename T>
	Serializer& operator>>( Serializer& s, ResourceHandle<T>& handle ) {
		Guid guid( 0x0, 0x0 );
		s >> guid;
		String filePath = String( "../ImportedFiles/" ) + guid.ToString();
		handle.Deserialize( filePath.GetData() );
		return s;
	}
}
