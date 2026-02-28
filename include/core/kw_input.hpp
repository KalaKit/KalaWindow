//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <array>
#include <vector>
#include <span>
#include <string>

#include "KalaHeaders/core_utils.hpp"
#include "KalaHeaders/math_utils.hpp"
#include "KalaHeaders/key_standards.hpp"

#include "core/kw_registry.hpp"

namespace KalaWindow::Core
{
	using std::array;
	using std::vector;
	using std::fill;
	using std::prev;
	using std::span;
	using std::string;

	using KalaHeaders::KalaMath::vec2;
	using KalaHeaders::KalaKeyStandards::KeyboardButton;
	using KalaHeaders::KalaKeyStandards::MouseButton;
	using KalaHeaders::KalaKeyStandards::keyboardButtons;
	using KalaHeaders::KalaKeyStandards::mouseButtons;

	struct InputCode
	{
		enum class Type : u8
		{
			Key,
			Mouse
		};

		Type type{};
		u32 code{}; //stores key or mouse button as u32

		static inline constexpr InputCode FromKey(KeyboardButton k)
		{ 
			return 
			{ 
				Type::Key, 
				scast<u32>(k) 
			}; 
		}
		static inline constexpr InputCode FromMouse(MouseButton k)
		{ 
			return 
			{ 
				Type::Mouse, 
				scast<u32>(k) 
			}; 
		}
	};

	class LIB_API Input
	{
	public:
		static KalaWindowRegistry<Input>& GetRegistry();

		//Toggle verbose logging. If true, then usually frequently updated runtime values like
		//key, mouse update messages will dump their logs into the console.
		static bool IsVerboseLoggingEnabled();
		static void SetVerboseLoggingState(bool newState);

		static Input* Initialize(u32 windowID);

		bool IsInitialized() const;

		u32 GetID() const;
		u32 GetWindowID() const;


		//Get the letter that was typed this frame
		const string& GetTypedLetter() const;
		void SetTypedLetter(const string& letter);

		void SetKeyState(
			KeyboardButton key,
			bool isDown);
		void SetMouseButtonState(
			MouseButton mouseButton,
			bool isDown);
		void SetMouseButtonDoubleClickState(
			MouseButton mouseButton,
			bool isDown);
		
		//Get the keys currently pressed this frame
		vector<KeyboardButton> GetPressedKeys();
		//Get the keys currently held this frame
		vector<KeyboardButton> GetHeldKeys();
		//Get the keys released this frame
		vector<KeyboardButton> GetReleasedKeys();
		
		//Get the mouse buttons currently pressed this frame
		vector<MouseButton> GetPressedMouseButtons();
		//Get the mouse buttons currently held this frame
		vector<MouseButton> GetHeldMouseButtons();
		//Get the mouse buttons released this frame
		vector<MouseButton> GetReleasedMouseButtons();
		//Get the mouse buttons double-clicked this frame
		vector<MouseButton> GetDoubleClickedMouseButtons();

		//Detect if any combination of keys and mouse buttons are down
		bool IsComboDown(const span<const InputCode>& codes);
		//Detect if any combination of keys and mouse buttons are pressed
		bool IsComboPressed(const span<const InputCode>& codes);
		//Detect if any combination of keys and mouse buttons are released
		bool IsComboReleased(const span<const InputCode>& codes);

		//Is the key currently held down
		bool IsKeyHeld(KeyboardButton key);
		//Was the key just pressed this frame
		bool IsKeyPressed(KeyboardButton key);
		//Was the key just released this frame
		bool IsKeyReleased(KeyboardButton key);

		//Is the mouse button currently held down
		bool IsMouseButtonHeld(MouseButton mouseButton);
		//Was the mouse button just pressed this frame
		bool IsMouseButtonPressed(MouseButton mouseButton);
		//Was the mouse button just released this frame
		bool IsMouseButtonReleased(MouseButton mouseButton);

		//Was the mouse button just double-clicked this frame
		bool IsMouseButtonDoubleClicked(MouseButton mouseButton);

		//Is the mouse button currently dragging
		bool IsMouseButtonDragging(MouseButton mouseButton);

		//Get current mouse position in window coordinates
		vec2 GetMousePosition() const;
		void SetMousePosition(vec2 newMousePos);

		//Get mouse delta movement since last frame
		vec2 GetMouseDelta();
		void SetMouseDelta(vec2 newMouseDelta);

		//Get mouse raw delta movement since last frame
		vec2 GetRawMouseDelta();
		void SetRawMouseDelta(vec2 newRawMouseDelta);

		//Get vertical scroll wheel delta (-1 to +1)
		float GetScrollwheelDelta() const;
		void SetScrollwheelDelta(float delta);

		//Return true if cursor is not hidden.
		bool IsMouseVisible() const;
		//Allows to set the visibility state of the cursor, if true then the cursor is visible.
		void SetMouseVisibility(bool isVisible);

		//Return true if the cursor is locked to the center of the window.
		bool IsMouseLocked() const;
		//Allows to set the lock state of the cursor, if true 
		//then the cursor is locked to the center of the window.
		void SetMouseLockState(bool newState);

		//If true, then mouse delta, raw delta and scroll delta wont be reset per frame.
		bool GetKeepMouseDeltaState() const;
		void SetKeepMouseDeltaState(bool newState);

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
		bool isInitialized{};

		u32 ID{};
		u32 windowID{};

		string lastLetter{};

		array<
			bool,
			keyboardButtons.size()>
			keyDown{};
		array<
			bool,
			keyboardButtons.size()>
			keyPressed{};
		array<
			bool,
			keyboardButtons.size()>
			keyReleased{};

		array<
			bool,
			mouseButtons.size()>
			mouseDown{};
		array<
			bool,
			mouseButtons.size()>
			mousePressed{};
		array<
			bool,
			mouseButtons.size()>
			mouseReleased{};
		array<
			bool,
			mouseButtons.size()>
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