#include "../stdafx.h"
#include "FileImport.h"

namespace axlt::editor {
	ResourceData ImportFile( File& file, const Guid& guid ) {

	#define IMPORT_FUNCTION( ImportFunction, Version, Ext, Type )												\
		if( file.Extension() == String(Ext) ) {																	\
			return ResourceData( guid, Version, GetHash( String(#Type) ), (void*) ImportFunction( file ) );		\
		}

	#include "ImportExtensionTypes.inl"

	#undef IMPORT_FUNCTION

		return ResourceData( guid, 0, 0, nullptr );
	}
}
