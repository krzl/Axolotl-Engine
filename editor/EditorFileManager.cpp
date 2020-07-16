#include "stdafx.h"
#include "EditorFileManager.h"
#include <Game.h>

namespace axlt::editor {
	FileSystem EditorFileManager::importFileSystem( "../ImportedFiles" );
	FileSystem EditorFileManager::assetsFileSystem( "../ProjectFiles" );

	void EditorFileManager::Init() {
		GameInstance.SetImportFileSystem( importFileSystem );
	}
}
