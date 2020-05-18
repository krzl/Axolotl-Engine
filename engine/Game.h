#pragma once

#include "Platform/Window.h"

namespace axlt {
	class Game {

	public:

		void Init();
		void Run();
		void Shutdown();

		const Window& GetWindow() const;

	private:

		Window m_window;
	};

	// ReSharper disable once CppInconsistentNaming
	extern Game GameInstance;
}