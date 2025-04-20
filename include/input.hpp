//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <iostream>
#include <unordered_map>
#include <initializer_list>
#include <string>
#ifdef KALAKIT_WINDOWS
#include <Windows.h>
#endif

//kalawindow
#include "platform.hpp"
#include "enums.hpp"

namespace KalaKit
{
	using std::unordered_map;
	using std::initializer_list;
	using std::string;

	class KALAWINDOW_API KalaInput
	{
	public:
		/// <summary>
		/// Initializes the input system by 
		/// automatically detecting the main window. 
		/// Call this after your window has been created.
		/// </summary>
		static void Initialize();
			
		/// <summary>
		/// Return true if assigned key is currently held down.
		/// </summary>
		static bool IsKeyHeld(Key key);
		/// <summary>
		/// Return true if assigned key was pressed.
		/// </summary>
		static bool IsKeyPressed(Key key);

		/// <summary>
		/// Return true after assigned initializer list of keys is held 
		/// up to last key in correct order, and if last key is pressed.
		/// Must have atleast two keys assigned.
		/// </summary>
		static bool IsComboPressed(initializer_list<Key> keys);

		/// <summary>
		/// Return true if left or right mouse button was double-clicked.
		/// </summary>
		static bool IsMouseKeyDoubleClicked();

		/// <summary>
		/// Return true if user is holding left or right mouse button and dragging mouse.
		/// </summary>
		static bool IsMouseDragging();

		/// <summary>
		/// Get current mouse position relative to the client area (top-left = 0,0).
		/// Coordinates are in pixels.
		/// </summary>
		static kvec2 GetMousePosition();
		/// <summary>
		/// Assign a new mouse position.
		/// </summary>
		static void SetMousePosition(kvec2 newMousePosition);

		/// <summary>
		/// Get how much the cursor moved on screen (in client space) since the last frame.
		/// This uses absolute screen-based movement, affected by OS acceleration and DPI.
		/// </summary>
		static kvec2 GetMouseDelta();
		/// <summary>
		/// Assign a new mouse delta.
		/// </summary>
		static void SetMouseDelta(kvec2 newMouseDelta);

		/// <summary>
		/// Get raw, unfiltered mouse movement from the hardware since the last frame.
		/// Not affected by DPI, sensitivity, or OS mouse settings. Ideal for FPS camera control.
		/// </summary>
		static kvec2 GetRawMouseDelta();
		/// <summary>
		/// Assign a new mouse raw mouse delta.
		/// </summary>
		static void SetRawMouseDelta(kvec2 newMouseRawDelta);

		/// <summary>
		/// Get how many scroll steps the mouse wheel moved since the last frame.
		/// Positive = scroll up, Negative = scroll down
		/// </summary>
		static int GetMouseWheelDelta();
		/// <summary>
		/// Assign a new mouse wheel delta.
		/// </summary>
		/// <param name="newMouseWheelDelta"></param>
		static void SetMouseWheelDelta(int newMouseWheelDelta);

		/// <summary>
		/// Return true if cursor is not hidden.
		/// </summary>
		static bool IsMouseVisible();
		/// <summary>
		/// Allows to set the visibility state of the cursor, if true then the cursor is visible.
		/// </summary>
		static void SetMouseVisibility(bool newMouseVisibleState);

		/// <summary>
		/// Return true if the cursor is locked to the center of the window.
		/// </summary>
		static bool IsMouseLocked();
		/// <summary>
		/// Allows to set the lock state of the cursor, if true 
		/// then the cursor is locked to the center of the window.
		/// </summary>
		static void SetMouseLockState(bool newLockedState);

		/// <summary>
		/// Locks cursor to the center of the window.
		/// Should not be called manually.
		/// </summary>
		static void LockCursorToCenter();

		/// <summary>
		/// Resets all frame-specific variables to defaults.
		/// Should not be called manually.
		/// </summary>
		static void ResetFrameInput();

		/// <summary>
		/// Used for setting a key state for any of the keys.
		/// Should not be called manually.
		/// </summary>
		/// <param name="key"></param>
		/// <param name="isDown"></param>
		static void SetKeyState(Key key, bool isDown)
		{
			keyHeld[key] = isDown;
			if (isDown) keyPressed[key] = true;
		}
	private:
		static inline bool isInitialized;

		/// <summary>
		/// Whether to show the mouse or not.
		/// </summary>
		static inline bool isMouseVisible = true;
		/// <summary>
		/// Whether to lock the mouse to the center of the screen or not.
		/// </summary>
		static inline bool isMouseLocked = false;

		//Where the cursor is on screen or in window.
		static inline kvec2 mousePosition = { 0, 0 };
		//How much the cursor moved since the last frame.
		static inline kvec2 mouseDelta = { 0, 0 };
		//Raw, unfiltered mouse move since last frame.
		static inline kvec2 rawMouseDelta = { 0, 0 };

		//How many steps scrollwheel scrolled since last frame.
		static inline int mouseWheelDelta = 0;

		static inline unordered_map<Key, bool> keyHeld;
		static inline unordered_map<Key, bool> keyPressed;

		/// <summary>
		/// Convert key enum to string with magic enum.
		/// </summary>
		static string ToString(Key key);
	};
}