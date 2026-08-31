//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <functional>
#include <vector>
#include <array>
#include <filesystem>

#include "core_utils.hpp"
#include "math_utils.hpp"

#include "core/kw_messageloop.hpp"
#include "core/kw_registry.hpp"

namespace KalaWindow::Core
{
	class Input;
};

namespace KalaWindow::Graphics
{
	using std::string;
	using std::string_view;
	using std::function;
	using std::vector;
	using std::array;
	using std::pair;
	using std::filesystem::path;
	using std::default_delete;

	using KalaHeaders::KalaMath::vec2;

	using KalaWindow::Core::KalaWindowRegistry;

	static constexpr f32 MIN_WINDOW_SIZE = 100.0f;
	static constexpr f32 MAX_WINDOW_SIZE = 10000.0f;

	//Display mode / monitor ownership state
	enum class WindowMode : u8
	{
		//Window will float and can be resized and moved around
		WINDOWMODE_WINDOWED   = 0,
		//Window will go fullscreen and will hide decorations and top bar,
		//stays alt-tab friendly and uses the compositor, best for low-performance applications and games
		WINDOWMODE_BORDERLESS = 1,
		//Window will go fullscreen and will hide decorations and top bar,
		//flashes when alt-tabbing and uses full gpu, best for high-performance applications and games,
		//uses WINDOWMODE_BORDERLESS on x11
		WINDOWMODE_EXCLUSIVE  = 2
	};

	//Presentation/visibility state
	enum class WindowState : u8
	{
		WINDOW_NORMAL         = 0, //Show the window with default size and position
		WINDOW_MAXIMIZE       = 1, //Maximize window to full monitor size
		WINDOW_MINIMIZE       = 2, //Minimize window to taskbar
		WINDOW_HIDE           = 3, //Hide the window, including from taskbar
		WINDOW_SHOWNOACTIVATE = 4  //Display the window without focusing to it, uses WINDOW_NORMAL on x11
	};

#if defined(KWIN_ANY)
	struct LIB_API WindowData
	{
		uintptr_t window{};
		uintptr_t handle{};
		uintptr_t hInstance{};
		uintptr_t hMenu{};
		uintptr_t wndProc{};
	};
#elif defined(KLIN_ANY)
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
	friend struct default_delete<ProcessWindow>;
	public:
		KNODISCARD
		static KalaWindowRegistry<ProcessWindow>& GetRegistry();

		//Create a new window with a title and at the desired position and size.
		//Assign a parent window to display this window as a child of that window
		KNODISCARD
		static ProcessWindow* Initialize(
			string&& title,
			vec2 pos = 600,
			vec2 size = { 800, 600 },
			ProcessWindow* parentWindow = nullptr);

		//Global update call for all process windows, inputs and Vulkan contexts.
		//  - global early update: what happens BEFORE KalaWindow window logic and input logic
		//  - global update: what happens IN BETWEEN KalaWindow window logic and input logic
		//  - global late update: what happens AFTER KalaWindow window logic and input logic
		static void Update(
			const function<void()>& globalEarlyUpdate = {},
			const function<void()>& globalUpdate = {},
			const function<void()>& globalLateUpdate = {});

		KNODISCARD
		u32 GetID() const;
		KNODISCARD
		u32 GetInputID() const;
		KNODISCARD
		u32 GetGraphicsContextID() const;

		//Do something whenever file drag onto window succeeds
		void SetDraggedFilesCallback(function<void(const vector<path>&, vec2)>&& newValue);
		//Returns last dragged files that were dragged onto window
		KNODISCARD
		const vector<path>& GetLastDraggedFiles() const;
		//Clears last dragged files that were dragged onto window
		void ClearLastDraggedFiles();

		KNODISCARD
		string GetTitle() const;
		void SetTitle(string&& newTitle) const;

		//Bring this window to the foreground and make it focused
		void BringToFocus();

		KNODISCARD
		vec2 GetSize() const;
		void SetSize(vec2 newSize);

		//X11 does not expose outer size reliably
#if defined(KWIN_ANY)
		KNODISCARD
		vec2 GetOuterSize() const;
		void SetOuterSize(vec2 newSize);
#endif

		KNODISCARD
		vec2 GetMaxSize() const;
		void SetMaxSize(vec2 newMaxSize);

		KNODISCARD
		vec2 GetMinSize() const;
		void SetMinSize(vec2 newMinSize);

		KNODISCARD
		vec2 GetPosition();
		void SetPosition(vec2 newPos);

		//If true, then this window is always on top of other windows
		KNODISCARD
		bool IsAlwaysOnTop() const;		
		void SetAlwaysOnTopState(bool state);

		//If true, then this shows the outer frame and can be resized
		KNODISCARD
		bool IsResizable() const;		
		void SetResizableState(bool state);

#if defined(KLIN_ANY)
		KNODISCARD
		pair<string, string> GetWindowClass() const;
		void SetWindowClass(string&& newValue);
#endif

		//Returns true if one of these is true:
		//  - not foreground
		//  - minimized
		//  - not visible
		KNODISCARD
		bool IsIdle() const;

		//Returns true if this window is being hovered over by the cursor
		KNODISCARD
		bool IsHovered() const;
		//Returns true if this window is in the front, maps to IsFocused on X11
		KNODISCARD
		bool IsForegroundWindow() const;
		//Returns true if this window is currently receiving keyboard input
		KNODISCARD
		bool IsFocused() const;
		//Returns true if this window is undecorated and its size matches the monitor size
		KNODISCARD
		bool IsFullscreen();
		//Returns true if this window is not open, but exists, maps to opposite of IsVisible on X11
		KNODISCARD
		bool IsMinimized() const;
		//Returns false if this window is not rendered but also not minimized
		KNODISCARD
		bool IsVisible() const;

		//Can assign the window mode to one of the supported types
		KNODISCARD
		WindowMode GetWindowMode();
		void SetWindowMode(WindowMode mode);

		//Can assign the window state to one of the supported types
		KNODISCARD
		WindowState GetWindowState() const;
		void SetWindowState(WindowState state);

		//What happens before per-window logic
		//but after global early update
		void SetEarlyUpdateCallback(function<void()>&& newValue);
		//What happens after this window logic
		//but before global update
		void SetUpdateCallback(function<void()>&& newValue);
		//What happens after global update and after this window input logic
		//but before global late update
		void SetLateUpdateCallback(function<void()>&& newValue);

		void SetResizeCallback(function<void()>&& newValue);

		void SetShutdownCallback(function<void()>&& newValue);

		const WindowData& GetWindowData() const;

		void Destroy();
	private:
		~ProcessWindow();

		void UpdateIdleState();

		bool isWindowHovered{}; //If true, then this window is currently being hovered by the cursor.
		bool isIdle{};          //Toggled dynamically by isfocused, isminimized and isvisible checks.

		u32 parentID = UINT32_MAX;
		vector<u32> childIDs{};

#if defined(KLIN_ANY)
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

		uintptr_t currentDndSource{};
#endif

		vec2 maxSize = vec2{ 7680, 4320 }; //The maximum size this window can become
		vec2 minSize = vec2{ 400, 300 };   //The minimum size this window can become

		vec2 oldPos{};  //Stored pre-fullscreen window pos
		vec2 oldSize{}; //Stored pre-fullscreen window size

		u32 ID{};            //ID for this window
		u32 iconID{};        //ID for this window icon
		u32 overlayIconID{}; //ID for this window toolbar overlay icon

		//last files that were dragged onto screen
		vector<path> lastDraggedFiles{};
		vec2 draggedFilesPos{};
		function<void(const vector<path>&, vec2)> draggedFilesCallback{};

		u32 inputID{};
		u32 graphicsContextID{};
		
		WindowData windowData{};

		function<void()> earlyUpdateCallback{};
		function<void()> updateCallback{};
		function<void()> lateUpdateCallback{};

		function<void()> resizeCallback{};
		function<void()> shutdownCallback{};
	};
}