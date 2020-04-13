#include "../stdafx.h"
#include "FileImport.h"

#include <Resources/ModelResource.h>

namespace axlt::editor {
	ResourceData ImportFile( File& file, const Guid& guid ) {
		void* importData = nullptr;
		
		if( file.Extension() == "fbx" ) {
			importData = (void*) ImportFbx( file );
		}

		return ResourceData( guid, 0, 0, importData );
	}
}
