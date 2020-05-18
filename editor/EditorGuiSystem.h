#pragma once
#include <Entities/SystemBase.h>

namespace axlt::editor {
	class EditorGuiSystem : SystemBase {
		void OnInitialize() override;
		void Update() override;
	};
}
