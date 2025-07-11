//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <array>

#include "core/platform.hpp"
#include "graphics/window.hpp"

namespace KalaWindow::Core
{
	using KalaWindow::Graphics::Window;

	using std::array;
	using std::fill;

	enum class Key : uint32_t
	{
		Unknown = 0,

		// --- Letters ---

		A, B, C, D, E, F, G,
		H, I, J, K, L, M, N,
		O, P, Q, R, S, T, U,
		V, W, X, Y, Z,

		// --- Top-row Numbers (0–9) ---

		Num0, Num1, Num2, Num3, Num4,
		Num5, Num6, Num7, Num8, Num9,

		// --- Function Keys (Full Range) ---

		F1, F2, F3, F4, F5, F6,
		F7, F8, F9, F10, F11, F12,
		F13, F14, F15, F16, F17, F18,
		F19, F20, F21, F22, F23, F24,

		// --- Numpad ---

		Numpad0, Numpad1, Numpad2, Numpad3, Numpad4,
		Numpad5, Numpad6, Numpad7, Numpad8, Numpad9,
		NumpadAdd, NumpadSubtract, NumpadMultiply,
		NumpadDivide, NumpadDecimal, NumLock,

		// --- Navigation ---

		ArrowLeft,
		ArrowRight,
		ArrowUp,
		ArrowDown,
		Home,
		End,
		PageUp,
		PageDown,
		Insert,
		Delete,

		// --- Basic Controls ---

		Enter,
		Escape,
		Backspace,
		Tab,
		CapsLock,
		Space,

		// --- Modifiers ---

		ShiftLeft,
		ShiftRight,
		CtrlLeft,
		CtrlRight,
		AltLeft,
		AltRight,
		SuperLeft,   // Win / Cmd
		SuperRight,

		// --- System / Special Keys ---

		PrintScreen,
		ScrollLock,
		Pause,
		Menu,

		// --- Common Symbols ---

		Minus,         // -
		Equal,         // =
		BracketLeft,   // [
		BracketRight,  // ]
		Backslash,     // '\'
		Semicolon,     // ;
		Apostrophe,    // '
		Comma,         // ,
		Period,        // .
		Slash,         // /
		Tilde,         // `~
		Oem102,        // <>

		// --- Media & Browser ---

		MediaPlayPause,
		MediaStop,
		MediaNextTrack,
		MediaPrevTrack,
		VolumeUp,
		VolumeDown,
		VolumeMute,
		LaunchMail,
		LaunchApp1,
		LaunchApp2,
		BrowserBack,
		BrowserForward,
		BrowserRefresh,
		BrowserStop,
		BrowserSearch,
		BrowserFavorites,
		BrowserHome,

		KeyCount
	};

	enum class MouseButton : uint32_t
	{
		Left,
		Right,
		Middle,
		X1,
		X2,
		X3,
		X4,
		X5,

		MouseButtonCount
	};

	class KALAWINDOW_API Input
	{
	public:
		static inline bool isInitialized = false;

		static bool Initialize(Window* window);

		static void SetKeyState(Key key, bool isDown)
		{
			if (!isInitialized) return;

			size_t index = static_cast<size_t>(key);

			keyPressed[index] = isDown && !keyDown[index];
			keyDown[index] = isDown;
		}
		static void SetMouseButtonState(MouseButton button, bool isDown)
		{
			if (!isInitialized) return;

			size_t index = static_cast<size_t>(button);

			mousePressed[index] = isDown && !mouseDown[index];
			mouseDown[index] = isDown;
		}
		static void SetMouseButtonDoubleClickState(MouseButton button, bool isDown)
		{
			if (!isInitialized) return;

			size_t index = static_cast<size_t>(button);

			mouseDoubleClicked[index] = isDown && !mouseDown[index];
		}

		//Is the key currently held down?
		static bool IsKeyDown(Key key)
		{ 
			if (!isInitialized) return false;

			size_t index = static_cast<size_t>(key);

			return keyDown[index];
		}
		//Was the key just pressed this frame?
		static bool IsKeyPressed(Key key)
		{
			if (!isInitialized) return false;

			size_t index = static_cast<size_t>(key);

			bool result = keyPressed[index];
			keyPressed[index] = false;
			return result;
		}
		//Was the key just released this frame?
		static bool IsKeyReleased(Key key)
		{
			if (!isInitialized) return false;

			size_t index = static_cast<size_t>(key);

			bool result = keyReleased[index];
			keyReleased[index] = false;
			return result;
		}

		//Is the mouse button currently held down?
		static bool IsMouseDown(MouseButton button)
		{
			if (!isInitialized) return false;

			size_t index = static_cast<size_t>(button);

			return mouseDown[index];
		}
		//Was the mouse button just pressed this frame?
		static bool IsMousePressed(MouseButton button)
		{
			if (!isInitialized) return false;

			size_t index = static_cast<size_t>(button);

			bool result = mousePressed[index];
			mousePressed[index] = false;
			return result;
		}
		//Was the mouse button just released this frame?
		static bool IsMouseReleased(MouseButton button)
		{
			if (!isInitialized) return false;

			size_t index = static_cast<size_t>(button);

			bool result = mouseReleased[index];
			mouseReleased[index] = false;
			return result;
		}

		//Was the mouse button just double-clicked this frame?
		static bool IsMouseButtonDoubleClicked(MouseButton button)
		{
			if (!isInitialized) return false;

			size_t index = static_cast<size_t>(button);

			bool result = mouseDoubleClicked[index];
			mouseDoubleClicked[index] = false;
			return result;
		}

		//Get current mouse position in window coordinates
		static kvec2 GetMousePosition()
		{
			return mousePos;
		}
		static void SetMousePosition(kvec2 newMousePos)
		{
			mousePos = newMousePos;
		}

		//Get mouse delta movement since last frame
		static kvec2 GetMouseDelta()
		{
			kvec2 currMouseDelta = mouseDelta;

			//reset after retrieval for per-frame delta behavior
			mouseDelta = kvec2{ 0.0f, 0.0f };

			return currMouseDelta;
		}
		static void SetMouseDelta(kvec2 newMouseDelta)
		{
			mouseDelta = newMouseDelta;
		}

		//Get mouse raw delta movement since last frame
		static kvec2 GetRawMouseDelta()
		{
			kvec2 currMouseDelta = rawMouseDelta;

			//reset after retrieval for per-frame delta behavior
			rawMouseDelta = kvec2{ 0.0f, 0.0f };

			return currMouseDelta;
		}
		static void SetRawMouseDelta(kvec2 newRawMouseDelta)
		{
			rawMouseDelta = newRawMouseDelta;
		}

		//Get vertical scroll wheel delta (-1 to +1)
		static float GetMouseWheelDelta()
		{
			return mouseWheelDelta;
		}
		static void SetMouseWheelDelta(float delta)
		{
			mouseWheelDelta = delta;
		}

		static bool IsMouseDragging()
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

		//Return true if cursor is not hidden.
		static bool IsMouseVisible() { return isMouseVisible; }
		//Allows to set the visibility state of the cursor, if true then the cursor is visible.
		static void SetMouseVisibility(bool isVisible);

		//Return true if the cursor is locked to the center of the window.
		static bool IsMouseLocked() { return isMouseLocked; }
		//Allows to set the lock state of the cursor, if true 
		//then the cursor is locked to the center of the window.
		static void SetMouseLockState(
			bool isLocked,
			Window* window);

		//Call at end of frame to reset pressed/released states
		static void EndFrameUpdate()
		{
			fill(keyPressed.begin(), keyPressed.end(), false);
			fill(keyReleased.begin(), keyReleased.end(), false);
			fill(mousePressed.begin(), mousePressed.end(), false);
			fill(mouseReleased.begin(), mouseReleased.end(), false);
			fill(mouseDoubleClicked.begin(), mouseDoubleClicked.end(), false);

			if (!IsMouseDragging())
			{
				mouseDelta = { 0, 0 };
				rawMouseDelta = { 0, 0 };
				mouseWheelDelta = 0;
			}
		}
	private:
		static inline array<
			bool, 
			static_cast<size_t>(Key::KeyCount)> 
			keyDown{};
		static inline array<
			bool, 
			static_cast<size_t>(Key::KeyCount)> 
			keyPressed{};
		static inline array<
			bool, 
			static_cast<size_t>(Key::KeyCount)> 
			keyReleased{};

		static inline array<
			bool, 
			static_cast<size_t>(MouseButton::MouseButtonCount)> 
			mouseDown{};
		static inline array<
			bool, 
			static_cast<size_t>(MouseButton::MouseButtonCount)> 
			mousePressed{};
		static inline array<
			bool, 
			static_cast<size_t>(MouseButton::MouseButtonCount)> 
			mouseReleased{};
		static inline array<
			bool,
			static_cast<size_t>(MouseButton::MouseButtonCount)>
			mouseDoubleClicked{};

		static inline bool isMouseVisible = true;
		static inline bool isMouseLocked = false;

		static inline kvec2 mousePos = kvec2{ 0.0f, 0.0f };
		static inline kvec2 mouseDelta = kvec2 { 0.0f, 0.0f };
		static inline kvec2 rawMouseDelta = kvec2{ 0.0f, 0.0f };

		static inline float mouseWheelDelta = 0.0f;
	};
}