//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <functional>
#include <vector>
#include <array>

#include "core_utils.hpp"
#include "math_utils.hpp"

#ifdef _WIN32
#include "core/kw_messageloop_windows.hpp"
#else
#include "core/kw_messageloop_x11.hpp"
#endif

#include "core/kw_registry.hpp"

namespace KalaWindow::Core
{
	class Input;
};

namespace KalaWindow::Graphics
{
	constexpr f32 MIN_WINDOW_SIZE = 100.0f;
	constexpr f32 MAX_WINDOW_SIZE = 10000.0f;

	using std::string;
	using std::string_view;
	using std::function;
	using std::vector;
	using std::array;
	using std::pair;

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
		//flashes when alt-tabbing and uses full gpu, best for high-performance applications and games,
		//uses WINDOWMODE_BORDERLESS on x11
		WINDOWMODE_EXCLUSIVE
	};

	//Presentation/visibility state
	enum class WindowState
	{
		WINDOW_NORMAL,        //Show the window with default size and position
		WINDOW_MAXIMIZE,      //Maximize window to full monitor size
		WINDOW_MINIMIZE,      //Minimize window to taskbar
		WINDOW_HIDE,          //Hide the window, including from taskbar
		WINDOW_SHOWNOACTIVATE //Display the window without focusing to it, uses WINDOW_NORMAL on x11
	};

	enum class DpiContext
	{
		//sharpest, ideal DPI scaling between monitors
		DPI_PER_MONITOR,

		//sharp on primary monitor, blurry if dragged to higher DPI monitor
		DPI_SYSTEM_AWARE,

		//blurry on high DPI screens, fastest performance
		DPI_UNAWARE
	};

#ifdef _WIN32
	struct LIB_API WindowData
	{
		uintptr_t window{};
		uintptr_t handle{};
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
		uintptr_t window{};
		uintptr_t xic{};
	};
#endif

	class LIB_API ProcessWindow
	{
	friend class KalaWindow::Core::MessageLoop;
	friend class KalaWindow::Core::Input;
	friend class VulkanContext;
	public:
		static KalaWindowRegistry<ProcessWindow>& GetRegistry();

		//Create a new window with a title and at the desired position and size.
		//Assign a parent window to display this window as a child of that window.
		//Set the context to your preferred dpi state to modify how
		//window dpi state affects performance and quality of the framebuffer
		static ProcessWindow* Initialize(
			string&& title,
			vec2 pos = 600,
			vec2 size = { 800, 600 },
			ProcessWindow* parentWindow = nullptr,
			DpiContext context = DpiContext::DPI_SYSTEM_AWARE);

		u32 GetID() const;
		u32 GetInputID() const;
		u32 GetGraphicsContextID() const;
		
#ifdef _WIN32
		u32 GetMenuBarID() const;
#endif

		//Draws the window, handles messages for active frame,
		//only handles idle state on X11,
		//you must call message loop update before calling per-window update on X11
		void Update();

		const vector<string>& GetLastDraggedFiles() const;
		//Assigns paths of last dragged files. This is called through WM_DROPFILES on windows.
		void SetLastDraggedFiles(vector<string>&& files);
		//Clears paths to last file paths that were dragged onto window
		void ClearLastDraggedFiles();

		string GetTitle() const;
		void SetTitle(string&& newTitle) const;

		//Bring this window to the foreground and make it focused
		void BringToFocus();

		//X11 does not have rounding state logic
#ifdef _WIN32
		WindowRounding GetWindowRoundingState() const;
		void SetWindowRoundingState(WindowRounding roundState) const;
#endif

		vec2 GetSize() const;
		void SetSize(vec2 newSize);

		//X11 does not expose outer size reliably
#ifdef _WIN32
		vec2 GetOuterSize() const;
		void SetOuterSize(vec2 newSize);
#endif

		vec2 GetMaxSize() const;
		void SetMaxSize(vec2 newMaxSize);

		vec2 GetMinSize() const;
		void SetMinSize(vec2 newMinSize);

		vec2 GetPosition();
		void SetPosition(vec2 newPos);

		//If true, then this window is always on top of other windows
		bool IsAlwaysOnTop() const;		
		void SetAlwaysOnTopState(bool state);

		//If true, then this shows the outer frame and can be resized
		bool IsResizable() const;		
		void SetResizableState(bool state);

#ifdef _WIN32
		//If true, then this window shows its top bar
		bool IsTopBarEnabled() const;		
		void SetTopBarState(bool state) const;

		//If true, then this window has a functional and visible minimize button
		bool IsMinimizeButtonEnabled() const;
		void SetMinimizeButtonState(bool state) const;

		//If true, then this window has a functional and visible maximize button
		bool IsMaximizeButtonEnabled() const;
		void SetMaximizeButtonState(bool state) const;

		//If true, then this window has a functional close button.
		//Close button won't be grayed out or won't stop rendering due to Windows limits
		bool IsCloseButtonEnabled() const;
		void SetCloseButtonState(bool state) const;

		//If false, then minimize, maximize, close buttons and the logo are hidden.
		bool IsSystemMenuEnabled() const;
		void SetSystemMenuState(bool state) const;

		float GetOpacity() const;
		void SetOpacity(float alpha) const;
#else
		pair<string, string> GetWindowClass() const;
		void SetWindowClass(string&& newValue);
#endif

		//Returns true if one of these is true:
		//  - not foreground
		//  - minimized
		//  - not visible
		bool IsIdle() const;

		//Returns true if this window is being hovered over by the cursor
		bool IsHovered() const;
		//Returns true if this window is in the front, maps to IsFocused on X11
		bool IsForegroundWindow() const;
		//Returns true if this window is currently receiving keyboard input
		bool IsFocused() const;
		//Returns true if this window is undecorated and its size matches the monitor size
		bool IsFullscreen();
		//Returns true if this window is not open, but exists, maps to opposite of IsVisible on X11
		bool IsMinimized() const;
		//Returns false if this window is not rendered but also not minimized
		bool IsVisible() const;
		//Returns true if this window is currently being resized
		bool IsResizing() const;

		//Can assign the window mode to one of the supported types
		WindowMode GetWindowMode();
		void SetWindowMode(WindowMode mode);

		//Can assign the window state to one of the supported types
		WindowState GetWindowState() const;
		void SetWindowState(WindowState state);

#ifdef _WIN32
		//If true, then Windows stops this app from closing
		//when shutting down or logging off to enable you to close your work
		bool IShutdownBlockEnabled() const;
		void SetShutdownBlockState(bool state);

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
#endif

		void ResizeCallback();
		void SetResizeCallback(function<void()>&& newValue);

		void SetShutdownCallback(function<void()>&& newValue);

		const WindowData& GetWindowData() const;
		void SetWindowData(WindowData&& newWindowStruct);

		void CheckWindowHandle(string&& errorMessage);

		void Destroy();

		~ProcessWindow();
	private:
		bool isWindowHovered{};            //If true, then this window is currently being hovered by the cursor.
		bool isWindowFocusRequired = true; //If true, then this window will not update unless selected.
		bool isIdle{};                     //Toggled dynamically by isfocused, isminimized and isvisible checks.
		bool isResizing{};                 //If true, then this window is currently being resized
		bool shutdownBlockState{};         //Prevents Windows from shutting off or logging off if this is true so you can save your data

		u32 parentID = UINT32_MAX;
		vector<u32> childIDs{};

#ifdef __linux__
		void UpdateFullscreenAndMinimizedState();

		bool isFocused{};
		bool isVisible{};
		bool isMinimized{};
		
		bool isFullscreen{};

		vec2 pos{};
		vec2 size{};
		vec2 outerSize{};

		WindowMode windowMode{};
		WindowState windowState{};
#endif

		vec2 maxSize = vec2{ 7680, 4320 }; //The maximum size this window can become
		vec2 minSize = vec2{ 400, 300 };   //The minimum size this window can become

		vec2 oldPos{};  //Stored pre-fullscreen window pos
		vec2 oldSize{}; //Stored pre-fullscreen window size

		u32 ID{};            //ID for this window
		u32 iconID{};        //ID for this window icon
		u32 overlayIconID{}; //ID for this window toolbar overlay icon

		vector<string> lastDraggedFiles{}; //The path of the last files which were dragged onto this window

		u32 inputID{};
		u32 graphicsContextID{};
#ifdef _WIN32
		u32 menuBarID{};
#endif
		
		WindowData windowData{};

		function<void()> resizeCallback{};
		function<void()> shutdownCallback{};
	};
}