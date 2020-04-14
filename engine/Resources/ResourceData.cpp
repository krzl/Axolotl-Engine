#include "stdafx.h"
#include "ResourceData.h"
#include "ModelResource.h"

namespace axlt {
	ResourceData::ResourceData( const Guid& guid, const uint32_t version, const uint32_t type, void* data ) :
		guid( guid ),
		version( version ),
		type( type ),
		data( data ) {}

	void ResourceData::Serialize( const File& file ) {
		FILE* fp;
		fopen_s( &fp, file.AbsolutePath().GetData(), "wb" );
		fwrite( &guid, sizeof guid, 1, fp );
		fwrite( &version, sizeof version, 1, fp );
		fwrite( &type, sizeof type, 1, fp );
		((ModelResource*) data)->Serialize( fp );
		fclose( fp );
	}
}
