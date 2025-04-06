//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

//main log macro
#define WRITE_LOG(type, msg) std::cout << "[KALAKIT_MESSAGELOOP | " << type << "] " << msg << "\n"

//log types
#if KALAWINDOW_DEBUG
#define LOG_DEBUG(msg) WRITE_LOG("DEBUG", msg)
#else
#define LOG_DEBUG(msg)
#endif
#define LOG_SUCCESS(msg) WRITE_LOG("SUCCESS", msg)
#define LOG_ERROR(msg) WRITE_LOG("ERROR", msg)

#include <vector>
#include <Windows.h>
#include <iostream>

#ifndef GET_X_LPARAM
	#define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#endif
#ifndef GET_Y_LPARAM
	#define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))
#endif

using std::vector;

#include "messageloop.hpp"
#include "enums.hpp"
#include "window.hpp"
#include "input.hpp"
#include "opengl_loader.hpp"

namespace KalaKit
{
	LRESULT MessageLoop::CursorTest(
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

	LRESULT CALLBACK MessageLoop::WindowProcCallback(
		HWND hwnd,
		UINT msg,
		WPARAM wParam,
		LPARAM lParam)
	{
		//
		// ENSURE CURSOR ICON IS CORRECT WHEN INSIDE WINDOW
		//

		if (msg == WM_NCHITTEST)
		{
			auto result = CursorTest(hwnd, msg, wParam, lParam);

			if (KalaWindow::GetDebugType() == DebugType::DEBUG_ALL
				|| KalaWindow::GetDebugType() == DebugType::DEBUG_WINDOW_CORNER_EDGE)
			{
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

				LOG_DEBUG("WM_NCHITTEST result: " << resultValue << " [" << result << "]");
			}

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

		bool handled = ProcessMessage(msgObj);

		//tell windows this message is fully handled
		if (handled) return 0;

		return CallWindowProc(KalaWindow::proc, hwnd, msg, wParam, lParam);
	}

	bool MessageLoop::ProcessMessage(const MSG& msg)
	{
		DebugType type = KalaWindow::GetDebugType();
		if (type == DebugType::DEBUG_ALL
			|| type == DebugType::DEBUG_PROCESS_MESSAGE_TEST)
		{
			if (msg.message == 0)
			{
				LOG_DEBUG("Received empty or WM_NULL message.");
			}
			else
			{
				LOG_DEBUG("Got message: " << msg.message);
			}
		}

		switch (msg.message)
		{
			//
			// KEYBOARD INPUT
			//

		case WM_KEYDOWN:
		{
			Key key = static_cast<Key>(msg.wParam);
			KalaInput::SetKeyState(key, true);
			return false;
		}
		case WM_KEYUP:
		{
			Key key = static_cast<Key>(msg.wParam);
			KalaInput::SetKeyState(key, false);
			return false;
		}

		//
		// MOUSE MOVE
		//

		case WM_MOUSEMOVE:
		{
			POINT newPos =
			{
				GET_X_LPARAM(msg.lParam),
				GET_Y_LPARAM(msg.lParam)
			};
			KalaInput::SetMousePosition(newPos);

			POINT mouseCurrentPos = KalaInput::GetMousePosition();
			POINT mouseCurrentDelta = KalaInput::GetMouseDelta();
			mouseCurrentDelta.x = newPos.x - mouseCurrentPos.x;
			mouseCurrentDelta.y = newPos.y - mouseCurrentPos.y;
			KalaInput::SetMouseDelta(mouseCurrentDelta);
			return false;
		}

		//
		// MOUSE WHEEL
		//

		case WM_MOUSEWHEEL:
		{
			int currentDelta = KalaInput::GetMouseWheelDelta();

			int delta = GET_WHEEL_DELTA_WPARAM(msg.wParam);
			if (delta > 0) currentDelta += 1;
			else if (delta < 0) currentDelta -= 1;

			KalaInput::SetMouseWheelDelta(currentDelta);

			return false;
		}

		//
		// MOUSE DOUBLE CLICK
		//

		case WM_LBUTTONDBLCLK:
		{
			KalaInput::SetKeyState(Key::MouseLeft, true);
			return false;
		}
		case WM_RBUTTONDBLCLK:
		{
			KalaInput::SetKeyState(Key::MouseRight, true);
			return false;
		}

		//
		// MOUSE BUTTONS
		//

		case WM_LBUTTONDOWN: KalaInput::SetKeyState(Key::MouseLeft, true); return false;
		case WM_LBUTTONUP: KalaInput::SetKeyState(Key::MouseLeft, false); return false;

		case WM_RBUTTONDOWN: KalaInput::SetKeyState(Key::MouseRight, true); return false;
		case WM_RBUTTONUP: KalaInput::SetKeyState(Key::MouseRight, false); return false;

		case WM_MBUTTONDOWN: KalaInput::SetKeyState(Key::MouseMiddle, true); return false;
		case WM_MBUTTONUP: KalaInput::SetKeyState(Key::MouseMiddle, false); return false;

		case WM_XBUTTONDOWN:
		{
			WORD button = GET_XBUTTON_WPARAM(msg.wParam);
			if (button == XBUTTON1) KalaInput::SetKeyState(Key::MouseX1, true);
			if (button == XBUTTON2) KalaInput::SetKeyState(Key::MouseX2, true);
			return false;
		}
		case WM_XBUTTONUP:
		{
			WORD button = GET_XBUTTON_WPARAM(msg.wParam);
			if (button == XBUTTON1) KalaInput::SetKeyState(Key::MouseX1, false);
			if (button == XBUTTON2) KalaInput::SetKeyState(Key::MouseX2, false);
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
						KalaInput::SetKeyState(static_cast<Key>(static_cast<int>(Key::MouseX3) + i), true);
					}
					if (mouse.usButtonFlags & upFlag)
					{
						KalaInput::SetKeyState(static_cast<Key>(static_cast<int>(Key::MouseX3) + i), false);
					}
				}

				//sets raw mouse movement
				if (mouse.usFlags == MOUSE_MOVE_RELATIVE)
				{
					POINT newMouseRawDelta = KalaInput::GetRawMouseDelta();

					newMouseRawDelta.x += mouse.lLastX;
					newMouseRawDelta.y += mouse.lLastY;

					KalaInput::SetRawMouseDelta(newMouseRawDelta);
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
			PAINTSTRUCT ps;
			BeginPaint(KalaWindow::window, &ps);
			EndPaint(KalaWindow::window, &ps);
			return true;
		}

		//
		// WINDOW RESIZE
		//

		case WM_SIZE:
		{
			int width = LOWORD(msg.lParam);
			int height = HIWORD(msg.lParam);

			if (OpenGLLoader::glViewportPtr)
			{
				OpenGLLoader::glViewportPtr(0, 0, width, height);
			}

			if (type == DebugType::DEBUG_ALL
				|| type == DebugType::DEBUG_WINDOW_RESIZE)
			{
				LOG_DEBUG("New resolution: " << width << ", " << height);
			}

			return true;
		}

		//
		// HANDLE RENDERING WHILE RESIZING
		//

		case WM_ERASEBKGND:
			return true;
		case WM_ENTERSIZEMOVE:
			SetTimer(KalaWindow::window, 1, 16, nullptr); //start count when entering resize, 60 fps
			return true;
		case WM_EXITSIZEMOVE:
			KillTimer(KalaWindow::window, 1); //stop count after exiting resize
			return true;
		case WM_TIMER:
			if (KalaWindow::OnRedraw) KalaWindow::OnRedraw(); //force redraw
			return true;

		//
		// CAP MIN AND MAX WINDOW SIZE
		//

		case WM_GETMINMAXINFO:
		{
			MINMAXINFO* mmi = reinterpret_cast<MINMAXINFO*>(msg.lParam);

			mmi->ptMinTrackSize.x = KalaWindow::GetWindowMinSize().x;
			mmi->ptMinTrackSize.y = KalaWindow::GetWindowMinSize().y;

			mmi->ptMaxTrackSize.x = KalaWindow::GetWindowMaxSize().x;
			mmi->ptMaxTrackSize.y = KalaWindow::GetWindowMaxSize().y;

			return true;
		}

		//
		// SHUTDOWN
		//

		//user clicked X button or pressed Alt + F4
		case WM_CLOSE:
			//first checks if developer has decided to enable the exit condition,
			//then returns true if uses presses YES to exit, otherwise cancels exit
			if (!KalaWindow::CanExit()
				&& KalaWindow::CreatePopup(
					KalaWindow::exitTitle,
					KalaWindow::exitInfo,
					PopupAction::POPUP_ACTION_YES_NO,
					PopupType::POPUP_TYPE_WARNING)
					== PopupResult::POPUP_RESULT_YES)
			{
				return true;
			}

			//signals the while loop to exit
			KalaWindow::SetShouldCloseState(true);

			//if user agrees to exit, then the window is destroyed
			DestroyWindow(KalaWindow::window);
			return true;

			//window was destroyed - tell the system to exit
		case WM_DESTROY:
			LOG_DEBUG("Window is destroyed.");
			PostQuitMessage(0);
			return true;

		default:
			return false;
		}

		return false;
	}
}