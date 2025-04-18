//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef KALAKIT_WINDOWS

#define KALAKIT_MODULE "INPUT"

#include <type_traits>
#include <Windows.h>
#include <TlHelp32.h>

//kalawindow
#include "input.hpp"
#include "window.hpp"
#include "messageloop.hpp"
#include "magic_enum.hpp"
#include "internal/window_windows.hpp"

using std::to_string;
using std::next;

namespace KalaKit
{
	void KalaInput::Initialize()
	{
		if (isInitialized)
		{
			LOG_ERROR("KalaInput is already initialized!");
			return;
		}

		//
		// MOUSE RAW INPUT
		//

		RAWINPUTDEVICE rid{};
		rid.usUsagePage = 0x01;
		rid.usUsage = 0x02;
		rid.dwFlags = RIDEV_INPUTSINK;
		rid.hwndTarget = Window_Windows::newWindow;

		RegisterRawInputDevices(&rid, 1, sizeof(rid));

		isInitialized = true;
	}

	bool KalaInput::IsKeyHeld(Key key)
	{
		if (!isInitialized)
		{
			LOG_ERROR("Cannot get key down because KalaInput is not initialized!");
			return false;
		}

		bool isKeyDown = keyHeld[key];

		DebugType type = KalaWindow::GetDebugType();
		if (type == DebugType::DEBUG_ALL
			|| type == DebugType::DEBUG_KEY_HELD)
		{
			//only print if key is down
			if (isKeyDown)
			{
				string keyName = ToString(key);
				LOG_DEBUG("Key '" << keyName << "' is down");
			}
		}

		return isKeyDown;
	}

	bool KalaInput::IsKeyPressed(Key key)
	{
		if (!isInitialized)
		{
			LOG_ERROR("Cannot get key pressed because KalaInput is not initialized!");
			return false;
		}

		bool wasKeyPressed = keyPressed[key];

		DebugType type = KalaWindow::GetDebugType();
		if (type == DebugType::DEBUG_ALL
			|| type == DebugType::DEBUG_KEY_PRESSED)
		{
			//only print if key was pressed
			if (wasKeyPressed)
			{
				string keyName = ToString(key);
				LOG_DEBUG("Pressed key '" << keyName << "'.");
			}
		}

		return wasKeyPressed;
	}

	bool KalaInput::IsComboPressed(initializer_list<Key> keys)
	{
		if (!isInitialized)
		{
			LOG_ERROR("Cannot get key combo pressed because KalaInput is not initialized!");
			return false;
		}

		//cannot proceed if only one key is assigned
		if (keys.size() < 2) return false;

		auto it = keys.begin();
		for (size_t i = 0; i < keys.size() - 1; i++)
		{
			if (!IsKeyHeld(*it++)) return false;
		}

		bool wasComboPressed = IsKeyPressed(*it);

		DebugType type = KalaWindow::GetDebugType();
		if (type == DebugType::DEBUG_ALL
			|| type == DebugType::DEBUG_COMBO_PRESSED)
		{
			//only print if combo was pressed
			if (wasComboPressed)
			{
				string comboString{};

				for (auto k = keys.begin(); k != keys.end(); ++k)
				{
					comboString += ToString(*k);
					if (next(k) != keys.end())
					{
						comboString += " + ";
					}
				}

				LOG_DEBUG("Pressed combo '" << comboString << "'.");
			}
		}

		//true as soon as last key is pressed
		return wasComboPressed;
	}

	bool KalaInput::IsMouseKeyDoubleClicked()
	{
		if (!isInitialized)
		{
			LOG_ERROR("Cannot get mouse key double clicked because KalaInput is not initialized!");
			return false;
		}

		bool wasDoubleClicked = 
			keyPressed[Key::MouseLeft]
			|| keyPressed[Key::MouseRight];

		DebugType type = KalaWindow::GetDebugType();
		if (type == DebugType::DEBUG_ALL
			|| type == DebugType::DEBUG_DOUBLE_CLICKED)
		{
			//only print if double clicked
			if (wasDoubleClicked)
			{
				LOG_DEBUG("Double clicked left mouse button.");
			}
		}

		return wasDoubleClicked;
	}

	bool KalaInput::IsMouseDragging()
	{
		if (!isInitialized)
		{
			LOG_ERROR("Cannot get mouse drag state because KalaInput is not initialized!");
			return false;
		}

		bool isHoldingDragKey =
			keyHeld[Key::MouseLeft]
			|| keyHeld[Key::MouseRight];

		bool isDragging = 
			isHoldingDragKey
			&& (mouseDelta.x != 0
			|| mouseDelta.y != 0);

		DebugType type = KalaWindow::GetDebugType();
		if (type == DebugType::DEBUG_ALL
			|| type == DebugType::DEBUG_IS_MOUSE_DRAGGING)
		{
			//only print if dragging
			if (isDragging)
			{
				LOG_DEBUG("Dragging mouse while holding left mouse button.");
			}
		}

		return isDragging;
	}

	POS KalaInput::GetMousePosition()
	{
		if (!isInitialized)
		{
			LOG_ERROR("Cannot get mouse position because KalaInput is not initialized!");
			return { 0, 0 };
		}

		DebugType type = KalaWindow::GetDebugType();
		if (type == DebugType::DEBUG_ALL
			|| type == DebugType::DEBUG_MOUSE_POSITION)
		{
			static POINT lastPos = { -9999, -9999 };

			//only print if mouse moved
			if (mousePosition.x != lastPos.x
				|| mousePosition.y != lastPos.y)
			{
				LOG_DEBUG("Mouse position: (" 
					<< mousePosition.x << ", " 
					<< mousePosition.y << ")");

				lastPos.x = mousePosition.x;
				lastPos.y = mousePosition.y;
			}
		}

		return mousePosition;
	}
	void KalaInput::SetMousePosition(POS newMousePosition)
	{
		if (!isInitialized)
		{
			LOG_ERROR("Cannot set mouse position because KalaInput is not initialized!");
			return;
		}

		mousePosition.x = newMousePosition.x;
		mousePosition.y = newMousePosition.y;
	}

	POS KalaInput::GetMouseDelta()
	{
		if (!isInitialized)
		{
			LOG_ERROR("Cannot get mouse delta because KalaInput is not initialized!");
			return { 0, 0 };
		}

		DebugType type = KalaWindow::GetDebugType();
		if (type == DebugType::DEBUG_ALL
			|| type == DebugType::DEBUG_MOUSE_DELTA)
		{
			//only print if mouse moved
			if (mouseDelta.x != 0
				|| mouseDelta.y != 0)
			{
				LOG_DEBUG("Mouse delta: (" 
					<< mouseDelta.x << ", " 
					<< mouseDelta.y << ")");
			}
		}

		return mouseDelta;
	}
	void KalaInput::SetMouseDelta(POS newMouseDelta)
	{
		if (!isInitialized)
		{
			LOG_ERROR("Cannot set mouse delta because KalaInput is not initialized!");
			return;
		}

		mouseDelta.x = newMouseDelta.x;
		mouseDelta.y = newMouseDelta.y;
	}

	POS KalaInput::GetRawMouseDelta()
	{
		if (!isInitialized)
		{
			LOG_ERROR("Cannot get mouse raw delta because KalaInput is not initialized!");
			return { 0, 0 };
		}

		DebugType type = KalaWindow::GetDebugType();
		if (type == DebugType::DEBUG_ALL
			|| type == DebugType::DEBUG_RAW_MOUSE_DELTA)
		{
			//only print if mouse moved
			if (rawMouseDelta.x != 0
				|| rawMouseDelta.y != 0)
			{
				LOG_DEBUG("Raw mouse delta: (" 
					<< rawMouseDelta.x << ", " 
					<< rawMouseDelta.y << ")");
			}
		}

		return rawMouseDelta;
	}
	void KalaInput::SetRawMouseDelta(POS newRawMouseDelta)
	{
		if (!isInitialized)
		{
			LOG_ERROR("Cannot set mouse raw delta because KalaInput is not initialized!");
			return;
		}

		rawMouseDelta.x = newRawMouseDelta.x;
		rawMouseDelta.y = newRawMouseDelta.y;
	}

	int KalaInput::GetMouseWheelDelta()
	{
		if (!isInitialized)
		{
			LOG_ERROR("Cannot get mouse delta because KalaInput is not initialized!");
			return 0;
		}

		DebugType type = KalaWindow::GetDebugType();
		if (type == DebugType::DEBUG_ALL
			|| type == DebugType::DEBUG_MOUSE_WHEEL_DELTA)
		{
			LOG_DEBUG("Mouse wheel delta: " << to_string(mouseWheelDelta) << "");
		}

		return mouseWheelDelta;
	}
	void KalaInput::SetMouseWheelDelta(int newMouseWheelDelta)
	{
		if (!isInitialized)
		{
			LOG_ERROR("Cannot set mouse wheel delta because KalaInput is not initialized!");
			return;
		}

		mouseWheelDelta = newMouseWheelDelta;
	}

	bool KalaInput::IsMouseVisible()
	{
		if (!isInitialized)
		{
			LOG_ERROR("Cannot get mouse visible state because KalaInput is not initialized!");
			return false;
		}

		return isMouseVisible;
	}
	void KalaInput::SetMouseVisibility(bool newMouseVisibleState)
	{
		if (!isInitialized)
		{
			LOG_ERROR("Cannot set mouse visible state because KalaInput is not initialized!");
			return;
		}

		//ignores same state
		if (isMouseVisible == newMouseVisibleState) return;

		DebugType type = KalaWindow::GetDebugType();
		if (type == DebugType::DEBUG_ALL
			|| type == DebugType::DEBUG_MOUSE_VISIBILITY)
		{
			string type = newMouseVisibleState ? "true" : "false";
			LOG_DEBUG("Mouse wheel visibility: " << type << "");
		}

		isMouseVisible = newMouseVisibleState;

		if (isMouseVisible) while (ShowCursor(TRUE) < 0); //increment until visible
		else while (ShowCursor(FALSE) >= 0);              //decrement until hidden
	}

	bool KalaInput::IsMouseLocked()
	{
		if (!isInitialized)
		{
			LOG_ERROR("Cannot get mouse locked state because KalaInput is not initialized!");
			return false;
		}

		return isMouseLocked;
	}
	void KalaInput::SetMouseLockState(bool newMouseLockState)
	{
		if (!isInitialized)
		{
			LOG_ERROR("Cannot set mouse lock state because KalaInput is not initialized!");
			return;
		}

		//ignores same state
		if (isMouseLocked == newMouseLockState) return;

		DebugType type = KalaWindow::GetDebugType();
		if (type == DebugType::DEBUG_ALL
			|| type == DebugType::DEBUG_MOUSE_LOCK_STATE)
		{
			string type = newMouseLockState ? "true" : "false";
			LOG_DEBUG("Mouse wheel lock state: " << type << "");
		}

		isMouseLocked = newMouseLockState;
		if (!isMouseLocked) ClipCursor(nullptr);
		else
		{
			LockCursorToCenter();

			//clip the cursor to the window to prevent it from leaving
			RECT rect{};
			GetClientRect(Window_Windows::newWindow, &rect);
			ClientToScreen(Window_Windows::newWindow, (POINT*)&rect.left);
			ClientToScreen(Window_Windows::newWindow, (POINT*)&rect.right);
			ClipCursor(&rect);
		}
	}

	string KalaInput::ToString(Key key)
	{
		return string(magic_enum::enum_name(key));
	}

	void KalaInput::ResetFrameInput()
	{
		//clear "pressed" keys after each frame
		for (auto& [key, pressed] : keyPressed)
		{
			pressed = false;
		}

		if (!IsMouseDragging())
		{
			mouseDelta = { 0, 0 };
			rawMouseDelta = { 0, 0 };
			mouseWheelDelta = 0;
		}
	}

	void KalaInput::LockCursorToCenter()
	{
		RECT rect{};
		GetClientRect(Window_Windows::newWindow, &rect);

		POINT center{};
		center.x = (rect.right - rect.left) / 2;
		center.y = (rect.bottom - rect.top) / 2;

		ClientToScreen(Window_Windows::newWindow, &center);
		SetCursorPos(center.x, center.y);
	}
}

#endif // KALAKIT_WINDOWS