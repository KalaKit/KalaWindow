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

#include <Windows.h>
#include <string>

namespace KalaKit
{
	/// <summary>
	/// Debug message type printed to console. These are usable only if 
	/// your program is in Debug mode and most of these, except
	/// DEBUG_NONE, DEBUG_PROCESS_MESSAGE_TEST and DEBUG_ALL
	/// requires that one of its function type is assigned somewhere
	/// in your program code for them to actually return something.
	/// </summary>
	enum class WindowDebugType
	{
		DEBUG_NONE,                    //Default option, assigning this does nothing
		DEBUG_WINDOW_TITLE,            //Print window title change updates
		DEBUG_WINDOW_BORDERLESS_STATE, //Print window borderless state updates (requires SetWindowBorderlessState)
		DEBUG_WINDOW_HIDDEN_STATE,     //Print window hidden state updates (requires SetWindowHiddenState)
		DEBUG_WINDOW_SET_POSITION,     //Print window position updates (requires SetWindowPosition)
		DEBUG_WINDOW_SET_FULL_SIZE,    //Print window full size updates (requires SetWindowFullSize)
		DEBUG_WIŃDOW_SET_CONTENT_SIZE, //Print window content size updates (requires GetWindowContentSize)
		DEBUG_ALL                      //Print ALL debug updates
	};

	/// <summary>
	/// A state the window can be switched to from its current state.
	/// </summary>
	enum class WindowState
	{
		WINDOW_RESET,      //Reset window to default state
		WINDOW_MINIMIZED,  //Minimize window to taskbar
		WINDOW_MAXIMIZED   //Maximize window to full screen size
	};

	using std::string;

	class KALAWINDOW_API KalaWindow
	{
	public:
		/// <summary>
		/// Initializes the window system and creates a window with the given parameters.
		/// Returns true if the window was successfully created.
		/// </summary>
		static bool Initialize(const string& title, int width, int height);

		/// <summary>
		/// Used for printing all input actions or specific ones 
		/// to console with cout if a console is attached to the window.
		/// You MUST be in Debug mode or else these messages will not be printed.
		/// </summary>
		/// <param name="newDebugType">Sets what debug message types will be printed to console.</param>
		static void SetDebugState(WindowDebugType newDebugType);

		/// <summary>
		/// Assign a title to the window.
		/// </summary>
		static void SetWindowTitle(const string& title);

		/// <summary>
		/// Set the window to one of the possible states.
		/// </summary>
		static void SetWindowState(WindowState state);

		/// <summary>
		/// Return true if the window is borderless.
		/// </summary>
		static bool IsWindowBorderless();
		/// <summary>
		/// Allows to set the borderless state of the window,
		/// if true, then the window will be set to borderless.
		/// </summary>
		static void SetWindowBorderlessState(bool newBorderlessState);

		/// <summary>
		/// Return true if the window is hidden.
		/// </summary>
		static bool IsWindowHidden();
		/// <summary>
		/// Allows to set the hidden state of the window,
		/// if true, then the window will be set to hidden.
		/// </summary>
		static void SetWindowHiddenState(bool newWindowHiddenState);

		/// <summary>
		/// Gets the position of the window.
		/// </summary>
		static POINT GetWindowPosition();
		/// <summary>
		/// Sets the position of the window.
		/// </summary>
		static void SetWindowPosition(int width, int height);

		/// <summary>
		/// Gets the total outer size (includes borders and top bar)
		/// </summary>
		static POINT GetWindowFullSize();
		/// <summary>
		/// Sets the total outer size of the window (includes borders and top bar)
		/// </summary>
		static void SetWindowFullSize(int width, int height);

		/// <summary>
		/// Gets the drawable/client area (without borders and top bar)
		/// </summary>
		static POINT GetWindowContentSize();
		/// <summary>
		/// Sets the drawable/client area (without borders and top bar)
		/// </summary>
		static void SetWindowContentSize(int width, int height);
	private:
		static inline bool isInitialized;

		/// <summary>
		/// Used for checking if this window is visible to the user or not.
		/// If true, then this window is visible.
		/// </summary>
		static inline bool isWindowVisible = true;

		/// <summary>
		/// Used for checking if this window is borderless or not.
		/// If true, then this window is borderless.
		/// </summary>
		static inline bool isWindowBorderless = false;

		/// <summary>
		/// Store original window flags when switching 
		/// between borderless and non-borderless
		/// </summary>
		static inline LONG originalStyle = 0;
		/// <summary>
		/// Store original size, position and show state 
		/// when switching between borderless and non-borderless
		/// </summary>
		static inline WINDOWPLACEMENT originalPlacement = { sizeof(WINDOWPLACEMENT) };

		static inline HWND window;

		/// <summary>
		/// Currently assigned debug type
		/// </summary>
		static inline WindowDebugType debugType = WindowDebugType::DEBUG_NONE;

		/// <summary>
		/// Convert window state enum to string with magic enum.
		/// </summary>
		static string ToString(WindowState state);
	};
}