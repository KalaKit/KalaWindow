//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <array>
#include <vector>
#include <span>
#include <string>

#include "core_utils.hpp"
#include "math_utils.hpp"
#include "key_standards.hpp"

#include "core/kw_registry.hpp"

namespace KalaWindow::Graphics
{
	class ProcessWindow;
	class Window_Global;
}

namespace KalaWindow::Core
{
	using std::array;
	using std::vector;
	using std::fill;
	using std::prev;
	using std::span;
	using std::string;
	using std::string_view;
	using std::default_delete;

	using KalaHeaders::KalaMath::vec2;
	using KalaHeaders::KalaKeyStandards::KeyboardButton;
	using KalaHeaders::KalaKeyStandards::MouseButton;
	using KalaHeaders::KalaKeyStandards::keyboardButtons;
	using KalaHeaders::KalaKeyStandards::mouseButtons;

	struct InputCode
	{
		KeyboardButton kb{};
		MouseButton mb{};
	};

	class LIB_API Input
	{
	friend class KalaWindow::Graphics::ProcessWindow;
	friend class KalaWindow::Graphics::Window_Global;
	friend class MessageLoop;
	friend struct default_delete<Input>;
	public:
		KNODISCARD
		static KalaWindowRegistry<Input>& GetRegistry();

		//Toggle verbose logging. If true, then usually frequently updated runtime values like
		//key, mouse update messages will dump their logs into the console.
		KNODISCARD
		static bool IsVerboseLoggingEnabled();
		static void SetVerboseLoggingState(bool newState);

		KNODISCARD
		u32 GetID() const;
		KNODISCARD
		u32 GetWindowID() const;

		//Get the letter that was typed this frame
		KNODISCARD
		const string& GetTypedLetter() const;
		
		//Get the keys currently pressed this frame
		KNODISCARD
		vector<KeyboardButton> GetPressedKeys();
		//Get the keys currently held this frame
		KNODISCARD
		vector<KeyboardButton> GetHeldKeys();
		//Get the keys released this frame
		KNODISCARD
		vector<KeyboardButton> GetReleasedKeys();
		
		//Get the mouse buttons currently pressed this frame
		KNODISCARD
		vector<MouseButton> GetPressedMouseButtons();
		//Get the mouse buttons currently held this frame
		KNODISCARD
		vector<MouseButton> GetHeldMouseButtons();
		//Get the mouse buttons released this frame
		KNODISCARD
		vector<MouseButton> GetReleasedMouseButtons();
		//Get the mouse buttons double-clicked this frame
		KNODISCARD
		vector<MouseButton> GetDoubleClickedMouseButtons();
		//Get the mouse buttons dragged this frame
		KNODISCARD
		vector<MouseButton> GetDraggingMouseButtons();

		//Detect if any combination of keys and mouse buttons are down
		KNODISCARD
		bool IsComboDown(const span<const InputCode>& codes);
		//Detect if any combination of keys and mouse buttons are pressed
		KNODISCARD
		bool IsComboPressed(const span<const InputCode>& codes);
		//Detect if any combination of keys and mouse buttons are released
		KNODISCARD
		bool IsComboReleased(const span<const InputCode>& codes);

		//Is the key currently held down
		KNODISCARD
		bool IsKeyHeld(KeyboardButton key);
		//Was the key just pressed this frame
		KNODISCARD
		bool IsKeyPressed(KeyboardButton key);
		//Was the key just released this frame
		KNODISCARD
		bool IsKeyReleased(KeyboardButton key);

		//Is the mouse button currently held down
		KNODISCARD
		bool IsMouseButtonHeld(MouseButton mouseButton);
		//Was the mouse button just pressed this frame
		KNODISCARD
		bool IsMouseButtonPressed(MouseButton mouseButton);
		//Was the mouse button just released this frame
		KNODISCARD
		bool IsMouseButtonReleased(MouseButton mouseButton);

		//Was the mouse button just double-clicked this frame
		KNODISCARD
		bool IsMouseButtonDoubleClicked(MouseButton mouseButton);

		//Is the mouse button currently dragging
		KNODISCARD
		bool IsMouseButtonDragging(MouseButton mouseButton);

		//Get current mouse position in window coordinates
		KNODISCARD
		vec2 GetMousePosition() const;
		//Get mouse delta movement since last frame
		KNODISCARD
		vec2 GetMouseDelta();
		//Get mouse raw delta movement since last frame
		KNODISCARD
		vec2 GetRawMouseDelta();
		//Get vertical scroll wheel delta (-1 to +1)
		KNODISCARD
		f32 GetScrollwheelDelta() const;

		//Return true if cursor is not hidden.
		KNODISCARD
		bool IsMouseVisible() const;
		//Allows to set the visibility state of the cursor, if true then the cursor is visible,
		//if updateBetweenFocus is true, then mouse visibility is disabled when unfocused without clearing internal flag
		void SetMouseVisibility(
			bool newState,
			bool updateBetweenFocus = true);

		//Return true if the cursor is locked to the center of the window.
		KNODISCARD
		bool IsMouseLocked() const;
		//Allows to set the lock state of the cursor, if true 
		//then the cursor is locked to the center of the window,
		//if updateBetweenFocus is true, then mouse lock is disabled when unfocused without clearing internal flag
		void SetMouseLockState(
			bool newState,
			bool updateBetweenFocus = true);

		//If true, then mouse delta, raw delta and scroll delta wont be reset per frame.
		KNODISCARD
		bool GetKeepMouseDeltaState() const;
		void SetKeepMouseDeltaState(bool newState);
	private:
		~Input();

		void Destroy();

		KNODISCARD
		static Input* Initialize(u32 windowID);

		void SetTypedLetter(string_view letter);

		void SetKeyState(
			KeyboardButton key,
			bool isDown);
		void SetMouseButtonState(
			MouseButton mouseButton,
			bool isDown);
		void SetMouseButtonDoubleClickState(
			MouseButton mouseButton,
			bool isDown);

		void ClearInputEvents(bool clearHeld = false);

		void EndFrameUpdate();

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

		f32 mouseWheelDelta = 0.0f;
	};
}