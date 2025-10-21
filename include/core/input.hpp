//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <array>
#include <span>
#include <string>

#include "KalaHeaders/core_utils.hpp"

#include "core/glm_global.hpp"
#include "core/registry.hpp"

namespace KalaWindow::Core
{
	using std::array;
	using std::fill;
	using std::prev;
	using std::span;
	using std::string;

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
		static inline Registry<Input> registry{};

		static Input* Initialize(u32 windowID);

		inline bool IsInitialized() const { return isInitialized; }

		inline u32 GetID() const { return ID; }
		inline u32 GetWindowID() const { return windowID; }

		//Toggle verbose logging. If true, then usually frequently updated runtime values like
		//key, mouse update messages will dump their logs into the console.
		static inline bool IsVerboseLoggingEnabled() { return isVerboseLoggingEnabled; }
		static inline void SetVerboseLoggingState(bool newState) { isVerboseLoggingEnabled = newState; }

		//Get the letter that was typed this frame
		inline const string& GetTypedLetter() const { return lastLetter; }
		inline void SetTypedLetter(const string& letter) { lastLetter = letter; }

		inline void SetKeyState(
			Key key,
			bool isDown)
		{
			if (key == Key::KeyCount) return;

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
		inline void SetMouseButtonState(
			MouseButton mouseButton,
			bool isDown)
		{
			if (mouseButton == MouseButton::MouseButtonCount) return;

			size_t index = static_cast<size_t>(mouseButton);

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
		inline void SetMouseButtonDoubleClickState(
			MouseButton mouseButton,
			bool isDown)
		{
			if (mouseButton == MouseButton::MouseButtonCount) return;

			mouseDoubleClicked[static_cast<size_t>(mouseButton)] = isDown;
		}

		//Detect if any combination of keys and mouse buttons are down
		bool IsComboDown(const span<const InputCode>& codes);
		//Detect if any combination of keys and mouse buttons are pressed
		bool IsComboPressed(const span<const InputCode>& codes);
		//Detect if any combination of keys and mouse buttons are released
		bool IsComboReleased(const span<const InputCode>& codes);

		//Is the key currently held down
		inline bool IsKeyHeld(Key key) { return keyDown[static_cast<size_t>(key)]; }
		//Was the key just pressed this frame
		inline bool IsKeyPressed(Key key) { return keyPressed[static_cast<size_t>(key)]; }
		//Was the key just released this frame
		inline bool IsKeyReleased(Key key) { return keyReleased[static_cast<size_t>(key)]; }

		//Is the mouse button currently held down
		inline bool IsMouseButtonHeld(MouseButton mouseButton) { return mouseDown[static_cast<size_t>(mouseButton)]; }
		//Was the mouse button just pressed this frame
		inline bool IsMouseButtonPressed(MouseButton mouseButton) { return mousePressed[static_cast<size_t>(mouseButton)]; }
		//Was the mouse button just released this frame
		inline bool IsMouseButtonReleased(MouseButton mouseButton) { return mouseReleased[static_cast<size_t>(mouseButton)]; }

		//Was the mouse button just double-clicked this frame
		inline bool IsMouseButtonDoubleClicked(MouseButton mouseButton) { return mouseDoubleClicked[static_cast<size_t>(mouseButton)]; }

		//Is the mouse button currently dragging
		inline bool IsMouseButtonDragging(MouseButton mouseButton)
		{
			bool isHoldingDragKey = IsMouseButtonHeld(mouseButton);

			bool isDragging =
				isHoldingDragKey
				&& (mouseDelta.x != 0
				|| mouseDelta.y != 0);

			return isDragging;
		}

		//Get current mouse position in window coordinates
		inline vec2 GetMousePosition() const { return mousePos; }
		inline void SetMousePosition(vec2 newMousePos) { mousePos = newMousePos; }

		//Get mouse delta movement since last frame
		inline vec2 GetMouseDelta()
		{
			vec2 currMouseDelta = mouseDelta;

			//reset after retrieval for per-frame delta behavior
			mouseDelta = vec2{ 0.0f, 0.0f };

			return currMouseDelta;
		}
		inline void SetMouseDelta(vec2 newMouseDelta) { mouseDelta = newMouseDelta; }

		//Get mouse raw delta movement since last frame
		inline vec2 GetRawMouseDelta()
		{
			vec2 currMouseDelta = rawMouseDelta;

			//reset after retrieval for per-frame delta behavior
			rawMouseDelta = vec2{ 0.0f, 0.0f };

			return currMouseDelta;
		}
		inline void SetRawMouseDelta(vec2 newRawMouseDelta) { rawMouseDelta = newRawMouseDelta; }

		//Get vertical scroll wheel delta (-1 to +1)
		inline float GetScrollwheelDelta() const { return mouseWheelDelta; }
		inline void SetScrollwheelDelta(float delta) { mouseWheelDelta = delta; }

		//Return true if cursor is not hidden.
		inline bool IsMouseVisible() const { return isMouseVisible; }
		//Allows to set the visibility state of the cursor, if true then the cursor is visible.
		void SetMouseVisibility(bool isVisible);

		//Return true if the cursor is locked to the center of the window.
		inline bool IsMouseLocked() const { return isMouseLocked; }
		//Allows to set the lock state of the cursor, if true 
		//then the cursor is locked to the center of the window.
		void SetMouseLockState(bool newState);

		//If true, then mouse delta, raw delta and scroll delta wont be reset per frame.
		inline bool GetKeepMouseDeltaState() const { return keepMouseDelta; }
		inline void SetKeepMouseDeltaState(bool newState) { keepMouseDelta = newState; }

		//If true, then mouse visibility is disabled when unfocused without clearing internal flag
		void SetMouseVisibilityBetweenFocus(bool state) const;

		//If true, then mouse lock is disabled when unfocused without clearing internal flag
		void SetMouseLockStateBetweenFocus(bool state) const;

		//Clear all keyboard and mouse input events and mouse position values,
		//used internally to "forget" any mouse and keyboard events if window is unfocused
		void ClearInputEvents();

		//Call at end of frame to reset pressed/released states
		void EndFrameUpdate();

		~Input();
	private:
		static inline bool isVerboseLoggingEnabled{};

		bool isInitialized{};

		u32 ID{};
		u32 windowID{};

		string lastLetter{};

		array<
			bool,
			static_cast<size_t>(Key::KeyCount)>
			keyDown{};
		array<
			bool,
			static_cast<size_t>(Key::KeyCount)>
			keyPressed{};
		array<
			bool,
			static_cast<size_t>(Key::KeyCount)>
			keyReleased{};

		array<
			bool,
			static_cast<size_t>(MouseButton::MouseButtonCount)>
			mouseDown{};
		array<
			bool,
			static_cast<size_t>(MouseButton::MouseButtonCount)>
			mousePressed{};
		array<
			bool,
			static_cast<size_t>(MouseButton::MouseButtonCount)>
			mouseReleased{};
		array<
			bool,
			static_cast<size_t>(MouseButton::MouseButtonCount)>
			mouseDoubleClicked{};

		bool isMouseVisible = true;
		bool isMouseLocked = false;
		bool keepMouseDelta = false;

		vec2 mousePos = vec2{ 0.0f, 0.0f };
		vec2 mouseDelta = vec2{ 0.0f, 0.0f };
		vec2 rawMouseDelta = vec2{ 0.0f, 0.0f };

		float mouseWheelDelta = 0.0f;
	};
}