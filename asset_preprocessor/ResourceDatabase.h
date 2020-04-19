#pragma once

#include <Collections/Set.h>
#include <FileSystem/FileSystem.h>
#include <FileSystem/Guid.h>
#include <Resources/ResourceHandle.h>

namespace axlt::editor {
	class ResourceDatabase {

	public:

		ResourceDatabase();

	private:

		File& GetImportFile( const File& file );
		bool IsGuidOccupied( const Guid& guid );
		Guid GenerateGuid();
		void ImportFile( File& file, const String& md5, const String& importMd5, const String& guid );

		FileSystem resourceFileSystem;
		FileSystem importsFileSystem;

		Set<ResourceHandle<void>> lookupSet{};
	};
}
