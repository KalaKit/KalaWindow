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
#include "graphics/render.hpp"

namespace KalaWindow::Graphics
{
	using std::string;
	using std::unique_ptr;
	using std::vector;
	using std::function;

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

	enum VSyncState
	{
		VSYNC_ON,              //Framerate is capped to monitor refresh rate.
		VSYNC_OFF,             //Framerate is uncapped, runs as fast as render loop allows, introduces tearing.
		VSYNC_TRIPLE_BUFFERING //Low latency, no screen tearing. Does not exist in OpenGL, will default to 'ON'!
	};

	struct Window_OpenGLData
	{
		uintptr_t hglrc;      //OPENGL CONTEXT VIA WGL, ONLY USED FOR WINDOWS
		uintptr_t hdc;        //OPENGL HANDLE TO DEVICE CONTEXT, ONLY USED FOR WINDOWS
		uintptr_t glxContext; //OPENGL CONTEXT VIA GLX, ONLY USED FOR X11
		unsigned int lastProgramID = 0;
	};
	struct Window_VulkanData
	{
		//Core surface & swapchain handles

		uintptr_t surface;   //VkSurfaceKHR
		uintptr_t swapchain; //VkSwapchainKHR

		//Swapchain image metadata

		uint32_t swapchainImageFormat;  //VkFormat
		uint32_t swapchainExtentWidth;  //VkExtent2D
		uint32_t swapchainExtentHeight; //VkExtent2D

		//Swapchain image views and framebuffers

		vector<uintptr_t>  images;       //VkImage
		vector<uintptr_t>  imageViews;   //VkImageView
		vector<uintptr_t>  framebuffers; //VkFramebuffer

		//Synchronization primitives, one set per swapchain image

		vector<uintptr_t>  imageAvailableSemaphores; //VkSemaphore
		vector<uintptr_t>  renderFinishedSemaphores; //VkSemaphore
		vector<uintptr_t>  inFlightFences;           //VkFence
		vector<uintptr_t>  imagesInFlight;           //VkFence

		//Command buffers & pool used for recording into those framebuffers

		vector<uintptr_t>  commandBuffers; //VkCommandBuffer
		uintptr_t commandPool;             //VkCommandPool

		//The render pass used when drawing into these framebuffers

		uintptr_t renderPass; //VkRenderPass
	};

	struct WindowStruct_Windows
	{
		uintptr_t hwnd;
		uintptr_t hInstance;
		uintptr_t wndProc;   //WINDOW PROC FOR OPENGL, NOT USED IN VULKAN
		Window_OpenGLData openglData;
		Window_VulkanData vulkanData;

	};
	struct WindowStruct_X11
	{
		uintptr_t display;
		uintptr_t window;
		uintptr_t visual;
		Window_OpenGLData openglData;
		Window_VulkanData vulkanData;
	};

	class KALAWINDOW_API Window
	{
	public:
		//All created windows are stored here.
		static inline vector<Window*> windows{};

		static unique_ptr<Window> Initialize(
			const string& title,
			kvec2 size);

		Window(
			string title,
			unsigned int ID,
			kvec2 size) :
			title(title),
			ID(ID),
			size(size) {
		}

		WindowStruct_Windows& GetWindow_Windows() { return window_windows; }
		void SetWindow_Windows(WindowStruct_Windows newWindowStruct)
		{
			window_windows = newWindowStruct;
		}

		WindowStruct_X11& GetWindow_X11() { return window_x11; }
		void SetWindow_X11(WindowStruct_X11 newWindowStruct)
		{
			window_x11 = newWindowStruct;
		}

		const string& GetTitle() const { return title; }
		void SetTitle(const string& newTitle);

		//Returns dpi-accurate framebuffer size.
		kvec2 GetSize();
		void SetSize(kvec2 newSize);

		kvec2 GetPosition();
		void SetPosition(kvec2 newPos);

		//Checks if vsync is enabled or not.
		//Managed internally in extensions_vulkan.cpp and opengl.cpp.
		VSyncState GetVSyncState();
		//Allows to set vsync true or false.
		//Managed internally in extensions_vulkan.cpp and opengl.cpp.
		void SetVSyncState(VSyncState vsyncState);

		kvec2 GetMaxSize() const { return maxSize; }
		void SetMaxSize(const kvec2& newMaxSize) { maxSize = newMaxSize; }

		kvec2 GetMinSize() const { return minSize; }
		void SetMinSize(const kvec2& newMinSize) { minSize = newMinSize; }

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

		//Returns true if window is idle
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

		static void DeleteWindow(Window* window);
	private:
		bool isInitialized = false;          //Cannot use this window if it is not yet initialized
		bool isWindowFocusRequired = true;   //If false, then this window will not update unless selected.
		bool isIdle = false;                 //If true, then this window will call drastically less updates.

		kvec2 maxSize = kvec2{ 7680, 4320 }; //The maximum size this window can become
		kvec2 minSize = kvec2{ 400, 300 };   //The minimum size this window can become
		RedrawCallback OnRedraw{};           //Called whenever the window needs to be redrawn

		//core variables

		string title;        //The title of this window
		unsigned int ID;     //The ID of this window
		kvec2 size;          //The width and height of this window

		//platform-specific variables

		WindowStruct_Windows window_windows{}; //The windows data of this window
		WindowStruct_X11 window_x11{};         //The X11 data of this window

		function<void()> resizeCallback;

		//KalaWindow will dynamically update window idle state
		void UpdateIdleState();
	};
}