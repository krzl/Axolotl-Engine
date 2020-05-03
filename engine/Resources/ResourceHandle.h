#pragma once
#include "FileSystem/Guid.h"
#include "FileSystem/File.h"
#include "Serialization/Serializer.h"
#include "FileSystem/FileSystem.h"

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

		ResourceHandle() :
			guid( 0, 0 ),
			version( 0 ),
			type( 0 ),
			data( nullptr ) {}

		explicit ResourceHandle( const String& guid ) :
			guid( Guid::FromString( guid ) ),
			version( 0 ),
			type( 0 ),
			data( nullptr ) {}

		explicit ResourceHandle( const Guid& guid ) :
			guid( guid ),
			version( 0 ),
			type( 0 ),
			data( nullptr ) {}

		ResourceHandle( const Guid& guid, const uint32_t version, const uint32_t type, T* data ) :
			guid( guid ),
			version( version ),
			type( type ),
			data( data ) {}

		void Serialize( const File& file ) {
			Serializer serializer( file, "wb" );
			serializer << guid << version << type;
			resourceHandleInner::SerializeResource( serializer, type, data );
			serializer << Serializer::end;
		}

		static ResourceHandle Load( const char* filePath ) {
			ResourceHandle handle;
			handle.Deserialize( filePath );
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
			serializer >> guid >> version >> type;
			data = (T*) resourceHandleInner::DeserializeResource<T>( serializer, type );
			serializer >> Serializer::end;
		}

		T& operator*() {
			return *data;
		}

		const T& operator*() const {
			return *data;
		}

		T* operator->() {
			return data;
		}

		const T* operator->() const {
			return data;
		}

		Guid guid;
		uint32_t version;
		uint32_t type;
		T* data;
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
