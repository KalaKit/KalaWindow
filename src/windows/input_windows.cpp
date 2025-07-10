//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef _WIN32

#include <Windows.h>
#include <unordered_map>

#include "core/input.hpp"

using KalaWindow::Graphics::WindowStruct_Windows;

namespace KalaWindow::Core
{
	bool Input::Initialize(Window* window)
	{
		if (isInitialized)
		{
			LOG_ERROR("KalaInput is already initialized!");
			return false;
		}

		//
		// MOUSE RAW INPUT
		//

		WindowStruct_Windows& win = window->GetWindow_Windows();
		HWND winRef = ToVar<HWND>(win.hwnd);

		RAWINPUTDEVICE rid{};
		rid.usUsagePage = 0x01;
		rid.usUsage = 0x02;
		rid.dwFlags = RIDEV_INPUTSINK;
		rid.hwndTarget = winRef;

		RegisterRawInputDevices(&rid, 1, sizeof(rid));

		isInitialized = true;

		return true;
	}

	void Input::SetMouseVisibility(bool isVisible)
	{
		if (!isInitialized) return;

		isMouseVisible = isVisible;

		if (isMouseVisible) while (ShowCursor(TRUE) < 0); //increment until visible
		else while (ShowCursor(FALSE) >= 0);              //decrement until hidden
	}

	void Input::SetMouseLockState(
		bool isLocked,
		Window* targetWindow)
	{
		if (!isInitialized) return;

		if (isLocked)
		{
			WindowStruct_Windows& window = targetWindow->GetWindow_Windows();
			HWND windowRef = ToVar<HWND>(window.hwnd);

			RECT rect{};
			GetClientRect(windowRef, &rect);

			POINT center{};
			center.x = (rect.right - rect.left) / 2;
			center.y = (rect.bottom - rect.top) / 2;

			ClientToScreen(windowRef, &center);
			SetCursorPos(center.x, center.y);

			RECT clipRect{};
			GetClientRect(windowRef, &clipRect);
			MapWindowPoints(windowRef, nullptr, reinterpret_cast<POINT*>(&clipRect), 2);
			ClipCursor(&clipRect);

			isMouseLocked = true;
		}
		else
		{
			ClipCursor(nullptr);
			isMouseLocked = false;
		}
	}
}

#endif //_WIN32