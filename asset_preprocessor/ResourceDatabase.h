#pragma once

#include "ResourceData.h"
#include <Collections/Set.h>
#include <FileSystem/FileSystem.h>

namespace axlt::editor {
	class ResourceDatabase {

	public:

		ResourceDatabase();

	private:
		
		File& GetImportFile( const File& file );
		bool IsGuidOccupied( const String& guid );
		String GenerateGuid();
		void ImportFile( File& file, const String& md5, const String& importMd5, const String& guid );

		FileSystem resourceFileSystem;
		FileSystem importsFileSystem;

		Set<ResourceData> lookupSet;
	};
}
