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

#include "OpenGL/wglext.h"

#include "KalaHeaders/log_utils.hpp"
#include "KalaHeaders/math_utils.hpp"
#include "KalaHeaders/key_standards.hpp"

#include "core/kw_messageloop_windows.hpp"
#include "core/kw_input.hpp"
#include "core/kw_core.hpp"
#include "core/kw_registry.hpp"
#include "graphics/kw_menubar_windows.hpp"
#include "graphics/kw_window.hpp"
#include "graphics/kw_window_global.hpp"
#include "opengl/kw_opengl.hpp"
#include "opengl/kw_opengl_functions_core.hpp"

using KalaHeaders::KalaCore::ToVar;
using KalaHeaders::KalaMath::vec2;
using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;
using KalaHeaders::KalaKeyStandards::KeyboardButton;
using KalaHeaders::KalaKeyStandards::MouseButton;
using KalaHeaders::KalaKeyStandards::GetValueByKey;

using KalaWindow::Core::MessageLoop;
using KalaWindow::Core::KalaWindowCore;
using KalaWindow::Core::ShutdownState;
using KalaWindow::Core::KalaWindowRegistry;
using KalaWindow::Core::Input;
using KalaWindow::Graphics::Window;
using KalaWindow::Graphics::Window_Global;
using KalaWindow::Graphics::PopupAction;
using KalaWindow::Graphics::PopupResult;
using KalaWindow::Graphics::PopupType;
using KalaWindow::Graphics::MenuBar;
using KalaWindow::Graphics::MenuBarEvent;
using KalaWindow::Graphics::WindowData;
using KalaWindow::OpenGL::OpenGL_Global;
using KalaWindow::OpenGL::OpenGLFunctions::GL_Core;
using KalaWindow::OpenGL::OpenGLFunctions::OpenGL_Functions_Core;

using std::string;
using std::to_string;
using std::vector;
using std::ostringstream;
using std::hex;
using std::dec;
using std::function;
using std::wstring;

using std::unordered_map;

static const unordered_map<WPARAM, KeyboardButton> VKToKeyMap = {
	// Letters
	{ 'A', KeyboardButton::K_A }, { 'B', KeyboardButton::K_B }, { 'C', KeyboardButton::K_C }, { 'D', KeyboardButton::K_D },
	{ 'E', KeyboardButton::K_E }, { 'F', KeyboardButton::K_F }, { 'G', KeyboardButton::K_G }, { 'H', KeyboardButton::K_H },
	{ 'I', KeyboardButton::K_I }, { 'J', KeyboardButton::K_J }, { 'K', KeyboardButton::K_K }, { 'L', KeyboardButton::K_L },
	{ 'M', KeyboardButton::K_M }, { 'N', KeyboardButton::K_N }, { 'O', KeyboardButton::K_O }, { 'P', KeyboardButton::K_P },
	{ 'Q', KeyboardButton::K_Q }, { 'R', KeyboardButton::K_R }, { 'S', KeyboardButton::K_S }, { 'T', KeyboardButton::K_T },
	{ 'U', KeyboardButton::K_U }, { 'V', KeyboardButton::K_V }, { 'W', KeyboardButton::K_W }, { 'X', KeyboardButton::K_X },
	{ 'Y', KeyboardButton::K_Y }, { 'Z', KeyboardButton::K_Z },

	// Numbers
	{ '0', KeyboardButton::K_0 }, { '1', KeyboardButton::K_1 }, { '2', KeyboardButton::K_2 }, { '3', KeyboardButton::K_3 },
	{ '4', KeyboardButton::K_4 }, { '5', KeyboardButton::K_5 }, { '6', KeyboardButton::K_6 }, { '7', KeyboardButton::K_7 },
	{ '8', KeyboardButton::K_8 }, { '9', KeyboardButton::K_9 },

	// Function Keys
	{ VK_F1, KeyboardButton::K_F1 }, { VK_F2, KeyboardButton::K_F2 }, { VK_F3, KeyboardButton::K_F3 }, { VK_F4, KeyboardButton::K_F4 },
	{ VK_F5, KeyboardButton::K_F5 }, { VK_F6, KeyboardButton::K_F6 }, { VK_F7, KeyboardButton::K_F7 }, { VK_F8, KeyboardButton::K_F8 },
	{ VK_F9, KeyboardButton::K_F9 }, { VK_F10, KeyboardButton::K_F10 }, { VK_F11, KeyboardButton::K_F11 }, { VK_F12, KeyboardButton::K_F12 },

	// Numpad
	{ VK_NUMPAD0, KeyboardButton::K_NUM_0 }, { VK_NUMPAD1, KeyboardButton::K_NUM_1 }, { VK_NUMPAD2, KeyboardButton::K_NUM_2 },
	{ VK_NUMPAD3, KeyboardButton::K_NUM_3 }, { VK_NUMPAD4, KeyboardButton::K_NUM_4 }, { VK_NUMPAD5, KeyboardButton::K_NUM_5 },
	{ VK_NUMPAD6, KeyboardButton::K_NUM_6 }, { VK_NUMPAD7, KeyboardButton::K_NUM_7 }, { VK_NUMPAD8, KeyboardButton::K_NUM_8 },
	{ VK_NUMPAD9, KeyboardButton::K_NUM_9 },
	{ VK_ADD, KeyboardButton::K_NUM_ADD }, { VK_SUBTRACT, KeyboardButton::K_NUM_SUBTRACT },
	{ VK_MULTIPLY, KeyboardButton::K_NUM_MULTIPLY }, { VK_DIVIDE, KeyboardButton::K_NUM_DIVIDE },
	{ VK_RETURN, KeyboardButton::K_NUM_RETURN }, { VK_NUMLOCK, KeyboardButton::K_NUM_LOCK },
	{ VK_DECIMAL, KeyboardButton::K_NUM_DECIMAL },

	// Navigation
	{ VK_LEFT, KeyboardButton::K_ARROW_LEFT }, { VK_RIGHT, KeyboardButton::K_ARROW_RIGHT },
	{ VK_UP, KeyboardButton::K_ARROW_UP }, { VK_DOWN, KeyboardButton::K_ARROW_DOWN },
	{ VK_HOME, KeyboardButton::K_HOME }, { VK_END, KeyboardButton::K_END },
	{ VK_PRIOR, KeyboardButton::K_PAGE_UP }, { VK_NEXT, KeyboardButton::K_PAGE_DOWN },
	{ VK_INSERT, KeyboardButton::K_INSERT }, { VK_DELETE, KeyboardButton::K_DELETE },

	// Controls
	{ VK_RETURN, KeyboardButton::K_RETURN }, { VK_ESCAPE, KeyboardButton::K_ESC },
	{ VK_BACK, KeyboardButton::K_BACKSPACE }, { VK_TAB, KeyboardButton::K_TAB },
	{ VK_CAPITAL, KeyboardButton::K_CAPS_LOCK }, { VK_SPACE, KeyboardButton::K_SPACE },

	// Modifiers
	{ VK_LSHIFT, KeyboardButton::K_LEFT_SHIFT }, { VK_RSHIFT, KeyboardButton::K_RIGHT_SHIFT },
	{ VK_LCONTROL, KeyboardButton::K_LEFT_CTRL }, { VK_RCONTROL, KeyboardButton::K_RIGHT_CTRL },
	{ VK_LMENU, KeyboardButton::K_LEFT_ALT }, { VK_RMENU, KeyboardButton::K_RIGHT_ALT },
	{ VK_LWIN, KeyboardButton::K_SUPERLEFT }, { VK_RWIN, KeyboardButton::K_SUPERRIGHT },

	// System / Special
	{ VK_SNAPSHOT, KeyboardButton::K_PRINT_SCREEN }, { VK_SCROLL, KeyboardButton::K_SCROLL_LOCK },
	{ VK_PAUSE, KeyboardButton::K_PAUSE }, { VK_APPS, KeyboardButton::K_MENU }
};

static string TranslateVirtualKeyToString(WPARAM vk, LPARAM lParam)
{
	KeyboardButton key = KeyboardButton::K_INVALID;

	switch (vk)
	{
	case VK_CONTROL:
		key = (lParam & 0x01000000) ? KeyboardButton::K_RIGHT_CTRL : KeyboardButton::K_LEFT_CTRL;

		break;

	case VK_MENU: // Alt
		key = (lParam & 0x01000000) ? KeyboardButton::K_RIGHT_ALT : KeyboardButton::K_LEFT_ALT;

		break;

	case VK_SHIFT:
	{
		//extract scancode
		UINT scancode = (lParam >> 16) & 0xFF;

		//map to left/right shift
		UINT vk_lr = MapVirtualKey(scancode, MAPVK_VSC_TO_VK_EX);
		key = (vk_lr == VK_RSHIFT) ? KeyboardButton::K_RIGHT_SHIFT : KeyboardButton::K_LEFT_SHIFT;

		break;
	}

	default:
	{
		auto it = VKToKeyMap.find(vk);
		if (it != VKToKeyMap.end()) key = it->second;
	}
	break;
	}

	string result = GetValueByKey(scast<u32>(key)).data();

	return result == "?" 
		? "Unknown" 
		: result;
}

static KeyboardButton TranslateVirtualKey(WPARAM vk, LPARAM lParam)
{
	switch (vk)
	{
	case VK_CONTROL:
		return (lParam & 0x01000000) ? KeyboardButton::K_RIGHT_CTRL : KeyboardButton::K_LEFT_CTRL;
	case VK_MENU: //alt
		return (lParam & 0x01000000) ? KeyboardButton::K_RIGHT_ALT : KeyboardButton::K_LEFT_ALT;
	case VK_SHIFT:
	{
		//extract scancode
		UINT scancode = (lParam >> 16) & 0xFF;

		//map to left/right shift
		UINT vk_lr = MapVirtualKey(scancode, MAPVK_VSC_TO_VK_EX);
		return (vk_lr == VK_RSHIFT) ? KeyboardButton::K_RIGHT_SHIFT : KeyboardButton::K_LEFT_SHIFT;
	}
	}

	//default lookup

	auto it = VKToKeyMap.find(vk);
	if (it != VKToKeyMap.end()) return it->second;

	return KeyboardButton::K_INVALID;
}

static bool ProcessMessage(
	const MSG& msg,
	Window* window);

static wstring ToWide(const string& str);
static string ToShort(const wstring& str);

static function<void(u32)> addCharCallback{};
static function<void()> removeFromBackCallback{};
static function<void()> addTabCallback{};
static function<void()> addNewlineCallback{};

namespace KalaWindow::Core
{
	LRESULT CALLBACK MessageLoop::WindowProcCallback(
		HWND hwnd,
		UINT msg,
		WPARAM wParam,
		LPARAM lParam)
	{
		Window* window = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

		if (!window
			|| !window->IsInitialized())
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

		//return false if we handled the message ourselves
		if (ProcessMessage(msgObj, window)) return false;

		return DefWindowProc(
			hwnd, 
			msg, 
			wParam, 
			lParam);
	}

	void MessageLoop::SetAddCharCallback(const function<void(u32)>& newCallback)
	{
		addCharCallback = newCallback;
	}
	void MessageLoop::SetRemoveFromBackCallback(const function<void()>& newCallback)
	{
		removeFromBackCallback = newCallback;
	}
	void MessageLoop::SetAddTabCallback(const function<void()>& newCallback)
	{
		addTabCallback = newCallback;
	}
	void MessageLoop::SetAddNewLineCallback(const function<void()>& newCallback)
	{
		addNewlineCallback = newCallback;
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

static bool ProcessMessage(
	const MSG& msg,
	Window* window)
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

	u32 windowID = window->GetID();
	vector<Input*> inputs = KalaWindowRegistry<Input>::GetAllWindowContent(windowID);
	Input* input = inputs.empty() ? nullptr : inputs.front();

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
		if (addCharCallback) addCharCallback(scast<u32>(msg.wParam));

		return true; //we handled it
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

		KeyboardButton key = TranslateVirtualKey(msg.wParam, msg.lParam);

		if (Input::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Detected keyboard key '" + TranslateVirtualKeyToString(msg.wParam, msg.lParam) + "' down.",
				"INPUT",
				LogType::LOG_INFO);
		}

		if (input)
		{
			input->SetKeyState(
				key,
				true);
				
			switch (msg.wParam)
			{
			case VK_BACK:
				if (removeFromBackCallback) removeFromBackCallback();
				break;
			case VK_TAB:
				if (addTabCallback) addTabCallback();
				break;
			case VK_RETURN:
				if (addNewlineCallback) addNewlineCallback();
				break;
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

		KeyboardButton key = TranslateVirtualKey(msg.wParam, msg.lParam);

		if (Input::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Detected keyboard key '" + TranslateVirtualKeyToString(msg.wParam, msg.lParam) + "' up.",
				"INPUT",
				LogType::LOG_INFO);
		}

		if (input)
		{
			input->SetKeyState(
				key,
				false);
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

		if (input) input->SetScrollwheelDelta(scroll);

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
				MouseButton::M_LEFT,
				true);

			if (Input::IsVerboseLoggingEnabled())
			{
				Log::Print(
					"Detected left mouse key down.",
					"INPUT",
					LogType::LOG_INFO);
			}
		}

		return false;
	}
	case WM_LBUTTONUP:
	{
		if (input)
		{
			input->SetMouseButtonState(
				MouseButton::M_LEFT,
				false);

			if (Input::IsVerboseLoggingEnabled())
			{
				Log::Print(
					"Detected left mouse key up.",
					"INPUT",
					LogType::LOG_INFO);
			}
		}

		return false;
	}

	case WM_RBUTTONDOWN:
	{
		if (input)
		{
			input->SetMouseButtonState(
				MouseButton::M_RIGHT,
				true);

			if (Input::IsVerboseLoggingEnabled())
			{
				Log::Print(
					"Detected right mouse key down.",
					"INPUT",
					LogType::LOG_INFO);
			}
		}

		return false;
	}
	case WM_RBUTTONUP:
	{
		if (input)
		{
			input->SetMouseButtonState(
				MouseButton::M_RIGHT,
				false);

			if (Input::IsVerboseLoggingEnabled())
			{
				Log::Print(
					"Detected right mouse key up.",
					"INPUT",
					LogType::LOG_INFO);
			}
		}

		return false;
	}

	case WM_MBUTTONDOWN:
	{
		if (input)
		{
			input->SetMouseButtonState(
				MouseButton::M_MIDDLE,
				true);

			if (Input::IsVerboseLoggingEnabled())
			{
				Log::Print(
					"Detected middle mouse key down.",
					"INPUT",
					LogType::LOG_INFO);
			}
		}

		return false;
	}
	case WM_MBUTTONUP:
	{
		if (input)
		{
			input->SetMouseButtonState(
				MouseButton::M_MIDDLE,
				false);

			if (Input::IsVerboseLoggingEnabled())
			{
				Log::Print(
					"Detected middle mouse key up.",
					"INPUT",
					LogType::LOG_INFO);
			}
		}

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
					MouseButton::M_X1,
					true);

				if (Input::IsVerboseLoggingEnabled())
				{
					Log::Print(
						"Detected x1 mouse key down.",
						"INPUT",
						LogType::LOG_INFO);
				}
			}
		}
		if (button == XBUTTON2)
		{
			if (input)
			{
				input->SetMouseButtonState(
					MouseButton::M_X2,
					true);

				if (Input::IsVerboseLoggingEnabled())
				{
					Log::Print(
						"Detected x2 mouse key down.",
						"INPUT",
						LogType::LOG_INFO);
				}
			}
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
					MouseButton::M_X1,
					false);

				if (Input::IsVerboseLoggingEnabled())
				{
					Log::Print(
						"Detected x1 mouse key up.",
						"INPUT",
						LogType::LOG_INFO);
				}
			}
		}
		if (button == XBUTTON2)
		{
			if (input)
			{
				input->SetMouseButtonState(
					MouseButton::M_X2,
					false);

				if (Input::IsVerboseLoggingEnabled())
				{
					Log::Print(
						"Detected x2 mouse key up.",
						"INPUT",
						LogType::LOG_INFO);
				}
			}
		}
		return false;
	}

	//
	// MOUSE DOUBLE CLICK
	//

	//TODO: figure out if double click timing delay slows down single clicks

	case WM_LBUTTONDBLCLK:
	{
		if (input)
		{
			input->SetMouseButtonDoubleClickState(
				MouseButton::M_LEFT,
				true);

			if (Input::IsVerboseLoggingEnabled())
			{
				Log::Print(
					"Detected left mouse key double click.",
					"INPUT",
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
				MouseButton::M_RIGHT,
				true);

			if (Input::IsVerboseLoggingEnabled())
			{
				Log::Print(
					"Detected right mouse key double click.",
					"INPUT",
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
				MouseButton::M_MIDDLE,
				true);

			if (Input::IsVerboseLoggingEnabled())
			{
				Log::Print(
					"Detected middle mouse key double click.",
					"INPUT",
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
					MouseButton::M_X1,
					true);

				if (Input::IsVerboseLoggingEnabled())
				{
					Log::Print(
						"Detected x1 mouse key double click.",
						"INPUT",
						LogType::LOG_INFO);
				}
			}
			if (button == XBUTTON2)
			{
				input->SetMouseButtonDoubleClickState(
					MouseButton::M_X2,
					true);

				if (Input::IsVerboseLoggingEnabled())
				{
					Log::Print(
						"Detected x2 mouse key double click.",
						"INPUT",
						LogType::LOG_INFO);
				}
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

		window->SetLastDraggedFiles(std::move(droppedFiles));

		return true; //we handled it
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

		return false;
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

		return true; //we handled it
	}

	//
	// WINDOW RESIZE
	//

	case WM_SIZE:
	{
		if (window->IsResizable())
		{
			vec2 fbSize = window->GetClientRectSize();

			if (OpenGL_Global::IsInitialized())
			{
				const GL_Core* coreFunc = OpenGL_Functions_Core::GetGLCore();

				coreFunc->glViewport(
					0,
					0,
					(GLsizei)fbSize.x,
					(GLsizei)fbSize.y);
			}

			window->TriggerResize();

			window->SetResizingState(false);
		}

		return true; //we handled it
	}
	case WM_SIZING:
	{
		if (window->IsResizable())
		{
			if (!window->IsResizing()) window->SetResizingState(true);

			window->TriggerRedraw();
		}

		return true; //we handled it
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
			const GL_Core* coreFunc = OpenGL_Functions_Core::GetGLCore();

			vec2 vpSize = window->GetClientRectSize();

			coreFunc->glViewport(
				0,
				0,
				vpSize.x,
				vpSize.y);
		}

		window->TriggerResize();
		window->TriggerRedraw();

		return true; //we handled it
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

		return true; //we handled it
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
			vector<MenuBar*> menuBars = KalaWindowRegistry<MenuBar>::GetAllWindowContent(windowID);
			MenuBar* menuBar = menuBars.empty() ? nullptr : menuBars.front();

			if (menuBar)
			{
				const vector<MenuBarEvent> events = menuBar->GetEvents();
				for (const auto& e : events)
				{
					u32 ID = e.labelID;
					if (ID == IDRef)
					{
						e.function();
						return true; //we handled it
					}
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
		window->CloseWindow();

		if (Window::GetRegistry().runtimeContent.empty())
		{
			KalaWindowCore::Shutdown(ShutdownState::SHUTDOWN_CLEAN);
		}

		return true; //we handled it
	}

	//full shutdown if all windows were destroyed
	case WM_DESTROY:
	{
		if (Window::GetRegistry().runtimeContent.empty()) PostQuitMessage(0);

		return true; //we handled it
	}

	default:
		return false;
	}

	return false;
}

wstring ToWide(const string& input)
{
	if (input.empty()) return wstring();

	int size_needed = MultiByteToWideChar(
		CP_UTF8,
		0,
		input.data(),
		scast<int>(input.size()),
		nullptr,
		0);

	wstring wstr(size_needed, 0);

	MultiByteToWideChar(
		CP_UTF8,
		0,
		input.data(),
		scast<int>(input.size()),
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
		str.data(),
		scast<int>(str.size()),
		nullptr,
		0,
		nullptr,
		nullptr);

	string result(size_needed, 0);

	WideCharToMultiByte(
		CP_UTF8,
		0,
		str.data(),
		scast<int>(str.size()),
		result.data(),
		size_needed,
		nullptr,
		nullptr);

	return result;
}

#endif //_WIN32
