//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef _WIN32

#define KALAKIT_MODULE "MESSAGELOOP"

#include <Windows.h>
#include <string>
#include <vector>

#ifndef GET_X_LPARAM
	#define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#endif
#ifndef GET_Y_LPARAM
	#define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))
#endif

#include "windows/messageloop.hpp"
#include "graphics/render.hpp"
#include "graphics/window.hpp"
#include "core/enums.hpp"
#include "core/input.hpp"

#ifdef KALAWINDOW_SUPPORT_OPENGL
#include "graphics/opengl/opengl_loader.hpp"
using KalaWindow::Graphics::OpenGLLoader;
#endif //KALAWINDOW_SUPPORT_OPENGL

using KalaWindow::Core::MessageLoop;
using KalaWindow::Graphics::Render;
using KalaWindow::Graphics::Window;
using KalaWindow::Graphics::WindowStruct_Windows;
using KalaWindow::DebugType;
using KalaWindow::Core::Input;
using KalaWindow::Core::Key;
using KalaWindow::Core::MouseButton;
using KalaWindow::Graphics::ShutdownState;

using std::string;
using std::vector;

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

		WindowStruct_Windows& data = windowPtr->GetWindow_Windows();
		if (reinterpret_cast<HWND>(data.hwnd) == hwnd)
		{
			window = windowPtr.get();
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

		if (Render::debugType == DebugType::DEBUG_ALL
			|| Render::debugType == DebugType::DEBUG_WINDOW_CORNER_EDGE)
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

	bool handled = ProcessMessage(msgObj, window);

	//tell windows this message is fully handled
	if (handled) return 0;

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

static bool ProcessMessage(const MSG& msg, Window* window)
{
	DebugType type = Render::debugType;
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

	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
	{
		Key key = static_cast<Key>(msg.wParam);
		Input::SetKeyState(key, true);
		return false;
	}
	case WM_SYSKEYUP:
	case WM_KEYUP:
	{
		Key key = static_cast<Key>(msg.wParam);
		Input::SetKeyState(key, false);
		return false;
	}

	//
	// MOUSE MOVE
	//

	case WM_MOUSEMOVE:
	{
		kvec2 newPos =
		{
			float(GET_X_LPARAM(msg.lParam)),
			float(GET_Y_LPARAM(msg.lParam))
		};

		//get the old position *before* updating
		kvec2 oldPos = Input::GetMousePosition();

		kvec2 delta = {
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
		return false;
	}
	case WM_RBUTTONDBLCLK:
	{
		Input::SetMouseButtonDoubleClickState(MouseButton::Right, true);
		return false;
	}
	case WM_MBUTTONDBLCLK:
	{
		Input::SetMouseButtonDoubleClickState(MouseButton::Right, true);
		return false;
	}
	case WM_XBUTTONDBLCLK:
	{
		WORD button = GET_XBUTTON_WPARAM(msg.wParam);
		if (button == XBUTTON1)
		{
			Input::SetMouseButtonDoubleClickState(MouseButton::X1, true);
		}
		if (button == XBUTTON2)
		{
			Input::SetMouseButtonDoubleClickState(MouseButton::X2, true);
		}
		return false;
	}

	//
	// MOUSE BUTTONS
	//

	case WM_LBUTTONDOWN:
	{
		Input::SetMouseButtonState(MouseButton::Left, true);
		return false;
	}
	case WM_LBUTTONUP:
	{
		Input::SetMouseButtonState(MouseButton::Left, false);
		return false;
	}

	case WM_RBUTTONDOWN:
	{
		Input::SetMouseButtonState(MouseButton::Right, true);
		return false;
	}
	case WM_RBUTTONUP:
	{
		Input::SetMouseButtonState(MouseButton::Right, false);
		return false;
	}

	case WM_MBUTTONDOWN:
	{
		Input::SetMouseButtonState(MouseButton::Middle, true);
		return false;
	}
	case WM_MBUTTONUP:
	{
		Input::SetMouseButtonState(MouseButton::Middle, false);
		return false;
	}

	case WM_XBUTTONDOWN:
	{
		WORD button = GET_XBUTTON_WPARAM(msg.wParam);
		if (button == XBUTTON1)
		{
			Input::SetMouseButtonState(MouseButton::X1, true);
		}
		if (button == XBUTTON2)
		{
			Input::SetMouseButtonState(MouseButton::X2, true);
		}
		return false;
	}
	case WM_XBUTTONUP:
	{
		WORD button = GET_XBUTTON_WPARAM(msg.wParam);
		if (button == XBUTTON1)
		{
			Input::SetMouseButtonState(MouseButton::X1, false);
		}
		if (button == XBUTTON2)
		{
			Input::SetMouseButtonState(MouseButton::X2, false);
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
				kvec2 newMouseRawDelta = Input::GetRawMouseDelta();

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
		WindowStruct_Windows& win = window->GetWindow_Windows();
		HWND hwnd = reinterpret_cast<HWND>(win.hwnd);
		
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
		
		window->SetWidth(width);
		window->SetHeight(height);

#ifdef KALAWINDOW_SUPPORT_OPENGL
		if (OpenGLLoader::glViewport)
		{
			OpenGLLoader::glViewport(0, 0, width, height);
		}
#endif //KALAWINDOW_SUPPORT_OPENGL

		if (type == DebugType::DEBUG_ALL
			|| type == DebugType::DEBUG_WINDOW_RESIZE)
		{
			LOG_DEBUG("New resolution: " << width << ", " << height);
		}

		return true;
	}
	case WM_SIZING:
	{
		HWND windowRef = reinterpret_cast<HWND>(window);

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
		HWND windowRef = reinterpret_cast<HWND>(window);

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
		Render::Shutdown(ShutdownState::SHUTDOWN_CLEAN);
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

#endif //_WIN32