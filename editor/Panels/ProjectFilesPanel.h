#pragma once
#include "EditorPanel.h"

namespace axlt::editor {
	class ProjectFilesPanel final : public EditorPanel {
		void Update() override;

		Array<File*> files;
	};
}
