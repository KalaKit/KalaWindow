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
#ifdef _WIN32
#include <Windows.h>
#endif

namespace KalaKit
{
	using std::unordered_map;
	using std::cout;
	using std::initializer_list;
	using std::string;

	enum class Key
	{
		// Letters
		A = 'A', B = 'B', C = 'C', D = 'D', E = 'E',
		F = 'F', G = 'G', H = 'H', I = 'I', J = 'J',
		K = 'K', L = 'L', M = 'M', N = 'N', O = 'O',
		P = 'P', Q = 'Q', R = 'R', S = 'S', T = 'T',
		U = 'U', V = 'V', W = 'W', X = 'X', Y = 'Y', Z = 'Z',

		// Numbers
		Num0 = '0', Num1 = '1', Num2 = '2', Num3 = '3', Num4 = '4',
		Num5 = '5', Num6 = '6', Num7 = '7', Num8 = '8', Num9 = '9',

		// Symbols and punctuation (OEM)
		Semicolon    = VK_OEM_1,      // ;:
		Equal        = VK_OEM_PLUS,   // =+
		Comma        = VK_OEM_COMMA,  // ,<
		Minus        = VK_OEM_MINUS,  // -_
		Period       = VK_OEM_PERIOD, // .>
		Slash        = VK_OEM_2,      // /?
		Backtick     = VK_OEM_3,      // `~
		BracketLeft  = VK_OEM_4,      // [{
		Backslash    = VK_OEM_5,      // \|
		BracketRight = VK_OEM_6,      // ]}
		Apostrophe   = VK_OEM_7,      // '"
		Oem102       = VK_OEM_102,    // <> or \ (ISO)

		// Numpad
		Numpad0        = VK_NUMPAD0,
		Numpad1        = VK_NUMPAD1,
		Numpad2        = VK_NUMPAD2,
		Numpad3        = VK_NUMPAD3,
		Numpad4        = VK_NUMPAD4,
		Numpad5        = VK_NUMPAD5,
		Numpad6        = VK_NUMPAD6,
		Numpad7        = VK_NUMPAD7,
		Numpad8        = VK_NUMPAD8,
		Numpad9        = VK_NUMPAD9,
		NumpadAdd      = VK_ADD,
		NumpadSubtract = VK_SUBTRACT,
		NumpadMultiply = VK_MULTIPLY,
		NumpadDivide   = VK_DIVIDE,
		NumpadDecimal  = VK_DECIMAL,
		NumLock        = VK_NUMLOCK,

		// Function keys
		F1 = VK_F1, F2 = VK_F2, F3 = VK_F3, F4 = VK_F4,
		F5 = VK_F5, F6 = VK_F6, F7 = VK_F7, F8 = VK_F8,
		F9 = VK_F9, F10 = VK_F10, F11 = VK_F11, F12 = VK_F12,
		F13 = VK_F13, F14 = VK_F14, F15 = VK_F15, F16 = VK_F16,
		F17 = VK_F17, F18 = VK_F18, F19 = VK_F19, F20 = VK_F20,
		F21 = VK_F21, F22 = VK_F22, F23 = VK_F23, F24 = VK_F24,

		// Control / navigation
		Escape    = VK_ESCAPE,
		Enter     = VK_RETURN,
		Tab       = VK_TAB,
		Backspace = VK_BACK,
		Insert    = VK_INSERT,
		Delete    = VK_DELETE,
		Home      = VK_HOME,
		End       = VK_END,
		PageUp    = VK_PRIOR,
		PageDown  = VK_NEXT,

		// Modifier keys
		LeftShift    = VK_LSHIFT,
		RightShift   = VK_RSHIFT,
		LeftControl  = VK_LCONTROL,
		RightControl = VK_RCONTROL,
		LeftAlt      = VK_LMENU,
		RightAlt     = VK_RMENU,
		CapsLock     = VK_CAPITAL,

		// System keys
		PrintScreen = VK_SNAPSHOT,
		ScrollLock  = VK_SCROLL,
		Pause       = VK_PAUSE,
		Menu        = VK_APPS,

		// Arrow keys
		Up    = VK_UP,
		Down  = VK_DOWN,
		Left  = VK_LEFT,
		Right = VK_RIGHT,

		// Spacebar
		Space = VK_SPACE,

		// Mouse buttons
		MouseLeft   = VK_LBUTTON,
		MouseRight  = VK_RBUTTON,
		MouseMiddle = VK_MBUTTON,
		MouseX1     = VK_XBUTTON1,
		MouseX2     = VK_XBUTTON2,

		// Extended mouse buttons
		MouseX3  = 0x100,
		MouseX4  = 0x101,
		MouseX5  = 0x102,
		MouseX6  = 0x103,
		MouseX7  = 0x104,
		MouseX8  = 0x105,
		MouseX9  = 0x106,
		MouseX10 = 0x107,

		// Media & browser keys
		MediaPlayPause   = VK_MEDIA_PLAY_PAUSE,
		MediaStop        = VK_MEDIA_STOP,
		MediaNextTrack   = VK_MEDIA_NEXT_TRACK,
		MediaPrevTrack   = VK_MEDIA_PREV_TRACK,
		VolumeUp         = VK_VOLUME_UP,
		VolumeDown       = VK_VOLUME_DOWN,
		VolumeMute       = VK_VOLUME_MUTE,
		LaunchMail       = VK_LAUNCH_MAIL,
		LaunchApp1       = VK_LAUNCH_APP1,
		LaunchApp2       = VK_LAUNCH_APP2,
		BrowserBack      = VK_BROWSER_BACK,
		BrowserForward   = VK_BROWSER_FORWARD,
		BrowserRefresh   = VK_BROWSER_REFRESH,
		BrowserStop      = VK_BROWSER_STOP,
		BrowserSearch    = VK_BROWSER_SEARCH,
		BrowserFavorites = VK_BROWSER_FAVORITES,
		BrowserHome      = VK_BROWSER_HOME
	};

	/// <summary>
	/// Debug message type printed to console. These are usable only if 
	/// your program is in Debug mode and most of these, except
	/// DEBUG_NONE, DEBUG_PROCESS_MESSAGE_TEST and DEBUG_ALL
	/// requires that one of its function type is assigned somewhere
	/// in your program code for them to actually return something.
	/// </summary>
	enum class DebugType
	{
		DEBUG_NONE,                    //Default option, assigning this does nothing
		DEBUG_KEY_HELD,                //Print key held updates (requires IsKeyDown)
		DEBUG_KEY_PRESSED,             //Print key pressed updates (requires WasKeyPressed)
		DEBUG_COMBO_PRESSED,           //Print combo pressed updates (requires WasComboPressed)
		DEBUG_DOUBLE_CLICKED,          //Print mouse double click updates (requires WasDoubleClicked)
		DEBUG_IS_MOUSE_DRAGGING,       //Print mouse dragging updates (requires IsMouseDragging)
		DEBUG_MOUSE_POSITION,          //Print mouse position updates (requires GetMousePosition)
		DEBUG_MOUSE_DELTA,             //Print regular mouse delta updates (requires GetMouseDelta)
		DEBUG_RAW_MOUSE_DELTA,         //Print raw mouse delta updates (requires GetRawMouseDelta)
		DEBUG_MOUSE_WHEEL_DELTA,       //Print scroll wheel updates (requires GetMouseWheelDelta)
		DEBUG_MOUSE_VISIBILITY,        //Print scroll mouse visibility updates (requires SetMouseVisibility)
		DEBUG_MOUSE_LOCK_STATE,        //Print scroll lock state updates (requires SetMouseLockState)
		DEBUG_WINDOW_SHOULD_CLOSE,     //Print should close state updates
		DEBUG_PROCESS_MESSAGE_TEST,    //Print all processing messages user input sends
		DEBUG_ALL                      //Print ALL debug updates
	};

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
		/// Used for printing all input actions or specific ones 
		/// to console with cout if a console is attached to the window.
		/// You MUST be in Debug mode or else these messages will not be printed.
		/// </summary>
		/// <param name="newDebugType">Sets what debug message types will be printed to console.</param>
		static void SetDebugState(DebugType newDebugType);

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
			const string& title = "NO TITLE",
			const string& info = "NO INFO");

		/// <summary>
		/// Allows to set the close state of the window. If true, then the program will close.
		/// </summary>
		/// <param name="newShouldCloseState"></param>
		static void SetShouldCloseState(bool newShouldCloseState);

#ifdef _WIN32
		/// <summary>
		/// Used for manually passing a window 
		/// reference to this Input System library.
		/// </summary>
		static void SetWindow(HWND newWindow);
#endif
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
		static inline bool canExit;

		/// <summary>
		/// Title of the warning popup when user wants to exit
		/// </summary>
		static inline string exitTitle = "NO TITLE";
		/// <summary>
		/// Description of the warning popup when user wants to exit
		/// </summary>
		static inline string exitInfo = "NO INFO";

		/// <summary>
		/// Used for checking whether window should be focused or not 
		/// to enable any input to be registered.
		/// </summary>
		static inline bool isWindowFocusRequired;

#ifdef _WIN32
		static inline HWND window;

		/// <summary>
		/// Whether to show the mouse or not.
		/// </summary>
		static inline bool isMouseVisible = true;
		/// <summary>
		/// Whether to lock the mouse to the center of the screen or not.
		/// </summary>
		static inline bool isMouseLocked = false;

		/// <summary>
		/// Currently assigned debug type
		/// </summary>
		static inline DebugType debugType = DebugType::DEBUG_NONE;

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

		/// <summary>
		/// Resets all frame-specific variables to defaults.
		/// </summary>
		static void ResetFrameInput();

		static BOOL CALLBACK EnumWindowsCallback(HWND hwnd, LPARAM lParam);

		static HWND GetMainWindowHandle();

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
		static void ProcessMessage(const MSG& msg);

		/// <summary>
		/// Convert key enum to string with magic enum.
		/// </summary>
		static string ToString(Key key);
#endif
	};
}