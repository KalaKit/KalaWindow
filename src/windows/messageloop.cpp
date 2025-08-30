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

#include "imgui/imgui.h"

#include "windows/messageloop.hpp"
#include "graphics/window.hpp"
#include "core/input.hpp"
#include "core/core.hpp"
#include "core/containers.hpp"
#include "graphics/opengl/opengl_functions_core.hpp"
#include "graphics/opengl/opengl.hpp"
#include "ui/debug_ui.hpp"

using KalaHeaders::Log;
using KalaHeaders::LogType;

using KalaWindow::Core::MessageLoop;
using KalaWindow::Graphics::Window;
using KalaWindow::Graphics::MenuBar;
using KalaWindow::Graphics::MenuBarEvent;
using KalaWindow::Graphics::WindowData;
using KalaWindow::Graphics::OpenGL::OpenGL_Renderer;
using namespace KalaWindow::Graphics::OpenGLFunctions;
using KalaWindow::Core::Input;
using KalaWindow::Core::Key;
using KalaWindow::Core::MouseButton;
using KalaWindow::Core::Key;
using KalaWindow::Core::KalaWindowCore;
using KalaWindow::Core::PopupAction;
using KalaWindow::Core::PopupType;
using KalaWindow::Core::PopupResult;
using KalaWindow::Core::ShutdownState;
using KalaWindow::Core::runtimeWindows;
using KalaWindow::Core::runtimeMenuBarEvents;
using KalaWindow::Core::createdWindows;
using KalaWindow::UI::DebugUI;

using std::string;
using std::to_string;
using std::vector;
using std::ostringstream;
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

static const std::unordered_map<WPARAM, ImGuiKey> VKToImGuiKeyMap = {
	// Letters
	{ 'A', ImGuiKey_A }, { 'B', ImGuiKey_B }, { 'C', ImGuiKey_C }, { 'D', ImGuiKey_D },
	{ 'E', ImGuiKey_E }, { 'F', ImGuiKey_F }, { 'G', ImGuiKey_G }, { 'H', ImGuiKey_H },
	{ 'I', ImGuiKey_I }, { 'J', ImGuiKey_J }, { 'K', ImGuiKey_K }, { 'L', ImGuiKey_L },
	{ 'M', ImGuiKey_M }, { 'N', ImGuiKey_N }, { 'O', ImGuiKey_O }, { 'P', ImGuiKey_P },
	{ 'Q', ImGuiKey_Q }, { 'R', ImGuiKey_R }, { 'S', ImGuiKey_S }, { 'T', ImGuiKey_T },
	{ 'U', ImGuiKey_U }, { 'V', ImGuiKey_V }, { 'W', ImGuiKey_W }, { 'X', ImGuiKey_X },
	{ 'Y', ImGuiKey_Y }, { 'Z', ImGuiKey_Z },

	// Numbers (top row)
	{ '0', ImGuiKey_0 }, { '1', ImGuiKey_1 }, { '2', ImGuiKey_2 }, { '3', ImGuiKey_3 },
	{ '4', ImGuiKey_4 }, { '5', ImGuiKey_5 }, { '6', ImGuiKey_6 }, { '7', ImGuiKey_7 },
	{ '8', ImGuiKey_8 }, { '9', ImGuiKey_9 },

	// Function Keys
	{ VK_F1, ImGuiKey_F1 }, { VK_F2, ImGuiKey_F2 }, { VK_F3, ImGuiKey_F3 }, { VK_F4, ImGuiKey_F4 },
	{ VK_F5, ImGuiKey_F5 }, { VK_F6, ImGuiKey_F6 }, { VK_F7, ImGuiKey_F7 }, { VK_F8, ImGuiKey_F8 },
	{ VK_F9, ImGuiKey_F9 }, { VK_F10, ImGuiKey_F10 }, { VK_F11, ImGuiKey_F11 }, { VK_F12, ImGuiKey_F12 },
	{ VK_F13, ImGuiKey_F13 }, { VK_F14, ImGuiKey_F14 }, { VK_F15, ImGuiKey_F15 }, { VK_F16, ImGuiKey_F16 },
	{ VK_F17, ImGuiKey_F17 }, { VK_F18, ImGuiKey_F18 }, { VK_F19, ImGuiKey_F19 }, { VK_F20, ImGuiKey_F20 },
	{ VK_F21, ImGuiKey_F21 }, { VK_F22, ImGuiKey_F22 }, { VK_F23, ImGuiKey_F23 }, { VK_F24, ImGuiKey_F24 },

	// Numpad
	{ VK_NUMPAD0, ImGuiKey_Keypad0 }, { VK_NUMPAD1, ImGuiKey_Keypad1 },
	{ VK_NUMPAD2, ImGuiKey_Keypad2 }, { VK_NUMPAD3, ImGuiKey_Keypad3 },
	{ VK_NUMPAD4, ImGuiKey_Keypad4 }, { VK_NUMPAD5, ImGuiKey_Keypad5 },
	{ VK_NUMPAD6, ImGuiKey_Keypad6 }, { VK_NUMPAD7, ImGuiKey_Keypad7 },
	{ VK_NUMPAD8, ImGuiKey_Keypad8 }, { VK_NUMPAD9, ImGuiKey_Keypad9 },
	{ VK_ADD, ImGuiKey_KeypadAdd }, { VK_SUBTRACT, ImGuiKey_KeypadSubtract },
	{ VK_MULTIPLY, ImGuiKey_KeypadMultiply }, { VK_DIVIDE, ImGuiKey_KeypadDivide },
	{ VK_DECIMAL, ImGuiKey_KeypadDecimal },

	// Navigation
	{ VK_LEFT, ImGuiKey_LeftArrow }, { VK_RIGHT, ImGuiKey_RightArrow },
	{ VK_UP, ImGuiKey_UpArrow }, { VK_DOWN, ImGuiKey_DownArrow },
	{ VK_HOME, ImGuiKey_Home }, { VK_END, ImGuiKey_End },
	{ VK_PRIOR, ImGuiKey_PageUp }, { VK_NEXT, ImGuiKey_PageDown },
	{ VK_INSERT, ImGuiKey_Insert }, { VK_DELETE, ImGuiKey_Delete },

	// Controls
	{ VK_RETURN, ImGuiKey_Enter }, { VK_ESCAPE, ImGuiKey_Escape },
	{ VK_BACK, ImGuiKey_Backspace }, { VK_TAB, ImGuiKey_Tab },
	{ VK_CAPITAL, ImGuiKey_CapsLock }, { VK_SPACE, ImGuiKey_Space },

	// Modifiers
	{ VK_LSHIFT, ImGuiKey_LeftShift }, { VK_RSHIFT, ImGuiKey_RightShift },
	{ VK_LCONTROL, ImGuiKey_LeftCtrl }, { VK_RCONTROL, ImGuiKey_RightCtrl },
	{ VK_LMENU, ImGuiKey_LeftAlt }, { VK_RMENU, ImGuiKey_RightAlt },
	{ VK_LWIN, ImGuiKey_LeftSuper }, { VK_RWIN, ImGuiKey_RightSuper },

	// Symbols / OEM
	{ VK_OEM_MINUS, ImGuiKey_Minus }, { VK_OEM_PLUS, ImGuiKey_Equal },
	{ VK_OEM_4, ImGuiKey_LeftBracket }, { VK_OEM_6, ImGuiKey_RightBracket },
	{ VK_OEM_5, ImGuiKey_Backslash }, { VK_OEM_1, ImGuiKey_Semicolon },
	{ VK_OEM_7, ImGuiKey_Apostrophe }, { VK_OEM_COMMA, ImGuiKey_Comma },
	{ VK_OEM_PERIOD, ImGuiKey_Period }, { VK_OEM_2, ImGuiKey_Slash },
	{ VK_OEM_3, ImGuiKey_GraveAccent }, { VK_OEM_102, ImGuiKey_Oem102 },

	// System / Special
	{ VK_SNAPSHOT, ImGuiKey_PrintScreen }, { VK_SCROLL, ImGuiKey_ScrollLock },
	{ VK_PAUSE, ImGuiKey_Pause }, { VK_APPS, ImGuiKey_Menu },
};

static Key TranslateVirtualKey(WPARAM vk)
{
	auto it = VKToKeyMap.find(vk);
	if (it != VKToKeyMap.end()) return it->second;

	return Key::Unknown;
}

static ImGuiKey TranslateVirtualKeyToImGuiKey(WPARAM vk)
{
	auto it = VKToImGuiKeyMap.find(vk);
	if (it != VKToImGuiKeyMap.end()) return it->second;

	return ImGuiKey_None; //fallback if not recognized by ImGui
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
	for (auto& windowPtr : runtimeWindows)
	{
		if (!windowPtr) continue;

		const WindowData& data = windowPtr->GetWindowData();
		if (ToVar<HWND>(data.hwnd) == hwnd)
		{
			window = windowPtr;
			break;
		}
	}
	if (window == nullptr) return DefWindowProc(hwnd, msg, wParam, lParam);

	switch (msg)
	{
	//asks if user wants to log off or shut down (in case any data is unsaved)
	case WM_QUERYENDSESSION:
	{
		if (KalaWindowCore::CreatePopup(
			"Quitting application",
			"Are you sure you want to quit? Unclosed data may be lost!",
			PopupAction::POPUP_ACTION_YES_NO,
			PopupType::POPUP_TYPE_WARNING)
			== PopupResult::POPUP_RESULT_YES)
		{
			KalaWindowCore::Shutdown(ShutdownState::SHUTDOWN_CLEAN);
			return TRUE; //user clicked yes, continuing to logoff/shutdown
		}
		else return FALSE; //user clicked no, cancelling logoff/shutdown
	}
	//actually go through with logoff/shutdown
	case WM_ENDSESSION:
	{
		return 0;
	}
	}

	//
	// ENSURE CURSOR ICON IS CORRECT WHEN INSIDE WINDOW
	//

	/*
	if (msg == WM_NCHITTEST)
	{
		auto result = CursorTest(hwnd, msg, wParam, lParam);

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

		Log::Print(
			"WM_NCHITTEST result: " + resultValue + " [" + to_string(result) + "]",
			"MESSAGELOOP",
			LogType::LOG_INFO);

		return result;
	}
	*/

	//
	// OTHER MESSAGES
	//

	MSG msgObj{};
	msgObj.hwnd = hwnd;
	msgObj.message = msg;
	msgObj.wParam = wParam;
	msgObj.lParam = lParam;

	//return 0 if we handled the message ourselves
	if (ProcessMessage(msgObj, window)) return 0;

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

static bool ProcessMessage(const MSG& msg, Window* window)
{
	if (window == nullptr)
	{
		Log::Print(
			"Cannot use 'ProcessMessage' because window is nullptr!",
			"MESSAGELOOP",
			LogType::LOG_ERROR,
			2);

		return false;
	}

	ImGuiIO* io = nullptr;
	if (DebugUI::IsInitialized()) io = &ImGui::GetIO();

	/*
	if (msg.message == 0)
	{
		Log::Print(
			"Received empty or WM_NULL message.",
			"MESSAGELOOP",
			LogType::LOG_INFO);
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

		Log::Print(
			"Got message: " + ss.str(),
			"MESSAGELOOP",
			LogType::LOG_INFO);
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

		if (Input::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Windows detected keyboard key down: " + to_string(static_cast<int>(key)),
				"MESSAGELOOP",
				LogType::LOG_INFO);
		}

		Input::SetKeyState(key, true);

		if (DebugUI::IsInitialized()
			&& msg.wParam < 512
			&& io)
		{
			ImGuiKey k = TranslateVirtualKeyToImGuiKey(msg.wParam);
			if (k != ImGuiKey_None)
			{
				io->AddKeyEvent(k, true);

				//also imgui support combos
				io->AddKeyEvent(ImGuiMod_Ctrl,  (GetKeyState(VK_CONTROL) & 0x8000) != 0);
				io->AddKeyEvent(ImGuiMod_Shift, (GetKeyState(VK_SHIFT) & 0x8000)   != 0);
				io->AddKeyEvent(ImGuiMod_Alt,   (GetKeyState(VK_MENU) & 0x8000)    != 0);
				io->AddKeyEvent(ImGuiMod_Super, ((GetKeyState(VK_LWIN) & 0x8000)   != 0) 
					                         || ((GetKeyState(VK_RWIN) & 0x8000)   != 0));
			}
		}

		return false;
	}
	case WM_SYSKEYUP:
	case WM_KEYUP:
	{
		Key key = TranslateVirtualKey(msg.wParam);

		if (Input::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Windows detected keyboard key up: " + to_string(static_cast<int>(key)),
				"MESSAGELOOP",
				LogType::LOG_INFO);
		}

		Input::SetKeyState(key, false);

		if (DebugUI::IsInitialized()
			&& msg.wParam < 512
			&& io)
		{
			ImGuiKey k = TranslateVirtualKeyToImGuiKey(msg.wParam);
			if (k != ImGuiKey_None)
			{
				io->AddKeyEvent(k, false);

				//also imgui support combos
				io->AddKeyEvent(ImGuiMod_Ctrl,  (GetKeyState(VK_CONTROL) & 0x8000) != 0);
				io->AddKeyEvent(ImGuiMod_Shift, (GetKeyState(VK_SHIFT) & 0x8000)   != 0);
				io->AddKeyEvent(ImGuiMod_Alt,   (GetKeyState(VK_MENU) & 0x8000)    != 0);
				io->AddKeyEvent(ImGuiMod_Super, ((GetKeyState(VK_LWIN) & 0x8000)   != 0) 
					                         || ((GetKeyState(VK_RWIN) & 0x8000)   != 0));
			}
		}

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

		//get the old position before updating
		vec2 oldPos = Input::GetMousePosition();

		vec2 delta = 
		{
			newPos.x - oldPos.x,
			newPos.y - oldPos.y
		};

		Input::SetMousePosition(newPos);
		Input::SetMouseDelta(delta);

		if (DebugUI::IsInitialized() && io) io->AddMousePosEvent(newPos.x, newPos.y);

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

		if (DebugUI::IsInitialized() && io) io->AddMouseWheelEvent(0.0f, scroll);

		return false;
	}

	//
	// MOUSE DOUBLE CLICK
	//

	case WM_LBUTTONDBLCLK:
	{
		Input::SetMouseButtonDoubleClickState(MouseButton::Left, true);

		if (Input::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Windows detected left mouse key double click.",
				"MESSAGELOOP",
				LogType::LOG_INFO);
		}

		return false;
	}
	case WM_RBUTTONDBLCLK:
	{
		Input::SetMouseButtonDoubleClickState(MouseButton::Right, true);

		if (Input::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Windows detected right mouse key double click.",
				"MESSAGELOOP",
				LogType::LOG_INFO);
		}

		return false;
	}
	case WM_MBUTTONDBLCLK:
	{
		Input::SetMouseButtonDoubleClickState(MouseButton::Right, true);

		if (Input::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Windows detected middle mouse key double click.",
				"MESSAGELOOP",
				LogType::LOG_INFO);
		}

		return false;
	}
	case WM_XBUTTONDBLCLK:
	{
		WORD button = GET_XBUTTON_WPARAM(msg.wParam);
		if (button == XBUTTON1)
		{
			Input::SetMouseButtonDoubleClickState(MouseButton::X1, true);

			if (Input::IsVerboseLoggingEnabled())
			{
				Log::Print(
					"Windows detected x1 mouse key double click.",
					"MESSAGELOOP",
					LogType::LOG_INFO);
			}
		}
		if (button == XBUTTON2)
		{
			Input::SetMouseButtonDoubleClickState(MouseButton::X2, true);

			if (Input::IsVerboseLoggingEnabled())
			{
				Log::Print(
					"Windows detected x2 mouse key double click.",
					"MESSAGELOOP",
					LogType::LOG_INFO);
			}
		}
		return false;
	}

	//
	// MOUSE BUTTONS
	//

	case WM_LBUTTONDOWN:
	{
		Input::SetMouseButtonState(MouseButton::Left, true);

		if (DebugUI::IsInitialized() && io) io->AddMouseButtonEvent(0, true);

		if (Input::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Windows detected left mouse key down.",
				"MESSAGELOOP",
				LogType::LOG_INFO);
		}

		return false;
	}
	case WM_LBUTTONUP:
	{
		Input::SetMouseButtonState(MouseButton::Left, false);

		if (DebugUI::IsInitialized() && io) io->AddMouseButtonEvent(0, false);

		if (Input::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Windows detected left mouse key up.",
				"MESSAGELOOP",
				LogType::LOG_INFO);
		}

		return false;
	}

	case WM_RBUTTONDOWN:
	{
		Input::SetMouseButtonState(MouseButton::Right, true);

		if (DebugUI::IsInitialized() && io) io->AddMouseButtonEvent(1, true);

		if (Input::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Windows detected right mouse key down.",
				"MESSAGELOOP",
				LogType::LOG_INFO);
		}

		return false;
	}
	case WM_RBUTTONUP:
	{
		Input::SetMouseButtonState(MouseButton::Right, false);

		if (DebugUI::IsInitialized() && io) io->AddMouseButtonEvent(1, false);

		if (Input::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Windows detected right mouse key up.",
				"MESSAGELOOP",
				LogType::LOG_INFO);
		}

		return false;
	}

	case WM_MBUTTONDOWN:
	{
		Input::SetMouseButtonState(MouseButton::Middle, true);

		if (DebugUI::IsInitialized() && io) io->AddMouseButtonEvent(2, true);

		if (Input::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Windows detected middle mouse key down.",
				"MESSAGELOOP",
				LogType::LOG_INFO);
		}

		return false;
	}
	case WM_MBUTTONUP:
	{
		Input::SetMouseButtonState(MouseButton::Middle, false);

		if (DebugUI::IsInitialized() && io) io->AddMouseButtonEvent(2, false);

		if (Input::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Windows detected middle mouse key up.",
				"MESSAGELOOP",
				LogType::LOG_INFO);
		}

		return false;
	}

	case WM_XBUTTONDOWN:
	{
		WORD button = GET_XBUTTON_WPARAM(msg.wParam);
		if (button == XBUTTON1)
		{
			Input::SetMouseButtonState(MouseButton::X1, true);

			if (DebugUI::IsInitialized() && io) io->AddMouseButtonEvent(3, true);

			if (Input::IsVerboseLoggingEnabled())
			{
				Log::Print(
					"Windows detected x1 mouse key down.",
					"MESSAGELOOP",
					LogType::LOG_INFO);
			}
		}
		if (button == XBUTTON2)
		{
			Input::SetMouseButtonState(MouseButton::X2, true);

			if (DebugUI::IsInitialized() && io) io->AddMouseButtonEvent(4, true);

			if (Input::IsVerboseLoggingEnabled())
			{
				Log::Print(
					"Windows detected x2 mouse key down.",
					"MESSAGELOOP",
					LogType::LOG_INFO);
			}
		}
		return false;
	}
	case WM_XBUTTONUP:
	{
		WORD button = GET_XBUTTON_WPARAM(msg.wParam);
		if (button == XBUTTON1)
		{
			Input::SetMouseButtonState(MouseButton::X1, false);

			if (DebugUI::IsInitialized() && io) io->AddMouseButtonEvent(3, false);

			if (Input::IsVerboseLoggingEnabled())
			{
				Log::Print(
					"Windows detected x1 mouse key up.",
					"MESSAGELOOP",
					LogType::LOG_INFO);
			}
		}
		if (button == XBUTTON2)
		{
			Input::SetMouseButtonState(MouseButton::X2, false);

			if (DebugUI::IsInitialized() && io) io->AddMouseButtonEvent(4, false);

			if (Input::IsVerboseLoggingEnabled())
			{
				Log::Print(
					"Windows detected x2 mouse key up.",
					"MESSAGELOOP",
					LogType::LOG_INFO);
			}
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
	// WINDOW FOCUS
	//

	//window gains focus
	case WM_SETFOCUS:
	{
		Input::SetMouseVisibilityBetweenFocus(false);
		Input::SetMouseLockStateBetweenFocus(false, window);

		if (Window::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Focusing on window '" + window->GetTitle() + "'",
				"MESSAGELOOP",
				LogType::LOG_INFO);
		}

		return false;
	}

	//window loses focus
	case WM_KILLFOCUS:
	{
		Input::SetMouseVisibilityBetweenFocus(true);
		Input::SetMouseLockStateBetweenFocus(true, window);
		Input::ClearInputEvents();

		if (DebugUI::IsInitialized()
			&& io)
		{
			//reset all keyboard/gamepad keys
			for (int key = ImGuiKey_NamedKey_BEGIN; key < ImGuiKey_NamedKey_END; key++)
			{
				ImGuiKey ikey = (ImGuiKey)key;

				//skip mouse aliases
				if (ikey >= ImGuiKey_MouseLeft
					&& ikey <= ImGuiKey_MouseWheelY)
				{
					continue;
				}

				//skip reserved mod storage
				if (ikey >= ImGuiKey_ReservedForModCtrl
					&& ikey <= ImGuiKey_ReservedForModSuper)
				{
					continue;
				}

				io->AddKeyEvent(ikey, false);
			}

			//reset mouse buttons
			for (int btn = 0; btn < 5; btn++)
			{
				io->AddMouseButtonEvent(btn, false);
			}

			//reset all digital keys
			static const ImGuiKey gamepadDigitalKeys[] = 
			{
				ImGuiKey_GamepadStart, ImGuiKey_GamepadBack,
				ImGuiKey_GamepadFaceLeft, ImGuiKey_GamepadFaceRight,
				ImGuiKey_GamepadFaceUp, ImGuiKey_GamepadFaceDown,
				ImGuiKey_GamepadDpadLeft, ImGuiKey_GamepadDpadRight,
				ImGuiKey_GamepadDpadUp, ImGuiKey_GamepadDpadDown,
				ImGuiKey_GamepadL1, ImGuiKey_GamepadR1,
				ImGuiKey_GamepadL3, ImGuiKey_GamepadR3
			};
			for (ImGuiKey key : gamepadDigitalKeys) io->AddKeyEvent(key, false);

			//reset all analog keys
			static const ImGuiKey gamepadAnalogKeys[] = 
			{
				ImGuiKey_GamepadL2, ImGuiKey_GamepadR2,
				ImGuiKey_GamepadLStickLeft, ImGuiKey_GamepadLStickRight,
				ImGuiKey_GamepadLStickUp, ImGuiKey_GamepadLStickDown,
				ImGuiKey_GamepadRStickLeft, ImGuiKey_GamepadRStickRight,
				ImGuiKey_GamepadRStickUp, ImGuiKey_GamepadRStickDown
			};
			for (ImGuiKey key : gamepadAnalogKeys) io->AddKeyAnalogEvent(key, false, 0.0f);

			//reset mouse wheel
			io->AddMouseWheelEvent(0.0f, 0.0f); 

			//mark mouse as "left the window"
			io->AddMousePosEvent(-FLT_MAX, -FLT_MAX);

			//reset advanced input
			io->AddMouseSourceEvent(ImGuiMouseSource_Mouse);
			io->AddMouseViewportEvent(0);
		}

		if (Window::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"No longer focusing on window '" + window->GetTitle() + "'",
				"MESSAGELOOP",
				LogType::LOG_INFO);
		}

		return false;
	}

	//
	// WINDOW REDRAW
	//

	case WM_PAINT:
	{
		const WindowData& win = window->GetWindowData();
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

		if (OpenGL_Renderer::IsInitialized())
		{
			vec2 framebufferSize = window->GetFramebufferSize();

			glViewport(
				0,
				0,
				framebufferSize.x,
				framebufferSize.y);
		}

		window->TriggerResize();

		return true;
	}
	case WM_SIZING:
	{
		HWND windowRef = ToVar<HWND>(window->GetWindowData().hwnd);

		window->TriggerRedraw();
		return true;
	}
	//scale correctly when going to other monitor
	case WM_DPICHANGED:
	{
		UINT dpiX = LOWORD(msg.wParam);
		UINT dpiY = HIWORD(msg.wParam);

		RECT* suggestedRect = reinterpret_cast<RECT*>(msg.lParam);

		//resize window to suggestedRect
		SetWindowPos(
			ToVar<HWND>(window->GetWindowData().hwnd),
			nullptr,
			suggestedRect->left,
			suggestedRect->top,
			suggestedRect->right - suggestedRect->left,
			suggestedRect->bottom - suggestedRect->top,
			SWP_NOZORDER
			| SWP_NOACTIVATE);

		if (OpenGL_Renderer::IsInitialized())
		{
			vec2 framebufferSize = window->GetFramebufferSize();

			glViewport(
				0,
				0,
				framebufferSize.x,
				framebufferSize.y);
		}

		window->TriggerResize();
		window->TriggerRedraw();

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
	// MENU BAR EVENTS
	//

	//leaf was clicked
	case WM_COMMAND:
	{
		u32 IDRef = LOWORD(msg.wParam);

		if (window)
		{
			for (const auto& e : runtimeMenuBarEvents)
			{
				u32 ID = e->labelID;
				if (ID == IDRef)
				{
					e->function();
					return true;
				}
			}

			Log::Print(
				"Did not find leaf event with ID '" + to_string(IDRef) + "'.",
				"MESSAGELOOP",
				LogType::LOG_INFO);
		}

		return false;
	}

	//
	// SHUTDOWN
	//

	//destroy current window if user clicked X button or pressed Alt + F4
	case WM_CLOSE:
	{
		Log::Print(
			"Window '" + window->GetTitle() + "' is destroyed.",
			"MESSAGELOOP",
			LogType::LOG_SUCCESS);

		OpenGL_Renderer::Shutdown(window);
		DestroyWindow(ToVar<HWND>(window->GetWindowData().hwnd));

		if (createdWindows.size() == 0)
		{
			KalaWindowCore::Shutdown(ShutdownState::SHUTDOWN_CLEAN);
		}

		return true;
	}

	//full shutdown if all windows were destroyed
	case WM_DESTROY:
	{
		if (createdWindows.size() == 0) PostQuitMessage(0);

		return true;
	}

	default:
		return false;
	}

	return false;
}

#endif //_WIN32