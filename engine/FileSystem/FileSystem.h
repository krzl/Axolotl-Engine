#pragma once
#include "Directory.h"
#include "File.h"
#include "Collections/SparseArray.h"

namespace axlt {

	class FileSystem {

		friend class File;
		
	public:

		explicit FileSystem( const String& basePath );

		void PrintContent() const;
		Directory& RootDirectory();
		const Directory& RootDirectory() const;
		File& FindOrCreateFile( const String& fileName, uint32_t directoryIndex );
		File* FindFile( const String& fileName, uint32_t directoryIndex );
		File* FindFile( const String& absolutePath );

		SparseArray<Directory> directories;
		SparseArray<File> files;

	private:

		void GetDirectoryContents( uint32_t currentDirectoryIndex );
		static bool DeleteFileFromDisk( const String& path );
	};
}
