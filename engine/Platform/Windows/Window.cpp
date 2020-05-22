// ReSharper disable CppParameterMayBeConst
// ReSharper disable CppZeroConstantCanBeReplacedWithNullptr
#include "stdafx.h"
#include "Window.h"

#include "Collections/Map.h"

#include "windowsx.h"

namespace axlt {

	Map<HWND, Window*> windowMap;

	LRESULT CALLBACK WindowCallbackHandler( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam ) {
		Window** windowPtr = windowMap.Find( hwnd );
		Window* window = windowPtr == nullptr ? nullptr : *windowPtr;
		if(window != nullptr && window->customCallback != nullptr ) {
			window->customCallback( hwnd, msg, wParam, lParam );
		}
		
		switch (msg) {
		case WM_CLOSE:
			DestroyWindow( hwnd );
			break;
		case WM_DESTROY:
			window->isRunning = false;
			PostQuitMessage( 0 );
			break;
		case WM_SIZE: {
			window->width = LOWORD( lParam );
			window->height = HIWORD( lParam );
			window->hasResized = true;
		}
		case WM_KEYDOWN:
			Window::HandleInputPress( wParam );
			break;
		case WM_KEYUP:
			Window::HandleInputRelease( wParam );
			break;
		case WM_MOUSEMOVE:
			Window::HandleMouseMove( GET_X_LPARAM( lParam ), GET_Y_LPARAM( lParam ) );
			break;
		case WM_LBUTTONDOWN:
			Window::HandleMousePress( 0 );
			break;
		case WM_LBUTTONUP:
			Window::HandleMouseRelease( 0 );
			break;
		case WM_RBUTTONDOWN:
			Window::HandleMousePress( 1 );
			break;
		case WM_RBUTTONUP:
			Window::HandleMouseRelease( 1 );
			break;
		case WM_MBUTTONDOWN:
			Window::HandleMousePress( 2 );
			break;
		case WM_MBUTTONUP:
			Window::HandleMouseRelease( 2 );
			break;
		case WM_MOUSEWHEEL:
			Window::HandleMouseWheel( GET_WHEEL_DELTA_WPARAM( wParam ) );
			break;
		default:
			return DefWindowProc( hwnd, msg, wParam, lParam );
		}
		return 0;
	}

	bool Window::Init( uint32_t width, uint32_t height, char const* windowName ) {

		char const* className = "ApplicationWindowClass";

		m_hInstance = GetModuleHandle( 0 );

		WNDCLASSEX wc{
			sizeof( WNDCLASSEX ),
			0,
			WindowCallbackHandler,
			0,
			0,
			m_hInstance,
			LoadIcon( NULL, IDI_APPLICATION ),
			LoadCursor( NULL, IDC_ARROW ),
			(HBRUSH)(COLOR_WINDOW + 1),
			NULL,
			className,
			LoadIcon( NULL, IDI_APPLICATION )
		};

		if (!RegisterClassEx( &wc )) {
			MessageBox( NULL, "Window registration failed", "Error", MB_ICONEXCLAMATION | MB_OK );
			return false;
		}

		m_hWnd = CreateWindowEx( WS_EX_CLIENTEDGE, className, windowName, WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT, width, height,
			NULL, NULL, m_hInstance, NULL );

		if (m_hWnd == NULL) {
			MessageBox( NULL, "Window creation failed", "Error", MB_ICONEXCLAMATION | MB_OK );
			return false;
		}

		this->width = width;
		this->height = height;

		windowMap.Add( m_hWnd, this );

		ShowWindow( m_hWnd, SW_SHOWNORMAL );
		UpdateWindow( m_hWnd );

		isRunning = true;

		return true;
	}

	void Window::Update() {
		MSG msg;
		while (PeekMessage( &msg, nullptr, 0, 0, PM_REMOVE )) {
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
	}

	uint32_t Window::GetWidth() const {
		return width;
	}

	uint32_t Window::GetHeight() const {
		return height;
	}

	HWND Window::GetHandle() const {
		return m_hWnd;
	}

	void Window::SetCustomCallbackHandler( LRESULT ( *callback )(HWND, UINT, WPARAM, LPARAM) ) {
		customCallback = callback;
	}
}
