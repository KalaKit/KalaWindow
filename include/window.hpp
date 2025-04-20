//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <memory>

//kalawindow
#include "platform.hpp"
#include "enums.hpp"

namespace KalaKit
{
	using std::string;
	using std::unique_ptr;

	class KALAWINDOW_API KalaWindow
	{
	public:
		/// <summary>
		/// Title of the warning popup when user wants to exit
		/// </summary>
		static inline string exitTitle = "Closing program";
		/// <summary>
		/// Description of the warning popup when user wants to exit
		/// </summary>
		static inline string exitInfo = "Do you want to exit? You might lose unsaved data.";

		/// <summary>
		/// Initializes the window system, input system, creates an opengl context
		/// and loads all the opengl functions and creates a window with the given parameters.
		/// Returns true if the window was successfully created. If you set initializeOpenGL
		/// to false then OpenGL and OpenGL functions will not be loaded and you will
		/// only get the most basic window available with no support for rendering.
		/// </summary>
		static bool Initialize(
			const string& title, 
			int width, 
			int height,
			bool initializeOpenGL = true);

		/// <summary>
		/// Handles all input at runtime. 
		/// </summary>
		static void Update();

		/// <summary>
		/// Cross-platform buffer swapper.
		/// </summary>
		static void SwapOpenGLBuffers();

		using RedrawCallback = void(*)();
		static inline RedrawCallback OnRedraw = nullptr;
		/// <summary>
		/// Sets a custom function to be called during WM_TIMER events.
		/// Use this to re-render the window to prevent artifacts and black boxes while resizing.
		/// </summary>
		static inline void SetRedrawCallback(RedrawCallback callback)
		{
			OnRedraw = callback;
		}

		/// <summary>
		/// Should be used in the update loop. When this returns false
		/// then the main window will be shut off if there is nothing
		/// called after it. Place the shutdown function after this
		/// if you want to control shutdown mechanics afterwards.
		/// </summary>
		static bool ShouldClose();

		/// <summary>
		/// Used for setting window focus required state.
		/// If true, then the assigned window needs to be focused
		/// for any input to be able to be passed to it.
		/// </summary>
		/// <param name="newWindowFocusedState"></param>
		static void SetWindowFocusRequiredState(bool newWindowFocusRequiredState);

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
		static kvec2 GetWindowPosition();
		/// <summary>
		/// Sets the position of the window.
		/// </summary>
		static void SetWindowPosition(int width, int height);

		/// <summary>
		/// Gets the total outer size (includes borders and top bar)
		/// </summary>
		static kvec2 GetWindowFullSize();
		/// <summary>
		/// Sets the total outer size of the window (includes borders and top bar)
		/// </summary>
		static void SetWindowFullSize(int width, int height);

		/// <summary>
		/// Gets the drawable/client area (without borders and top bar)
		/// </summary>
		static kvec2 GetWindowContentSize();
		/// <summary>
		/// Sets the drawable/client area (without borders and top bar)
		/// </summary>
		static void SetWindowContentSize(int width, int height);
	
		/// <summary>
		/// Get window maximum allowed size.
		/// </summary>
		static kvec2 GetWindowMaxSize();
		/// <summary>
		/// Get window minimum allowed size.
		/// </summary>
		static kvec2 GetWindowMinSize();
		/// <summary>
		/// Set window minimum and maximum allowed size.
		/// </summary>
		static void SetMinMaxSize(
			int maxWidth, 
			int maxHeight, 
			int minWidth, 
			int minHeight);

		/// <summary>
		/// Controls whether the developer wants to show the warning
		/// popup when user wants to close the program window.
		/// </summary>
		static bool CanExit();

		/// <summary>
		/// Controls whether the while loop should end or not.
		/// If true, then the while loop will end.
		/// </summary>
		static void SetShouldCloseState(bool newShouldCloseState);

		/// <summary>
		/// Create a popup with any supported action and type with custom title and message.
		/// </summary>
		static PopupResult CreatePopup(
			const string& title,
			const string& message,
			PopupAction action,
			PopupType type);

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
		/// Used for checking whether window should be focused or not 
		/// to enable any input to be registered.
		/// </summary>
		static inline bool isWindowFocusRequired;

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

#ifdef KALAKIT_WINDOWS
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
#endif

		static inline int maxWidth = 7680;
		static inline int maxHeight = 4320;
		static inline int minWidth = 800;
		static inline int minHeight = 600;

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