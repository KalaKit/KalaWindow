//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <array>
#include <span>

#include "KalaHeaders/core_utils.hpp"

#include "core/glm_global.hpp"

namespace KalaWindow::Graphics 
{ 
	class Window;
}

namespace KalaWindow::Core
{
	using KalaWindow::Graphics::Window;

	using std::array;
	using std::fill;
	using std::prev;
	using std::span;

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
		static void Initialize(Window* window);
		static bool IsInitialized(Window* window);

		//Toggle verbose logging. If true, then usually frequently updated runtime values like
		//key, mouse update messages will dump their logs into the console.
		static inline void SetVerboseLoggingState(bool newState) { isVerboseLoggingEnabled = newState; }
		static inline bool IsVerboseLoggingEnabled() { return isVerboseLoggingEnabled; }

		static void SetKeyState(
			Window* window,
			Key key,
			bool isDown);
		static void SetMouseButtonState(
			Window* window,
			MouseButton button,
			bool isDown);
		static void SetMouseButtonDoubleClickState(
			Window* window,
			MouseButton button,
			bool isDown);

		//Detect if any combination of keys and mouse buttons are down
		static bool IsComboDown(
			Window* window,
			const span<const InputCode>& codes);
		//Detect if any combination of keys and mouse buttons are pressed
		static bool IsComboPressed(
			Window* window,
			const span<const InputCode>& codes);
		//Detect if any combination of keys and mouse buttons are released
		static bool IsComboReleased(
			Window* window,
			const span<const InputCode>& codes);

		//Is the key currently held down?
		static bool IsKeyDown(
			Window* window,
			Key key);
		//Was the key just pressed this frame?
		static bool IsKeyPressed(
			Window* window,
			Key key);
		//Was the key just released this frame?
		static bool IsKeyReleased(
			Window* window,
			Key key);

		//Is the mouse button currently held down?
		static bool IsMouseDown(
			Window* window,
			MouseButton button);
		//Was the mouse button just pressed this frame?
		static bool IsMousePressed(
			Window* window,
			MouseButton button);
		//Was the mouse button just released this frame?
		static bool IsMouseReleased(
			Window* window,
			MouseButton button);

		//Was the mouse button just double-clicked this frame?
		static bool IsMouseButtonDoubleClicked(
			Window* window,
			MouseButton button);

		//Get current mouse position in window coordinates
		static vec2 GetMousePosition(Window* window);
		static void SetMousePosition(
			Window* window,
			vec2 newMousePos);

		//Get mouse delta movement since last frame
		static vec2 GetMouseDelta(Window* window);
		static void SetMouseDelta(
			Window* window,
			vec2 newMouseDelta);

		//Get mouse raw delta movement since last frame
		static vec2 GetRawMouseDelta(Window* window);
		static void SetRawMouseDelta(
			Window* window,
			vec2 newRawMouseDelta);

		//Get vertical scroll wheel delta (-1 to +1)
		static float GetMouseWheelDelta(Window* window);
		static void SetMouseWheelDelta(
			Window* window,
			float delta);

		static bool IsMouseDragging(Window* window);

		//Return true if cursor is not hidden.
		static bool IsMouseVisible(Window* window);
		//Allows to set the visibility state of the cursor, if true then the cursor is visible.
		static void SetMouseVisibility(
			Window* window,
			bool isVisible);

		//Return true if the cursor is locked to the center of the window.
		static bool IsMouseLocked(Window* window);
		//Allows to set the lock state of the cursor, if true 
		//then the cursor is locked to the center of the window.
		static void SetMouseLockState(
			Window* window,
			bool newState);

		//If true, then mouse delta, raw delta and scroll delta wont be reset per frame.
		static bool GetKeepMouseDeltaState(Window* window);
		static void SetKeepMouseDeltaState(
			Window* window,
			bool newState);

		//If true, then mouse visibility is disabled when unfocused without clearing internal flag
		static void SetMouseVisibilityBetweenFocus(
			Window* window,
			bool state);

		//If true, then mouse lock is disabled when unfocused without clearing internal flag
		static void SetMouseLockStateBetweenFocus(
			Window* window,
			bool state);

		//Clear all keyboard and mouse input events and mouse position values,
		//used internally to "forget" any mouse and keyboard events if window is unfocused
		static void ClearInputEvents(Window* window);

		//Call at end of frame to reset pressed/released states
		static void EndFrameUpdate(Window* window);
	private:
		static inline bool isVerboseLoggingEnabled{};
	};
}