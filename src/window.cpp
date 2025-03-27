//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

//main log macro
#define WRITE_LOG(type, msg) std::cout << "[KALAKIT_WINDOW | " << type << "] " << msg << "\n"

//log types
#if KALAWINDOW_DEBUG
	#define LOG_DEBUG(msg) WRITE_LOG("DEBUG", msg)
#else
	#define LOG_DEBUG(msg)
#endif
#define LOG_SUCCESS(msg) WRITE_LOG("SUCCESS", msg)
#define LOG_ERROR(msg) WRITE_LOG("ERROR", msg)

#include <iostream>

#include "window.hpp"
#include "magic_enum.hpp"

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

		HINSTANCE hInstance = GetModuleHandle(nullptr);

		WNDCLASSA wc = {};
		wc.lpfnWndProc = DefWindowProcA;
		wc.hInstance = hInstance;
		wc.lpszClassName = "KalaWindowClass";

		if (!RegisterClassA(&wc))
		{
			LOG_ERROR("Failed to register window class.");
			return false;
		}

		window = CreateWindowExA(
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

		if (!window)
		{
			LOG_ERROR("Failed to create window!");
			return false;
		}

		ShowWindow(window, SW_SHOW);
		isInitialized = true;

		LOG_SUCCESS("Window successfully initialized!");
		return true;
	}

	bool KalaWindow::ShouldClose()
	{
		return shouldClose;
	}

	bool KalaWindow::AllowExit()
	{
		return canExit;
	}

	void KalaWindow::SetShouldCloseState(bool newShouldCloseState)
	{
		if (!isInitialized)
		{
			LOG_ERROR("Cannot set should close state because KalaWindow is not initialized!");
			return;
		}

		shouldClose = newShouldCloseState;
	}

	void KalaWindow::SetAllowExitState(bool newAllowExitState)
	{
		if (!isInitialized)
		{
			LOG_ERROR("Cannot set allow exit state because KalaWindow is not initialized!");
			return;
		}

		canExit = newAllowExitState;
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

	void KalaWindow::SetDebugState(DebugType newDebugType)
	{
		if (!isInitialized)
		{
			LOG_ERROR("Cannot set debug state because KalaWindow is not initialized!");
			return;
		}

		debugType = newDebugType;
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
			ShowWindow(window, SW_RESTORE);
			break;
		case WindowState::WINDOW_MINIMIZED:
			ShowWindow(window, SW_MINIMIZE);
			break;
		case WindowState::WINDOW_MAXIMIZED:
			ShowWindow(window, SW_MAXIMIZE);
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

		LONG style = GetWindowLong(window, GWL_STYLE);
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
			originalStyle = GetWindowLong(window, GWL_STYLE);
			GetWindowPlacement(window, &originalPlacement);

			//set style to borderless
			SetWindowLong(window, GWL_STYLE, WS_POPUP);

			//resize to full monitor
			MONITORINFO mi = { sizeof(mi) };
			GetMonitorInfo(MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST), &mi);
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
			SetWindowLong(window, GWL_STYLE, originalStyle);

			//restore previous size/position
			SetWindowPlacement(window, &originalPlacement);
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

		return !IsWindowVisible(window);
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
		GetWindowRect(window, &rect);

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
			window,
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
		GetWindowRect(window, &rect);

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
			window,
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
		GetClientRect(window, &rect);

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
				window,
				GWL_STYLE),
			FALSE);

		SetWindowPos(
			window,
			nullptr,
			0,
			0,
			rect.right - rect.left,
			rect.bottom - rect.top,
			SWP_NOMOVE
			| SWP_NOZORDER
			| SWP_NOOWNERZORDER);
	}

	string KalaWindow::ToString(WindowState state)
	{
		return string(magic_enum::enum_name(state));
	}
}