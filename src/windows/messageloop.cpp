//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef _WIN32

#include <Windows.h>
#include <shellapi.h> 
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

#include "KalaHeaders/log_utils.hpp"

#include "windows/messageloop.hpp"
#include "graphics/window.hpp"
#include "graphics/window_global.hpp"
#include "core/input.hpp"
#include "core/core.hpp"
#include "core/containers.hpp"
#include "graphics/opengl/opengl_functions_core.hpp"
#include "graphics/opengl/opengl.hpp"
#include "ui/debug_ui.hpp"

using KalaHeaders::Log;
using KalaHeaders::LogType;

using KalaWindow::Windows::MessageLoop;
using KalaWindow::Graphics::Window;
using KalaWindow::Graphics::Window_Global;
using KalaWindow::Graphics::PopupAction;
using KalaWindow::Graphics::PopupResult;
using KalaWindow::Graphics::PopupType;
using KalaWindow::Windows::MenuBar;
using KalaWindow::Windows::MenuBarEvent;
using KalaWindow::Graphics::WindowData;
using KalaWindow::Graphics::OpenGL::OpenGL_Global;
using namespace KalaWindow::Graphics::OpenGLFunctions;
using namespace KalaWindow::Core;
using KalaWindow::UI::DebugUI;

using std::string;
using std::to_string;
using std::vector;
using std::ostringstream;
using std::hex;
using std::dec;
using std::function;
using std::wstring;

using std::unordered_map;

static const unordered_map<Key, string> KeyToStringMap = {
    // Letters
    { Key::A, "A" }, { Key::B, "B" }, { Key::C, "C" }, { Key::D, "D" },
    { Key::E, "E" }, { Key::F, "F" }, { Key::G, "G" }, { Key::H, "H" },
    { Key::I, "I" }, { Key::J, "J" }, { Key::K, "K" }, { Key::L, "L" },
    { Key::M, "M" }, { Key::N, "N" }, { Key::O, "O" }, { Key::P, "P" },
    { Key::Q, "Q" }, { Key::R, "R" }, { Key::S, "S" }, { Key::T, "T" },
    { Key::U, "U" }, { Key::V, "V" }, { Key::W, "W" }, { Key::X, "X" },
    { Key::Y, "Y" }, { Key::Z, "Z" },

    // Numbers
    { Key::Num0, "0" }, { Key::Num1, "1" }, { Key::Num2, "2" },
    { Key::Num3, "3" }, { Key::Num4, "4" }, { Key::Num5, "5" },
    { Key::Num6, "6" }, { Key::Num7, "7" }, { Key::Num8, "8" },
    { Key::Num9, "9" },

    // Function Keys
    { Key::F1, "F1" }, { Key::F2, "F2" }, { Key::F3, "F3" }, { Key::F4, "F4" },
    { Key::F5, "F5" }, { Key::F6, "F6" }, { Key::F7, "F7" }, { Key::F8, "F8" },
    { Key::F9, "F9" }, { Key::F10, "F10" }, { Key::F11, "F11" }, { Key::F12, "F12" },
    { Key::F13, "F13" }, { Key::F14, "F14" }, { Key::F15, "F15" }, { Key::F16, "F16" },
    { Key::F17, "F17" }, { Key::F18, "F18" }, { Key::F19, "F19" }, { Key::F20, "F20" },
    { Key::F21, "F21" }, { Key::F22, "F22" }, { Key::F23, "F23" }, { Key::F24, "F24" },

    // Numpad
    { Key::Numpad0, "Numpad0" }, { Key::Numpad1, "Numpad1" }, { Key::Numpad2, "Numpad2" },
    { Key::Numpad3, "Numpad3" }, { Key::Numpad4, "Numpad4" }, { Key::Numpad5, "Numpad5" },
    { Key::Numpad6, "Numpad6" }, { Key::Numpad7, "Numpad7" }, { Key::Numpad8, "Numpad8" },
    { Key::Numpad9, "Numpad9" },
    { Key::NumpadAdd, "NumpadAdd" }, { Key::NumpadSubtract, "NumpadSubtract" },
    { Key::NumpadMultiply, "NumpadMultiply" }, { Key::NumpadDivide, "NumpadDivide" },
    { Key::NumpadDecimal, "NumpadDecimal" }, { Key::NumLock, "NumLock" },

    // Navigation
    { Key::ArrowLeft, "ArrowLeft" }, { Key::ArrowRight, "ArrowRight" },
    { Key::ArrowUp, "ArrowUp" }, { Key::ArrowDown, "ArrowDown" },
    { Key::Home, "Home" }, { Key::End, "End" },
    { Key::PageUp, "PageUp" }, { Key::PageDown, "PageDown" },
    { Key::Insert, "Insert" }, { Key::Delete, "Delete" },

    // Controls
    { Key::Enter, "Enter" }, { Key::Escape, "Escape" },
    { Key::Backspace, "Backspace" }, { Key::Tab, "Tab" },
    { Key::CapsLock, "CapsLock" }, { Key::Space, "Space" },

    // Modifiers
    { Key::ShiftLeft, "ShiftLeft" }, { Key::ShiftRight, "ShiftRight" },
    { Key::CtrlLeft, "CtrlLeft" }, { Key::CtrlRight, "CtrlRight" },
    { Key::AltLeft, "AltLeft" }, { Key::AltRight, "AltRight" },
    { Key::SuperLeft, "SuperLeft" }, { Key::SuperRight, "SuperRight" },

    // System / Special
    { Key::PrintScreen, "PrintScreen" }, { Key::ScrollLock, "ScrollLock" },
    { Key::Pause, "Pause" }, { Key::Menu, "Menu" },

    // Symbols / OEM
    { Key::Minus, "Minus" }, { Key::Equal, "Equal" },
    { Key::BracketLeft, "BracketLeft" }, { Key::BracketRight, "BracketRight" },
    { Key::Backslash, "Backslash" }, { Key::Semicolon, "Semicolon" },
    { Key::Apostrophe, "Apostrophe" }, { Key::Comma, "Comma" },
    { Key::Period, "Period" }, { Key::Slash, "Slash" },
    { Key::Tilde, "Tilde" }, { Key::Oem102, "Oem102" },

    // Media & Browser
    { Key::MediaPlayPause, "MediaPlayPause" },
    { Key::MediaStop, "MediaStop" },
    { Key::MediaNextTrack, "MediaNextTrack" },
    { Key::MediaPrevTrack, "MediaPrevTrack" },
    { Key::VolumeUp, "VolumeUp" }, { Key::VolumeDown, "VolumeDown" }, { Key::VolumeMute, "VolumeMute" },
    { Key::LaunchMail, "LaunchMail" }, { Key::LaunchApp1, "LaunchApp1" }, { Key::LaunchApp2, "LaunchApp2" },
    { Key::BrowserBack, "BrowserBack" }, { Key::BrowserForward, "BrowserForward" },
    { Key::BrowserRefresh, "BrowserRefresh" }, { Key::BrowserStop, "BrowserStop" },
    { Key::BrowserSearch, "BrowserSearch" }, { Key::BrowserFavorites, "BrowserFavorites" }, { Key::BrowserHome, "BrowserHome" }
};


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

static string TranslateVirtualKeyToString(WPARAM vk, LPARAM lParam)
{
	Key key = Key::Unknown;

	switch (vk)
	{
	case VK_CONTROL:
		key = (lParam & 0x01000000) ? Key::CtrlRight : Key::CtrlLeft;

		break;

	case VK_MENU: // Alt
		key = (lParam & 0x01000000) ? Key::AltRight : Key::AltLeft;

		break;

	case VK_SHIFT:
	{
		//extract scancode
		UINT scancode = (lParam >> 16) & 0xFF;

		//map to left/right shift
		UINT vk_lr = MapVirtualKey(scancode, MAPVK_VSC_TO_VK_EX);
		key = (vk_lr == VK_RSHIFT) ? Key::ShiftRight : Key::ShiftLeft;

		break;
	}

	default:
	{
		auto it = VKToKeyMap.find(vk);
		if (it != VKToKeyMap.end()) key = it->second;
	}
	break;
	}

	// convert to string if known
	auto strIt = KeyToStringMap.find(key);
	if (strIt != KeyToStringMap.end()) return strIt->second;

	return "Unknown";
}

static Key TranslateVirtualKey(WPARAM vk, LPARAM lParam)
{
	switch (vk)
	{
	case VK_CONTROL:
		return (lParam & 0x01000000) ? Key::CtrlRight : Key::CtrlLeft;
	case VK_MENU: //alt
		return (lParam & 0x01000000) ? Key::AltRight : Key::AltLeft;
	case VK_SHIFT:
	{
		//extract scancode
		UINT scancode = (lParam >> 16) & 0xFF;

		//map to left/right shift
		UINT vk_lr = MapVirtualKey(scancode, MAPVK_VSC_TO_VK_EX);
		return (vk_lr == VK_RSHIFT) ? Key::ShiftRight : Key::ShiftLeft;
	}
	}

	//default lookup

	auto it = VKToKeyMap.find(vk);
	if (it != VKToKeyMap.end()) return it->second;

	return Key::Unknown;
}

static ImGuiKey TranslateVirtualKeyToImGuiKey(WPARAM vk, LPARAM lParam)
{
	switch (vk)
	{
	case VK_CONTROL:
		return (lParam & 0x01000000) ? ImGuiKey_RightCtrl : ImGuiKey_LeftCtrl;
	case VK_MENU: //alt
		return (lParam & 0x01000000) ? ImGuiKey_RightAlt : ImGuiKey_LeftAlt;
	case VK_SHIFT:
	{
		//extract scancode
		UINT scancode = (lParam >> 16) & 0xFF;

		//map to left/right shift
		UINT vk_lr = MapVirtualKey(scancode, MAPVK_VSC_TO_VK_EX);
		return (vk_lr == VK_RSHIFT) ? ImGuiKey_RightShift : ImGuiKey_LeftShift;
	}
	}

	//default lookup

	auto it = VKToImGuiKeyMap.find(vk);
	if (it != VKToImGuiKeyMap.end()) return it->second;

	return ImGuiKey_None; //fallback if not recognized by ImGui
}

static bool ProcessMessage(const MSG& msg, Window* window);

static wstring ToWide(const string& str);
static string ToShort(const wstring& str);

namespace KalaWindow::Windows
{
	LRESULT CALLBACK MessageLoop::WindowProcCallback(
		HWND hwnd,
		UINT msg,
		WPARAM wParam,
		LPARAM lParam)
	{
		Window* window = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

		if (!window)
		{
			return DefWindowProc(
				hwnd, 
				msg, 
				wParam, 
				lParam);
		}

		switch (msg)
		{
		//asks if user wants to log off or shut down (in case any data is unsaved)
		case WM_QUERYENDSESSION:
		{
			if (Window_Global::CreatePopup(
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

		case WM_MOUSEACTIVATE:
		{
			if (Window_Global::IsVerboseLoggingEnabled())
			{
				Log::Print(
					"Clicked on window '" + window->GetTitle() + "' client area.",
					"MESSAGELOOP",
					LogType::LOG_INFO);
			}

			return MA_ACTIVATE;
		}

		case WM_NCLBUTTONDOWN:
		{
			if (Window_Global::IsVerboseLoggingEnabled())
			{
				Log::Print(
					"Clicked on window '" + window->GetTitle() + "' non-client area.",
					"MESSAGELOOP",
					LogType::LOG_INFO);
			}

			return DefWindowProc(
				hwnd,
				msg,
				wParam,
				lParam);
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

		return DefWindowProc(
			hwnd, 
			msg, 
			wParam, 
			lParam);
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

static bool ProcessMessage(const MSG& msg, Window* window)
{
	if (!window)
	{
		Log::Print(
			"Cannot use 'ProcessMessage' because its window was not found!",
			"MESSAGELOOP",
			LogType::LOG_ERROR,
			2);

		return false;
	}

	ImGuiIO* io = nullptr;

	Input* input = window->GetInput();

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

	//typing text
	case WM_UNICHAR:
	case WM_CHAR:
	{
		if (io) io->AddInputCharacter((u32)msg.wParam);

		return true;
	}

	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
	{
		if (msg.wParam == VK_LBUTTON
			|| msg.wParam == VK_RBUTTON
			|| msg.wParam == VK_MBUTTON
			|| msg.wParam == VK_XBUTTON1
			|| msg.wParam == VK_XBUTTON2)
		{
			return false;
		}

		Key key = TranslateVirtualKey(msg.wParam, msg.lParam);

		if (Input::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Detected keyboard key '" + TranslateVirtualKeyToString(msg.wParam, msg.lParam) + "' down.",
				"INPUT_WINDOWS",
				LogType::LOG_INFO);
		}

		if (input)
		{
			input->SetKeyState(
				key,
				true);
		}

		if (io && msg.wParam < 512)
		{
			ImGuiKey k = TranslateVirtualKeyToImGuiKey(msg.wParam, msg.lParam);
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
		if (msg.wParam == VK_LBUTTON
			|| msg.wParam == VK_RBUTTON
			|| msg.wParam == VK_MBUTTON
			|| msg.wParam == VK_XBUTTON1
			|| msg.wParam == VK_XBUTTON2)
		{
			return false;
		}

		Key key = TranslateVirtualKey(msg.wParam, msg.lParam);

		if (Input::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Detected keyboard key '" + TranslateVirtualKeyToString(msg.wParam, msg.lParam) + "' up.",
				"INPUT_WINDOWS",
				LogType::LOG_INFO);
		}

		if (input)
		{
			input->SetKeyState(
				key,
				false);
		}

		if (io && msg.wParam < 512)
		{
			ImGuiKey k = TranslateVirtualKeyToImGuiKey(msg.wParam, msg.lParam);
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

		if (input)
		{
			//get the old position before updating
			vec2 oldPos = input->GetMousePosition();

			vec2 delta =
			{
				newPos.x - oldPos.x,
				newPos.y - oldPos.y
			};

			input->SetMousePosition(newPos);
			input->SetMouseDelta(delta);
		}

		if (io) io->AddMousePosEvent(newPos.x, newPos.y);

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

		if (input) input->SetMouseWheelDelta(scroll);

		if (io) io->AddMouseWheelEvent(0.0f, scroll);

		return false;
	}

	//
	// MOUSE DOUBLE CLICK
	//

	case WM_LBUTTONDBLCLK:
	{
		if (input)
		{
			input->SetMouseButtonDoubleClickState(
				MouseButton::Left,
				true);

			if (Input::IsVerboseLoggingEnabled())
			{
				Log::Print(
					"Detected left mouse key double click.",
					"INPUT_WINDOWS",
					LogType::LOG_INFO);
			}
		}

		return false;
	}
	case WM_RBUTTONDBLCLK:
	{
		if (input)
		{
			input->SetMouseButtonDoubleClickState(
				MouseButton::Right,
				true);

			if (Input::IsVerboseLoggingEnabled())
			{
				Log::Print(
					"Detected right mouse key double click.",
					"INPUT_WINDOWS",
					LogType::LOG_INFO);
			}
		}

		return false;
	}
	case WM_MBUTTONDBLCLK:
	{
		if (input)
		{
			input->SetMouseButtonDoubleClickState(
				MouseButton::Right,
				true);

			if (Input::IsVerboseLoggingEnabled())
			{
				Log::Print(
					"Detected middle mouse key double click.",
					"INPUT_WINDOWS",
					LogType::LOG_INFO);
			}
		}

		return false;
	}
	case WM_XBUTTONDBLCLK:
	{
		WORD button = GET_XBUTTON_WPARAM(msg.wParam);

		if (input)
		{
			if (button == XBUTTON1)
			{
				input->SetMouseButtonDoubleClickState(
					MouseButton::X1,
					true);

				if (Input::IsVerboseLoggingEnabled())
				{
					Log::Print(
						"Detected x1 mouse key double click.",
						"INPUT_WINDOWS",
						LogType::LOG_INFO);
				}
			}
			if (button == XBUTTON2)
			{
				input->SetMouseButtonDoubleClickState(
					MouseButton::X2,
					true);

				if (Input::IsVerboseLoggingEnabled())
				{
					Log::Print(
						"Detected x2 mouse key double click.",
						"INPUT_WINDOWS",
						LogType::LOG_INFO);
				}
			}
		}
		return false;
	}

	//
	// MOUSE BUTTONS
	//

	case WM_LBUTTONDOWN:
	{
		if (input)
		{
			input->SetMouseButtonState(
				MouseButton::Left,
				true);

			if (Input::IsVerboseLoggingEnabled())
			{
				Log::Print(
					"Detected left mouse key down.",
					"INPUT_WINDOWS",
					LogType::LOG_INFO);
			}
		}

		if (io) io->AddMouseButtonEvent(0, true);

		return false;
	}
	case WM_LBUTTONUP:
	{
		if (input)
		{
			input->SetMouseButtonState(
				MouseButton::Left,
				false);

			if (Input::IsVerboseLoggingEnabled())
			{
				Log::Print(
					"Detected left mouse key up.",
					"INPUT_WINDOWS",
					LogType::LOG_INFO);
			}
		}

		if (io) io->AddMouseButtonEvent(0, false);

		return false;
	}

	case WM_RBUTTONDOWN:
	{
		if (input)
		{
			input->SetMouseButtonState(
				MouseButton::Right,
				true);

			if (Input::IsVerboseLoggingEnabled())
			{
				Log::Print(
					"Detected right mouse key down.",
					"INPUT_WINDOWS",
					LogType::LOG_INFO);
			}
		}

		if (io) io->AddMouseButtonEvent(1, true);

		return false;
	}
	case WM_RBUTTONUP:
	{
		if (input)
		{
			input->SetMouseButtonState(
				MouseButton::Right,
				false);

			if (Input::IsVerboseLoggingEnabled())
			{
				Log::Print(
					"Detected right mouse key up.",
					"INPUT_WINDOWS",
					LogType::LOG_INFO);
			}
		}

		if (io) io->AddMouseButtonEvent(1, false);

		return false;
	}

	case WM_MBUTTONDOWN:
	{
		if (input)
		{
			input->SetMouseButtonState(
				MouseButton::Middle,
				true);

			if (Input::IsVerboseLoggingEnabled())
			{
				Log::Print(
					"Detected middle mouse key down.",
					"INPUT_WINDOWS",
					LogType::LOG_INFO);
			}
		}

		if (io) io->AddMouseButtonEvent(2, true);

		return false;
	}
	case WM_MBUTTONUP:
	{
		if (input)
		{
			input->SetMouseButtonState(
				MouseButton::Middle,
				false);

			if (Input::IsVerboseLoggingEnabled())
			{
				Log::Print(
					"Detected middle mouse key up.",
					"INPUT_WINDOWS",
					LogType::LOG_INFO);
			}
		}

		if (io) io->AddMouseButtonEvent(2, false);

		return false;
	}

	case WM_XBUTTONDOWN:
	{
		WORD button = GET_XBUTTON_WPARAM(msg.wParam);
		if (button == XBUTTON1)
		{
			if (input)
			{
				input->SetMouseButtonState(
					MouseButton::X1,
					true);

				if (Input::IsVerboseLoggingEnabled())
				{
					Log::Print(
						"Detected x1 mouse key down.",
						"INPUT_WINDOWS",
						LogType::LOG_INFO);
				}
			}

			if (io) io->AddMouseButtonEvent(3, true);
		}
		if (button == XBUTTON2)
		{
			if (input)
			{
				input->SetMouseButtonState(
					MouseButton::X2,
					true);

				if (Input::IsVerboseLoggingEnabled())
				{
					Log::Print(
						"Detected x2 mouse key down.",
						"INPUT_WINDOWS",
						LogType::LOG_INFO);
				}
			}

			if (io) io->AddMouseButtonEvent(4, true);
		}
		return false;
	}
	case WM_XBUTTONUP:
	{
		WORD button = GET_XBUTTON_WPARAM(msg.wParam);
		if (button == XBUTTON1)
		{
			if (input)
			{
				input->SetMouseButtonState(
					MouseButton::X1,
					false);

				if (Input::IsVerboseLoggingEnabled())
				{
					Log::Print(
						"Detected x1 mouse key up.",
						"INPUT_WINDOWS",
						LogType::LOG_INFO);
				}
			}

			if (io) io->AddMouseButtonEvent(3, false);
		}
		if (button == XBUTTON2)
		{
			if (input)
			{
				input->SetMouseButtonState(
					MouseButton::X2,
					false);

				if (Input::IsVerboseLoggingEnabled())
				{
					Log::Print(
						"Detected x2 mouse key up.",
						"INPUT_WINDOWS",
						LogType::LOG_INFO);
				}
			}

			if (io) io->AddMouseButtonEvent(4, false);
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

		const RAWMOUSE& mouse = reinterpret_cast<RAWINPUT*>(buffer.data())->data.mouse;

		//sets raw mouse movement
		if (mouse.usFlags == MOUSE_MOVE_RELATIVE
			&& input)
		{
			vec2 newMouseRawDelta = input->GetRawMouseDelta();

			newMouseRawDelta.x += mouse.lLastX;
			newMouseRawDelta.y += mouse.lLastY;

			input->SetRawMouseDelta(newMouseRawDelta);
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
	// FILE WAS DRAGGED ONTO WINDOW
	//

	case WM_DROPFILES:
	{
		HDROP hDrop = (HDROP)msg.wParam;

		//count how many files were dropped
		UINT fileCount = DragQueryFileW(
			hDrop,
			0xFFFFFFFF,
			nullptr,
			0);

		vector<string> droppedFiles{};
		droppedFiles.reserve(fileCount);

		for (UINT i = 0; i < fileCount; i++)
		{
			//get length of this file path
			UINT length = DragQueryFileW(
				hDrop, 
				i, 
				nullptr, 
				0);

			if (length == 0) continue;

			wstring wstr(length + 1, L'\0');
			DragQueryFileW(
				hDrop,
				i,
				&wstr[0],
				length + 1);

			wstr.resize(length);

			string path = ToShort(wstr);
			droppedFiles.push_back(path);
		}

		DragFinish(hDrop);

		if (Window_Global::IsVerboseLoggingEnabled())
		{
			for (const auto& file : droppedFiles)
			{
				Log::Print(
					"File '" + file + "' was dragged to window '" + window->GetTitle() + "'",
					"WINDOW_WINDOWS",
					LogType::LOG_INFO);
			}
		}

		window->SetLastDraggedFiles(move(droppedFiles));

		return true;
	}

	//
	// WINDOW FOCUS
	//

	case WM_ACTIVATE:
	{
		switch (LOWORD(msg.wParam))
		{
		case WA_INACTIVE:
		{
			if (Window_Global::IsVerboseLoggingEnabled())
			{
				Log::Print(
					"Window '" + window->GetTitle() + "' was deactivated.",
					"MESSAGELOOP",
					LogType::LOG_INFO);
			}

			break;
		}
		case WA_ACTIVE:      //outside focus
		case WA_CLICKACTIVE: //direct click focus
		{
			if (Window_Global::IsVerboseLoggingEnabled())
			{
				Log::Print(
					"Window '" + window->GetTitle() + "' was activated.",
					"MESSAGELOOP",
					LogType::LOG_INFO);
			}

			break;
		}
		}

		return true; //let DefWindowProc run its logic
	}

	//window gains focus
	case WM_SETFOCUS:
	{
		if (input)
		{
			input->SetMouseVisibilityBetweenFocus(false);
			input->SetMouseLockStateBetweenFocus(false);

			if (Window_Global::IsVerboseLoggingEnabled())
			{
				Log::Print(
					"Returned focus to window '" + window->GetTitle() + "'!",
					"MESSAGELOOP",
					LogType::LOG_INFO);
			}
		}

		if (!window->IsFocused()) window->BringToFocus();

		return false;
	}

	//window loses focus
	case WM_KILLFOCUS:
	{
		if (input)
		{
			input->SetMouseVisibilityBetweenFocus(true);
			input->SetMouseLockStateBetweenFocus(true);
			input->ClearInputEvents();

			if (Window_Global::IsVerboseLoggingEnabled())
			{
				Log::Print(
					"No longer focusing on window '" + window->GetTitle() + "'.",
					"MESSAGELOOP",
					LogType::LOG_INFO);
			}
		}

		if (io)
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
		if (window->IsResizable())
		{
			vec2 clientRectSize = window->GetFramebufferSize();

			if (OpenGL_Global::IsInitialized())
			{
				glViewport(
					0,
					0,
					(GLsizei)clientRectSize.x,
					(GLsizei)clientRectSize.y);
			}

			window->TriggerResize();

			window->SetResizingState(false);
		}

		return true;
	}
	case WM_SIZING:
	{
		if (window->IsResizable())
		{
			if (!window->IsResizing()) window->SetResizingState(true);

			window->TriggerRedraw();
		}

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

		if (OpenGL_Global::IsInitialized())
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
		createdOpenGLContext.erase(window->GetOpenGLContext()->GetID());
		createdInput.erase(window->GetInput()->GetID());
		createdUI.erase(window->GetDebugUI()->GetID());

		createdWindows.erase(window->GetID());

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

wstring ToWide(const string& str)
{
	if (str.empty()) return wstring();

	int size_needed = MultiByteToWideChar(
		CP_UTF8,
		0,
		str.c_str(),
		-1,
		nullptr,
		0);

	wstring wstr(size_needed - 1, 0);

	MultiByteToWideChar(
		CP_UTF8,
		0,
		str.c_str(),
		-1,
		wstr.data(),
		size_needed);

	return wstr;
}
string ToShort(const wstring& str)
{
	if (str.empty()) return{};

	int size_needed = WideCharToMultiByte(
		CP_UTF8,
		0,
		str.c_str(),
		-1,
		nullptr,
		0,
		nullptr,
		nullptr);

	string result(size_needed - 1, 0);

	WideCharToMultiByte(
		CP_UTF8,
		0,
		str.c_str(),
		-1,
		result.data(),
		size_needed,
		nullptr,
		nullptr);

	return result;
}

#endif //_WIN32