//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef _WIN32
#include <windows.h>
#else
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/extensions/XInput2.h>
#endif
#include <string>
#include <memory>

#include "log_utils.hpp"
#include "key_standards.hpp"

#include "core/kw_core.hpp"
#include "core/kw_input.hpp"
#include "graphics/kw_window.hpp"

#ifdef __linux__
#include "core/kw_messageloop_x11.hpp"
#include "graphics/kw_window_global.hpp"
#endif

using KalaHeaders::KalaCore::ToVar;

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;

using KalaHeaders::KalaKeyStandards::KeyToIndex;
using KalaHeaders::KalaKeyStandards::MouseToIndex;
using KalaHeaders::KalaKeyStandards::IndexToKey;
using KalaHeaders::KalaKeyStandards::IndexToMouse;

using KalaWindow::Graphics::ProcessWindow;
using KalaWindow::Graphics::WindowData;
#ifdef __linux__
using KalaWindow::Graphics::X11GlobalData;
using KalaWindow::Graphics::Window_Global;
#endif

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
		ProcessWindow* w = ProcessWindow::GetRegistry().GetContent(windowID);

		if (!w
			|| !w->IsInitialized())
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

		inputPtr->ID = newID;

		//
		// MOUSE RAW INPUT
		//

		//x11 raw input is enabled globally in x11 global window init
#ifdef _WIN32
		const WindowData& windowData = w->GetWindowData();

		if (!windowData.window)
        {
            KalaWindowCore::ForceClose(
                "Window error",
                "Failed to initialize raw input device because the attached window was invalid!");
        }

		HWND winRef = ToVar<HWND>(windowData.window);

		RAWINPUTDEVICE rid{};
		rid.usUsagePage = 0x01;
		rid.usUsage = 0x02;
		rid.dwFlags = RIDEV_INPUTSINK;
		rid.hwndTarget = winRef;

		RegisterRawInputDevices(&rid, 1, sizeof(rid));
#endif

		registry.AddContent(newID, std::move(newInput));
		w->SetInputID(newID);

		inputPtr->windowID = w->GetID();

		inputPtr->isInitialized = true;

		Log::Print(
			"Initialized input context with ID '" + to_string(newID) + "' for window '" + w->GetTitle() + "'!",
			"INPUT",
			LogType::LOG_SUCCESS);

		return inputPtr;
	}

	bool Input::IsInitialized() const { return isInitialized; }

	u32 Input::GetID() const { return ID; }
	u32 Input::GetWindowID() const { return windowID; }

	const string& Input::GetTypedLetter() const { return lastLetter; }
	void Input::SetTypedLetter(string_view letter) { lastLetter = letter; }

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
	void Input::SetMouseVisibility(
		bool state,
		bool updateBetweenFocus)
	{
		ProcessWindow* w = ProcessWindow::GetRegistry().GetContent(windowID);
		if (!w) return;

		if (updateBetweenFocus) isMouseVisible = state;

#ifdef _WIN32
		if (state) while (ShowCursor(TRUE) < 0);   //increment until visible
		else       while (ShowCursor(FALSE) >= 0); //decrement until hidden
#else
		const X11GlobalData& globalData = Window_Global::GetGlobalData();
		const WindowData& windowData = w->GetWindowData();

		if (!globalData.display)
        {
			KalaWindowCore::ForceClose(
				"Window error",
				"Failed to set mouse visibility because the attached display was invalid!");
        }
        if (!windowData.window)
        {
			KalaWindowCore::ForceClose(
				"Window error",
				"Failed to set mouse visibility because the attached window was invalid!");
        }

		Display* display = ToVar<Display*>(globalData.display);
		Window window = ToVar<Window>(windowData.window);
		
		if (state) XUndefineCursor(display, window);
		else
		{
			static Cursor hiddenCursor{};

			if (!hiddenCursor)
			{
				char data[1]{};
				Pixmap blank = XCreateBitmapFromData(
					display,
					window,
					data,
					1,
					1);

				XColor dummy{};
				hiddenCursor = XCreatePixmapCursor(
					display,
					blank,
					blank,
					&dummy,
					&dummy,
					0,
					0);

				XFreePixmap(display, blank);
			}

			XDefineCursor(display, window, hiddenCursor);
		}

		XFlush(display);
#endif

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
	void Input::SetMouseLockState(
		bool state,
		bool updateBetweenFocus)
	{
		ProcessWindow* w = ProcessWindow::GetRegistry().GetContent(windowID);
		if (!w) return;

		if (updateBetweenFocus) isMouseLocked = state;

		const WindowData& windowData = w->GetWindowData();

#ifdef _WIN32
		if (!state) ClipCursor(nullptr);
		else
		{
			if (!windowData.window)
			{
				KalaWindowCore::ForceClose(
					"Window error",
					"Failed to set mouse lock state because the attached window was invalid!");
			}

			HWND hwnd = ToVar<HWND>(windowData.window);

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
#else
		const X11GlobalData& globalData = Window_Global::GetGlobalData();
		
		if (!globalData.display)
        {
			KalaWindowCore::ForceClose(
				"Window error",
				"Failed to set mouse lock state because the attached display was invalid!");
        }
        if (!windowData.window)
        {
			KalaWindowCore::ForceClose(
				"Window error",
				"Failed to set mouse lock state because the attached window was invalid!");
        }

		Display* display = ToVar<Display*>(globalData.display);
		Window window = ToVar<Window>(windowData.window);

		if (!state) XUngrabPointer(display, CurrentTime);
		else
		{
			XGrabPointer(
				display,
				window,
				True,
				ButtonPressMask
				| ButtonReleaseMask
				| PointerMotionMask,
				GrabModeAsync,
				GrabModeAsync,
				window,
				None,
				CurrentTime);
		}
#endif

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
			ProcessWindow* w = ProcessWindow::GetRegistry().GetContent(windowID);
			if (!w) return;

			const WindowData& windowData = w->GetWindowData();

#ifdef _WIN32
			if (!windowData.window)
			{
				KalaWindowCore::ForceClose(
					"Window error",
					"Failed to end input frame update because the attached window was invalid!");
			}

			HWND windowRef = ToVar<HWND>(windowData.window);

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
#else
			const X11GlobalData& globalData = Window_Global::GetGlobalData();

			if (!globalData.display)
			{
				KalaWindowCore::ForceClose(
					"Window error",
					"Failed to end input frame update because the attached display was invalid!");
			}
			if (!windowData.window)
			{
				KalaWindowCore::ForceClose(
					"Window error",
					"Failed to end input frame update because the attached window was invalid!");
			}

			Display* display = ToVar<Display*>(globalData.display);
			Window window = ToVar<Window>(windowData.window);

			vec2 windowSize = w->GetClientRectSize();

			int centerX = windowSize.x / 2;
			int centerY = windowSize.y / 2;

			XWarpPointer(
				display,
				None,
				window,
				0, 0, 0, 0,
				centerX,
				centerY);
#endif
		}
	}

	Input::~Input()
	{
		ProcessWindow* w = ProcessWindow::GetRegistry().GetContent(windowID);
		if (!w) return;

		Log::Print(
			"Destroying input context for window '" + w->GetTitle() + "'.",
			"INPUT",
			LogType::LOG_INFO);

		EndFrameUpdate();
	}
}