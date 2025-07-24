//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef _WIN32

#include <Windows.h>
#include <string>
#include <vector>
#include <sstream>
#include <functional>

#ifndef GET_X_LPARAM
	#define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#endif
#ifndef GET_Y_LPARAM
	#define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))
#endif

#include "windows/messageloop.hpp"
#include "graphics/window.hpp"
#include "core/input.hpp"
#include "core/log.hpp"

#include "graphics/opengl/opengl_core.hpp"
#include "graphics/opengl/opengl.hpp"

using KalaWindow::Core::MessageLoop;
using KalaWindow::Graphics::Window;
using KalaWindow::Graphics::WindowData;
using KalaWindow::Graphics::ShutdownState;
using KalaWindow::Graphics::OpenGL::Renderer_OpenGL;
using KalaWindow::Core::Input;
using KalaWindow::Core::Key;
using KalaWindow::Core::MouseButton;
using KalaWindow::Core::Logger;
using KalaWindow::Core::LogType;
using KalaWindow::Core::Key;

using std::string;
using std::to_string;
using std::vector;
using std::stringstream;
using std::hex;
using std::dec;
using std::function;

using std::unordered_map;

static const unordered_map<WPARAM, Key> VKToKeyMap = {
	// Letters
	{ 'A', Key::A }, { 'B', Key::B }, { 'C', Key::C }, { 'D', Key::D },
	{ 'E', Key::E }, { 'F', Key::F }, { 'G', Key::G }, { 'H', Key::H },
	{ 'I', Key::I }, { 'J', Key::J }, { 'K', Key::K }, { 'L', Key::L },
	{ 'M', Key::M }, { 'N', Key::N }, { 'O', Key::O }, { 'P', Key::P },
	{ 'Q', Key::Q }, { 'R', Key::R }, { 'S', Key::S }, { 'T', Key::T },
	{ 'U', Key::U }, { 'V', Key::V }, { 'W', Key::W }, { 'X', Key::X },
	{ 'Y', Key::Y }, { 'Z', Key::Z },

	// Numbers
	{ '0', Key::Num0 }, { '1', Key::Num1 }, { '2', Key::Num2 }, { '3', Key::Num3 },
	{ '4', Key::Num4 }, { '5', Key::Num5 }, { '6', Key::Num6 }, { '7', Key::Num7 },
	{ '8', Key::Num8 }, { '9', Key::Num9 },

	// Function Keys
	{ VK_F1, Key::F1 }, { VK_F2, Key::F2 }, { VK_F3, Key::F3 }, { VK_F4, Key::F4 },
	{ VK_F5, Key::F5 }, { VK_F6, Key::F6 }, { VK_F7, Key::F7 }, { VK_F8, Key::F8 },
	{ VK_F9, Key::F9 }, { VK_F10, Key::F10 }, { VK_F11, Key::F11 }, { VK_F12, Key::F12 },
	{ VK_F13, Key::F13 }, { VK_F14, Key::F14 }, { VK_F15, Key::F15 }, { VK_F16, Key::F16 },
	{ VK_F17, Key::F17 }, { VK_F18, Key::F18 }, { VK_F19, Key::F19 }, { VK_F20, Key::F20 },
	{ VK_F21, Key::F21 }, { VK_F22, Key::F22 }, { VK_F23, Key::F23 }, { VK_F24, Key::F24 },

	// Numpad
	{ VK_NUMPAD0, Key::Numpad0 }, { VK_NUMPAD1, Key::Numpad1 }, { VK_NUMPAD2, Key::Numpad2 },
	{ VK_NUMPAD3, Key::Numpad3 }, { VK_NUMPAD4, Key::Numpad4 }, { VK_NUMPAD5, Key::Numpad5 },
	{ VK_NUMPAD6, Key::Numpad6 }, { VK_NUMPAD7, Key::Numpad7 }, { VK_NUMPAD8, Key::Numpad8 },
	{ VK_NUMPAD9, Key::Numpad9 },
	{ VK_ADD, Key::NumpadAdd }, { VK_SUBTRACT, Key::NumpadSubtract },
	{ VK_MULTIPLY, Key::NumpadMultiply }, { VK_DIVIDE, Key::NumpadDivide },
	{ VK_DECIMAL, Key::NumpadDecimal }, { VK_NUMLOCK, Key::NumLock },

	// Navigation
	{ VK_LEFT, Key::ArrowLeft }, { VK_RIGHT, Key::ArrowRight },
	{ VK_UP, Key::ArrowUp }, { VK_DOWN, Key::ArrowDown },
	{ VK_HOME, Key::Home }, { VK_END, Key::End },
	{ VK_PRIOR, Key::PageUp }, { VK_NEXT, Key::PageDown },
	{ VK_INSERT, Key::Insert }, { VK_DELETE, Key::Delete },

	// Controls
	{ VK_RETURN, Key::Enter }, { VK_ESCAPE, Key::Escape },
	{ VK_BACK, Key::Backspace }, { VK_TAB, Key::Tab },
	{ VK_CAPITAL, Key::CapsLock }, { VK_SPACE, Key::Space },

	// Modifiers
	{ VK_LSHIFT, Key::ShiftLeft }, { VK_RSHIFT, Key::ShiftRight },
	{ VK_LCONTROL, Key::CtrlLeft }, { VK_RCONTROL, Key::CtrlRight },
	{ VK_LMENU, Key::AltLeft }, { VK_RMENU, Key::AltRight },
	{ VK_LWIN, Key::SuperLeft }, { VK_RWIN, Key::SuperRight },

	// System / Special
	{ VK_SNAPSHOT, Key::PrintScreen }, { VK_SCROLL, Key::ScrollLock },
	{ VK_PAUSE, Key::Pause }, { VK_APPS, Key::Menu },

	// Symbols / OEM
	{ VK_OEM_MINUS, Key::Minus }, { VK_OEM_PLUS, Key::Equal },
	{ VK_OEM_4, Key::BracketLeft }, { VK_OEM_6, Key::BracketRight },
	{ VK_OEM_5, Key::Backslash }, { VK_OEM_1, Key::Semicolon },
	{ VK_OEM_7, Key::Apostrophe }, { VK_OEM_COMMA, Key::Comma },
	{ VK_OEM_PERIOD, Key::Period }, { VK_OEM_2, Key::Slash },
	{ VK_OEM_3, Key::Tilde }, { VK_OEM_102, Key::Oem102 },

	// Media & Browser
	{ VK_MEDIA_PLAY_PAUSE, Key::MediaPlayPause },
	{ VK_MEDIA_STOP, Key::MediaStop },
	{ VK_MEDIA_NEXT_TRACK, Key::MediaNextTrack },
	{ VK_MEDIA_PREV_TRACK, Key::MediaPrevTrack },
	{ VK_VOLUME_UP, Key::VolumeUp },
	{ VK_VOLUME_DOWN, Key::VolumeDown },
	{ VK_VOLUME_MUTE, Key::VolumeMute },
	{ VK_LAUNCH_MAIL, Key::LaunchMail },
	{ VK_LAUNCH_APP1, Key::LaunchApp1 },
	{ VK_LAUNCH_APP2, Key::LaunchApp2 },
	{ VK_BROWSER_BACK, Key::BrowserBack },
	{ VK_BROWSER_FORWARD, Key::BrowserForward },
	{ VK_BROWSER_REFRESH, Key::BrowserRefresh },
	{ VK_BROWSER_STOP, Key::BrowserStop },
	{ VK_BROWSER_SEARCH, Key::BrowserSearch },
	{ VK_BROWSER_FAVORITES, Key::BrowserFavorites },
	{ VK_BROWSER_HOME, Key::BrowserHome }
};

static Key TranslateVirtualKey(WPARAM vk)
{
	auto it = VKToKeyMap.find(vk);
	if (it != VKToKeyMap.end()) return it->second;

	return Key::Unknown;
}

static LRESULT CALLBACK InternalWindowProcCallback(
	HWND hwnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam);
static bool ProcessMessage(const MSG& msg, Window* window);

namespace KalaWindow::Core
{
	void* MessageLoop::WindowProcCallback()
	{
		return reinterpret_cast<void*>(&InternalWindowProcCallback);
	}
}

static LRESULT CursorTest(
	HWND hwnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam)
{
	POINT cursor{};
	cursor.x = GET_X_LPARAM(lParam);
	cursor.y = GET_Y_LPARAM(lParam);

	RECT rect{};
	GetWindowRect(hwnd, &rect);

	//if cursor isnt inside the window
	if (!PtInRect(&rect, cursor)) return HTNOWHERE;

	constexpr int border = 10;

	bool onLeft = cursor.x >= rect.left && cursor.x < rect.left + border;
	bool onRight = cursor.x < rect.right && cursor.x >= rect.right - border;
	bool onTop = cursor.y >= rect.top && cursor.y < rect.top + border;
	bool onBottom = cursor.y < rect.bottom && cursor.y >= rect.bottom - border;

	//corners
	if (onLeft && onTop) return HTTOPLEFT;
	if (onRight && onTop) return HTTOPRIGHT;
	if (onLeft && onBottom) return HTBOTTOMLEFT;
	if (onRight && onBottom) return HTBOTTOMRIGHT;

	//edges
	if (onLeft) return HTLEFT;
	if (onRight) return HTRIGHT;
	if (onTop) return HTTOP;
	if (onBottom) return HTBOTTOM;

	//not near border
	return HTCLIENT;
}

static LRESULT CALLBACK InternalWindowProcCallback(
	HWND hwnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam)
{
	Window* window{};
	for (auto& windowPtr : Window::windows)
	{
		if (!windowPtr) continue;

		WindowData& data = windowPtr->GetWindowData();
		if (ToVar<HWND>(data.hwnd) == hwnd)
		{
			window = windowPtr;
			break;
		}
	}
	if (window == nullptr) return DefWindowProc(hwnd, msg, wParam, lParam);

	//
	// ENSURE CURSOR ICON IS CORRECT WHEN INSIDE WINDOW
	//

	if (msg == WM_NCHITTEST)
	{
		auto result = CursorTest(hwnd, msg, wParam, lParam);

		/*
		string resultValue{};

		if (result == 1) resultValue = "center";
		if (result == 10) resultValue = "left edge";
		if (result == 11) resultValue = "right edge";
		if (result == 12) resultValue = "top bar";
		if (result == 13) resultValue = "top left corner";
		if (result == 14) resultValue = "top right corner";
		if (result == 15) resultValue = "bottom edge";
		if (result == 16) resultValue = "bottom left corner";
		if (result == 17) resultValue = "bottom right corner";

		Logger::Print(
			"WM_NCHITTEST result: " + resultValue + " [" + to_string(result) + "]",
			"MESSAGELOOP",
			LogType::LOG_DEBUG);
		*/

		return result;
	}

	//
	// OTHER MESSAGES
	//

	MSG msgObj{};
	msgObj.hwnd = hwnd;
	msgObj.message = msg;
	msgObj.wParam = wParam;
	msgObj.lParam = lParam;

	bool handled = ProcessMessage(msgObj, window);

	//tell windows this message is fully handled
	if (handled) return 0;

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

static bool ProcessMessage(const MSG& msg, Window* window)
{
	if (window == nullptr)
	{
		Logger::Print(
			"Cannot use 'ProcessMessage' because window is nullptr!",
			"MESSAGELOOP",
			LogType::LOG_ERROR,
			2);

		return false;
	}

	/*
	if (msg.message == 0)
	{
		Logger::Print(
			"Received empty or WM_NULL message.",
			"MESSAGELOOP",
			LogType::LOG_DEBUG);
	}
	else
	{
		stringstream ss{};
		ss << "MSG { " << "\n"
			<< "hwnd: " << msg.hwnd << "\n"
			<< ", message: 0x" << hex << msg.message << "\n"
			<< ", wParam: 0x" << hex << msg.wParam << "\n"
			<< ", lParam: 0x" << hex << msg.lParam << "\n"
			<< ", time: " << dec << msg.time << "\n"
			<< ", pt: (" << msg.pt.x << ", " << msg.pt.y << ")" << "\n"
			<< " }";

		Logger::Print(
			"Got message: " + ss.str(),
			"MESSAGELOOP",
			LogType::LOG_DEBUG);
	}
	*/

	switch (msg.message)
	{
		//
		// KEYBOARD INPUT
		//

	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
	{
		Key key = TranslateVirtualKey(msg.wParam);
		Input::SetKeyState(key, true);

		Logger::Print(
			"Windows detected keyboard key down: " + to_string(static_cast<int>(key)),
			"MESSAGELOOP",
			LogType::LOG_DEBUG);
		return false;
	}
	case WM_SYSKEYUP:
	case WM_KEYUP:
	{
		Key key = TranslateVirtualKey(msg.wParam);
		Input::SetKeyState(key, false);

		Logger::Print(
			"Windows detected keyboard key up: " + to_string(static_cast<int>(key)),
			"MESSAGELOOP",
			LogType::LOG_DEBUG);
			
		return false;
	}

	//
	// MOUSE MOVE
	//

	case WM_MOUSEMOVE:
	{
		vec2 newPos =
		{
			float(GET_X_LPARAM(msg.lParam)),
			float(GET_Y_LPARAM(msg.lParam))
		};

		//get the old position *before* updating
		vec2 oldPos = Input::GetMousePosition();

		vec2 delta = {
			newPos.x - oldPos.x,
			newPos.y - oldPos.y
		};

		Input::SetMousePosition(newPos);
		Input::SetMouseDelta(delta);

		return false;
	}

	//
	// MOUSE WHEEL
	//

	case WM_MOUSEWHEEL:
	{
		int delta = GET_WHEEL_DELTA_WPARAM(msg.wParam);

		//convert to float steps (+1 or -1)
		float scroll = 0.0f;
		if (delta > 0) scroll = +1.0f;
		else if (delta < 0) scroll = -1.0f;

		Input::SetMouseWheelDelta(scroll);

		return false;
	}

	//
	// MOUSE DOUBLE CLICK
	//

	case WM_LBUTTONDBLCLK:
	{
		Input::SetMouseButtonDoubleClickState(MouseButton::Left, true);

		Logger::Print(
			"Windows detected left mouse key double click.",
			"MESSAGELOOP",
			LogType::LOG_DEBUG);
		return false;
	}
	case WM_RBUTTONDBLCLK:
	{
		Input::SetMouseButtonDoubleClickState(MouseButton::Right, true);

		Logger::Print(
			"Windows detected right mouse key double click.",
			"MESSAGELOOP",
			LogType::LOG_DEBUG);
		return false;
	}
	case WM_MBUTTONDBLCLK:
	{
		Input::SetMouseButtonDoubleClickState(MouseButton::Right, true);

		Logger::Print(
			"Windows detected middle mouse key double click.",
			"MESSAGELOOP",
			LogType::LOG_DEBUG);
		return false;
	}
	case WM_XBUTTONDBLCLK:
	{
		WORD button = GET_XBUTTON_WPARAM(msg.wParam);
		if (button == XBUTTON1)
		{
			Input::SetMouseButtonDoubleClickState(MouseButton::X1, true);

			Logger::Print(
				"Windows detected x1 mouse key double click.",
				"MESSAGELOOP",
				LogType::LOG_DEBUG);
		}
		if (button == XBUTTON2)
		{
			Input::SetMouseButtonDoubleClickState(MouseButton::X2, true);

			Logger::Print(
				"Windows detected x2 mouse key double click.",
				"MESSAGELOOP",
				LogType::LOG_DEBUG);
		}
		return false;
	}

	//
	// MOUSE BUTTONS
	//

	case WM_LBUTTONDOWN:
	{
		Input::SetMouseButtonState(MouseButton::Left, true);

		Logger::Print(
			"Windows detected left mouse key down.",
			"MESSAGELOOP",
			LogType::LOG_DEBUG);
		return false;
	}
	case WM_LBUTTONUP:
	{
		Input::SetMouseButtonState(MouseButton::Left, false);

		Logger::Print(
			"Windows detected left mouse key up.",
			"MESSAGELOOP",
			LogType::LOG_DEBUG);
		return false;
	}

	case WM_RBUTTONDOWN:
	{
		Input::SetMouseButtonState(MouseButton::Right, true);

		Logger::Print(
			"Windows detected right mouse key down.",
			"MESSAGELOOP",
			LogType::LOG_DEBUG);
		return false;
	}
	case WM_RBUTTONUP:
	{
		Input::SetMouseButtonState(MouseButton::Right, false);

		Logger::Print(
			"Windows detected right mouse key up.",
			"MESSAGELOOP",
			LogType::LOG_DEBUG);
		return false;
	}

	case WM_MBUTTONDOWN:
	{
		Input::SetMouseButtonState(MouseButton::Middle, true);

		Logger::Print(
			"Windows detected middle mouse key down.",
			"MESSAGELOOP",
			LogType::LOG_DEBUG);
		return false;
	}
	case WM_MBUTTONUP:
	{
		Input::SetMouseButtonState(MouseButton::Middle, false);

		Logger::Print(
			"Windows detected middle mouse key up.",
			"MESSAGELOOP",
			LogType::LOG_DEBUG);
		return false;
	}

	case WM_XBUTTONDOWN:
	{
		WORD button = GET_XBUTTON_WPARAM(msg.wParam);
		if (button == XBUTTON1)
		{
			Input::SetMouseButtonState(MouseButton::X1, true);

			Logger::Print(
				"Windows detected x1 mouse key down.",
				"MESSAGELOOP",
				LogType::LOG_DEBUG);
		}
		if (button == XBUTTON2)
		{
			Input::SetMouseButtonState(MouseButton::X2, true);

			Logger::Print(
				"Windows detected x2 mouse key down.",
				"MESSAGELOOP",
				LogType::LOG_DEBUG);
		}
		return false;
	}
	case WM_XBUTTONUP:
	{
		WORD button = GET_XBUTTON_WPARAM(msg.wParam);
		if (button == XBUTTON1)
		{
			Input::SetMouseButtonState(MouseButton::X1, false);

			Logger::Print(
				"Windows detected x1 mouse key up.",
				"MESSAGELOOP",
				LogType::LOG_DEBUG);
		}
		if (button == XBUTTON2)
		{
			Input::SetMouseButtonState(MouseButton::X2, false);

			Logger::Print(
				"Windows detected x2 mouse key up.",
				"MESSAGELOOP",
				LogType::LOG_DEBUG);
		}
		return false;
	}

	//
	// RAW MOUSE INPUT FOR EXTRA BUTTONS
	//

	case WM_INPUT:
	{
		UINT size = 0;
		GetRawInputData(
			(HRAWINPUT)msg.lParam,
			RID_INPUT,
			nullptr,
			&size,
			sizeof(RAWINPUTHEADER));

		vector<BYTE> buffer(size);
		if (GetRawInputData(
			(HRAWINPUT)msg.lParam,
			RID_INPUT,
			buffer.data(),
			&size,
			sizeof(RAWINPUTHEADER)) != size)
		{
			return false;
		}

		RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(buffer.data());

		if (raw->header.dwType == RIM_TYPEMOUSE)
		{
			const RAWMOUSE& mouse = raw->data.mouse;

			//add support for up to 8 extra mouse buttons
			for (int i = 0; i < 8; i++)
			{
				USHORT downFlag = RI_MOUSE_BUTTON_1_DOWN << (i * 2);
				USHORT upFlag = RI_MOUSE_BUTTON_1_UP << (i * 2);

				if (mouse.usButtonFlags & downFlag)
				{
					Input::SetKeyState(static_cast<Key>(static_cast<int>(MouseButton::X3) + i), true);
				}
				if (mouse.usButtonFlags & upFlag)
				{
					Input::SetKeyState(static_cast<Key>(static_cast<int>(MouseButton::X3) + i), false);
				}
			}

			//sets raw mouse movement
			if (mouse.usFlags == MOUSE_MOVE_RELATIVE)
			{
				vec2 newMouseRawDelta = Input::GetRawMouseDelta();

				newMouseRawDelta.x += mouse.lLastX;
				newMouseRawDelta.y += mouse.lLastY;

				Input::SetRawMouseDelta(newMouseRawDelta);
			}
		}

		return false;
	}

	//
	// CURSOR ICON
	//

	case WM_SETCURSOR:
	{
		//use default cursor if cursor is over client area
		if (LOWORD(msg.lParam) == HTCLIENT)
		{
			SetCursor(LoadCursor(nullptr, IDC_ARROW));
			return true;
		}

		//let windows handle non-client areas
		return false;
	}

	//
	// WINDOW REDRAW
	//

	case WM_PAINT:
	{
		WindowData& win = window->GetWindowData();
		HWND hwnd = ToVar<HWND>(win.hwnd);
		
		PAINTSTRUCT ps;
		BeginPaint(hwnd, &ps);
		EndPaint(hwnd, &ps);

		return true;
	}

	//
	// WINDOW RESIZE
	//

	case WM_SIZE:
	{
		int width = LOWORD(msg.lParam);
		int height = HIWORD(msg.lParam);

		if (Renderer_OpenGL::IsInitialized()) glViewport(0, 0, width, height);

		/*
		Logger::Print(
			"New resolution: " + to_string(width) + ", " + to_string(height),
			"MESSAGELOOP",
			LogType::LOG_DEBUG);
		*/

		function<void()> callback = window->GetResizeCallback();
		if (callback) callback();

		return true;
	}
	case WM_SIZING:
	{
		HWND windowRef = ToVar<HWND>(window->GetWindowData().hwnd);

		if (window->IsInitialized())
		{
			window->TriggerRedraw();
		}
		return true;
	}

	//
	// CAP MIN AND MAX WINDOW SIZE
	//

	case WM_GETMINMAXINFO:
	{
		HWND windowRef = ToVar<HWND>(window->GetWindowData().hwnd);

		MINMAXINFO* mmi = reinterpret_cast<MINMAXINFO*>(msg.lParam);

		mmi->ptMinTrackSize.x = window->GetMinSize().x;
		mmi->ptMinTrackSize.y = window->GetMinSize().y;

		mmi->ptMaxTrackSize.x = window->GetMaxSize().x;
		mmi->ptMaxTrackSize.y = window->GetMaxSize().y;

		return true;
	}

	//
	// SHUTDOWN
	//

	//user clicked X button or pressed Alt + F4
	case WM_CLOSE:
		Window::Shutdown(ShutdownState::SHUTDOWN_CLEAN);
		return true;

	//window was destroyed - tell the system to exit
	case WM_DESTROY:
		Logger::Print(
			"Window is destroyed.",
			"MESSAGELOOP",
			LogType::LOG_DEBUG);
		PostQuitMessage(0);
		return true;

	default:
		return false;
	}

	return false;
}

#endif //_WIN32