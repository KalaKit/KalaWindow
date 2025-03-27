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

#include "enums.hpp"

namespace KalaKit
{
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
		/// Get the currently assigned debug type.
		/// </summary>
		/// <returns></returns>
		static DebugType GetDebugType();
		/// <summary>
		/// Used for printing all input actions or specific ones 
		/// to console with cout if a console is attached to the window.
		/// You MUST be in Debug mode or else these messages will not be printed.
		/// </summary>
		/// <param name="newDebugType">Sets what debug message types will be printed to console.</param>
		static void SetDebugType(DebugType newDebugType);

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
		static inline WNDPROC proc;

		/// <summary>
		/// Currently assigned debug type
		/// </summary>
		static inline DebugType debugType = DebugType::DEBUG_NONE;

		/// <summary>
		/// Convert window state enum to string with magic enum.
		/// </summary>
		static string ToString(WindowState state);
	};
}