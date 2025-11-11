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

#include "core/kw_core.hpp"
#include "core/kw_input.hpp"
#include "graphics/kw_window.hpp"

using KalaHeaders::Log;
using KalaHeaders::LogType;

using KalaWindow::Core::globalID;
using KalaWindow::Core::KalaWindowCore;
using KalaWindow::Graphics::Window;
using KalaWindow::Graphics::WindowData;
using KalaWindow::Graphics::TargetType;

using std::string;
using std::to_string;
using std::unique_ptr;
using std::make_unique;

namespace KalaWindow::Core
{
	Input* Input::Initialize(u32 windowID)
	{
		Window* window = Window::registry.GetContent(windowID);

		if (!window
			|| !window->IsInitialized())
		{
			KalaWindowCore::ForceClose(
				"Input error",
				"Failed to initialize input because its window was not found!");

			return nullptr;
		}

		u32 newID = ++globalID;
		unique_ptr<Input> newInput = make_unique<Input>();
		Input* inputPtr = newInput.get();

		Log::Print(
			"Creating input context for window '" + window->GetTitle() + "' with ID '" + to_string(newID) + "'.",
			"INPUT",
			LogType::LOG_DEBUG);

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

		registry.AddContent(newID, move(newInput));
		window->AddValue(TargetType::TYPE_INPUT, newID);

		inputPtr->windowID = window->GetID();

		inputPtr->isInitialized = true;

		Log::Print(
			"Initialized input context for window '" + window->GetTitle() + "' with ID '" + to_string(newID) + "'!",
			"INPUT",
			LogType::LOG_SUCCESS);

		return inputPtr;
	}

	bool Input::IsComboDown(const span<const InputCode>& codes)
	{
		if (codes.size() == 0) return false;

		for (const auto& c : codes)
		{
			if ((c.type == InputCode::Type::Key
				&& !IsKeyHeld(static_cast<Key>(c.code)))
				|| (c.type == InputCode::Type::Mouse
				&& !IsMouseButtonHeld(static_cast<MouseButton>(c.code))))
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
				&& !IsKeyHeld(static_cast<Key>(c.code)))
				|| (c.type == InputCode::Type::Mouse
				&& !IsMouseButtonHeld(static_cast<MouseButton>(c.code))))
			{
				return false;
			}
		}

		//last must be pressed
		const auto& c = *last;
		if ((c.type == InputCode::Type::Key
			&& !IsKeyPressed(static_cast<Key>(c.code)))
			|| (c.type == InputCode::Type::Mouse
			&& !IsMouseButtonPressed(static_cast<MouseButton>(c.code))))
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
				&& !IsKeyHeld(static_cast<Key>(c.code)))
				|| (c.type == InputCode::Type::Mouse
				&& !IsMouseButtonHeld(static_cast<MouseButton>(c.code))))
			{
				return false;
			}
		}

		//last must be released
		const auto& c = *last;
		if ((c.type == InputCode::Type::Key
			&& !IsKeyReleased(static_cast<Key>(c.code)))
			|| (c.type == InputCode::Type::Mouse
			&& !IsMouseButtonReleased(static_cast<MouseButton>(c.code))))
		{
			return false;
		}

		return true;
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
				"INPUT",
				LogType::LOG_INFO);
		}
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
			Window* window = Window::registry.GetContent(windowID);

			if (!window
				|| !window->IsInitialized())
			{
				Log::Print(
					"Cannot set mouse lock state because its window was not found!",
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
				"INPUT",
				LogType::LOG_INFO);
		}
	}

	void Input::SetMouseVisibilityBetweenFocus(bool state) const
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
				Window* window = Window::registry.GetContent(windowID);

				if (!window
					|| !window->IsInitialized())
				{
					Log::Print(
						"Cannot set mouse lock state between focus because its window was not found!",
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
		lastLetter.clear();

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
			Window* window = Window::registry.GetContent(windowID);

			if (!window
				|| !window->IsInitialized())
			{
				Log::Print(
					"Cannot get window reference at input end frame update because its window was not found!",
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
		if (!isInitialized)
		{
			Log::Print(
				"Cannot destroy input with ID '" + to_string(ID) + "' because it is not initialized!",
				"INPUT",
				LogType::LOG_ERROR,
				2);

			return;
		}

		Window* window = Window::registry.GetContent(windowID);

		if (!window
			|| !window->IsInitialized())
		{
			Log::Print(
				"Cannot destroy input with ID '" + to_string(ID) + "' because its window was not found!",
				"INPUT",
				LogType::LOG_ERROR,
				2);

			return;
		}

		Log::Print(
			"Destroying input with ID '" + to_string(ID) + "' for window '" + window->GetTitle() + "'.",
			"INPUT",
			LogType::LOG_DEBUG);

		EndFrameUpdate();
	}
}

#endif //_WIN32