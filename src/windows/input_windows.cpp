//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef _WIN32

#include <Windows.h>
#include <unordered_map>
#include <string>
#include <memory>

#include "KalaHeaders/log_utils.hpp"

#include "core/core.hpp"
#include "core/input.hpp"
#include "core/containers.hpp"
#include "graphics/window.hpp"

using KalaHeaders::Log;
using KalaHeaders::LogType;

using KalaWindow::Core::createdWindows;
using KalaWindow::Core::KalaWindowCore;
using KalaWindow::Graphics::WindowData;

using std::string;
using std::unique_ptr;
using std::make_unique;

namespace KalaWindow::Core
{
	Input* Input::Initialize(u32 windowID)
	{
		Window* window = GetValueByID<Window>(windowID);

		if (!window) 
		{
			KalaWindowCore::ForceClose(
				"Input error",
				"Failed to initialize input because its target window context is invalid!");

			return nullptr;
		}

		u32 newID = ++globalID;
		unique_ptr<Input> newInput = make_unique<Input>();
		Input* inputPtr = newInput.get();

		inputPtr->ID = newID;

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

		createdInput[newID] = move(newInput);
		runtimeInput.push_back(inputPtr);

		inputPtr->isInitialized = true;

		Log::Print(
			"Initialized input for window '" + window->GetTitle() + "'!",
			"DEBUG_UI",
			LogType::LOG_SUCCESS);

		return inputPtr;
	}
	bool Input::IsInitialized() const
	{
		return isInitialized;
	}

	void Input::SetKeyState(
		Key key,
		bool isDown)
	{
		size_t index = static_cast<size_t>(key);

		if (isDown
			&& !keyDown[index])
		{
			keyPressed[index] = true;
		}
		if (!isDown
			&& keyDown[index])
		{
			keyReleased[index] = true;
		}

		keyDown[index] = isDown;
	}
	void Input::SetMouseButtonState(
		MouseButton button,
		bool isDown)
	{
		size_t index = static_cast<size_t>(button);

		if (isDown
			&& !mouseDown[index])
		{
			mousePressed[index] = true;
		}
		if (!isDown
			&& mouseDown[index])
		{
			mouseReleased[index] = true;
		}

		mouseDown[index] = isDown;
	}
	void Input::SetMouseButtonDoubleClickState(
		MouseButton button,
		bool isDown)
	{
		size_t index = static_cast<size_t>(button);

		mouseDoubleClicked[index] = isDown;
	}

	bool Input::IsComboDown(const span<const InputCode>& codes)
	{
		if (codes.size() == 0) return false;

		for (const auto& c : codes)
		{
			if ((c.type == InputCode::Type::Key
				&& !IsKeyDown(static_cast<Key>(c.code)))
				|| (c.type == InputCode::Type::Mouse
				&& !IsMouseDown(static_cast<MouseButton>(c.code))))
			{
				return false;
			}
		}
		return true;
	}
	bool Input::IsComboPressed(const span<const InputCode>& codes)
	{
		if (codes.size() == 0) return false;

		auto it = codes.begin();
		auto last = prev(codes.end());

		//all except last must be held
		for (; it != last; ++it)
		{
			const auto& c = *it;
			if ((c.type == InputCode::Type::Key
				&& !IsKeyDown(static_cast<Key>(c.code)))
				|| (c.type == InputCode::Type::Mouse
				&& !IsMouseDown(static_cast<MouseButton>(c.code))))
			{
				return false;
			}
		}

		//last must be pressed
		const auto& c = *last;
		if ((c.type == InputCode::Type::Key
			&& !IsKeyPressed(static_cast<Key>(c.code)))
			|| (c.type == InputCode::Type::Mouse
			&& !IsMousePressed(static_cast<MouseButton>(c.code))))
		{
			return false;
		}

		return true;
	}
	bool Input::IsComboReleased(const span<const InputCode>& codes)
	{
		if (codes.size() == 0) return false;

		auto it = codes.begin();
		auto last = prev(codes.end());

		//all except last must be held
		for (; it != last; ++it)
		{
			const auto& c = *it;
			if ((c.type == InputCode::Type::Key
				&& !IsKeyDown(static_cast<Key>(c.code)))
				|| (c.type == InputCode::Type::Mouse
				&& !IsMouseDown(static_cast<MouseButton>(c.code))))
			{
				return false;
			}
		}

		//last must be released
		const auto& c = *last;
		if ((c.type == InputCode::Type::Key
			&& !IsKeyReleased(static_cast<Key>(c.code)))
			|| (c.type == InputCode::Type::Mouse
			&& !IsMouseReleased(static_cast<MouseButton>(c.code))))
		{
			return false;
		}

		return true;
	}

	bool Input::IsKeyDown(Key key)
	{
		size_t index = static_cast<size_t>(key);

		return keyDown[index];
	}
	bool Input::IsKeyPressed(Key key)
	{
		size_t index = static_cast<size_t>(key);

		return keyPressed[index];
	}
	bool Input::IsKeyReleased(Key key)
	{
		size_t index = static_cast<size_t>(key);

		return keyReleased[index];
	}

	bool Input::IsMouseDown(MouseButton button)
	{
		size_t index = static_cast<size_t>(button);

		return mouseDown[index];
	}
	bool Input::IsMousePressed(MouseButton button)
	{
		size_t index = static_cast<size_t>(button);

		return mousePressed[index];
	}
	bool Input::IsMouseReleased(MouseButton button)
	{
		size_t index = static_cast<size_t>(button);

		return mouseReleased[index];
	}

	bool Input::IsMouseButtonDoubleClicked(MouseButton button)
	{
		size_t index = static_cast<size_t>(button);

		return mouseDoubleClicked[index];
	}

	vec2 Input::GetMousePosition() const
	{
		return mousePos;
	}
	void Input::SetMousePosition(vec2 newMousePos)
	{
		mousePos = newMousePos;
	}

	vec2 Input::GetMouseDelta()
	{
		vec2 currMouseDelta = mouseDelta;

		//reset after retrieval for per-frame delta behavior
		mouseDelta = vec2{ 0.0f, 0.0f };

		return currMouseDelta;
	}
	void Input::SetMouseDelta(vec2 newMouseDelta)
	{
		mouseDelta = newMouseDelta;
	}

	vec2 Input::GetRawMouseDelta()
	{
		vec2 currMouseDelta = rawMouseDelta;

		//reset after retrieval for per-frame delta behavior
		rawMouseDelta = vec2{ 0.0f, 0.0f };

		return currMouseDelta;
	}
	void Input::SetRawMouseDelta(vec2 newRawMouseDelta)
	{
		rawMouseDelta = newRawMouseDelta;
	}

	float Input::GetMouseWheelDelta() const
	{
		return mouseWheelDelta;
	}
	void Input::SetMouseWheelDelta(float delta)
	{
		mouseWheelDelta = delta;
	}

	bool Input::IsMouseDragging()
	{
		bool isHoldingDragKey =
			IsMouseDown(MouseButton::Left)
			|| IsMouseDown(MouseButton::Right);

		bool isDragging =
			isHoldingDragKey
			&& (mouseDelta.x != 0
				|| mouseDelta.y != 0);

		return isDragging;
	}

	bool Input::IsMouseVisible() const
	{
		return isMouseVisible;
	}
	void Input::SetMouseVisibility(bool state)
	{
		isMouseVisible = state;

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

	bool Input::IsMouseLocked() const
	{
		return isMouseLocked;
	}
	void Input::SetMouseLockState(bool state)
	{
		isMouseLocked = state;

		if (!state)
		{
			ClipCursor(nullptr);
		}
		else
		{
			Window* window = GetValueByID<Window>(windowID);

			if (!window)
			{
				Log::Print(
					"Failed to get window reference when setting mouse lock state!",
					"INPUT",
					LogType::LOG_ERROR);

				return;
			}

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

	bool Input::GetKeepMouseDeltaState()
	{
		return keepMouseDelta;
	}
	void Input::SetKeepMouseDeltaState(bool newState)
	{
		keepMouseDelta = newState;
	}

	void Input::SetMouseVisibilityBetweenFocus(bool state)
	{
		if (!isMouseVisible)
		{
			if (state) while (ShowCursor(TRUE) < 0);   //increment until visible
			else       while (ShowCursor(FALSE) >= 0); //decrement until hidden
		}
	}

	void Input::SetMouseLockStateBetweenFocus(bool state) const
	{
		if (isMouseLocked)
		{
			if (!state)
			{
				ClipCursor(nullptr);
			}
			else
			{
				Window* window = GetValueByID<Window>(windowID);

				if (!window)
				{
					Log::Print(
						"Failed to get window reference when setting mouse lock state between focus!",
						"INPUT",
						LogType::LOG_ERROR);

					return;
				}

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

	void Input::ClearInputEvents()
	{
		fill(keyPressed.begin(), keyPressed.end(), false);
		fill(keyReleased.begin(), keyReleased.end(), false);
		fill(mousePressed.begin(), mousePressed.end(), false);
		fill(mouseReleased.begin(), mouseReleased.end(), false);
		fill(mouseDoubleClicked.begin(), mouseDoubleClicked.end(), false);

		//always reset mouse wheel delta
		mouseWheelDelta = 0;

		if (!keepMouseDelta)
		{
			mouseDelta = { 0, 0 };
			rawMouseDelta = { 0, 0 };
		}
	}

	void Input::EndFrameUpdate()
	{
		ClearInputEvents();

		if (isMouseLocked)
		{
			Window* window = GetValueByID<Window>(windowID);

			if (!window)
			{
				Log::Print(
					"Failed to get window reference at input end frame update!",
					"INPUT",
					LogType::LOG_ERROR);

				return;
			}

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

	Input::~Input()
	{

	}
}

#endif //_WIN32