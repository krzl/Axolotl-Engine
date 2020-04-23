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
			file = &directories[directoryIndex].CreateFile( fileName );
		}
		return *file;
	}

	File* FileSystem::FindFile( const String& fileName, uint32_t directoryIndex ) {
		return directories[directoryIndex].GetFileByName( fileName );
	}

	File* FileSystem::FindFile( const String& absolutePath ) {
		String path = absolutePath;
		uint32_t currentDirIndex = 0;
		if( path.Substring( 0, RootDirectory().path.Length() ) == RootDirectory().path ) {
			path = path.Substring( RootDirectory().path.Length() + 1, path.Length() - RootDirectory().path.Length() - 1 );
		} else {
			return nullptr;
		}
		uint32_t indexOf = path.FirstIndexOf( '/' );
		while( indexOf != String::indexNotFound ) {
			String dir = absolutePath.Substring( 0, indexOf );
			if( dir.Length() != 0 ) {
				for( uint32_t index : directories[currentDirIndex].childDirectoryIndices ) {
					if( directories[index].path == dir ) {
						currentDirIndex = index;
						path = path.Substring( indexOf + 1, path.Length() - indexOf - 1 );
						break;
					}
				}
			}
			indexOf = path.FirstIndexOf( '/' );
		}
		return FindFile( path, currentDirIndex );
	}
}
