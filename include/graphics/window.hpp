//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <functional>
#include <vector>

#include "KalaHeaders/api.hpp"
#include "KalaHeaders/core_types.hpp"

#include "core/glm_global.hpp"

namespace KalaWindow::Graphics
{
	//TODO: CREATE AN INTERNAL FRAMEBUFFER SYSTEM WHERE THE INTERNAL FRAMEBUFFER RESOLUTION ALWAYS MATCHES
	//USER RESOLUTION WHILE WINDOW RESOLUTION SCALES DYNAMICALLY
	
	//TODO: SEPARATE REUSABLE OPENGL/VULKAN STUFF FROM PER-WINDOW TO GLOBAL

	using std::string;
	using std::function;
	using std::vector;

	//Supported states the window can go to
	enum class WindowState
	{
		WINDOW_NORMAL,        //Show the window with default size and position
		WINDOW_MAXIMIZE,      //Maximize window to full monitor size
		WINDOW_MINIMIZE,      //Minimize window to taskbar
		WINDOW_HIDE,          //Hide the window, including from taskbar
		WINDOW_SHOWNOACTIVATE //Display the window without focusing to it
	};

	//TODO: ADD FILE EXPLORER FUNCTION

	enum class FileType
	{
		FILE_ANY,         //Can select any file type
		FILE_ANY_VIDEO,   //Can select any common video file type
		FILE_ANY_AUDIO,   //Can select any common audio file type
		FILE_ANY_MODEL,   //Can select any common model file type (for graphics software and game development)
		FILE_ANY_TEXTURE, //Can select any common texture file type (for graphics software and game development)
		FILE_EXE,         //Can select any executable
		FILE_FOLDER       //Can select any folder
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

	enum class LabelType
	{
		LABEL_LEAF,  //Clickable with required function, can't have children
		LABEL_BRANCH //Not clickable, won't work if function is added, can have children
	};
	struct MenuBarEvent
	{
		string parentLabel{};        //Name of parent label, leave empty if root

		string label{};              //Name of this label
		u32 labelID{};               //ID assigned to leaves, used for interaction
		function<void()> function{}; //Function assigned to leaves

		uintptr_t hMenu{};           //Branch HMENU handle for fast lookup
	};
#else
	struct WindowData
	{
		uintptr_t display{};
		uintptr_t window{};
		uintptr_t visual{};
	};
#endif

	//OpenGL data reusable across this window context
	struct OpenGLData
	{
		uintptr_t hglrc{};      //OPENGL CONTEXT VIA WGL, ONLY USED FOR WINDOWS
		uintptr_t hdc{};        //OPENGL HANDLE TO DEVICE CONTEXT, ONLY USED FOR WINDOWS
		uintptr_t glxContext{}; //OPENGL CONTEXT VIA GLX, ONLY USED FOR X11
		unsigned int lastProgramID{};
	};

	/*
	//Vulkan data reusable across this window context
	struct VulkanData_Core
	{
		//Core surface & swapchain handles

		uintptr_t surface{};   //VkSurfaceKHR
		uintptr_t swapchain{}; //VkSwapchainKHR

		//Swapchain image metadata

		uint32_t swapchainImageFormat{};  //VkFormat
		uint32_t swapchainExtentWidth{};  //VkExtent2D
		uint32_t swapchainExtentHeight{}; //VkExtent2D

		//Swapchain image views and framebuffers

		vector<uintptr_t>  images{};       //VkImage
		vector<uintptr_t>  imageViews{};   //VkImageView
		vector<uintptr_t>  framebuffers{}; //VkFramebuffer

		//Synchronization primitives, one set per swapchain image

		vector<uintptr_t>  imageAvailableSemaphores{}; //VkSemaphore
		vector<uintptr_t>  renderFinishedSemaphores{}; //VkSemaphore
		vector<uintptr_t>  inFlightFences{};           //VkFence
		vector<uintptr_t>  imagesInFlight{};           //VkFence

		//Command buffers & pool used for recording into those framebuffers

		vector<uintptr_t>  commandBuffers{}; //VkCommandBuffer
		uintptr_t commandPool{};             //VkCommandPool

		//The render pass used when drawing into these framebuffers

		uintptr_t renderPass{}; //VkRenderPass
	};

	//VkOffset2D, contents of offset in VD_VS_VkRect2D
	struct VD_VS_VkOffset2D
	{
		//Horizontal pixel offset, usually 0
		int32_t x = 0;
		//Vertical pixel offset, usually 0
		int32_t y = 0;
	};
	//VkExtent2D, contents of extent in VD_VS_VkRect2D
	struct VD_VS_VkExtent2D
	{
		//Width in pixels, usually matches framebuffer width
		uint32_t width{};
		//Height in pixels, usually matches framebuffer height
		uint32_t height{};
	};
	//VkViewport, contents of pViewports in VulkanData_ViewportState
	struct VD_VS_Viewports
	{
		//x-coordinate of top-left corner, usually 0.0f
		float x = 0.0f;
		//y-coordinate of top-left corner, usually 0.0f
		float y = 0.0f;
		//Viewport width, usually matches swapchain width
		float width{};
		//Viewport height, usually matches swapchain height
		float height{};
		//Minimum depth value, usually 0.0f
		float minDepth = 0.0f;
		//Maximum depth value, usually 1.0f
		float maxDepth = 0.0f;
	};
	//VkRect2D, contents of pScissors in VulkanData_ViewportState
	struct VD_VS_Scissors
	{
		//VkOffset2D, struct of VD_VS_VkOffset2D
		VD_VS_VkOffset2D offset{};
		//VkExtent2D, struct of VD_VS_VkRect2D
		VD_VS_VkExtent2D extent{};
	};
	//VkPipelineViewportStateCreateInfo
	struct VulkanData_ViewportState
	{
		//VkStructureType, always VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO
		uint32_t sType = 20;
		//Extension-specific structure, usually NULL
		uintptr_t pNext = NULL;
		//VkPipelineViewportStateCreateFlags, usually 0
		uint32_t flags = 0;
		//Number of viewports, usually 1
		uint32_t viewportCount = 1;
		//VkViewport, struct to VD_VS_Viewports
		VD_VS_Viewports pViewports{};
		//Number of scissors, usually 1
		uint32_t scissorCount = 1;
		//VkRect2D, struct to VD_VS_Scissors
		VD_VS_Scissors pScissors{};
	};

	//VkPipelineDynamicStateCreateInfo
	struct VulkanData_DynamicState
	{
		//VkStructureType, always VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO
		uint32_t sType = 27;
		//Extension-specific structure, usually NULL
		uintptr_t pNext = NULL;
		//VkPipelineDynamicStateCreateFlags, usually 0
		uint32_t flags = 0;
		//count of pDynamicStates, usually 2 (viewport and scissor)
		uint32_t dynamicStateCount = 0;
		//vector of VkDynamicState enums
		vector<uint32_t> pDynamicStates{};
	};

	//VkPipelineMultisampleStateCreateInfo
	struct VulkanData_MultisampleState
	{
		//VkStructureType, always VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO
		uint32_t sType = 24;
		//Extension-specific structure, usually NULL
		uintptr_t pNext = NULL;
		//VkPipelineMultisampleStateCreateFlags, usually 0
		uint32_t flags = 0;
		//VkSampleCountFlagBits enum, usually VK_SAMPLE_COUNT_1_BIT
		uint32_t rasterizationSamples = 0x00000001;
		//VkBool32, usually VK_FALSE
		uint32_t sampleShadingEnable = 0U;
		//Minimum sample shading value (clamped to [0,1]), usually 0.0f
		float minSampleShading = 0.0f;
		//VkSampleMask, usually 0
		uint32_t pSampleMask = 0;
		//VkBool32, usually VK_FALSE
		uint32_t alphaToCoverageEnable = 0U;
		//VkBool32. usually VK_FALSE
		uint32_t alphaToOneEnable = 0U;
	};

	//Window-level shader data passed by the user in its original format
	struct VulkanShaderWindowData
	{
		VulkanData_ViewportState viewportState{};
		VulkanData_DynamicState dynamicState{};
		VulkanData_MultisampleState multisampleState{};
	};
	*/

	class LIB_API Window
	{
	public:
		static Window* Initialize(
			const string& title,
			vec2 size);
		bool IsInitialized() const { return isInitialized; }

		//Draws the window, handles messages for active frame
		void Update();

		u32 GetID() const { return ID; }

		void SetTitle(const string& newTitle) const;
		string GetTitle() const;

		//Set executable icon. Loaded via the texture framework
		void SetIcon(const string& iconPath) const;
		//Returns icon ID (Texture object ID)
		u32 GetIcon() const { return iconID; }
		//Clears the current executable icon
		void ClearIcon() const;

		//Set overlay icon and optional tooltip. Loaded via the texture framework.
		//The overlay icon is shown in the task bar in the bottom right corner of the exe icon,
		//the tooltop is what text appears if you hover over it.
		//The overlay icon must be exactly 16x16px in size.
		void SetTaskbarOverlayIcon(
			const string& iconPath,
			const string& tooltip = "") const;
		u32 GetTaskbarOverlayIcon() const { return overlayIconID; }
		//Clears the current overlay icon and its tooltip
		void ClearTaskbarOverlayIcon() const;

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

		void SetMaxSize(vec2 newMaxSize) { maxSize = newMaxSize; }
		vec2 GetMaxSize() const { return maxSize; }

		void SetMinSize(vec2 newMinSize) { minSize = newMinSize; }
		vec2 GetMinSize() const { return minSize; }

		//If true, then this window is gonna go idle and reduces cpu and gpu
		//cycles by waiting for messageloop messages before updating the exe.
		bool IsFocusRequired() const { return isWindowFocusRequired; }
		void SetFocusRequired(bool newFocusRequired) { isWindowFocusRequired = newFocusRequired; }

		//If true, then this window is always on top of other windows
		void SetAlwaysOnTopState(bool state) const;
		bool IsAlwaysOnTop() const;

		//If true, then this shows the outer frame and can be resized
		void SetResizableState(bool state) const;
		bool IsResizable() const;

		//If true, then this window will be set to full screen size.
		//Switch between normal window and monitor-sized borderless window.
		void SetFullscreenState(bool state);
		bool IsFullscreen() const;

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

		//Returns true if this window is currently selected
		bool IsFocused() const;
		//Returns true if this window is not open, but exists
		bool IsMinimized() const;
		//Returns false if this window is not rendered but also not minimized
		bool IsVisible() const;
		//Can assign the window state to one of the supported types
		void SetWindowState(WindowState state) const;

		//Returns true if window is idle - not focused, minimized or not visible.
		bool IsIdle() const { return isIdle; }

		void TriggerResize() const { if (resizeCallback) resizeCallback(); }
		void SetResizeCallback(const function<void()>& callback) { resizeCallback = callback; }

		void TriggerRedraw() const { if (redrawCallback) redrawCallback(); }
		void SetRedrawCallback(const function<void()>& callback) { redrawCallback = callback; }

#ifdef _WIN32
		void SetWindowData(const WindowData& newWindowStruct)
		{
			window_windows = newWindowStruct;
		}
		const WindowData& GetWindowData() const { return window_windows; }
#else
		void SetWindowData(const WindowData& newWindowStruct)
		{
			window_x11 = newWindowStruct;
		}
		const WindowData& GetWindowData() const { return window_x11; }
#endif
		void SetOpenGLData(const OpenGLData& newOpenGLData)
		{
			openglData = newOpenGLData;
		}
		const OpenGLData& GetOpenGLData() const { return openglData; }

		/*
		void SetVulkanCoreData(const VulkanData_Core& newVulkanCoreData)
		{
			vulkanCoreData = newVulkanCoreData;
		}
		const VulkanData_Core& GetVulkanCoreData() const { return vulkanCoreData; }

		void SetVulkanShaderWindowStruct(const VulkanShaderWindowData& newVulkanShaderWindowData)
		{
			vulkanShaderWindowData = newVulkanShaderWindowData;
		}
		const VulkanShaderWindowData& GetVulkanShaderWindowStruct() const { return vulkanShaderWindowData; }
		*/

		//Do not destroy manually, erase from containers.hpp instead
		~Window();
	private:
		bool isInitialized = false;          //Cannot use this window if it is not yet initialized
		bool isWindowFocusRequired = true;   //If true, then this window will not update unless selected.
		bool isIdle = false;                 //Toggled dynamically by isfocused, isminimized and isvisible checks.

		vec2 maxSize = vec2{ 7680, 4320 }; //The maximum size this window can become
		vec2 minSize = vec2{ 400, 300 };   //The minimum size this window can become

		vec2 oldPos{};                     //Stored pre-fullscreen window pos
		vec2 oldSize{};                    //Stored pre-fullscreen window size
		//0 - WS_CAPTION
		//1 - WS_THICKFRAME
		//2 - WS_MINIMIZEBOX
		//3 - WS_MAXIMIZEBOX
		//4 - WS_SYSMENU
		u8 oldStyle{};                     //Stored pre-fullscreen window style (Windows-only)

		u32 ID{};            //The ID of this window
		u32 iconID{};        //The ID of this window icon
		u32 overlayIconID{}; //The ID of the toolbar overlay icon

		//platform-specific variables

#ifdef _WIN32
		WindowData window_windows{}; //The windows data of this window
#else
		WindowData window_x11{};         //The X11 data of this window
#endif

		//vendor-specific variables

		OpenGLData openglData{}; //The OpenGL data of this window

		//VulkanData_Core vulkanCoreData{}; //The core Vulkan data of this window
		//VulkanShaderWindowData vulkanShaderWindowData{}; //Window-level VkPipeline data

		function<void()> resizeCallback{}; //Called whenever the window needs to be resized
		function<void()> redrawCallback{}; //Called whenever the window needs to be redrawn
	};

	//Windows-only native menu bar. All leaf and and branch interactions are handled by the message loop.
	//Attach a function in CreateLabel for leaves, leave empty for functions so that the message loop
	//calls your function so that the menu bar interactions call your chosen functions.
	class LIB_API MenuBar
	{
	public:
		//Create a new empty menu bar at the top of the window.
		//Only one menu bar can be added to a window
		static void CreateMenuBar(Window* windowRef);
		static bool HasMenuBar(Window* windowRef);

		//Call a menu bar event function by menu label or its item label
		static void CallMenuBarEvent(
			Window* windowRef,
			const string& parentRef,
			const string& labelRef = "");
		//Call a menu bar event function by ID
		static void CallMenuBarEvent(
			Window* windowRef,
			u32 ID);

		//Create a menu bar label. Leaves must have functions, branches can't.
		//Leave parentRef empty if you want this label to be root
		static void CreateLabel(
			Window* windowRef,
			LabelType type,
			const string& parentRef,
			const string& labelRef,
			const function<void()> func = nullptr);

		//Add a horizontal separator line to the menu label.
		//If itemLabel isnt empty and exists then the sesparator is placed after the item label,
		//otherwise it is placed at the end of the menu label
		static void AddSeparator(
			Window* windowRef,
			const string& parentRef,
			const string& labelRef = "");

		//Destroy the existing menu bar inside the window
		static void DestroyMenuBar(Window* window);
	};
}