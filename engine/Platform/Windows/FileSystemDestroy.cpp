#include "stdafx.h"
#include "FileSystem/FileSystem.h"

#include <windows.h>

#include "FileSystem/Directory.h"

namespace axlt {

	bool FileSystem::DeleteFileFromDisk( const String& path ) {
		return DeleteFile( path.GetData() );
	}
}