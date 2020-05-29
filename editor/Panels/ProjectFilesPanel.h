#pragma once
#include "EditorPanel.h"

namespace axlt {
	class Directory;
}

namespace axlt::editor {
	class ProjectFilesPanel final : public EditorPanel {

	public:

		inline static const char* label = "Project Files";

	private:
		
		void Update() override;

		void DrawFile( File& file );
		void DrawFileList();
		void DrawDirectoryList( Directory& directory, const bool isFirst );

		Array<File*> files;

		Directory* selectedDirectory = nullptr;
		File* selectedFile = nullptr;
	};
}
