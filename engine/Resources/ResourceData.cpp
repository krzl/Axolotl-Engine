#include "stdafx.h"
#include "ResourceData.h"
#include "Serialization/Serializer.h"
#include "BinaryResource.h"
#include "ModelResource.h"

namespace axlt {

	ResourceData::ResourceData() :
		guid( 0, 0 ),
		version( 0 ),
		type( 0 ),
		data( nullptr ) {}

	ResourceData::ResourceData( const Guid& guid, const uint32_t version, const uint32_t type, void* data ) :
		guid( guid ),
		version( version ),
		type( type ),
		data( data ) {}

	void SerializeResource( Serializer& serializer, const uint32_t type, void* data ) {
		if( data == nullptr ) return;
		//TODO: Cache type hashes
	#define RESOURCE_TYPE( Type )					\
		if( GetHash( String( #Type ) ) == type ) {	\
			serializer << *(Type*)data;				\
			return;									\
		}

	#include "ResourceTypes.inl"
	}

	void ResourceData::Serialize( const File& file ) {
		Serializer serializer( file, "wb" );
		serializer << guid << version << type;
		SerializeResource( serializer, type, data );
		serializer << Serializer::end;
	}

	void* DeserializeResource( Serializer& serializer, const uint32_t type ) {
		//TODO: Cache type hashes
	#define RESOURCE_TYPE( Type )					\
		if( GetHash( String( #Type ) ) == type ) {	\
			Type* ptr = new Type();					\
			serializer >> *ptr;						\
			return (void*) ptr;						\
		}

	#include "ResourceTypes.inl"

		return nullptr;
	}

	ResourceData ResourceData::Deserialize( const File& file ) {
		ResourceData res;
		Serializer serializer( file, "rb" );
		serializer >> res.guid >> res.version >> res.type;
		res.data = DeserializeResource( serializer, res.type );
		serializer >> Serializer::end;
		return res;
	}
}
