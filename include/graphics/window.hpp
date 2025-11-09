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
#include "KalaHeaders/registry_utils.hpp"

namespace KalaWindow::Graphics
{
	using std::string;
	using std::function;
	using std::vector;
	using std::array;

	using KalaHeaders::vec2;

	using KalaHeaders::Registry;

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

	//Use the target type enum to access IDs of this window
	enum class TargetType
	{
		TYPE_INPUT,        //single instance ID
		TYPE_GL_CONTEXT,   //single instance ID
		TYPE_MENU_BAR,     //single instance ID
		TYPE_CAMERA,       //vector of IDs
		TYPE_WIDGET        //vector of IDs
	};

	//Supported states the window can go to
	enum class WindowState
	{
		WINDOW_NORMAL,        //Show the window with default size and position
		WINDOW_MAXIMIZE,      //Maximize window to full monitor size
		WINDOW_MINIMIZE,      //Minimize window to taskbar
		WINDOW_HIDE,          //Hide the window, including from taskbar
		WINDOW_SHOWNOACTIVATE //Display the window without focusing to it
	};

#ifdef _WIN32
	struct WindowData
	{
		uintptr_t hwnd{};
		uintptr_t hInstance{};
		uintptr_t hMenu{};
		uintptr_t wndProc{};   //WINDOW PROC FOR OPENGL, NOT USED IN VULKAN
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
	struct WindowData
	{
		uintptr_t display{};
		uintptr_t window{};
		uintptr_t visual{};
	};
#endif

	class LIB_API Window
	{
	public:
		static inline Registry<Window> registry{};

		//Create a new window with an optional choice to attach a parent window.
		//Assign a parent window to display this window as a child of that window.
		//Set window state to your preferred version, like hidden at startup etc.
		//Set the context to your preferred dpi state to modify how
		//window dpi state affects performance and quality of the framebuffer
		static Window* Initialize(
			const string& title,
			vec2 size,
			Window* parentWindow = nullptr,
			WindowState state = WindowState::WINDOW_NORMAL,
			DpiContext context = DpiContext::DPI_SYSTEM_AWARE);
		inline bool IsInitialized() const { return isInitialized; }

		inline u32 GetID() const { return ID; }

		//Assigns paths of last dragged files. This is called through WM_DROPFILES.
		inline void SetLastDraggedFiles(const vector<string>& files) { lastDraggedFiles = files; };
		inline const vector<string>& GetLastDraggedFiles() const { return lastDraggedFiles; };
		//Clears paths to last file paths that were dragged onto window
		inline void ClearLastDraggedFiles() { lastDraggedFiles.clear(); };

		//Draws the window, handles messages for active frame
		void Update();

		void SetTitle(const string& newTitle) const;
		const string& GetTitle() const;

		//Set executable icon. Loaded via the texture framework.
		//The first parameter requires an ID to the texture.
		void SetIcon(u32 texture) const;
		//Returns icon ID (Texture object ID)
		inline u32 GetIcon() const { return iconID; }
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
		inline u32 GetTaskbarOverlayIcon() const { return overlayIconID; }
		//Clears the current overlay icon and its tooltip
		void ClearTaskbarOverlayIcon() const;

		//Bring this window to the foreground and make it focused
		void BringToFocus() const;

		//Set Windows window rounding state. Has no effect in Linux.
		void SetWindowRounding(WindowRounding roundState) const;
		WindowRounding GetWindowRoundingState() const;

		//Set logical window size (client area, in DPI-independent units)
		void SetClientRectSize(vec2 newSize) const;
		vec2 GetClientRectSize() const;

		//Set full window size (including borders)
		void SetOuterSize(vec2 newSize) const;
		vec2 GetOuterSize() const;

		//Set dpi-accurate framebuffer size
		void SetFramebufferSize(vec2 newSize) const;
		vec2 GetFramebufferSize() const;

		//Set window position
		void SetPosition(vec2 newPos) const;
		vec2 GetPosition() const;

		inline void SetMaxSize(vec2 newMaxSize) { maxSize = newMaxSize; }
		inline vec2 GetMaxSize() const { return maxSize; }

		inline void SetMinSize(vec2 newMinSize) { minSize = newMinSize; }
		inline vec2 GetMinSize() const { return minSize; }

		//If true, then this window is gonna go idle and reduces cpu and gpu
		//cycles by waiting for messageloop messages before updating the exe.
		inline void SetFocusRequired(bool newFocusRequired) { isWindowFocusRequired = newFocusRequired; }
		inline bool IsFocusRequired() const { return isWindowFocusRequired; }

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
		//Returns true if this window is not open, but exists
		bool IsMinimized() const;
		//Returns false if this window is not rendered but also not minimized
		bool IsVisible() const;

		void SetResizingState(bool newState) { isResizing = newState; }
		bool IsResizing() const { return isResizing; }

		//If true, then this window will be set to true exclusive fullscreen state
		void SetExclusiveFullscreenState(bool state);
		inline bool IsExclusiveFullscreen() const { return isExclusiveFullscreen; };

		//If true, then this window will be set to borderless full screen size
		void SetBorderlessFullscreenState(bool state);
		bool IsBorderlessFullscreen() const;

		//Can assign the window state to one of the supported types
		void SetWindowState(WindowState state) const;
		WindowState GetWindowState() const;

		//If true, then Windows stops this app from closing
		//when shutting down or logging off to enable you to close your work
		void SetShutdownBlockState(bool state);
		inline bool IShutdownBlockEnabled() const { return shutdownBlockState; }

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
		inline void TriggerResize() { if (resizeCallback) resizeCallback(); }
		inline void SetResizeCallback(const function<void()>& callback) { resizeCallback = callback; }

		//Ensure content is redrawn while window is being resized
		inline void TriggerRedraw() { if (redrawCallback) redrawCallback(); }
		inline void SetRedrawCallback(const function<void()>& callback) { redrawCallback = callback; }

#ifdef _WIN32
		inline void SetWindowData(const WindowData& newWindowStruct)
		{
			window_windows = newWindowStruct;
		}
		inline const WindowData& GetWindowData() const { return window_windows; }
#else
		inline void SetWindowData(const WindowData& newWindowStruct)
		{
			window_x11 = newWindowStruct;
		}
		inline const WindowData& GetWindowData() const { return window_x11; }
#endif

		//
		// WINDOW CONTAINER
		//

		inline u32 GetSingleValue(TargetType targetType) const
		{
			switch (targetType)
			{
			default: return{};
			case TargetType::TYPE_INPUT:      return inputID; break;
			case TargetType::TYPE_GL_CONTEXT: return glContextID; break;
			case TargetType::TYPE_MENU_BAR:   return menuBarID; break;
			}

			return{};
		}
		inline const vector<u32> GetContainer(TargetType targetType) const
		{
			switch (targetType)
			{
			default: return{};
			case TargetType::TYPE_CAMERA: return cameras; break;
			case TargetType::TYPE_WIDGET: return widgets; break;
			}

			return{};
		}
		inline bool AddValue(
			TargetType targetType,
			u32 targetValue)
		{
			auto AddSingleValue = [](u32 targetValue, u32& targetPosition)
				{
					if (targetValue == 0) return false;
					targetPosition = targetValue;
					return true;
				};

			auto AddContainerValue = [](u32 targetValue, auto& targetContainer)
				{
					if (targetValue == 0) return false;

					if (find(targetContainer.begin(),
						targetContainer.end(),
						targetValue)
						!= targetContainer.end())
					{
						return false;
					}

					targetContainer.push_back(targetValue);

					return true;
				};

			switch (targetType)
			{
			case TargetType::TYPE_INPUT:
			{
				return AddSingleValue(targetValue, inputID);
			}
			case TargetType::TYPE_GL_CONTEXT:
			{
				return AddSingleValue(targetValue, glContextID);
			}
			case TargetType::TYPE_MENU_BAR:
			{
				return AddSingleValue(targetValue, menuBarID);
			}
			case TargetType::TYPE_CAMERA:
			{
				return AddContainerValue(targetValue, cameras);
			}
			case TargetType::TYPE_WIDGET:
			{
				return AddContainerValue(targetValue, widgets);
			}
			}

			return false;
		}
		inline bool RemoveValue(
			TargetType targetType,
			u32 targetValue)
		{
			auto RemoveSingleValue = [](u32 targetValue, u32& targetPosition)
				{
					if (targetValue == 0
						|| targetPosition != targetValue)
					{
						return false;
					}

					targetPosition = 0;
					return true;
				};

			auto RemoveContainerValue = [](u32 targetValue, auto& targetContainer)
				{
					if (targetValue == 0) return false;

					targetContainer.erase(remove(
						targetContainer.begin(),
						targetContainer.end(),
						targetValue),
						targetContainer.end());

					return true;
				};

			switch (targetType)
			{
			case TargetType::TYPE_INPUT:
			{
				return RemoveSingleValue(targetValue, inputID);
			}
			case TargetType::TYPE_GL_CONTEXT:
			{
				return RemoveSingleValue(targetValue, glContextID);
			}
			case TargetType::TYPE_MENU_BAR:
			{
				return RemoveSingleValue(targetValue, menuBarID);
			}
			case TargetType::TYPE_CAMERA:
			{
				return RemoveContainerValue(targetValue, cameras);
			}
			case TargetType::TYPE_WIDGET:
			{
				return RemoveContainerValue(targetValue, widgets);
			}
			}

			return false;
		}
		inline void CleanContainer(TargetType targetType)
		{
			switch (targetType)
			{
			case TargetType::TYPE_INPUT:
			case TargetType::TYPE_GL_CONTEXT:
			case TargetType::TYPE_MENU_BAR:
			{
				break;
			}
			case TargetType::TYPE_CAMERA: cameras.clear(); break;
			case TargetType::TYPE_WIDGET: widgets.clear(); break;
			}
		}

		//
		// WINDOW HIERARCHY
		//

		//Returns the top-most window of this window
		inline Window* GetRoot() { return parentWindow ? parentWindow->GetRoot() : this; }

		//Returns true if target window is connected
		//to current window as a child, parent or sibling.
		//Set recursive to true if you want deep window search
		inline bool HasWindow(
			Window* targetWindow,
			bool recursive = false)
		{
			if (!targetWindow) return false;

			if (this == targetWindow) return true;

			//check descendants
			for (auto* c : childWindows)
			{
				if (c == targetWindow) return true;

				if (recursive
					&& c->HasWindow(targetWindow, true))
				{
					return true;
				}
			}

			//check ancestors
			if (parentWindow)
			{
				if (parentWindow == targetWindow) return true;

				if (recursive
					&& HasWindow(targetWindow, true))
				{
					return true;
				}
			}

			return false;
		}

		inline bool IsParentWindow(
			Window* targetWindow,
			bool recursive = false)
		{
			if (!targetWindow
				|| this == targetWindow)
			{
				return false;
			}

			if (!parentWindow) return false;

			if (parentWindow == targetWindow) return true;

			if (recursive
				&& IsParentWindow(targetWindow, true))
			{
				return true;
			}

			return false;
		}
		inline Window* GetParentWindow(Window* currentWindow) { return parentWindow; }
		inline bool SetParentWindow(Window* targetWindow)
		{
			if (!targetWindow
				|| this == targetWindow
				|| HasWindow(targetWindow, true)
				|| targetWindow->HasWindow(this, true)
				|| (parentWindow
				&& (parentWindow == targetWindow
				|| parentWindow->HasWindow(this, true))))
			{
				return false;
			}

			parentWindow = targetWindow;
			parentWindow->childWindows.push_back(this);

			return true;
		}
		inline bool RemoveParentWindow()
		{
			if (!parentWindow) return false;

			vector<Window*>& parentChildren = parentWindow->childWindows;

			parentChildren.erase(remove(
				parentChildren.begin(),
				parentChildren.end(),
				this),
				parentChildren.end());

			parentWindow = nullptr;

			return true;
		}

		inline bool IsChildWindow(
			Window* targetWindow,
			bool recursive = false)
		{
			if (!targetWindow
				|| this == targetWindow
				|| parentWindow == this)
			{
				return false;
			}

			for (auto* c : childWindows)
			{
				if (c == targetWindow) return true;

				if (recursive
					&& c->IsChildWindow(targetWindow, true))
				{
					return true;
				}
			}

			return false;
		}
		inline bool AddChildWindow(Window* targetWindow)
		{
			if (!targetWindow
				|| this == targetWindow
				|| HasWindow(targetWindow, true)
				|| targetWindow->HasWindow(this, true))
			{
				return false;
			}

			childWindows.push_back(targetWindow);
			return true;
		}
		inline bool RemoveChildWindow(Window* targetWindow)
		{
			if (!targetWindow
				|| this == targetWindow
				|| parentWindow == targetWindow
				|| targetWindow->parentWindow != this)
			{
				return false;
			}

			childWindows.erase(remove(
				childWindows.begin(),
				childWindows.end(),
				targetWindow),
				childWindows.end());

			targetWindow->CloseWindow();

			return true;
		}

		inline const vector<Window*>& GetAllChildWindows() { return childWindows; }
		inline void RemoveAllChildWindows()
		{
			for (auto* c : childWindows) c->CloseWindow();
			childWindows.clear();
		}

		//Clear the content of this window and erase it from its registry
		inline void CloseWindow();

		//Do not destroy manually, erase from registry instead
		~Window();
	private:
		bool isInitialized = false;        //Cannot use this window if it is not yet initialized

		bool isWindowFocusRequired = true; //If true, then this window will not update unless selected.
		bool isIdle = false;               //Toggled dynamically by isfocused, isminimized and isvisible checks.
		bool isResizing = false;           //If true, then this window is currently being resized
		bool shutdownBlockState = false;   //Prevents Windows from shutting off or logging off if this is true so you can save your data

		bool isExclusiveFullscreen = false;

		vec2 maxSize = vec2{ 7680, 4320 }; //The maximum size this window can become
		vec2 minSize = vec2{ 400, 300 };   //The minimum size this window can become

		vec2 oldPos{};        //Stored pre-fullscreen window pos
		vec2 oldSize{};       //Stored pre-fullscreen window size
		//0 - WS_CAPTION
		//1 - WS_THICKFRAME
		//2 - WS_MINIMIZEBOX
		//3 - WS_MAXIMIZEBOX
		//4 - WS_SYSMENU
		u8 oldStyle{};        //Stored pre-fullscreen window style (Windows-only)

		u32 ID{};            //ID for this window
		u32 iconID{};        //ID for this window icon
		u32 overlayIconID{}; //ID for this window toolbar overlay icon

		vector<string> lastDraggedFiles{}; //The path of the last files which were dragged onto this window

		Window* parentWindow{};
		vector<Window*> childWindows{};

		u32 inputID{};
		u32 glContextID{};
		u32 menuBarID{};
		vector<u32> cameras{};
		vector<u32> widgets{};

#ifdef _WIN32
		WindowData window_windows{}; //The windows data of this window
#else
		WindowData window_x11{};     //The X11 data of this window
#endif

		function<void()> resizeCallback{}; //Called whenever the window needs to be resized
		function<void()> redrawCallback{}; //Called whenever the window needs to be redrawn
	};
}