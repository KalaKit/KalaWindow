//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef _WIN32

#include <Windows.h>
#include <unordered_map>

#include "KalaHeaders/logging.hpp"

#include "core/input.hpp"

using KalaHeaders::Log;
using KalaHeaders::LogType;

using KalaWindow::Graphics::WindowData;

namespace KalaWindow::Core
{
	bool Input::Initialize(Window* window)
	{
		if (isInitialized)
		{
			Log::Print(
				"Input is already initialized!",
				"INPUT",
				LogType::LOG_ERROR,
				2);
			return false;
		}

		//
		// MOUSE RAW INPUT
		//

		const WindowData& win = window->GetWindowData();
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

	void Input::SetMouseLockState(bool newState)
	{
		isMouseLocked = newState;
		if (!isMouseLocked)
		{
			ClipCursor(nullptr);
		}
	}

	void Input::EndFrameUpdate(Window* targetWindow)
	{
		if (!isInitialized) return;

		fill(keyPressed.begin(), keyPressed.end(), false);
		fill(keyReleased.begin(), keyReleased.end(), false);
		fill(mousePressed.begin(), mousePressed.end(), false);
		fill(mouseReleased.begin(), mouseReleased.end(), false);
		fill(mouseDoubleClicked.begin(), mouseDoubleClicked.end(), false);

		if (!keepMouseDelta)
		{
			mouseDelta = { 0, 0 };
			rawMouseDelta = { 0, 0 };
			mouseWheelDelta = 0;
		}

		if (isMouseLocked)
		{
			const WindowData& window = targetWindow->GetWindowData();
			HWND windowRef = ToVar<HWND>(window.hwnd);

			RECT rect{};
			GetClientRect(windowRef, &rect);

			POINT center{
				(rect.right - rect.left) / 2,
				(rect.bottom - rect.top) / 2
			};

			ClientToScreen(windowRef, &center);

			POINT pos;
			GetCursorPos(&pos);

			if (pos.x != center.x
				|| pos.y != center.y)
			{
				SetCursorPos(center.x, center.y);
			}
		}
	}
}

#endif //_WIN32