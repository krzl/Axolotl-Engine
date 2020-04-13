#include "stdafx.h"
#include <windows.h>

#include "FileSystem/FileSystem.h"
#include "FileSystem/Directory.h"

namespace axlt {

	void FileSystem::GetDirectoryContents( const uint32_t currentDirectoryIndex ) {

		String directoryAbsolutePath = directories[currentDirectoryIndex].AbsolutePath() + "\\*";
		CreateDirectory( directories[currentDirectoryIndex].AbsolutePath().GetData(), NULL );
		
		WIN32_FIND_DATA findFileData;
		const HANDLE hFind = FindFirstFileA( directoryAbsolutePath.GetData(), &findFileData ); // NOLINT(misc-misplaced-const)

		if( hFind != INVALID_HANDLE_VALUE ) {
			do {
				if( findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) {
					if( findFileData.cFileName[0] == '.' ) {
						continue;
					}
					const SparseArrayAllocationInfo allocationInfo = directories.Emplace( findFileData.cFileName, *this, currentDirectoryIndex );
					directories[currentDirectoryIndex].childDirectoryIndices.Add( allocationInfo.index );
					GetDirectoryContents( allocationInfo.index );
				} else {
					const SparseArrayAllocationInfo allocationInfo = files.Emplace( findFileData.cFileName, *this, currentDirectoryIndex );
					directories[currentDirectoryIndex].childFileIndices.Emplace( allocationInfo.index );
				}
			} while( FindNextFileA( hFind, &findFileData ) );
			FindClose( hFind );
		}
	}
}
