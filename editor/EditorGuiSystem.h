#pragma once
#include <Entities/SystemBase.h>
#include <vulkan/vulkan.h>

namespace axlt::editor {
	class EditorGuiSystem : SystemBase {
		void OnInitialize() override;
		void Update() override;
	};
}
