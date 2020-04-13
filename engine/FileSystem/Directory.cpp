#include "stdafx.h"
#include "Directory.h"
#include "FileSystem.h"

namespace axlt {

	Directory::Directory( String path, FileSystem& fileSystem, const uint32_t parentIndex ) :
		path( Move( path ) ),
		fileSystem( fileSystem ),
		parentIndex( parentIndex ) {

		childDirectoryIndices.Clear();
		childFileIndices.Clear();
	}

	String Directory::AbsolutePath() const {
		if( parentIndex == rootDirectory ) {
			return path;
		}
		return fileSystem.directories[parentIndex].AbsolutePath() + '\\' + path;
	}

	void Directory::PrintContent( const uint32_t indentation ) const {

		for( uint32_t i = 0; i < indentation; i++ ) {
			printf( "\t" );
		}

		printf( "- %s\n", path.GetData() );

		for( uint32_t fileIndex : childFileIndices ) {
			File& file = fileSystem.files[fileIndex];
			for( uint32_t i = 0; i < indentation + 1; i++ ) {
				printf( "\t" );
			}
			printf( "%s\n", file.fileName.GetData() );
		}

		for( uint32_t childIndex : childDirectoryIndices ) {
			Directory& child = fileSystem.directories[childIndex];
			child.PrintContent( indentation + 1 );
		}
	}

	File* Directory::GetFileByName( const String& fileName ) const {
		for( uint32_t fileIndex : childFileIndices ) {
			if( fileSystem.files[fileIndex].fileName == fileName ) {
				return &fileSystem.files[fileIndex];
			}
		}
		return nullptr;
	}

	uint32_t Directory::Index() const {
		return (uint32_t) ( ( SparseArray<Directory>::ArrayElement* ) this - fileSystem.directories.GetData() );
	}

	File& Directory::CreateFile( const String& fileName ) const {
		return *(File*) fileSystem.files.Emplace( fileName, fileSystem, Index() ).element;
	}
}
