#include "stdafx.h"
#include "../Input.h"
#include "Window.h"

#include "Collections/BitArray.h"

axlt::BitArray<axlt::FixedArrayAllocator<static_cast<int>(axlt::Key::KEY_ENUM_COUNT)>> heldButtons;
axlt::Array<axlt::Key> pressedButtons;
axlt::Array<axlt::Key> releasedButtons;

axlt::Vector2Int previousMousePos;
axlt::Vector2Int mousePos;

float mouseWheelDelta = 0.0f;

axlt::Key TranslateKeyCode( uint64_t keyCode );

namespace axlt {
	void Window::HandleInputPress( const uint64_t keyCode ) {
		const Key key = TranslateKeyCode( keyCode );
		if (key == axlt::Key::KEY_ENUM_COUNT) return;
		heldButtons[ (uint16_t)key ] = true;
		pressedButtons.Add( key );
	}

	void Window::HandleInputRelease( const uint64_t keyCode ) {
		const Key key = TranslateKeyCode( keyCode );
		if (key == axlt::Key::KEY_ENUM_COUNT) return;
		heldButtons[ (uint16_t)key ] = false;
		releasedButtons.Add( key );
	}

	void Window::HandleMousePress( uint8_t mouseButtonId ) {
		const Key key = (Key)mouseButtonId;
		heldButtons[ (uint16_t)key ] = true;
		pressedButtons.Add( key );
	}

	void Window::HandleMouseRelease( uint8_t mouseButtonId ) {
		const Key key = (Key)mouseButtonId;
		heldButtons[ (uint16_t)key ] = false;
		releasedButtons.Add( key );
	}

	void Window::HandleMouseMove( const int16_t x, const int16_t y ) {
		mousePos = Vector2Int( x, y );
	}

	void Window::HandleMouseWheel( const float delta ) {
		mouseWheelDelta = delta;
	}
}

namespace axlt::input {

	void ResetLastFrameInput() {
		pressedButtons.Clear();
		releasedButtons.Clear();
		previousMousePos = mousePos;
		mouseWheelDelta = 0.0f;
	}

	bool GetKey( const Key key ) {
		return heldButtons[ (uint16_t)key ];
	}

	bool GetKeyDown( const Key key ) {
		for (Key k : pressedButtons) {
			if (k == key) {
				return true;
			}
		}
		return false;
	}

	bool GetKeyUp( const Key key ) {
		for (Key k : releasedButtons) {
			if (k == key) {
				return true;
			}
		}
		return false;
	}

	Vector2Int GetMousePos() {
		return mousePos;
	}

	Vector2Int GetMouseDelta() {
		return mousePos - previousMousePos;
	}

	float GetMouseScrollDelta() {
		return mouseWheelDelta;
	}
}

using namespace axlt;

#undef DELETE

Key TranslateKeyCode( const uint64_t keyCode ) {
	switch (keyCode) {
	case VK_BACK:
		return Key::BACKSPACE;
	case VK_TAB:
		return Key::TAB;
	case VK_RETURN:
		return Key::ENTER;
	case VK_CAPITAL:
		return Key::CAPS_LOCK;
	case VK_ESCAPE:
		return Key::ESCAPE;
	case VK_SPACE:
		return Key::SPACE;
	case VK_PRIOR:
		return Key::PAGE_UP;
	case VK_NEXT:
		return Key::PAGE_DOWN;
	case VK_END:
		return Key::END;
	case VK_HOME:
		return Key::HOME;
	case VK_LEFT:
		return Key::LEFT_ARROW;
	case VK_UP:
		return Key::UP_ARROW;
	case VK_RIGHT:
		return Key::RIGHT_ARROW;
	case VK_DOWN:
		return Key::DOWN_ARROW;
	case VK_INSERT:
		return Key::INSERT;
	case VK_DELETE:
		return Key::DELETE;
	case 0x30:
		return Key::DIGIT_0;
	case 0x31:
		return Key::DIGIT_1;
	case 0x32:
		return Key::DIGIT_2;
	case 0x33:
		return Key::DIGIT_3;
	case 0x34:
		return Key::DIGIT_4;
	case 0x35:
		return Key::DIGIT_5;
	case 0x36:
		return Key::DIGIT_6;
	case 0x37:
		return Key::DIGIT_7;
	case 0x38:
		return Key::DIGIT_8;
	case 0x39:
		return Key::DIGIT_9;
	case 0x41:
		return Key::A;
	case 0x42:
		return Key::B;
	case 0x43:
		return Key::C;
	case 0x44:
		return Key::D;
	case 0x45:
		return Key::E;
	case 0x46:
		return Key::F;
	case 0x47:
		return Key::G;
	case 0x48:
		return Key::H;
	case 0x49:
		return Key::I;
	case 0x4A:
		return Key::J;
	case 0x4B:
		return Key::K;
	case 0x4C:
		return Key::L;
	case 0x4D:
		return Key::M;
	case 0x4E:
		return Key::N;
	case 0x4F:
		return Key::O;
	case 0x50:
		return Key::P;
	case 0x51:
		return Key::Q;
	case 0x52:
		return Key::R;
	case 0x53:
		return Key::S;
	case 0x54:
		return Key::T;
	case 0x55:
		return Key::U;
	case 0x56:
		return Key::V;
	case 0x57:
		return Key::W;
	case 0x58:
		return Key::X;
	case 0x59:
		return Key::Y;
	case 0x5A:
		return Key::Z;
	case VK_NUMPAD0:
		return Key::NUM_0;
	case VK_NUMPAD1:
		return Key::NUM_1;
	case VK_NUMPAD2:
		return Key::NUM_2;
	case VK_NUMPAD3:
		return Key::NUM_3;
	case VK_NUMPAD4:
		return Key::NUM_4;
	case VK_NUMPAD5:
		return Key::NUM_5;
	case VK_NUMPAD6:
		return Key::NUM_6;
	case VK_NUMPAD7:
		return Key::NUM_7;
	case VK_NUMPAD8:
		return Key::NUM_8;
	case VK_NUMPAD9:
		return Key::NUM_9;
	case VK_MULTIPLY:
		return Key::NUM_ASTERISK;
	case VK_ADD:
		return Key::NUM_PLUS;
	case VK_SUBTRACT:
		return Key::NUM_MINUS;
	case VK_DECIMAL:
		return Key::NUM_PERIOD;
	case VK_DIVIDE:
		return Key::NUM_SLASH;
	case VK_F1:
		return Key::F1;
	case VK_F2:
		return Key::F2;
	case VK_F3:
		return Key::F3;
	case VK_F4:
		return Key::F4;
	case VK_F5:
		return Key::F5;
	case VK_F6:
		return Key::F6;
	case VK_F7:
		return Key::F7;
	case VK_F8:
		return Key::F8;
	case VK_F9:
		return Key::F9;
	case VK_F10:
		return Key::F10;
	case VK_F11:
		return Key::F11;
	case VK_F12:
		return Key::F12;
	case VK_NUMLOCK:
		return Key::NUM_LOCK;
	case VK_LSHIFT:
	case VK_RSHIFT:
		return Key::SHIFT;
	case VK_LCONTROL:
	case VK_RCONTROL:
		return Key::CTRL;
	case VK_LMENU:
	case VK_RMENU:
		return Key::ALT;
	case VK_OEM_MINUS:
		return Key::MINUS;
	case VK_OEM_PLUS:
		return Key::PLUS;
	case VK_OEM_3:
		return Key::APOSTROPHE;
	case VK_OEM_4:
		return Key::LEFT_BRACKET;
	case VK_OEM_6:
		return Key::RIGHT_BRACKET;
	case VK_OEM_5:
		return Key::BACKSLASH;
	case VK_OEM_1:
		return Key::SEMICOLON;
	case VK_OEM_7:
		return Key::QUOTE;
	case VK_OEM_COMMA:
		return Key::COMMA;
	case VK_OEM_PERIOD:
		return Key::PERIOD;
	case VK_OEM_2:
		return Key::SLASH;
	default:
		return Key::KEY_ENUM_COUNT;
	}
}