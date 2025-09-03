//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <array>

#include "KalaHeaders/api.hpp"
#include "KalaHeaders/core_types.hpp"

#include "graphics/window.hpp"

namespace KalaWindow::Core
{
	using KalaWindow::Graphics::Window;

	using std::array;
	using std::fill;
	using std::prev;

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
		Unknown = 0,

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

	struct InputCode
	{
		enum class Type : u8
		{
			Key,
			Mouse
		};

		Type type{};
		u32 code{}; //stores key or mouse button as u32

		static inline constexpr InputCode FromKey(Key k)
		{ 
			return 
			{ 
				Type::Key, 
				static_cast<u32>(k) 
			}; 
		}
		static inline constexpr InputCode FromMouse(MouseButton k)
		{ 
			return 
			{ 
				Type::Mouse, 
				static_cast<u32>(k) 
			}; 
		}
	};

	class LIB_API Input
	{
	public:
		static inline bool isInitialized = false;

		static bool Initialize(Window* window);

		//Toggle verbose logging. If true, then usually frequently updated runtime values like
		//key, mouse update messages will dump their logs into the console.
		static inline void SetVerboseLoggingState(bool newState) { isVerboseLoggingEnabled = newState; }
		static inline bool IsVerboseLoggingEnabled() { return isVerboseLoggingEnabled; }

		static inline void SetKeyState(Key key, bool isDown)
		{
			if (!isInitialized) return;

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
		static inline void SetMouseButtonState(MouseButton button, bool isDown)
		{
			if (!isInitialized) return;

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
		static inline void SetMouseButtonDoubleClickState(MouseButton button, bool isDown)
		{
			if (!isInitialized) return;

			size_t index = static_cast<size_t>(button);

			mouseDoubleClicked[index] = isDown;
		}

		//Detect if any combination of keys and mouse buttons are down
		template<typename Range>
		static inline bool IsComboDown(const Range& codes)
		{
			if (!isInitialized) return false;
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
		//Detect if any combination of keys and mouse buttons are pressed
		template<typename Range>
		static inline bool IsComboPressed(const Range& codes)
		{
			if (!isInitialized) return false;
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
		//Detect if any combination of keys and mouse buttons are released
		template<typename Range>
		static inline bool IsComboReleased(const Range& codes)
		{
			if (!isInitialized) return false;
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

		//Is the key currently held down?
		static inline bool IsKeyDown(Key key)
		{ 
			if (!isInitialized) return false;

			size_t index = static_cast<size_t>(key);

			return keyDown[index];
		}
		//Was the key just pressed this frame?
		static inline bool IsKeyPressed(Key key)
		{
			if (!isInitialized) return false;

			size_t index = static_cast<size_t>(key);

			return keyPressed[index];
		}
		//Was the key just released this frame?
		static inline bool IsKeyReleased(Key key)
		{
			if (!isInitialized) return false;

			size_t index = static_cast<size_t>(key);

			return keyReleased[index];
		}

		//Is the mouse button currently held down?
		static inline bool IsMouseDown(MouseButton button)
		{
			if (!isInitialized) return false;

			size_t index = static_cast<size_t>(button);

			return mouseDown[index];
		}
		//Was the mouse button just pressed this frame?
		static inline bool IsMousePressed(MouseButton button)
		{
			if (!isInitialized) return false;

			size_t index = static_cast<size_t>(button);

			return mousePressed[index];
		}
		//Was the mouse button just released this frame?
		static inline bool IsMouseReleased(MouseButton button)
		{
			if (!isInitialized) return false;

			size_t index = static_cast<size_t>(button);

			return mouseReleased[index];
		}

		//Was the mouse button just double-clicked this frame?
		static inline bool IsMouseButtonDoubleClicked(MouseButton button)
		{
			if (!isInitialized) return false;

			size_t index = static_cast<size_t>(button);

			return mouseDoubleClicked[index];
		}

		//Get current mouse position in window coordinates
		static inline vec2 GetMousePosition()
		{
			return mousePos;
		}
		static inline void SetMousePosition(vec2 newMousePos)
		{
			mousePos = newMousePos;
		}

		//Get mouse delta movement since last frame
		static inline vec2 GetMouseDelta()
		{
			vec2 currMouseDelta = mouseDelta;

			//reset after retrieval for per-frame delta behavior
			mouseDelta = vec2{ 0.0f, 0.0f };

			return currMouseDelta;
		}
		static inline void SetMouseDelta(vec2 newMouseDelta)
		{
			mouseDelta = newMouseDelta;
		}

		//Get mouse raw delta movement since last frame
		static inline vec2 GetRawMouseDelta()
		{
			vec2 currMouseDelta = rawMouseDelta;

			//reset after retrieval for per-frame delta behavior
			rawMouseDelta = vec2{ 0.0f, 0.0f };

			return currMouseDelta;
		}
		static inline void SetRawMouseDelta(vec2 newRawMouseDelta)
		{
			rawMouseDelta = newRawMouseDelta;
		}

		//Get vertical scroll wheel delta (-1 to +1)
		static inline float GetMouseWheelDelta()
		{
			return mouseWheelDelta;
		}
		static inline void SetMouseWheelDelta(float delta)
		{
			mouseWheelDelta = delta;
		}

		static inline bool IsMouseDragging()
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
		static inline bool IsMouseVisible() { return isMouseVisible; }
		//Allows to set the visibility state of the cursor, if true then the cursor is visible.
		static void SetMouseVisibility(bool isVisible);

		//Return true if the cursor is locked to the center of the window.
		static inline bool IsMouseLocked() { return isMouseLocked; }
		//Allows to set the lock state of the cursor, if true 
		//then the cursor is locked to the center of the window.
		static void SetMouseLockState(
			bool newState,
			Window* window);

		//If true, then mouse delta, raw delta and scroll delta wont be reset per frame.
		static inline bool GetKeepMouseDeltaState() { return keepMouseDelta; }
		static inline void SetKeepMouseDeltaState(bool newState) { keepMouseDelta = newState; }

		//If true, then mouse visibility is disabled when unfocused without clearing internal flag
		static void SetMouseVisibilityBetweenFocus(bool state);

		//If true, then mouse lock is disabled when unfocused without clearing internal flag
		static void SetMouseLockStateBetweenFocus(
			bool state,
			Window* window);

		//Clear all keyboard and mouse input events and mouse position values,
		//used internally to "forget" any mouse and keyboard events if window is unfocused
		static void ClearInputEvents();

		//Call at end of frame to reset pressed/released states
		static void EndFrameUpdate(Window* window);
	private:
		static inline bool isVerboseLoggingEnabled{};

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
		static inline bool keepMouseDelta = false;

		static inline vec2 mousePos = vec2{ 0.0f, 0.0f };
		static inline vec2 mouseDelta = vec2 { 0.0f, 0.0f };
		static inline vec2 rawMouseDelta = vec2{ 0.0f, 0.0f };

		static inline float mouseWheelDelta = 0.0f;
	};
}