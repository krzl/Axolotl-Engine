#pragma once
#include "FileSystem/Guid.h"
#include "FileSystem/File.h"
#include "Serialization/Serializer.h"

namespace axlt {

	namespace resourceHandleInner {
		void SerializeResource( Serializer& serializer, const uint32_t type, void* data );
		void* DeserializeResource( Serializer& serializer, const uint32_t type );
	}

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

		static ResourceHandle Deserialize( const File& file ) {
			ResourceHandle res;
			Serializer serializer( file, "rb" );
			serializer >> res.guid >> res.version >> res.type;
			res.data = (T*) resourceHandleInner::DeserializeResource( serializer, res.type );
			serializer >> Serializer::end;
			return res;
		}

		static ResourceHandle Deserialize( const char* filePath ) {
			ResourceHandle res;
			Serializer serializer( filePath, "rb" );
			serializer >> res.guid >> res.version >> res.type;
			res.data = (T*) resourceHandleInner::DeserializeResource( serializer, res.type );
			serializer >> Serializer::end;
			return res;
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
