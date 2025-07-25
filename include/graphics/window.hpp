//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <functional>

#include "core/platform.hpp"

namespace KalaWindow::Graphics
{
	using std::string;
	using std::unique_ptr;
	using std::vector;
	using std::function;

	enum class ShutdownState
	{
		SHUTDOWN_CLEAN,   //Regular exit (exit)
		SHUTDOWN_FAILURE, //Problem detected, controlled shutdown (terminate)
		SHUTDOWN_CRITICAL //Catastrophic/forced shutdown, worst case scenario (abort)
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

	//Buttons shown on the popup
	enum class PopupAction
	{
		POPUP_ACTION_OK,            // OK button only
		POPUP_ACTION_OK_CANCEL,     // OK and Cancel buttons
		POPUP_ACTION_YES_NO,        // Yes and No buttons
		POPUP_ACTION_YES_NO_CANCEL, // Yes, No, and Cancel buttons
		POPUP_ACTION_RETRY_CANCEL   // Retry and Cancel buttons
	};

	//Icon shown on the popup
	enum class PopupType
	{
		POPUP_TYPE_INFO,    // Info icon (blue 'i')
		POPUP_TYPE_WARNING, // Warning icon (yellow triangle)
		POPUP_TYPE_ERROR,   // Error icon (red X)
		POPUP_TYPE_QUESTION // Question icon (used for confirmations)
	};

	//User response from the popup
	enum class PopupResult
	{
		POPUP_RESULT_NONE,   //No response or unknown
		POPUP_RESULT_OK,     //User clicked OK
		POPUP_RESULT_CANCEL, //User clicked Cancel
		POPUP_RESULT_YES,    //User clicked Yes
		POPUP_RESULT_NO,     //User clicked No
		POPUP_RESULT_RETRY   //User clicked Retry
	};

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
		uintptr_t wndProc{};   //WINDOW PROC FOR OPENGL, NOT USED IN VULKAN
	};
#elif __linux__
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

	class KALAWINDOW_API Window
	{
	public:
		//All created windows are stored here.
		static inline vector<Window*> windows{};

		static unique_ptr<Window> Initialize(
			const string& title,
			vec2 size);

		//Get the handle to opengl32.dll
		static uintptr_t GetOpenGLLib() { return openglLib; }
		//Set the handle to opengl32.dll
		static void SetOpenGLLib(uintptr_t newOpenglLib) { openglLib = newOpenglLib; }

		//Get the handle to vulkan-1.dll
		static uintptr_t GetVulkanLib() { return vulkanLib; }
		//Set the handle to vulkan-1.dll
		static void SetVulkanLib(uintptr_t newVulkanLib) { vulkanLib = newVulkanLib; }

		Window(
			string title,
			unsigned int ID,
			vec2 size) :
			title(title),
			ID(ID),
			size(size) {}

#ifdef _WIN32
		WindowData& GetWindowData() { return window_windows; }
		void SetWindowData(WindowData newWindowStruct)
		{
			window_windows = newWindowStruct;
		}
#elif __linux__
		WindowData& GetWindowData() { return window_x11; }
		void SetWindowData(WindowData newWindowStruct)
		{
			window_x11 = newWindowStruct;
		}
#endif

		OpenGLData& GetOpenGLData() { return openglData; }
		void SetOpenGLData(OpenGLData newOpenGLData)
		{
			openglData = newOpenGLData;
		}

		VulkanData_Core& GetVulkanCoreData() { return vulkanCoreData; }
		void SetVulkanCoreData(VulkanData_Core newVulkanCoreData)
		{
			vulkanCoreData = newVulkanCoreData;
		}

		VulkanShaderWindowData& GetVulkanShaderWindowStruct() { return vulkanShaderWindowData; }
		void SetVulkanShaderWindowStruct(VulkanShaderWindowData newVulkanShaderWindowData)
		{
			vulkanShaderWindowData = newVulkanShaderWindowData;
		}

		const string& GetTitle() const { return title; }
		void SetTitle(const string& newTitle);

		//Returns dpi-accurate framebuffer size.
		vec2 GetSize();
		void SetSize(vec2 newSize);

		vec2 GetPosition();
		void SetPosition(vec2 newPos);

		vec2 GetMaxSize() const { return maxSize; }
		void SetMaxSize(const vec2& newMaxSize) { maxSize = newMaxSize; }

		vec2 GetMinSize() const { return minSize; }
		void SetMinSize(const vec2& newMinSize) { minSize = newMinSize; }

		bool IsInitialized() const { return isInitialized; }
		void SetInitializedState(bool newInitialized) { isInitialized = newInitialized; }

		bool IsFocusRequired() const { return isWindowFocusRequired; }
		void SetFocusRequired(bool newFocusRequired) { isWindowFocusRequired = newFocusRequired; }

		//Returns true if this window is currently selected
		bool IsFocused() const;
		//Returns true if this window is not open, but exists
		bool IsMinimized() const;
		//Returns false if this window is not rendered but also not minimized
		bool IsVisible() const;
		//Can assign the window state to one of the supported types
		void SetWindowState(WindowState state);

		//Returns true if window is idle - not focused, minimized or not visible.
		bool IsIdle() const { return isIdle; }

		PopupResult CreatePopup(
			const string& title,
			const string& message,
			PopupAction action,
			PopupType type);

		void SetResizeCallback(function<void()> callback)
		{
			resizeCallback = callback;
		}
		function<void()> GetResizeCallback()
		{
			return resizeCallback;
		}

		using RedrawCallback = void(*)();
		void SetRedrawCallback(RedrawCallback callback) { OnRedraw = callback; }
		void TriggerRedraw() const { if (OnRedraw) OnRedraw(); }

		static Window* FindWindowByName(const string& targetName);
		static Window* FindWindowByID(unsigned int targetID);

		static void Update(Window* targetWindow);

		~Window();

		//Intended to be used for regular shutdown conditions, if program exited
		//with no errors and so on. Called at shutdown stage before any
		//windows or the render pipeline are destroyed.
		static void SetUserShutdownFunction(function<void()> regularShutdown);

		/// <summary>
		/// Handles the shutdown conditions of KalaWindow.
		/// </summary>
		/// <param name="state">
		///		Targets either regular exit, terminate or abort
		///		based on ShutdownState enum.
		/// </param>
		/// <param name="useWindowShutdown">
		///		If false, then KalaWindow ShutdownState and its actions are ignored
		///		and user must provide their own setup that is called after all windows and the render pipeline are destroyed.
		/// </param>
		/// <param name="userShutdown">
		///		The function user can optionally pass to KalaWindow shutdown procedure,
		///     called dynamically either before window and render pipeline shutdown
		///     if useWindowShutdown is true, otherwise it is called after.
		/// </param>
		static void Shutdown(
			ShutdownState state,
			bool useWindowShutdown = true,
			function<void()> userShutdown = nullptr);
	private:
		static inline uintptr_t openglLib{}; //The handle to opengl32.dll
		static inline uintptr_t vulkanLib{}; //The handle to vulkan-1.dll

		bool isInitialized = false;          //Cannot use this window if it is not yet initialized
		bool isWindowFocusRequired = true;   //If true, then this window will not update unless selected.
		bool isIdle = false;                 //Toggled dynamically by isfocused, isminimized and isvisible checks.

		vec2 maxSize = vec2{ 7680, 4320 }; //The maximum size this window can become
		vec2 minSize = vec2{ 400, 300 };   //The minimum size this window can become
		RedrawCallback OnRedraw{};           //Called whenever the window needs to be redrawn

		//core variables

		string title{};        //The title of this window
		unsigned int ID{};     //The ID of this window
		vec2 size{};          //The width and height of this window

		//platform-specific variables

#ifdef _WIN32
		WindowData window_windows{}; //The windows data of this window
#elif __linux__
		WindowData window_x11{};         //The X11 data of this window
#endif

		//vendor-specific variables

		OpenGLData openglData{}; //The OpenGL data of this window

		VulkanData_Core vulkanCoreData{}; //The core Vulkan data of this window
		VulkanShaderWindowData vulkanShaderWindowData{}; //Window-level VkPipeline data

		function<void()> resizeCallback{};
	};
}