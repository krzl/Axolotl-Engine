#include "../stdafx.h"
#include "FileImport.h"

namespace axlt::editor {
	ResourceHandle<void> ImportFile( File& file, const Guid& guid ) {

	#define IMPORT_FUNCTION( ImportFunction, Version, Ext, Type )														\
		if( file.Extension() == String(Ext) ) {																			\
			return ResourceHandle<void>( guid, Version, GetHash( String(#Type) ), (void*) ImportFunction( file ) );		\
		}

	#include "ImportExtensionTypes.inl"

	#undef IMPORT_FUNCTION

		return ResourceHandle<void>( guid, 0, 0, nullptr );
	}
}
