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
#include "KalaHeaders/key_standards.hpp"

#include "core/kw_core.hpp"
#include "core/kw_input.hpp"
#include "graphics/kw_window.hpp"

using KalaHeaders::KalaCore::ToVar;
using KalaHeaders::KalaCore::FromVar;

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;

using KalaHeaders::KalaKeyStandards::KeyToIndex;
using KalaHeaders::KalaKeyStandards::MouseToIndex;
using KalaHeaders::KalaKeyStandards::IndexToKey;
using KalaHeaders::KalaKeyStandards::IndexToMouse;

using KalaWindow::Core::KalaWindowCore;
using KalaWindow::Graphics::Window;
using KalaWindow::Graphics::WindowData;

using std::string;
using std::to_string;
using std::unique_ptr;
using std::make_unique;

namespace KalaWindow::Core
{
	static KalaWindowRegistry<Input> registry{};

	static bool isVerboseLoggingEnabled{};

	KalaWindowRegistry<Input>& Input::GetRegistry() { return registry; }

	bool Input::IsVerboseLoggingEnabled() { return isVerboseLoggingEnabled; }
	void Input::SetVerboseLoggingState(bool newState) { isVerboseLoggingEnabled = newState; }

	Input* Input::Initialize(u32 windowID)
	{
		Window* window = Window::GetRegistry().GetContent(windowID);

		if (!window
			|| !window->IsInitialized())
		{
			KalaWindowCore::ForceClose(
				"Input error",
				"Failed to initialize input because its window was not found!");

			return nullptr;
		}

		u32 newID = KalaWindowCore::GetGlobalID() + 1;
		KalaWindowCore::SetGlobalID(newID);

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
		window->SetInputID(newID);

		inputPtr->windowID = window->GetID();

		inputPtr->isInitialized = true;

		Log::Print(
			"Initialized input context for window '" + window->GetTitle() + "' with ID '" + to_string(newID) + "'!",
			"INPUT",
			LogType::LOG_SUCCESS);

		return inputPtr;
	}

	bool Input::IsInitialized() const { return isInitialized; }

	u32 Input::GetID() const { return ID; }
	u32 Input::GetWindowID() const { return windowID; }

	const string& Input::GetTypedLetter() const { return lastLetter; }
	void Input::SetTypedLetter(const string& letter) { lastLetter = letter; }

	void Input::SetKeyState(
		KeyboardButton key,
		bool isDown)
	{
		size_t index = KeyToIndex(key);
		if (index == MAXSIZE_T) return;

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
		MouseButton mouseButton,
		bool isDown)
	{
		size_t index = MouseToIndex(mouseButton);
		if (index == MAXSIZE_T) return;

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
		MouseButton mouseButton,
		bool isDown)
	{
		size_t index = MouseToIndex(mouseButton);
		if (index == MAXSIZE_T) return;

		mouseDoubleClicked[index] = isDown;
	}

	vector<KeyboardButton> Input::GetPressedKeys()
	{
		vector<KeyboardButton> result{};

		for (size_t i = 0; i < keyPressed.size(); ++i)
		{
			if (keyPressed[i]) result.push_back(IndexToKey(i));
		}

		return result;
	}
	vector<KeyboardButton> Input::GetHeldKeys()
	{
		vector<KeyboardButton> result{};

		for (size_t i = 0; i < keyDown.size(); ++i)
		{
			if (keyDown[i]) result.push_back(IndexToKey(i));
		}

		return result;
	}
	vector<KeyboardButton> Input::GetReleasedKeys()
	{
		vector<KeyboardButton> result{};

		for (size_t i = 0; i < keyReleased.size(); ++i)
		{
			if (keyReleased[i]) result.push_back(IndexToKey(i));
		}

		return result;
	}

	vector<MouseButton> Input::GetPressedMouseButtons()
	{
		vector<MouseButton> result{};

		for (size_t i = 0; i < mousePressed.size(); ++i)
		{
			if (mousePressed[i]) result.push_back(IndexToMouse(i));
		}

		return result;
	}
	vector<MouseButton> Input::GetHeldMouseButtons()
	{
		vector<MouseButton> result{};

		for (size_t i = 0; i < mouseDown.size(); ++i)
		{
			if (mouseDown[i]) result.push_back(IndexToMouse(i));
		}

		return result;
	}
	vector<MouseButton> Input::GetReleasedMouseButtons()
	{
		vector<MouseButton> result{};

		for (size_t i = 0; i < mouseReleased.size(); ++i)
		{
			if (mouseReleased[i]) result.push_back(IndexToMouse(i));
		}

		return result;
	}
	vector<MouseButton> Input::GetDoubleClickedMouseButtons()
	{
		vector<MouseButton> result{};

		for (size_t i = 0; i < mouseDoubleClicked.size(); ++i)
		{
			if (mouseDoubleClicked[i]) result.push_back(IndexToMouse(i));
		}

		return result;
	}

	bool Input::IsComboDown(const span<const InputCode>& codes)
	{
		if (codes.size() == 0) return false;

		for (const auto& c : codes)
		{
			if ((c.type == InputCode::Type::Key
				&& !IsKeyHeld(scast<KeyboardButton>(c.code)))
				|| (c.type == InputCode::Type::Mouse
				&& !IsMouseButtonHeld(scast<MouseButton>(c.code))))
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
				&& !IsKeyHeld(scast<KeyboardButton>(c.code)))
				|| (c.type == InputCode::Type::Mouse
				&& !IsMouseButtonHeld(scast<MouseButton>(c.code))))
			{
				return false;
			}
		}

		//last must be pressed
		const auto& c = *last;
		if ((c.type == InputCode::Type::Key
			&& !IsKeyPressed(scast<KeyboardButton>(c.code)))
			|| (c.type == InputCode::Type::Mouse
			&& !IsMouseButtonPressed(scast<MouseButton>(c.code))))
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
				&& !IsKeyHeld(scast<KeyboardButton>(c.code)))
				|| (c.type == InputCode::Type::Mouse
				&& !IsMouseButtonHeld(scast<MouseButton>(c.code))))
			{
				return false;
			}
		}

		//last must be released
		const auto& c = *last;
		if ((c.type == InputCode::Type::Key
			&& !IsKeyReleased(scast<KeyboardButton>(c.code)))
			|| (c.type == InputCode::Type::Mouse
			&& !IsMouseButtonReleased(scast<MouseButton>(c.code))))
		{
			return false;
		}

		return true;
	}

	bool Input::IsKeyHeld(KeyboardButton key)
	{ 
		size_t index = KeyToIndex(key);
		if (index == MAXSIZE_T) return false;

		return keyDown[index];
	}
	bool Input::IsKeyPressed(KeyboardButton key)
	{
		size_t index = KeyToIndex(key);
		if (index == MAXSIZE_T) return false;

		return keyPressed[index];
	}
	bool Input::IsKeyReleased(KeyboardButton key)
	{
		size_t index = KeyToIndex(key);
		if (index == MAXSIZE_T) return false;

		return keyReleased[index];
	}

	bool Input::IsMouseButtonHeld(MouseButton mouseButton)
	{
		size_t index = MouseToIndex(mouseButton);
		if (index == MAXSIZE_T) return false;

		return mouseDown[index];
	}
	bool Input::IsMouseButtonPressed(MouseButton mouseButton)
	{
		size_t index = MouseToIndex(mouseButton);
		if (index == MAXSIZE_T) return false;

		return mousePressed[index];
	}
	bool Input::IsMouseButtonReleased(MouseButton mouseButton)
	{
		size_t index = MouseToIndex(mouseButton);
		if (index == MAXSIZE_T) return false;

		return mouseReleased[index];
	}

	bool Input::IsMouseButtonDoubleClicked(MouseButton mouseButton)
	{
		size_t index = MouseToIndex(mouseButton);
		if (index == MAXSIZE_T) return false;

		return mouseDoubleClicked[index];
	}

	bool Input::IsMouseButtonDragging(MouseButton mouseButton)
	{
		bool isHoldingDragKey = IsMouseButtonHeld(mouseButton);

		bool isDragging =
			isHoldingDragKey
			&& (mouseDelta.x != 0
				|| mouseDelta.y != 0);

		return isDragging;
	}

	vec2 Input::GetMousePosition() const { return mousePos; }
	void Input::SetMousePosition(vec2 newMousePos) { mousePos = newMousePos; }

	vec2 Input::GetMouseDelta()
	{
		vec2 currMouseDelta = mouseDelta;

		//reset after retrieval for per-frame delta behavior
		mouseDelta = vec2{ 0.0f, 0.0f };

		return currMouseDelta;
	}
	void Input::SetMouseDelta(vec2 newMouseDelta) { mouseDelta = newMouseDelta; }

	vec2 Input::GetRawMouseDelta()
	{
		vec2 currMouseDelta = rawMouseDelta;

		//reset after retrieval for per-frame delta behavior
		rawMouseDelta = vec2{ 0.0f, 0.0f };

		return currMouseDelta;
	}
	void Input::SetRawMouseDelta(vec2 newRawMouseDelta) { rawMouseDelta = newRawMouseDelta; }

	float Input::GetScrollwheelDelta() const { return mouseWheelDelta; }
	void Input::SetScrollwheelDelta(float delta) { mouseWheelDelta = delta; }

	bool Input::IsMouseVisible() const { return isMouseVisible; }

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

	bool Input::IsMouseLocked() const { return isMouseLocked; }

	void Input::SetMouseLockState(bool state)
	{
		isMouseLocked = state;

		if (!state)
		{
			ClipCursor(nullptr);
		}
		else
		{
			Window* window = Window::GetRegistry().GetContent(windowID);

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

	bool Input::GetKeepMouseDeltaState() const { return keepMouseDelta; }
	void Input::SetKeepMouseDeltaState(bool newState) { keepMouseDelta = newState; }

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
				Window* window = Window::GetRegistry().GetContent(windowID);

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
			Window* window = Window::GetRegistry().GetContent(windowID);

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

		Window* window = Window::GetRegistry().GetContent(windowID);

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