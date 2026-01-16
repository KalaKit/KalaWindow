//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <functional>
#include <vector>
#include <array>

#include "KalaHeaders/core_utils.hpp"
#include "KalaHeaders/math_utils.hpp"

#include "core/kw_registry.hpp"

namespace KalaWindow::Graphics
{
	using std::string;
	using std::function;
	using std::vector;
	using std::array;

	using KalaHeaders::KalaMath::vec2;

	using KalaWindow::Core::KalaWindowRegistry;

	//Display mode / monitor ownership state
	enum class WindowMode
	{
		//Window will float and can be resized and moved around
		WINDOWMODE_WINDOWED,
		//Window will go fullscreen and will hide decorations and top bar,
		//stays alt-tab friendly and uses the compositor, best for low-performance applications and games
		WINDOWMODE_BORDERLESS,
		//Window will go fullscreen and will hide decorations and top bar,
		//flashes when alt-tabbing and uses full gpu, best for high-performance applications and games
		WINDOWMODE_EXCLUSIVE
	};

	//Presentation/visibility state
	enum class WindowState
	{
		WINDOW_NORMAL,        //Show the window with default size and position
		WINDOW_MAXIMIZE,      //Maximize window to full monitor size
		WINDOW_MINIMIZE,      //Minimize window to taskbar
		WINDOW_HIDE,          //Hide the window, including from taskbar
		WINDOW_SHOWNOACTIVATE //Display the window without focusing to it
	};

	enum class DpiContext
	{
		//sharpest, ideal DPI scaling between monitors,
		//nearly identical in performance compared to DPI_SYSTEM_AWARE
		//but slower than DPI_UNAWARE at higher resolutions
		DPI_PER_MONITOR,

		//sharp on primary monitor, blurry if dragged to higher DPI monitor,
		//nearly identical in performance compared to DPI_PER_MONITOR
		//but slower than DPI_UNAWARE at higher resolutions
		DPI_SYSTEM_AWARE,

		//always as 96 DPI, blurry on high DPI screens, fastest performance
		DPI_UNAWARE
	};

#ifdef _WIN32
	struct LIB_API WindowData
	{
		uintptr_t hwnd{};
		uintptr_t hdc{};
		uintptr_t hInstance{};
		uintptr_t hMenu{};
		uintptr_t wndProc{};
	};

	enum class WindowRounding
	{
		ROUNDING_DEFAULT,    //system default (usually ROUNDING_ROUND in Win11)
		ROUNDING_NONE,       //sharp corners
		ROUNDING_ROUND,      //rounded regular radius
		ROUNDING_ROUND_SMALL //rounded but smaller radius
	};

	enum class FlashTarget
	{
		TARGET_WINDOW, //flashes the window border and title
		TARGET_TASKBAR //flashes the window button on the taskar
	};
	enum class FlashType
	{
		FLASH_ONCE,        //single flash
		FLASH_UNTIL_FOCUS, //keep flashing until user focuses on window
		FLASH_TIMED        //flash x times
	};

	enum class TaskbarProgressBarMode
	{
		PROGRESS_NONE,          //hide the bar
		PROGRESS_INDETERMINATE, //pulse
		PROGRESS_NORMAL,        //green bar
		PROGRESS_PAUSED,        //yellow bar
		PROGRESS_ERROR          //red bar
	};
#else
	struct LIB_API WindowData
	{
		uintptr_t display{};
		uintptr_t window{};
		uintptr_t visual{};
	};
#endif

	class LIB_API Window
	{
	public:
		static KalaWindowRegistry<Window>& GetRegistry();

		//Create a new window that is always hidden by default, you must manually make it visible.
		//Assign a parent window to display this window as a child of that window.
		//Set the context to your preferred dpi state to modify how
		//window dpi state affects performance and quality of the framebuffer
		static Window* Initialize(
			const string& title,
			vec2 size,
			Window* parentWindow = nullptr,
			DpiContext context = DpiContext::DPI_SYSTEM_AWARE);

		bool IsInitialized() const;

		u32 GetID() const;

		//Draws the window, handles messages for active frame
		void Update();

		//Assigns paths of last dragged files. This is called through WM_DROPFILES.
		void SetLastDraggedFiles(const vector<string>& files);
		const vector<string>& GetLastDraggedFiles() const;
		//Clears paths to last file paths that were dragged onto window
		void ClearLastDraggedFiles();

		void SetTitle(const string& newTitle) const;
		const string& GetTitle() const;

		//Set executable icon. Loaded via the texture framework.
		//The first parameter requires an ID to the texture.
		void SetIcon(u32 texture) const;
		//Returns icon ID (Texture object ID)
		u32 GetIcon() const;
		//Clears the current executable icon
		void ClearIcon() const;

		//Set overlay icon and optional tooltip. Loaded via the texture framework.
		//The overlay icon is shown in the task bar in the bottom right corner of the exe icon,
		//the tooltip is what text appears if you hover over it.
		//The overlay icon must be exactly 16x16px in size.
		//The first parameter requires an ID to the texture.
		void SetTaskbarOverlayIcon(
			u32 texture,
			const string& tooltip = "") const;
		u32 GetTaskbarOverlayIcon() const;
		//Clears the current overlay icon and its tooltip
		void ClearTaskbarOverlayIcon() const;

		//Bring this window to the foreground and make it focused
		void BringToFocus();

		//Set Windows window rounding state. Has no effect in Linux.
		void SetWindowRounding(WindowRounding roundState) const;
		WindowRounding GetWindowRoundingState() const;

		//Set logical window size (client area, in DPI-independent units)
		void SetClientRectSize(vec2 newSize);
		vec2 GetClientRectSize() const;

		//Set full window size (including borders)
		void SetOuterSize(vec2 newSize);
		vec2 GetOuterSize() const;

		//Set window position
		void SetPosition(vec2 newPos) const;
		vec2 GetPosition();

		void SetMaxSize(vec2 newMaxSize);
		vec2 GetMaxSize() const;

		void SetMinSize(vec2 newMinSize);
		vec2 GetMinSize() const;

		//If true, then this window is gonna go idle and reduces cpu and gpu
		//cycles by waiting for messageloop messages before updating the exe.
		void SetFocusRequired(bool newFocusRequired);
		bool IsFocusRequired() const;

		//If true, then this window is always on top of other windows
		void SetAlwaysOnTopState(bool state) const;
		bool IsAlwaysOnTop() const;

		//If true, then this shows the outer frame and can be resized
		void SetResizableState(bool state) const;
		bool IsResizable() const;

		//If true, then this window shows its top bar
		void SetTopBarState(bool state) const;
		bool IsTopBarEnabled() const;

		//If true, then this window has a functional and visible minimize button
		void SetMinimizeButtonState(bool state) const;
		bool IsMinimizeButtonEnabled() const;

		//If true, then this window has a functional and visible maximize button
		void SetMaximizeButtonState(bool state) const;
		bool IsMaximizeButtonEnabled() const;

		//If true, then this window has a functional close button.
		//Close button won't be grayed out or won't stop rendering due to Windows limits
		void SetCloseButtonState(bool state) const;
		bool IsCloseButtonEnabled() const;

		//If false, then minimize, maximize, close buttons and the logo are hidden.
		void SetSystemMenuState(bool state) const;
		bool IsSystemMenuEnabled() const;

		//Set window opacity/transparency. Internally clamped between 0.0f and 1.0f
		void SetOpacity(float alpha) const;
		float GetOpacity() const;

		//Returns true if one of these is true:
		//  - not foreground
		//  - minimized
		//  - not visible
		bool IsIdle() const { return isIdle; }

		//Returns true if this window is in the front
		bool IsForegroundWindow() const;
		//Returns true if this window is currently receiving keyboard input
		bool IsFocused() const;
		//Returns true if this window is undecorated and its size matches the monitor size
		bool IsFullscreen();
		//Returns true if this window is not open, but exists
		bool IsMinimized() const;
		//Returns false if this window is not rendered but also not minimized
		bool IsVisible() const;

		//Called internally in message loop
		void SetResizingState(bool newState) { isResizing = newState; }
		bool IsResizing() const { return isResizing; }

		//Can assign the window mode to one of the supported types
		void SetWindowMode(WindowMode mode);
		WindowMode GetWindowMode();

		//Can assign the window state to one of the supported types
		void SetWindowState(WindowState state);
		WindowState GetWindowState() const;

		//If true, then Windows stops this app from closing
		//when shutting down or logging off to enable you to close your work
		void SetShutdownBlockState(bool state);
		bool IShutdownBlockEnabled() const;

		//Flash the window or taskbar to attract user attention
		void Flash(
			FlashTarget target,
			FlashType type,
			u32 count = 0) const;

		//Set taskbar progress bar mode.
		//Max is internally clamped from 0 to 100,
		//current is internally clamped from 0 to max - 1
		void SetTaskbarProgressBarState(
			TaskbarProgressBarMode,
			u8 currentProgress,
			u8 maxProgress) const;

		//Correctly handle aspect ratio during window resize for camera
		void TriggerResize();
		void SetResizeCallback(const function<void()>& callback);

		//Ensure content is redrawn while window is being resized
		void TriggerRedraw();
		void SetRedrawCallback(const function<void()>& callback);

#ifdef _WIN32
		void SetWindowData(const WindowData& newWindowStruct);
		const WindowData& GetWindowData() const;
#else
		void SetWindowData(const WindowData& newWindowStruct);
		const WindowData& GetWindowData() const;
#endif

		//
		// WINDOW CONTENT
		//

		u32 GetInputID() const;
		void SetInputID(u32 newValue);
		
		u32 GetGLID() const;
		void SetGLID(u32 newValue);
		
		u32 GetMenuBarID() const;
		void SetMenuBarID(u32 newValue);
		
		//Clean up the external content of this window
		void SetCleanExternalContent(function<void(u32)> newValue);

		//Clean up the content of this window and erase it from its registry.
		//Calls the functional assigned with SetCleanExternalContent if it was assigned
		void CloseWindow();

		//Do not destroy manually, erase from registry instead
		~Window();
	private:
		uintptr_t GetHWND(const string& errorMessage) const;

		bool isInitialized = false;        //Cannot use this window if it is not yet initialized

		bool isWindowFocusRequired = true; //If true, then this window will not update unless selected.
		bool isIdle = false;               //Toggled dynamically by isfocused, isminimized and isvisible checks.
		bool isResizing = false;           //If true, then this window is currently being resized
		bool shutdownBlockState = false;   //Prevents Windows from shutting off or logging off if this is true so you can save your data

		vec2 maxSize = vec2{ 7680, 4320 }; //The maximum size this window can become
		vec2 minSize = vec2{ 400, 300 };   //The minimum size this window can become

		vec2 oldPos{};  //Stored pre-fullscreen window pos
		vec2 oldSize{}; //Stored pre-fullscreen window size

		u32 ID{};            //ID for this window
		u32 iconID{};        //ID for this window icon
		u32 overlayIconID{}; //ID for this window toolbar overlay icon

		vector<string> lastDraggedFiles{}; //The path of the last files which were dragged onto this window

		u32 inputID{};
		u32 glID{};
		u32 menuBarID{};
		
		//functional for cleaning the external content of this window
		function<void(u32)> cleanExternalContent{};

#ifdef _WIN32
		WindowData window_windows{}; //The windows data of this window
#else
		WindowData window_x11{};     //The X11 data of this window
#endif

		function<void()> resizeCallback{}; //Called whenever the window needs to be resized
		function<void()> redrawCallback{}; //Called whenever the window needs to be redrawn
	};
}