//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#ifdef _WIN32
	#ifdef KALAWINDOW_DLL_EXPORT
		#define KALAWINDOW_API __declspec(dllexport)
	#else
		#define KALAWINDOW_API __declspec(dllimport)
	#endif
#else
	#define KALAWINDOW_API
#endif

#include <iostream>
#include <unordered_map>
#include <initializer_list>
#include <string>
#include <Windows.h>

#include "enums.hpp"

namespace KalaKit
{
	using std::unordered_map;
	using std::cout;
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
		/// Handles all input at runtime. 
		/// </summary>
		static void Update();

		/// <summary>
		/// Used for setting window focus required state.
		/// If true, then the assigned window needs to be focused
		/// for any input to be able to be passed to it.
		/// </summary>
		/// <param name="newWindowFocusedState"></param>
		static void SetWindowFocusRequiredState(bool newWindowFocusRequiredState);
			
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
		static POINT GetMousePosition();

		/// <summary>
		/// Get how much the cursor moved on screen (in client space) since the last frame.
		/// This uses absolute screen-based movement, affected by OS acceleration and DPI.
		/// </summary>
		static POINT GetMouseDelta();

		/// <summary>
		/// Get raw, unfiltered mouse movement from the hardware since the last frame.
		/// Not affected by DPI, sensitivity, or OS mouse settings. Ideal for FPS camera control.
		/// </summary>
		static POINT GetRawMouseDelta();

		/// <summary>
		/// Get how many scroll steps the mouse wheel moved since the last frame.
		/// Positive = scroll up, Negative = scroll down
		/// </summary>
		static int GetMouseWheelDelta();

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
		/// Sets the exit state, if false then window will 
		/// ask user if they want to close or not, otherwise
		/// the window will continue with normal shutdown.
		/// Defaults to true, you should also assign the 
		/// title and info parameters so the popup 
		/// will show them when this is set to false 
		/// and the application is shut down.
		/// </summary>
		static void SetExitState(
			bool setExitAllowedState,
			const string& title,
			const string& info);

		/// <summary>
		/// Detects if the window should close or not.
		/// </summary>
		static bool ShouldClose();
		/// <summary>
		/// Allows to set the close state of the window. If true, then the program will close.
		/// </summary>
		/// <param name="newShouldCloseState"></param>
		static void SetShouldCloseState(bool newShouldCloseState);

		/// <summary>
		/// Returns the window reference that was assigned.
		/// </summary>
		/// <returns></returns>
		static HWND GetWindow();
		/// <summary>
		/// Used for manually passing a window 
		/// reference to this Input System library.
		/// </summary>
		static void SetWindow(HWND newWindow);
	private:
		static inline bool isInitialized;

		/// <summary>
		/// Very important - turning this to true closes the window.
		/// </summary>
		static inline bool shouldClose;

		/// <summary>
		/// If true, then the user can exit the program, if false 
		/// then the user needs to choose yes on the warning popup 
		/// before they can close the program.
		/// </summary>
		static inline bool canExit = true;

		/// <summary>
		/// Title of the warning popup when user wants to exit
		/// </summary>
		static inline string exitTitle = "Closing program";
		/// <summary>
		/// Description of the warning popup when user wants to exit
		/// </summary>
		static inline string exitInfo = "Do you want to exit?";

		/// <summary>
		/// Used for checking whether window should be focused or not 
		/// to enable any input to be registered.
		/// </summary>
		static inline bool isWindowFocusRequired;

		/// <summary>
		/// Whether to show the mouse or not.
		/// </summary>
		static inline bool isMouseVisible = true;
		/// <summary>
		/// Whether to lock the mouse to the center of the screen or not.
		/// </summary>
		static inline bool isMouseLocked = false;

		//Where the cursor is on screen or in window.
		static inline POINT mousePosition = { 0, 0 };
		//How much the cursor moved since the last frame.
		static inline POINT mouseDelta = { 0, 0 };
		//Raw, unfiltered mouse move since last frame.
		static inline POINT rawMouseDelta = { 0, 0 };

		//How many steps scrollwheel scrolled since last frame.
		static inline int mouseWheelDelta = 0;

		static inline unordered_map<Key, bool> keyHeld;
		static inline unordered_map<Key, bool> keyPressed;

		static inline HWND window;
		static inline WNDPROC proc;

		/// <summary>
		/// Resets all frame-specific variables to defaults.
		/// </summary>
		static void ResetFrameInput();

		static BOOL CALLBACK EnumWindowsCallback(HWND hwnd, LPARAM lParam);

		static HWND GetMainWindowHandle();

		static LRESULT CALLBACK WindowProcCallback(
			HWND hwnd,
			UINT msg,
			WPARAM wParam,
			LPARAM lParam);

		/// <summary>
		/// Locks cursor to the center of the window
		/// </summary>
		static void LockCursorToCenter();

		static void SetMouseKeyState(Key key, bool isDown)
		{
			keyHeld[key] = isDown;
			if (isDown) keyPressed[key] = true;
		}

		/// <summary>
		/// Warning popup that asks if user wants to close or not.
		/// </summary>
		/// <returns></returns>
		static bool AllowExit();

		/// <summary>
		/// Handle all inputs.
		/// </summary>
		static bool ProcessMessage(const MSG& msg);

		/// <summary>
		/// Convert key enum to string with magic enum.
		/// </summary>
		static string ToString(Key key);
	};
}