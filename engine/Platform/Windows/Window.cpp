// ReSharper disable CppParameterMayBeConst
// ReSharper disable CppZeroConstantCanBeReplacedWithNullptr
#include "stdafx.h"
#include "Window.h"

namespace axlt {

	bool isRunning = false;

	LRESULT CALLBACK WindowCallbackHandler( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam ) {
		switch( msg ) {
			case WM_CLOSE:
				DestroyWindow( hwnd );
				break;
			case WM_DESTROY:
				isRunning = false;
				PostQuitMessage( 0 );
				break;
			default:
				return DefWindowProc( hwnd, msg, wParam, lParam );
		}
		return 0;
	}

	bool Window::Init( int width, int height, char const* windowName ) {

		char const* className = "ApplicationWindowClass";

		m_hInstance = GetModuleHandle(0);

		WNDCLASSEX wc{
			sizeof( WNDCLASSEX ),
			0,
			WindowCallbackHandler,
			0,
			0,
			m_hInstance,
			LoadIcon( NULL, IDI_APPLICATION ),
			LoadCursor( NULL, IDC_ARROW ),
			(HBRUSH) ( COLOR_WINDOW + 1 ),
			NULL,
			className,
			LoadIcon( NULL, IDI_APPLICATION )
		};

		if( !RegisterClassEx( &wc ) ) {
			MessageBox( NULL, "Window registration failed", "Error", MB_ICONEXCLAMATION | MB_OK );
			return false;
		}

		m_hWnd = CreateWindowEx( WS_EX_CLIENTEDGE, className, windowName, WS_OVERLAPPEDWINDOW,
								 CW_USEDEFAULT, CW_USEDEFAULT, width, height,
								 NULL, NULL, m_hInstance, NULL );

		if( m_hWnd == NULL ) {
			MessageBox( NULL, "Window creation failed", "Error", MB_ICONEXCLAMATION | MB_OK );
			return false;
		}

		ShowWindow( m_hWnd, SW_SHOWNORMAL );
		UpdateWindow( m_hWnd );

		isRunning = true;

		return true;
	}

	void Window::Update() {
		MSG msg;
		while( PeekMessage( &msg, nullptr, 0, 0, PM_REMOVE ) ) {
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
	}

	bool Window::IsRunning() {
		return isRunning;
	}
}
