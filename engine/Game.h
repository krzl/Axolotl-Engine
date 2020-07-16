#pragma once

#include "Platform/Window.h"
#include "FileSystem/FileSystem.h"

namespace axlt {
	class Game {

		template<typename T>
		friend class ResourceHandle;

	public:

		void Init();
		void Run();
		void Shutdown();

		const Window& GetWindow() const;
		void SetImportFileSystem( FileSystem& fileSystem );

	private:

		Window m_window;
		FileSystem* fileSystem = nullptr;
	};

	// ReSharper disable once CppInconsistentNaming
	extern Game GameInstance;
}
