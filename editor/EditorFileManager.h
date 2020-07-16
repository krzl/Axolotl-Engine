#pragma once

#include <FileSystem/FileSystem.h>

namespace axlt::editor {
	class EditorFileManager {

	public:
		
		static FileSystem importFileSystem;
		static FileSystem assetsFileSystem;
		
		static void Init();
	};
}
