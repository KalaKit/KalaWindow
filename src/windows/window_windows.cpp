//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef KALAKIT_WINDOWS

#define KALAKIT_MODULE "WINDOW"

#include <memory>
#include <format>
#include <windows.h>
#include <gl/GL.h>

//external
#include "crashHandler.hpp"
#include "magic_enum.hpp"

//kalawindow
#include "window.hpp"
#include "input.hpp"
#include "messageloop.hpp"
#include "opengl.hpp"
#include "opengl_loader.hpp"
#include "internal/window_windows.hpp"

using std::to_string;
using std::make_unique;
using std::format;

namespace KalaKit
{
	bool KalaWindow::Initialize(
		const string& title, 
		int width, 
		int height,
		bool initializeOpenGL)
	{
		if (isInitialized)
		{
			LOG_ERROR("Window is already initialized!");
			return false;
		}

		if (initializeOpenGL) OpenGL::isInitialized = true;

		//first initialize crash handler
		KalaCrashHandler::Initialize();

		//initialize FreeType
		KalaWindow::freeType = make_unique<FreeType>();

		HINSTANCE hInstance = GetModuleHandle(nullptr);

		WNDCLASSA wc = {};
		wc.lpfnWndProc = MessageLoop::WindowProcCallback;
		wc.hInstance = hInstance;
		wc.lpszClassName = "KalaWindowClass";

		if (!RegisterClassA(&wc))
		{
			DWORD err = GetLastError();
			string message{};
			if (err == ERROR_CLASS_ALREADY_EXISTS)
			{
				message = "Window class already exists with different definition.\n";
			}
			else
			{
				message = "RegisterClassA failed with error: " + to_string(err) + "\n";
			}
			LOG_ERROR(message);

			string title = "Window initialize error!";

			if (KalaWindow::CreatePopup(
				title,
				message,
				PopupAction::POPUP_ACTION_OK,
				PopupType::POPUP_TYPE_ERROR)
				== PopupResult::POPUP_RESULT_OK)
			{
				KalaWindow::SetShouldCloseState(true);
			}

			return false;
		}

		HWND newWindow = CreateWindowExA(
			0,
			"KalaWindowClass",
			title.c_str(),
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT,
			width, height,
			nullptr,
			nullptr,
			hInstance,
			nullptr);

		if (!newWindow)
		{
			DWORD errorCode = GetLastError();
			LPSTR errorMsg = nullptr;
			FormatMessageA(
				FORMAT_MESSAGE_ALLOCATE_BUFFER 
				| FORMAT_MESSAGE_FROM_SYSTEM 
				| FORMAT_MESSAGE_IGNORE_INSERTS,
				nullptr, 
				errorCode, 
				0, 
				(LPSTR)&errorMsg, 
				0, 
				nullptr);

			string message = "CreateWindowExA failed with error "
				+ to_string(errorCode)
				+ ": "
				+ (errorMsg ? errorMsg : "Unknown");

			LOG_ERROR(message);

			if (errorMsg) LocalFree(errorMsg);

			string title = "Window initialize error!";

			if (KalaWindow::CreatePopup(
				title,
				message,
				PopupAction::POPUP_ACTION_OK,
				PopupType::POPUP_TYPE_ERROR)
				== PopupResult::POPUP_RESULT_OK)
			{
				KalaWindow::SetShouldCloseState(true);
			}

			return false;
		}
		Window_Windows::newWindow = newWindow;
		Window_Windows::newProc = (WNDPROC)GetWindowLongPtr(newWindow, GWLP_WNDPROC);

		ShowWindow(newWindow, SW_SHOW);
		UpdateWindow(newWindow);

		//also initialize input
		KalaInput::Initialize();

		if (initializeOpenGL)
		{
        	//initialize opengl
        	if (!OpenGL::Initialize()) return false;
		}
		else
		{
			LOG_DEBUG("User chose not to initialize OpenGL!");
		}

		isInitialized = true;

		//silly fix for window not showing triangle before resizing
		KalaWindow::SetWindowContentSize(width, height);

		LOG_SUCCESS("Window successfully initialized!");
		return true;
	}

	void KalaWindow::Update()
	{
		if (!isInitialized)
		{
			LOG_ERROR("Cannot run loop because KalaWindow is not initialized!");
			return;
		}

		//prevent updates when not focused
		if (isWindowFocusRequired
			&& GetForegroundWindow() != Window_Windows::newWindow)
		{
			return;
		}

		MSG msg;
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg); //translate virtual-key messages (like WM_KEYDOWN) to character messages (WM_CHAR)
			DispatchMessage(&msg);  //send the message to the window procedure
		}

		//ensures cursor stays locked every frame
		if (KalaInput::IsMouseLocked()) KalaInput::LockCursorToCenter();

		KalaInput::ResetFrameInput();
	}

	void KalaWindow::SwapOpenGLBuffers()
    {
		HDC hdc = GetDC(Window_Windows::newWindow);
		if (!SwapBuffers(hdc))
		{
			LOG_ERROR("SwapOpenGLBuffers failed on Windows!");
		}
    }

	void KalaWindow::SetWindowFocusRequiredState(bool newWindowFocusRequiredState)
	{
		if (!isInitialized)
		{
			LOG_ERROR("Cannot set window focus required state because KalaWindow is not initialized!");
			return;
		}

		isWindowFocusRequired = newWindowFocusRequiredState;
	}

	DebugType KalaWindow::GetDebugType()
	{
		return debugType;
	}
	void KalaWindow::SetDebugType(DebugType newDebugType)
	{
		debugType = newDebugType;
	}

	void KalaWindow::SetWindowTitle(const string& title)
	{
		if (!isInitialized)
		{
			LOG_ERROR("Cannot set window title because KalaWindow is not initialized!");
			return;
		}

		if (debugType == DebugType::DEBUG_ALL
			|| debugType == DebugType::DEBUG_WINDOW_TITLE)
		{
			LOG_DEBUG("New window title: " << title << "");
		}

		SetWindowTextA(Window_Windows::newWindow, title.c_str());
	}

	void KalaWindow::SetWindowState(WindowState state)
	{
		if (!isInitialized)
		{
			LOG_ERROR("Cannot set window state because KalaWindow is not initialized!");
			return;
		}

		if (debugType == DebugType::DEBUG_ALL
			|| debugType == DebugType::DEBUG_WINDOW_TITLE)
		{
			string type = ToString(state);
			LOG_DEBUG("New window type: " << type << "");
		}

		switch (state)
		{
		case WindowState::WINDOW_RESET:
			ShowWindow(Window_Windows::newWindow, SW_RESTORE);
			break;
		case WindowState::WINDOW_MINIMIZED:
			ShowWindow(Window_Windows::newWindow, SW_MINIMIZE);
			break;
		case WindowState::WINDOW_MAXIMIZED:
			ShowWindow(Window_Windows::newWindow, SW_MAXIMIZE);
			break;
		}
	}

	bool KalaWindow::IsWindowBorderless()
	{
		if (!isInitialized)
		{
			LOG_ERROR("Cannot get window borderless state because KalaWindow is not initialized!");
			return false;
		}

		LONG style = GetWindowLong(Window_Windows::newWindow, GWL_STYLE);
		return (style & WS_OVERLAPPEDWINDOW);
	}
	void KalaWindow::SetWindowBorderlessState(bool newWindowBorderlessState)
	{
		if (!isInitialized)
		{
			LOG_ERROR("Cannot set window borderless state because KalaWindow is not initialized!");
			return;
		}

		if (debugType == DebugType::DEBUG_ALL
			|| debugType == DebugType::DEBUG_WINDOW_BORDERLESS_STATE)
		{
			string type = newWindowBorderlessState ? "true" : "false";
			LOG_DEBUG("New window borderless state: " << type << "");
		}

		if (!newWindowBorderlessState)
		{
			//save original style and placement
			originalStyle = GetWindowLong(Window_Windows::newWindow, GWL_STYLE);
			GetWindowPlacement(Window_Windows::newWindow, &originalPlacement);

			//set style to borderless
			SetWindowLong(Window_Windows::newWindow, GWL_STYLE, WS_POPUP);

			//resize to full monitor
			MONITORINFO mi = { sizeof(mi) };
			GetMonitorInfo(MonitorFromWindow(Window_Windows::newWindow, MONITOR_DEFAULTTONEAREST), &mi);
			SetWindowPos(
				Window_Windows::newWindow,
				HWND_TOP,
				mi.rcMonitor.left, mi.rcMonitor.top,
				mi.rcMonitor.right - mi.rcMonitor.left,
				mi.rcMonitor.bottom - mi.rcMonitor.top,
				SWP_FRAMECHANGED
				| SWP_NOOWNERZORDER
				| SWP_SHOWWINDOW);

			isWindowBorderless = true;
		}
		else
		{
			//restore original style
			SetWindowLong(Window_Windows::newWindow, GWL_STYLE, originalStyle);

			//restore previous size/position
			SetWindowPlacement(Window_Windows::newWindow, &originalPlacement);
			SetWindowPos(
				Window_Windows::newWindow,
				nullptr,
				0,
				0,
				0,
				0,
				SWP_NOMOVE
				| SWP_NOSIZE
				| SWP_FRAMECHANGED
				| SWP_NOOWNERZORDER
				| SWP_NOZORDER
				| SWP_SHOWWINDOW);

			isWindowBorderless = true;
		}
	}

	bool KalaWindow::IsWindowHidden()
	{
		if (!isInitialized)
		{
			LOG_ERROR("Cannot get window hidden state because KalaWindow is not initialized!");
			return false;
		}

		return !IsWindowVisible(Window_Windows::newWindow);
	}
	void KalaWindow::SetWindowHiddenState(bool newWindowHiddenState)
	{
		if (!isInitialized)
		{
			LOG_ERROR("Cannot set window hidden state because KalaWindow is not initialized!");
			return;
		}

		if (debugType == DebugType::DEBUG_ALL
			|| debugType == DebugType::DEBUG_WINDOW_HIDDEN_STATE)
		{
			string type = newWindowHiddenState ? "true" : "false";
			LOG_DEBUG("New window hidden state: " << type << "");
		}

		ShowWindow(Window_Windows::newWindow, newWindowHiddenState ? SW_HIDE : SW_SHOW);
	}

	POS KalaWindow::GetWindowPosition()
	{
		if (!isInitialized)
		{
			LOG_ERROR("Cannot get window position because KalaWindow is not initialized!");
			return { 0, 0 };
		}

		RECT rect{};
		GetWindowRect(Window_Windows::newWindow, &rect);

		POS pos{};
		pos.x = rect.left;
		pos.y = rect.top;
		return pos;
	}
	void KalaWindow::SetWindowPosition(int width, int height)
	{
		if (!isInitialized)
		{
			LOG_ERROR("Cannot set window position because KalaWindow is not initialized!");
			return;
		}

		if (debugType == DebugType::DEBUG_ALL
			|| debugType == DebugType::DEBUG_WINDOW_SET_POSITION)
		{
			string sizeX = to_string(width);
			string sizeY = to_string(height);

			LOG_DEBUG("New window position: (" << width << ", " << height);
		}

		SetWindowPos(
			Window_Windows::newWindow,
			nullptr,
			width,
			height,
			0,
			0,
			SWP_NOSIZE
			| SWP_NOZORDER
			| SWP_NOOWNERZORDER);
	}

	POS KalaWindow::GetWindowFullSize()
	{
		if (!isInitialized)
		{
			LOG_ERROR("Cannot get window full size because KalaWindow is not initialized!");
			return { 0, 0 };
		}

		RECT rect{};
		GetWindowRect(Window_Windows::newWindow, &rect);

		POS size{};
		size.x = rect.right - rect.left;
		size.y = rect.bottom - rect.top;
		return size;
	}
	void KalaWindow::SetWindowFullSize(int width, int height)
	{
		if (!isInitialized)
		{
			LOG_ERROR("Cannot set window full size because KalaWindow is not initialized!");
			return;
		}

		if (debugType == DebugType::DEBUG_ALL
			|| debugType == DebugType::DEBUG_WINDOW_SET_FULL_SIZE)
		{
			string sizeX = to_string(width);
			string sizeY = to_string(height);

			LOG_DEBUG("New window full size: (" << width << ", " << height);
		}

		SetWindowPos(
			Window_Windows::newWindow,
			nullptr,
			0,
			0,
			width,
			height,
			SWP_NOMOVE
			| SWP_NOZORDER
			| SWP_NOOWNERZORDER);
	}

	POS KalaWindow::GetWindowContentSize()
	{
		if (!isInitialized)
		{
			LOG_ERROR("Cannot get window content size because KalaWindow is not initialized!");
			return { 0, 0 };
		}

		RECT rect{};
		GetClientRect(Window_Windows::newWindow, &rect);

		POS size{};
		size.x = rect.right - rect.left;
		size.y = rect.bottom - rect.top;
		return size;
	}
	void KalaWindow::SetWindowContentSize(int width, int height)
	{
		if (!isInitialized)
		{
			LOG_ERROR("Cannot set window content size because KalaWindow is not initialized!");
			return;
		}

		if (debugType == DebugType::DEBUG_ALL
			|| debugType == DebugType::DEBUG_WIŃDOW_SET_CONTENT_SIZE)
		{
			string sizeX = to_string(width);
			string sizeY = to_string(height);

			LOG_DEBUG("New window content size: (" << width << ", " << height);
		}

		RECT rect = { 0, 0, width, height };
		AdjustWindowRect(
			&rect,
			GetWindowLong(
				Window_Windows::newWindow,
				GWL_STYLE),
			FALSE);

		SetWindowPos(
			Window_Windows::newWindow,
			nullptr,
			0,
			0,
			rect.right - rect.left,
			rect.bottom - rect.top,
			SWP_NOMOVE
			| SWP_NOZORDER
			| SWP_NOOWNERZORDER);
	}

	POS KalaWindow::GetWindowMaxSize()
	{
		if (!isInitialized)
		{
			LOG_DEBUG("Returning default window max size because KalaWindow is not initialized!");
			return { 7680, 4320 };
		}

		POS point{};
		point.x = maxWidth;
		point.y = maxHeight;
		return point;
	}
	POS KalaWindow::GetWindowMinSize()
	{
		if (!isInitialized)
		{
			LOG_DEBUG("Returning default window min size because KalaWindow is not initialized!");
			return { 800, 600 };
		}

		POS point{};
		point.x = minWidth;
		point.y = minHeight;
		return point;
	}
	void KalaWindow::SetMinMaxSize(
		int newMaxWidth,
		int newMaxHeight,
		int newMinWidth,
		int newMinHeight)
	{
		if (!isInitialized)
		{
			LOG_ERROR("Cannot set window max and min size because KalaWindow is not initialized!");
			return;
		}

		if (debugType == DebugType::DEBUG_ALL
			|| debugType == DebugType::DEBUG_WINDOW_SET_MINMAX_SIZE)
		{
			LOG_DEBUG("Set new max size: " << maxWidth << ", " << maxHeight 
				<< ", min size:" << minWidth << ", " << minHeight);
		}

		maxWidth = newMaxWidth;
		maxHeight = newMaxHeight;
		minWidth = newMinWidth;
		minHeight = newMinHeight;
	}

	bool KalaWindow::CanExit()
	{
		return canExit;
	}

	string KalaWindow::ToString(WindowState state)
	{
		return string(magic_enum::enum_name(state));
	}

	bool KalaWindow::ShouldClose()
	{
		return shouldClose;
	}
	void KalaWindow::SetShouldCloseState(bool newShouldCloseState)
	{
		shouldClose = newShouldCloseState;
	}

	PopupResult KalaWindow::CreatePopup(
		const string& title,
		const string& message,
		PopupAction action,
		PopupType type)
	{
		int flags = 0;

		switch (action)
		{
		case PopupAction::POPUP_ACTION_OK: flags |= MB_OK; break;
		case PopupAction::POPUP_ACTION_OK_CANCEL: flags |= MB_OKCANCEL; break;
		case PopupAction::POPUP_ACTION_YES_NO: flags |= MB_YESNO; break;
		case PopupAction::POPUP_ACTION_YES_NO_CANCEL: flags |= MB_YESNOCANCEL; break;
		case PopupAction::POPUP_ACTION_RETRY_CANCEL: flags |= MB_RETRYCANCEL; break;
		default:
			flags |= MB_OK;
			break;
		}

		switch (type)
		{
		case PopupType::POPUP_TYPE_INFO: flags |= MB_ICONINFORMATION; break;
		case PopupType::POPUP_TYPE_WARNING: flags |= MB_ICONWARNING; break;
		case PopupType::POPUP_TYPE_ERROR: flags |= MB_ICONERROR; break;
		case PopupType::POPUP_TYPE_QUESTION: flags |= MB_ICONQUESTION; break;
		default:
			flags |= MB_ICONINFORMATION;
			break;
		}

		int result = MessageBox(
			nullptr,
			message.c_str(),
			title.c_str(),
			flags);

		//cast the result directly to your strongly-typed enum
		return static_cast<PopupResult>(result);
	}

	void KalaWindow::SetExitState(
		bool setExitAllowedState,
		const string& title,
		const string& info)
	{
		if (!isInitialized)
		{
			LOG_ERROR("Cannot set exit state because KalaWindow is not initialized!");
			return;
		}

		canExit = setExitAllowedState;
		exitTitle = title;
		exitInfo = info;
	}
}

#endif // KALAKIT_WINDOWS