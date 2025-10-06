//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef _WIN32

#include <Windows.h>
#include <unordered_map>
#include <string>

#include "KalaHeaders/log_utils.hpp"

#include "core/core.hpp"
#include "core/input.hpp"
#include "graphics/window.hpp"

using KalaHeaders::Log;
using KalaHeaders::LogType;

using KalaWindow::Core::KalaWindowCore;
using KalaWindow::Graphics::WindowData;
using KalaWindow::Graphics::InputData;

using std::string;

namespace KalaWindow::Core
{
	void Input::Initialize(Window* window)
	{
		if (!window) 
		{
			KalaWindowCore::ForceClose(
				"Input error",
				"Failed to initialize input because its target window context is invalid!");

			return;
		}

		InputData& iData = window->GetInputData();

		if (!iData.isInitialized)
		{
			Log::Print(
				"Input is already initialized for window '" + window->GetTitle() + "'!",
				"INPUT_WINDOWS",
				LogType::LOG_ERROR,
				2);

			return;
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

		iData.isInitialized = true;
	}
	bool Input::IsInitialized(Window* window)
	{
		if (!window) return false;

		InputData& iData = window->GetInputData();

		return iData.isInitialized;
	}

	void Input::SetKeyState(
		Window* window,
		Key key,
		bool isDown)
	{
		if (!window) return;

		InputData& iData = window->GetInputData();

		if (!iData.isInitialized) return;

		size_t index = static_cast<size_t>(key);

		if (isDown
			&& !iData.keyDown[index])
		{
			iData.keyPressed[index] = true;
		}
		if (!isDown
			&& iData.keyDown[index])
		{
			iData.keyReleased[index] = true;
		}

		iData.keyDown[index] = isDown;
	}
	void Input::SetMouseButtonState(
		Window* window,
		MouseButton button,
		bool isDown)
	{
		if (!window) return;

		InputData& iData = window->GetInputData();

		if (!iData.isInitialized) return;

		size_t index = static_cast<size_t>(button);

		if (isDown
			&& !iData.mouseDown[index])
		{
			iData.mousePressed[index] = true;
		}
		if (!isDown
			&& iData.mouseDown[index])
		{
			iData.mouseReleased[index] = true;
		}

		iData.mouseDown[index] = isDown;
	}
	void Input::SetMouseButtonDoubleClickState(
		Window* window,
		MouseButton button,
		bool isDown)
	{
		if (!window) return;

		InputData& iData = window->GetInputData();

		if (!iData.isInitialized) return;

		size_t index = static_cast<size_t>(button);

		iData.mouseDoubleClicked[index] = isDown;
	}

	bool Input::IsComboDown(
		Window* window,
		const span<const InputCode>& codes)
	{
		if (!window) return false;

		InputData& iData = window->GetInputData();

		if (!iData.isInitialized) return false;

		if (codes.size() == 0) return false;

		for (const auto& c : codes)
		{
			if ((c.type == InputCode::Type::Key
				&& !IsKeyDown(window, static_cast<Key>(c.code)))
				|| (c.type == InputCode::Type::Mouse
				&& !IsMouseDown(window, static_cast<MouseButton>(c.code))))
			{
				return false;
			}
		}
		return true;
	}
	bool Input::IsComboPressed(
		Window* window,
		const span<const InputCode>& codes)
	{
		if (!window) return false;

		InputData& iData = window->GetInputData();

		if (!iData.isInitialized) return false;

		if (codes.size() == 0) return false;

		auto it = codes.begin();
		auto last = prev(codes.end());

		//all except last must be held
		for (; it != last; ++it)
		{
			const auto& c = *it;
			if ((c.type == InputCode::Type::Key
				&& !IsKeyDown(window, static_cast<Key>(c.code)))
				|| (c.type == InputCode::Type::Mouse
				&& !IsMouseDown(window, static_cast<MouseButton>(c.code))))
			{
				return false;
			}
		}

		//last must be pressed
		const auto& c = *last;
		if ((c.type == InputCode::Type::Key
			&& !IsKeyPressed(window, static_cast<Key>(c.code)))
			|| (c.type == InputCode::Type::Mouse
			&& !IsMousePressed(window, static_cast<MouseButton>(c.code))))
		{
			return false;
		}

		return true;
	}
	bool Input::IsComboReleased(
		Window* window,
		const span<const InputCode>& codes)
	{
		if (!window) return false;

		InputData& iData = window->GetInputData();

		if (!iData.isInitialized) return false;

		if (codes.size() == 0) return false;

		auto it = codes.begin();
		auto last = prev(codes.end());

		//all except last must be held
		for (; it != last; ++it)
		{
			const auto& c = *it;
			if ((c.type == InputCode::Type::Key
				&& !IsKeyDown(window, static_cast<Key>(c.code)))
				|| (c.type == InputCode::Type::Mouse
				&& !IsMouseDown(window, static_cast<MouseButton>(c.code))))
			{
				return false;
			}
		}

		//last must be released
		const auto& c = *last;
		if ((c.type == InputCode::Type::Key
			&& !IsKeyReleased(window, static_cast<Key>(c.code)))
			|| (c.type == InputCode::Type::Mouse
			&& !IsMouseReleased(window, static_cast<MouseButton>(c.code))))
		{
			return false;
		}

		return true;
	}

	bool Input::IsKeyDown(
		Window* window,
		Key key)
	{
		if (!window) return false;

		InputData& iData = window->GetInputData();

		if (!iData.isInitialized) return false;

		size_t index = static_cast<size_t>(key);

		return iData.keyDown[index];
	}
	bool Input::IsKeyPressed(
		Window* window,
		Key key)
	{
		if (!window) return false;

		InputData& iData = window->GetInputData();

		if (!iData.isInitialized) return false;

		size_t index = static_cast<size_t>(key);

		return iData.keyPressed[index];
	}
	bool Input::IsKeyReleased(
		Window* window,
		Key key)
	{
		if (!window) return false;

		InputData& iData = window->GetInputData();

		if (!iData.isInitialized) return false;

		size_t index = static_cast<size_t>(key);

		return iData.keyReleased[index];
	}

	bool Input::IsMouseDown(
		Window* window,
		MouseButton button)
	{
		if (!window) return false;

		InputData& iData = window->GetInputData();

		if (!iData.isInitialized) return false;

		size_t index = static_cast<size_t>(button);

		return iData.mouseDown[index];
	}
	bool Input::IsMousePressed(
		Window* window,
		MouseButton button)
	{
		if (!window) return false;

		InputData& iData = window->GetInputData();

		if (!iData.isInitialized) return false;

		size_t index = static_cast<size_t>(button);

		return iData.mousePressed[index];
	}
	bool Input::IsMouseReleased(
		Window* window,
		MouseButton button)
	{
		if (!window) return false;

		InputData& iData = window->GetInputData();

		if (!iData.isInitialized) return false;

		size_t index = static_cast<size_t>(button);

		return iData.mouseReleased[index];
	}

	bool Input::IsMouseButtonDoubleClicked(
		Window* window,
		MouseButton button)
	{
		if (!window) return false;

		InputData& iData = window->GetInputData();

		if (!iData.isInitialized) return false;

		size_t index = static_cast<size_t>(button);

		return iData.mouseDoubleClicked[index];
	}

	vec2 Input::GetMousePosition(Window* window)
	{
		if (!window) return{};

		InputData& iData = window->GetInputData();

		if (!iData.isInitialized) return{};

		return iData.mousePos;
	}
	void Input::SetMousePosition(
		Window* window,
		vec2 newMousePos)
	{
		if (!window) return;

		InputData& iData = window->GetInputData();

		if (!iData.isInitialized) return;

		iData.mousePos = newMousePos;
	}

	vec2 Input::GetMouseDelta(Window* window)
	{
		if (!window) return{};

		InputData& iData = window->GetInputData();

		if (!iData.isInitialized) return{};

		vec2 currMouseDelta = iData.mouseDelta;

		//reset after retrieval for per-frame delta behavior
		iData.mouseDelta = vec2{ 0.0f, 0.0f };

		return currMouseDelta;
	}
	void Input::SetMouseDelta(
		Window* window,
		vec2 newMouseDelta)
	{
		if (!window) return;

		InputData& iData = window->GetInputData();

		if (!iData.isInitialized) return;

		iData.mouseDelta = newMouseDelta;
	}

	vec2 Input::GetRawMouseDelta(Window* window)
	{
		if (!window) return{};

		InputData& iData = window->GetInputData();

		if (!iData.isInitialized) return{};

		vec2 currMouseDelta = iData.rawMouseDelta;

		//reset after retrieval for per-frame delta behavior
		iData.rawMouseDelta = vec2{ 0.0f, 0.0f };

		return currMouseDelta;
	}
	void Input::SetRawMouseDelta(
		Window* window,
		vec2 newRawMouseDelta)
	{
		if (!window) return;

		InputData& iData = window->GetInputData();

		if (!iData.isInitialized) return;

		iData.rawMouseDelta = newRawMouseDelta;
	}

	float Input::GetMouseWheelDelta(Window* window)
	{
		if (!window) return{};

		InputData& iData = window->GetInputData();

		if (!iData.isInitialized) return{};

		return iData.mouseWheelDelta;
	}
	void Input::SetMouseWheelDelta(
		Window* window,
		float delta)
	{
		if (!window) return;

		InputData& iData = window->GetInputData();

		if (!iData.isInitialized) return;

		iData.mouseWheelDelta = delta;
	}

	bool Input::IsMouseDragging(Window* window)
	{
		if (!window) return false;

		InputData& iData = window->GetInputData();

		if (!iData.isInitialized) return false;

		bool isHoldingDragKey =
			IsMouseDown(window, MouseButton::Left)
			|| IsMouseDown(window, MouseButton::Right);

		bool isDragging =
			isHoldingDragKey
			&& (iData.mouseDelta.x != 0
				|| iData.mouseDelta.y != 0);

		return isDragging;
	}

	bool Input::IsMouseVisible(Window* window)
	{
		if (!window) return false;

		InputData& iData = window->GetInputData();

		if (!iData.isInitialized) return false;

		return iData.isMouseVisible;
	}
	void Input::SetMouseVisibility(
		Window* window,
		bool state)
	{
		if (!window) return;

		InputData& iData = window->GetInputData();

		if (!iData.isInitialized) return;

		iData.isMouseVisible = state;

		if (state) while (ShowCursor(TRUE) < 0);   //increment until visible
		else       while (ShowCursor(FALSE) >= 0); //decrement until hidden

		if (isVerboseLoggingEnabled)
		{
			string val = state ? "true" : "false";

			Log::Print(
				"Set mouse visibility state to " + val,
				"INPUT_WINDOWS",
				LogType::LOG_INFO);
		}
	}

	bool Input::IsMouseLocked(Window* window)
	{
		if (!window) return false;

		InputData& iData = window->GetInputData();

		if (!iData.isInitialized) return false;

		return iData.isMouseLocked;
	}
	void Input::SetMouseLockState(
		Window* window,
		bool state)
	{
		if (!window) return;

		InputData& iData = window->GetInputData();

		if (!iData.isInitialized) return;

		iData.isMouseLocked = state;

		if (!state)
		{
			ClipCursor(nullptr);
		}
		else
		{
			HWND hwnd = ToVar<HWND>(window->GetWindowData().hwnd);

			RECT rect{};
			GetClientRect(hwnd, &rect);
			POINT ul{ rect.left, rect.top };
			POINT lr{ rect.right, rect.bottom };
			ClientToScreen(hwnd, &ul);
			ClientToScreen(hwnd, &lr);
			rect.left = ul.x; rect.top = ul.y;
			rect.right = lr.x; rect.bottom = lr.y;

			ClipCursor(&rect);
		}

		if (isVerboseLoggingEnabled)
		{
			string val = state ? "true" : "false";

			Log::Print(
				"Set mouse lock state to " + val,
				"INPUT_WINDOWS",
				LogType::LOG_INFO);
		}
	}

	bool Input::GetKeepMouseDeltaState(Window* window)
	{
		if (!window) return false;

		InputData& iData = window->GetInputData();

		if (!iData.isInitialized) return false;

		return iData.keepMouseDelta;
	}
	void Input::SetKeepMouseDeltaState(
		Window* window,
		bool newState)
	{
		if (!window) return;

		InputData& iData = window->GetInputData();

		if (!iData.isInitialized) return;

		iData.keepMouseDelta = newState;
	}

	void Input::SetMouseVisibilityBetweenFocus(
		Window* window,
		bool state)
	{
		if (!window) return;

		InputData& iData = window->GetInputData();

		if (!iData.isInitialized) return;

		if (!iData.isMouseVisible)
		{
			if (state) while (ShowCursor(TRUE) < 0);   //increment until visible
			else       while (ShowCursor(FALSE) >= 0); //decrement until hidden
		}
	}

	void Input::SetMouseLockStateBetweenFocus(
		Window* window,
		bool state)
	{
		if (!window) return;

		InputData& iData = window->GetInputData();

		if (!iData.isInitialized) return;

		if (iData.isMouseLocked)
		{
			if (!state)
			{
				ClipCursor(nullptr);
			}
			else
			{
				HWND hwnd = ToVar<HWND>(window->GetWindowData().hwnd);

				RECT rect{};
				GetClientRect(hwnd, &rect);
				POINT ul{ rect.left, rect.top };
				POINT lr{ rect.right, rect.bottom };
				ClientToScreen(hwnd, &ul);
				ClientToScreen(hwnd, &lr);
				rect.left = ul.x; rect.top = ul.y;
				rect.right = lr.x; rect.bottom = lr.y;

				ClipCursor(&rect);
			}
		}
	}

	void Input::ClearInputEvents(Window* window)
	{
		if (!window) return;

		InputData& iData = window->GetInputData();

		if (!iData.isInitialized) return;

		fill(iData.keyPressed.begin(), iData.keyPressed.end(), false);
		fill(iData.keyReleased.begin(), iData.keyReleased.end(), false);
		fill(iData.mousePressed.begin(), iData.mousePressed.end(), false);
		fill(iData.mouseReleased.begin(), iData.mouseReleased.end(), false);
		fill(iData.mouseDoubleClicked.begin(), iData.mouseDoubleClicked.end(), false);

		//always reset mouse wheel delta
		iData.mouseWheelDelta = 0;

		if (!iData.keepMouseDelta)
		{
			iData.mouseDelta = { 0, 0 };
			iData.rawMouseDelta = { 0, 0 };
		}
	}

	void Input::EndFrameUpdate(Window* window)
	{
		if (!window) return;

		InputData& iData = window->GetInputData();

		if (!iData.isInitialized) return;

		ClearInputEvents(window);

		if (iData.isMouseLocked)
		{
			const WindowData& windowData = window->GetWindowData();
			HWND windowRef = ToVar<HWND>(windowData.hwnd);

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