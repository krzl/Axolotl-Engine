#pragma once
#include "Common/String.h"
#include <rapidjson/document.h>

namespace axlt {

	class Directory;
	class FileSystem;

	class File {

		friend class FileSystem;
		friend class Directory;

	public:

		explicit File( String path, uint64_t fileSize, FileSystem& fileSystem, uint32_t parentDirectoryIndex );

		String AbsolutePath() const;
		String Extension() const;
		uint64_t FileSize() const;
		Directory& ParentDirectory() const;

		uint32_t Index() const;
		void DeleteFile() const;

		String CalculateMd5() const;

		rapidjson::Document ToJson() const;
		void FromJson( const rapidjson::Document& doc ) const;
		
		const String fileName;
		const uint32_t directoryIndex;

	private:

		FileSystem& fileSystem;
		uint64_t fileSize;
	};
}
