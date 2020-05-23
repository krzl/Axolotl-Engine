#pragma once
#include <rapidjson/document.h>
#include "Common/String.h"

namespace axlt {
	class Directory;
	class FileSystem;

	class File {

		friend class FileSystem;
		friend class Directory;

	public:

		explicit File( String path, FileSystem& fileSystem, uint32_t parentDirectoryIndex );
		File( const File& other ) = default;
		File( File&& other ) noexcept;
		File& operator=( const File& other );
		File& operator=( File&& other ) noexcept;

		~File() = default;

		String AbsolutePath() const;
		String Extension() const;
		Directory& ParentDirectory() const;

		uint32_t Index() const;
		void DeleteFromDisk() const;

		String CalculateMd5() const;

		rapidjson::Document ToJson() const;
		void FromJson( const rapidjson::Document& doc ) const;
		void ChangeParentDirectory( Directory& directory );

		String fileName;
		uint32_t directoryIndex;

	private:

		FileSystem& fileSystem;
	};
}
