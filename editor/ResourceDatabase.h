#pragma once

#include <Collections/Set.h>
#include <FileSystem/FileSystem.h>
#include <FileSystem/Guid.h>
#include <Resources/ResourceHandle.h>
#include <Collections/Map.h>

namespace axlt::editor {
	class ResourceDatabase {

	public:

		static ResourceDatabase* instance;
		
		ResourceDatabase();

		void ImportAll();

		FileSystem resourceFileSystem;
		FileSystem importsFileSystem;

		Set<ResourceHandle<uint8_t>> lookupSet{};
		Map<Guid, String> guidToFilepath{};

	private:

		File& GetImportFile( const File& file );
		bool IsGuidOccupied( const Guid& guid );
		Guid GenerateGuid();
		void ImportFile( File& file, const String& md5, const String& importMd5, const String& guid );
	};
}
