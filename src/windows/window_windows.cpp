//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef KALAKIT_WINDOWS

#define KALAKIT_MODULE "WINDOW"

//external
#include "crashHandler.hpp"

//kalawindow
#include "window.hpp"
#include "input.hpp"
#include "messageloop.hpp"
#include "magic_enum.hpp"
#include "opengl.hpp"
#include "opengl_loader.hpp"

using std::to_string;

namespace KalaKit
{
	bool KalaWindow::Initialize(const string& title, int width, int height)
	{
		if (isInitialized)
		{
			LOG_ERROR("Window is already initialized!");
			return false;
		}

		//first initialize crash handler
		KalaCrashHandler::Initialize();

		HINSTANCE hInstance = GetModuleHandle(nullptr);

		WNDCLASSA wc = {};
		wc.lpfnWndProc = DefWindowProcA;
		wc.hInstance = hInstance;
		wc.lpszClassName = "KalaWindowClass";

		if (!RegisterClassA(&wc))
		{
			LOG_ERROR("Failed to register window class.");

			string title = "Window initialize error!";
			string message = "Failed to register window class!";

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

		HWND win = CreateWindowExA(
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

		if (!win)
		{
			LOG_ERROR("Failed to create window!");

			string title = "Window initialize error!";
			string message = "Failed to create window!";

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
		window.handle = win;

		window.context = GetDC(window.handle);

		proc = (WNDPROC)SetWindowLongPtr(
			window.handle,
			GWLP_WNDPROC,
			(LONG_PTR)MessageLoop::WindowProcCallback);
		window.callback = proc;

		ShowWindow(window.handle, SW_SHOW);

		//also initialize input
		KalaInput::Initialize();

		//also initialize opengl
		bool openglInitialized = OpenGL::Initialize();
		if (!openglInitialized) return false;

		//and finally set opengl viewport size
		OpenGLLoader::glViewportPtr(0, 0, width, height);

		isInitialized = true;

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
			&& GetForegroundWindow() != window.handle)
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

		SetWindowTextA(window, title.c_str());
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
			ShowWindow(window.handle, SW_RESTORE);
			break;
		case WindowState::WINDOW_MINIMIZED:
			ShowWindow(window.handle, SW_MINIMIZE);
			break;
		case WindowState::WINDOW_MAXIMIZED:
			ShowWindow(window.handle, SW_MAXIMIZE);
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

		LONG style = GetWindowLong(window.handle, GWL_STYLE);
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
			originalStyle = GetWindowLong(window.handle, GWL_STYLE);
			GetWindowPlacement(window, &originalPlacement);

			//set style to borderless
			SetWindowLong(window, GWL_STYLE, WS_POPUP);

			//resize to full monitor
			MONITORINFO mi = { sizeof(mi) };
			GetMonitorInfo(MonitorFromWindow(window.handle, MONITOR_DEFAULTTONEAREST), &mi);
			SetWindowPos(
				window,
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
			SetWindowLong(window.handle, GWL_STYLE, originalStyle);

			//restore previous size/position
			SetWindowPlacement(window.handle, &originalPlacement);
			SetWindowPos(
				window,
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

		return !IsWindowVisible(window.handle);
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

		ShowWindow(window, newWindowHiddenState ? SW_HIDE : SW_SHOW);
	}

	POINT KalaWindow::GetWindowPosition()
	{
		if (!isInitialized)
		{
			LOG_ERROR("Cannot get window position because KalaWindow is not initialized!");
			return { 0, 0 };
		}

		RECT rect{};
		GetWindowRect(window.handle, &rect);

		POINT pos{};
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
			window.handle,
			nullptr,
			width,
			height,
			0,
			0,
			SWP_NOSIZE
			| SWP_NOZORDER
			| SWP_NOOWNERZORDER);
	}

	POINT KalaWindow::GetWindowFullSize()
	{
		if (!isInitialized)
		{
			LOG_ERROR("Cannot get window full size because KalaWindow is not initialized!");
			return { 0, 0 };
		}

		RECT rect{};
		GetWindowRect(window.handle, &rect);

		POINT size{};
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
			window.handle,
			nullptr,
			0,
			0,
			width,
			height,
			SWP_NOMOVE
			| SWP_NOZORDER
			| SWP_NOOWNERZORDER);
	}

	POINT KalaWindow::GetWindowContentSize()
	{
		if (!isInitialized)
		{
			LOG_ERROR("Cannot get window content size because KalaWindow is not initialized!");
			return { 0, 0 };
		}

		RECT rect{};
		GetClientRect(window.handle, &rect);

		POINT size{};
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
				window.handle,
				GWL_STYLE),
			FALSE);

		SetWindowPos(
			window.handle,
			nullptr,
			0,
			0,
			rect.right - rect.left,
			rect.bottom - rect.top,
			SWP_NOMOVE
			| SWP_NOZORDER
			| SWP_NOOWNERZORDER);
	}

	POINT KalaWindow::GetWindowMaxSize()
	{
		if (!isInitialized)
		{
			LOG_ERROR("Cannot get window max size because KalaWindow is not initialized!");
			return { 0, 0 };
		}

		POINT point{};
		point.x = maxWidth;
		point.y = maxHeight;
		return point;
	}
	POINT KalaWindow::GetWindowMinSize()
	{
		if (!isInitialized)
		{
			LOG_ERROR("Cannot get window min size because KalaWindow is not initialized!");
			return { 0, 0 };
		}

		POINT point{};
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