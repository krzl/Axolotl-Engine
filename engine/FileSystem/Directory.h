#pragma once
#include "File.h"
#include "Collections/Array.h"
#include "Common/String.h"

namespace axlt {

	class FileSystem;

	class Directory {

		friend class FileSystem;
		friend class File;

	public:

		explicit Directory( String path, FileSystem& fileSystem, uint32_t parentIndex );

		String AbsolutePath() const;
		void PrintContent( uint32_t indentation ) const;
		File* GetFileByName( const String& fileName ) const;
		uint32_t Index() const;
		File& CreateFile( const String& fileName ) const;

		static inline const uint32_t rootDirectory = 0xFFFFFFFF;

		const String path;
		Array<uint32_t> childDirectoryIndices;
		Array<uint32_t> childFileIndices;

	private:

		FileSystem& fileSystem;
		uint32_t parentIndex;
	};
}
