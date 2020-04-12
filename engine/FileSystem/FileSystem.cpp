#include "stdafx.h"
#include "FileSystem.h"

namespace axlt {

	FileSystem::FileSystem( const String& basePath ) {
		directories.Emplace( basePath, *this, Directory::rootDirectory );
		GetDirectoryContents( 0 );
	}

	void FileSystem::PrintContent() const {
		RootDirectory().PrintContent( 0 );
	}

	Directory& FileSystem::RootDirectory() {
		return directories[0];
	}

	const Directory& FileSystem::RootDirectory() const {
		return directories[0];
	}

	File& FileSystem::FindOrCreateFile( const String& fileName, uint32_t directoryIndex ) {
		File* file = directories[directoryIndex].GetFileByName( fileName );
		if( file == nullptr ) {
			file = (File*) files.Emplace( fileName, 0, *this, directoryIndex ).element;
		}
		return *file;
	}

	File* FileSystem::FindFile( const String& fileName, uint32_t directoryIndex ) {
		return directories[directoryIndex].GetFileByName( fileName );
	}
}
