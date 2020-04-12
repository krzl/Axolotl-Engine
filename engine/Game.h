#pragma once

#include "Platform/Window.h"

namespace axlt {
	class Game {

	public:

		Game() : m_window() {}
		void Init();
		void Run();
		void Shutdown();

	private:

		Window m_window;
	};

	// ReSharper disable once CppInconsistentNaming
	extern Game GameInstance;
}