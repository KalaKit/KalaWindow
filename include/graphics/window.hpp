//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <memory>
#include <vector>

#include "core/platform.hpp"
#include "core/enums.hpp"
#include "graphics/render.hpp"

namespace KalaWindow::Graphics
{
	using std::string;
	using std::unique_ptr;
	using std::vector;

	struct WindowStruct_Windows
	{
		void* hwnd;
		void* hInstance;
		void* wndProc;   //WINDOW PROC, NOT USED IN VULKAN
		void* hglrc;     //OPENGL CONTEXT VIA WGL, NOT USED IN VULKAN
		void* hdc;       //OPENGL HANDLE TO DEVICE CONTEXT, NOT USED IN VULKAN
		void* surface;   //VULKAN SURFACE, NOT USED IN OPENGL
	};
	struct WindowStruct_X11
	{
		void* display;
		void* window;
		void* visual;
		void* glxContext; //OPENGL CONTEXT VIA GLX, NOT USED IN VULKAN
		void* surface;    //VULKAN SURFACE, NOT USED IN OPENGL
	};

	class KALAWINDOW_API Window
	{
	public:
		//All created windows are stored here.
		static inline vector<unique_ptr<Window>> windows;

		static Window* Initialize(
			const string& title,
			int width,
			int height);

		Window(
			string title,
			unsigned int ID,
			int width,
			int height) :
			title(title),
			ID(ID),
			width(width),
			height(height) {
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
		void SetTitle(const string& newTitle) { title = newTitle; }

		unsigned int GetWidth() const { return width; }
		void SetWidth(unsigned int newWidth) { width = newWidth; }

		unsigned int GetHeight() const { return height; }
		void SetHeight(unsigned int newHeight) { height = newHeight; }

		kvec2 GetMaxSize() const { return maxSize; }
		void SetMaxSize(const kvec2& newMaxSize) { maxSize = newMaxSize; }

		kvec2 GetMinSize() const { return minSize; }
		void SetMinSize(const kvec2& newMinSize) { minSize = newMinSize; }

		bool IsInitialized() const { return isInitialized; }
		void SetInitializedState(bool newInitialized) { isInitialized = newInitialized; }

		bool IsFocusRequired() const { return isWindowFocusRequired; }
		void SetFocusRequired(bool newFocusRequired) { isWindowFocusRequired = newFocusRequired; }

		//Returns true if this window is currently selected
		bool IsFocused(Window* window) const;
		//Returns true if this window is not open, but exists
		bool IsMinimized(Window* window) const;
		//Returns false if this window is not rendered but also not minimized
		bool IsVisible(Window* window) const;

		//Returns true if window is idle
		bool IsIdle() const { return isIdle; }

		PopupResult CreatePopup(
			const string& title,
			const string& message,
			PopupAction action,
			PopupType type);

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
		unsigned int width;  //The width of this window
		unsigned int height; //The height of this window

		//platform-specific variables

		WindowStruct_Windows window_windows{}; //The windows data of this window
		WindowStruct_X11 window_x11{};         //The X11 data of this window

		//KalaWindow will dynamically update window idle state
		void UpdateIdleState(Window* window);
	};
}