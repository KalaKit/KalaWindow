//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include "core/kw_messageloop_windows.hpp"

#if defined(KWIN_ANY)

#include <windows.h>
#include <winuser.h>
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

#include "log_utils.hpp"
#include "math_utils.hpp"
#include "key_standards.hpp"

#include "core/kw_input.hpp"
#include "core/kw_core.hpp"
#include "core/kw_registry.hpp"
#include "graphics/kw_window.hpp"
#include "graphics/kw_window_global.hpp"

using KalaHeaders::KalaCore::ToVar;
using KalaHeaders::KalaMath::vec2;
using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;
using KalaHeaders::KalaKeyStandards::KeyboardButton;
using KalaHeaders::KalaKeyStandards::MouseButton;
using KalaHeaders::KalaKeyStandards::GetValueByKey;

using KalaWindow::Core::Input;
using KalaWindow::Graphics::WindowState;
using KalaWindow::Graphics::ProcessWindow;
using KalaWindow::Graphics::Window_Global;
using KalaWindow::Graphics::WindowData;

using std::string;
using std::string_view;
using std::to_string;
using std::vector;
using std::ostringstream;
using std::function;
using std::unordered_map;

using std::wstring;

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
		ProcessWindow* window = rcast<ProcessWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
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
			//TODO: recreate identically for Linux

			/*
			if (Window_Global::CreatePopup(
				"Quitting application",
				"Are you sure you want to quit? Unclosed data may be lost!",
				PopupAction::POPUP_ACTION_YES_NO,
				PopupType::POPUP_TYPE_WARNING)
				== PopupResult::POPUP_RESULT_YES)
			{
				ProcessWindow::GetRegistry().DestroyAllContent();
				return TRUE; //user clicked yes, continuing to logoff/shutdown
			}
			else return FALSE; //user clicked no, cancelling logoff/shutdown
			*/

			return TRUE;
		}
		//actually go through with logoff/shutdown
		case WM_ENDSESSION: return 0;

		case WM_MOUSEACTIVATE:
		{
			if (Window_Global::IsVerboseLoggingEnabled())
			{
				Log::Print(
					"Clicked on window '" + to_string(window->GetID()) + "' client area.",
					"KW_MESSAGE_LOOP",
					LogType::LOG_VERBOSE);
			}

			return MA_ACTIVATE;
		}

		case WM_NCLBUTTONDOWN:
		{
			if (Window_Global::IsVerboseLoggingEnabled())
			{
				Log::Print(
					"Clicked on window '" + to_string(window->GetID()) + "' non-client area.",
					"KW_MESSAGE_LOOP",
					LogType::LOG_VERBOSE);
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
			auto cursor_test = [](
				HWND hwnd,
				UINT msg,
				WPARAM wParam,
				LPARAM lParam) -> LRESULT
				{
					POINT cursor{};
					cursor.x = GET_X_LPARAM(lParam);
					cursor.y = GET_Y_LPARAM(lParam);

					RECT rect{};
					GetWindowRect(hwnd, &rect);

					//if cursor isnt inside the window
					if (!PtInRect(&rect, cursor)) return HTNOWHERE;

					static constexpr int border = 10;

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
				};

			auto result = cursor_test(hwnd, msg, wParam, lParam);

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
				"KW_MESSAGE_LOOP",
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

		auto process_message = [](
			const MSG& msg,
			ProcessWindow* window) -> LRESULT
			{
				if (!window)
				{
					KalaWindowCore::ForceClose(
						"KalaWindow message loop error",
						"Failed to call process_message because its window was invalid!");
				}

				Input* input{};
				string err = Input::GetRegistry().GetContent(window->GetInputID(), input);
				if (!err.empty())
				{
					KalaWindowCore::ForceClose(
						"Kalawindow message loop error",
						"Failed to process message for window '" + to_string(window->ID) + "' because input was invalid! Reason: " + err);
				}

				/*
				if (msg.message == 0)
				{
					Log::Print(
						"Received empty or WM_NULL message.",
						"KW_MESSAGE_LOOP",
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
						"KW_MESSAGE_LOOP",
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

					return 0; //we handled it
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
							LogType::LOG_VERBOSE);
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

					return DefWindowProc(
						msg.hwnd,
						msg.message,
						msg.wParam,
						msg.lParam);
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
						return DefWindowProc(
							msg.hwnd,
							msg.message,
							msg.wParam,
							msg.lParam);
					}

					KeyboardButton key = TranslateVirtualKey(msg.wParam, msg.lParam);

					if (Input::IsVerboseLoggingEnabled())
					{
						Log::Print(
							"Detected keyboard key '" + TranslateVirtualKeyToString(msg.wParam, msg.lParam) + "' up.",
							"INPUT",
							LogType::LOG_VERBOSE);
					}

					if (input)
					{
						input->SetKeyState(
							key,
							false);
					}

					return DefWindowProc(
						msg.hwnd,
						msg.message,
						msg.wParam,
						msg.lParam);
				}

				//
				// MOUSE MOVE
				//

				case WM_MOUSEMOVE:
				{
					vec2 newPos =
					{
						f32(GET_X_LPARAM(msg.lParam)),
						f32(GET_Y_LPARAM(msg.lParam))
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

						input->mousePos = newPos;
						input->mouseDelta = delta;

						if (!window->isWindowHovered)
						{
							window->isWindowHovered = true;

							const WindowData& win = window->GetWindowData();
							HWND hwnd = ToVar<HWND>(win.window);

							TRACKMOUSEEVENT tme{};
							tme.cbSize = sizeof(tme);
							tme.dwFlags = TME_LEAVE;
							tme.hwndTrack = hwnd;

							TrackMouseEvent(&tme);
						}
					}

					return DefWindowProc(
						msg.hwnd,
						msg.message,
						msg.wParam,
						msg.lParam);
				}
				case WM_MOUSELEAVE:
				{
					window->isWindowHovered = false;

					return DefWindowProc(
						msg.hwnd,
						msg.message,
						msg.wParam,
						msg.lParam);
				}

				//
				// MOUSE WHEEL
				//

				case WM_MOUSEWHEEL:
				{
					int delta = GET_WHEEL_DELTA_WPARAM(msg.wParam);

					//convert to float steps (+1 or -1)
					f32 scroll = 0.0f;
					if (delta > 0) scroll = +1.0f;
					else if (delta < 0) scroll = -1.0f;

					if (input) input->mouseWheelDelta = scroll;

					return DefWindowProc(
						msg.hwnd,
						msg.message,
						msg.wParam,
						msg.lParam);
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
								LogType::LOG_VERBOSE);
						}
					}

					return DefWindowProc(
						msg.hwnd,
						msg.message,
						msg.wParam,
						msg.lParam);
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
								LogType::LOG_VERBOSE);
						}
					}

					return DefWindowProc(
						msg.hwnd,
						msg.message,
						msg.wParam,
						msg.lParam);
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
								LogType::LOG_VERBOSE);
						}
					}

					return DefWindowProc(
						msg.hwnd,
						msg.message,
						msg.wParam,
						msg.lParam);
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
								LogType::LOG_VERBOSE);
						}
					}

					return DefWindowProc(
						msg.hwnd,
						msg.message,
						msg.wParam,
						msg.lParam);
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
								LogType::LOG_VERBOSE);
						}
					}

					return DefWindowProc(
						msg.hwnd,
						msg.message,
						msg.wParam,
						msg.lParam);
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
								LogType::LOG_VERBOSE);
						}
					}

					return DefWindowProc(
						msg.hwnd,
						msg.message,
						msg.wParam,
						msg.lParam);
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
									LogType::LOG_VERBOSE);
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
									LogType::LOG_VERBOSE);
							}
						}
					}

					return DefWindowProc(
						msg.hwnd,
						msg.message,
						msg.wParam,
						msg.lParam);
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
									LogType::LOG_VERBOSE);
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
									LogType::LOG_VERBOSE);
							}
						}
					}

					return DefWindowProc(
						msg.hwnd,
						msg.message,
						msg.wParam,
						msg.lParam);
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
								LogType::LOG_VERBOSE);
						}
					}

					return DefWindowProc(
						msg.hwnd,
						msg.message,
						msg.wParam,
						msg.lParam);
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
								LogType::LOG_VERBOSE);
						}
					}

					return DefWindowProc(
						msg.hwnd,
						msg.message,
						msg.wParam,
						msg.lParam);
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
								LogType::LOG_VERBOSE);
						}
					}

					return DefWindowProc(
						msg.hwnd,
						msg.message,
						msg.wParam,
						msg.lParam);
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
									LogType::LOG_VERBOSE);
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
									LogType::LOG_VERBOSE);
							}
						}
					}

					return DefWindowProc(
						msg.hwnd,
						msg.message,
						msg.wParam,
						msg.lParam);
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
						return DefWindowProc(
							msg.hwnd,
							msg.message,
							msg.wParam,
							msg.lParam);
					}

					const RAWMOUSE& mouse = rcast<RAWINPUT*>(buffer.data())->data.mouse;

					//sets raw mouse movement
					if (mouse.usFlags == MOUSE_MOVE_RELATIVE
						&& input)
					{
						input->rawMouseDelta.x += (f32)mouse.lLastX;
						input->rawMouseDelta.y += (f32)mouse.lLastY;
					}

					return DefWindowProc(
						msg.hwnd,
						msg.message,
						msg.wParam,
						msg.lParam);
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

						return 1;
					}

					return DefWindowProc(
						msg.hwnd,
						msg.message,
						msg.wParam,
						msg.lParam);
				}

				//
				// FILE WAS DRAGGED ONTO WINDOW
				//

				case WM_DROPFILES:
				{
					HDROP hDrop = (HDROP)msg.wParam;

					POINT dropPoint{};
					DragQueryPoint(hDrop, &dropPoint);
					window->draggedFilesPos = vec2((f32)dropPoint.x, (f32)dropPoint.y);

					//count how many files were dropped
					UINT fileCount = DragQueryFileW(
						hDrop,
						0xFFFFFFFF,
						nullptr,
						0);

					vector<path> droppedFiles{};
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

					window->lastDraggedFiles = std::move(droppedFiles);

					if (Window_Global::IsVerboseLoggingEnabled())
					{
						for (const path& file : window->lastDraggedFiles)
						{
							Log::Print(
								"File '" + file.string() + "' was dragged to window '" + to_string(window->GetID()) + "'",
								"KW_MESSAGE_LOOP",
								LogType::LOG_VERBOSE);
						}
					}

					if (window->draggedFilesCallback)
					{
						window->draggedFilesCallback(window->lastDraggedFiles, window->draggedFilesPos);
					}

					return 0; //we handled it
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
								"Window '" + to_string(window->GetID()) + "' was deactivated.",
								"KW_MESSAGE_LOOP",
								LogType::LOG_VERBOSE);
						}

						break;
					}
					case WA_ACTIVE:      //outside focus
					case WA_CLICKACTIVE: //direct click focus
					{
						if (Window_Global::IsVerboseLoggingEnabled())
						{
							Log::Print(
								"Window '" + to_string(window->GetID()) + "' was activated.",
								"KW_MESSAGE_LOOP",
								LogType::LOG_VERBOSE);
						}

						break;
					}
					}

					return DefWindowProc(
						msg.hwnd,
						msg.message,
						msg.wParam,
						msg.lParam);
				}

				//window gains focus
				case WM_SETFOCUS:
				{
					if (input)
					{
						if (Window_Global::IsVerboseLoggingEnabled())
						{
							Log::Print(
								"Returned focus to window '" + to_string(window->GetID()) + "'!",
								"KW_MESSAGE_LOOP",
								LogType::LOG_VERBOSE);
						}
					}

					if (!window->IsFocused()) window->BringToFocus();

					return DefWindowProc(
						msg.hwnd,
						msg.message,
						msg.wParam,
						msg.lParam);
				}
				//window loses focus
				case WM_KILLFOCUS:
				{
					if (input)
					{
						if (Window_Global::IsVerboseLoggingEnabled())
						{
							Log::Print(
								"No longer focusing on window '" + to_string(window->GetID()) + "'.",
								"KW_MESSAGE_LOOP",
								LogType::LOG_VERBOSE);
						}
					}

					return DefWindowProc(
						msg.hwnd,
						msg.message,
						msg.wParam,
						msg.lParam);
				}

				//
				// WINDOW REDRAW
				//

				case WM_PAINT:
				{
					const WindowData& win = window->GetWindowData();
					HWND hwnd = ToVar<HWND>(win.window);
					
					PAINTSTRUCT ps;
					BeginPaint(hwnd, &ps);
					EndPaint(hwnd, &ps);

					return 0; //we handled it
				}

				//
				// WINDOW RESIZE
				//

				case WM_SIZE:
				{
					switch (msg.wParam)
					{
					case SIZE_MINIMIZED:
					{
						for (u32 childID : window->childIDs)
						{
							ProcessWindow* w{};
							string err = ProcessWindow::GetRegistry().GetContent(childID, w);
							if (!err.empty())
							{
								KalaWindowCore::ForceClose(
									"KalaWindow message loop error",
									"Failed to minimize child window '" + to_string(childID) 
									+ "' under parent '" + to_string(window->ID) + "'! Reason: " + err);
							}

							w->SetWindowState(WindowState::WINDOW_MINIMIZE);
						}

						break;
					}

					case SIZE_MAXIMIZED:
					case SIZE_RESTORED:
					{
						for (u32 childID : window->childIDs)
						{
							ProcessWindow* w{};
							string err = ProcessWindow::GetRegistry().GetContent(childID, w);
							if (!err.empty())
							{
								KalaWindowCore::ForceClose(
									"KalaWindow message loop error",
									"Failed to bring child window '" + to_string(childID) 
									+ "' under parent '" + to_string(window->ID) + "' to focus! Reason: " + err);
							}

							w->BringToFocus();
						}

						break;
					}
					}

					if (window->IsResizable()) window->isResizing = false;

					if (window->resizeCallback) window->resizeCallback();

					return 0; //we handled it
				}
				case WM_SIZING:
				{
					if (window->IsResizable()
						&& !window->IsResizing())
					{
						window->isResizing = true;
					}

					return 0; //we handled it
				}
				//scale correctly when going to other monitor
				case WM_DPICHANGED:
				{
					RECT* suggestedRect = rcast<RECT*>(msg.lParam);

					//resize window to suggestedRect
					SetWindowPos(
						ToVar<HWND>(window->GetWindowData().window),
						nullptr,
						suggestedRect->left,
						suggestedRect->top,
						suggestedRect->right - suggestedRect->left,
						suggestedRect->bottom - suggestedRect->top,
						SWP_NOZORDER
						| SWP_NOACTIVATE);

					return 0; //we handled it
				}

				//
				// CAP MIN AND MAX WINDOW SIZE
				//

				case WM_GETMINMAXINFO:
				{
					MINMAXINFO* mmi = rcast<MINMAXINFO*>(msg.lParam);

					mmi->ptMinTrackSize.x = window->GetMinSize().x;
					mmi->ptMinTrackSize.y = window->GetMinSize().y;

					mmi->ptMaxTrackSize.x = window->GetMaxSize().x;
					mmi->ptMaxTrackSize.y = window->GetMaxSize().y;

					return 0; //we handled it
				}

				//
				// SHUTDOWN
				//

				//destroy current window if user clicked X button or pressed Alt + F4
				case WM_CLOSE:
				{
					window->Destroy();
					
					return 0; //we handled it
				}

				default:
				{
					return DefWindowProc(
						msg.hwnd,
						msg.message,
						msg.wParam,
						msg.lParam);
				}
				}
			};

		return process_message(msgObj, window);
	}

    void MessageLoop::SetAddCharCallback(function<void(u32)>&& newCallback)
	{
		addCharCallback = std::move(newCallback);
	}
	void MessageLoop::SetRemoveFromBackCallback(function<void()>&& newCallback)
	{
		removeFromBackCallback = std::move(newCallback);
	}
	void MessageLoop::SetAddTabCallback(function<void()>&& newCallback)
	{
		addTabCallback = std::move(newCallback);
	}
	void MessageLoop::SetAddNewLineCallback(function<void()>&& newCallback)
	{
		addNewlineCallback = std::move(newCallback);
	}
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

#endif //KWIN_ANY
