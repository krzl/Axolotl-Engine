#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace axlt {
	class Window {

		friend class Game;

	private:

		Window() : m_hInstance(), m_hWnd() {}
		bool Init( int width, int height, char const* windowName );
		void Update();
		bool IsRunning();

		HINSTANCE m_hInstance;
		HWND m_hWnd;
	};
}