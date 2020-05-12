#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace axlt {
	class Window {

		friend class Game;
		friend LRESULT CALLBACK WindowCallbackHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	public:

		uint32_t GetWidth() const;
		uint32_t GetHeight() const;

	private:

		Window() : m_hInstance(), m_hWnd() {}
		bool Init( uint32_t width, uint32_t height, char const* windowName );
		void Update();

		static void HandleInputPress( uint64_t keyCode );
		static void HandleInputRelease( uint64_t keyCode );

		uint32_t width = 0;
		uint32_t height = 0;
		
		bool hasResized = false;
		bool isRunning = false;

		HINSTANCE m_hInstance;
		HWND m_hWnd;
	};
}