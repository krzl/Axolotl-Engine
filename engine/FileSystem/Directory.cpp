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

	File& Directory::CreateFile( const String& fileName ) {
		const auto allocationInfo = fileSystem.files.Emplace( fileName, fileSystem, Index() );
		childFileIndices.Add( allocationInfo.index );
		return *(File*) allocationInfo.element;
	}

	Directory& Directory::GetChildDirectory( const uint32_t index ) {
		return fileSystem.directories[ childDirectoryIndices[ index ] ];
	}

	File& Directory::GetChildFile( const uint32_t index ) {
		return fileSystem.files[ childFileIndices[ index ] ];

	}

	Directory* Directory::ParentDirectory() const {
		if( parentIndex == rootDirectory ) return nullptr;
		return &fileSystem.directories[ parentIndex ];
	}

	bool Directory::IsChildDirectoryOf( const Directory& directory ) const {
		if( ParentDirectory() == nullptr ) {
			return false;
		}
		if( ParentDirectory()->Index() == directory.Index() ) {
			return true;
		}
		return ParentDirectory()->IsChildDirectoryOf( directory );
	}

	void Directory::ChangeParentDirectory( Directory& directory ) {
		if( &directory == this || directory.IsChildDirectoryOf( *this ) ) {
			return;
		}
		Directory* movedDirectoryParent = ParentDirectory();
		if (movedDirectoryParent != nullptr) {
			for (uint32_t i = 0; i < movedDirectoryParent->childDirectoryIndices.GetSize(); i++) {
				if (movedDirectoryParent->childDirectoryIndices[ i ] == Index()) {
					movedDirectoryParent->childDirectoryIndices.Remove( i );
					break;
				}
			}
		}
		parentIndex = directory.Index();
		directory.childDirectoryIndices.Add( Index() );
	}
}
