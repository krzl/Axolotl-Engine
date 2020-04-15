#include "stdafx.h"
#include "ResourceData.h"
#include "ModelResource.h"
#include "Serialization/Serializer.h"

namespace axlt {
	ResourceData::ResourceData( const Guid& guid, const uint32_t version, const uint32_t type, void* data ) :
		guid( guid ),
		version( version ),
		type( type ),
		data( data ) {}

	ResourceData::ResourceData() :
		guid( 0, 0 ),
		version( 0 ),
		type( 0 ),
		data( nullptr ) {}


	void ResourceData::Serialize( const File& file ) {
		Serializer serializer( file, "wb" );
		serializer << guid << version << type << *(ModelResource*) data << Serializer::end;
	}

	ResourceData ResourceData::Deserialize( const File& file ) {
		ResourceData res;
		res.data = new ModelResource();

		Serializer serializer( file, "rb" );
		serializer >> res.guid >> res.version >> res.type >> *(ModelResource*) res.data >> Serializer::end;
		return res;
	}
}
