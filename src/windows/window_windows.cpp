//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef _WIN32

#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#include <ShlObj.h>
#include <algorithm>
#include <functional>

//#define VK_NO_PROTOTYPES
//#include <Volk/volk.h>
//#include <vulkan/vulkan.h>

//#include "graphics/vulkan/vulkan.hpp"
#include "graphics/opengl/opengl.hpp"
#include "graphics/opengl/shader_opengl.hpp"
#include "graphics/window.hpp"
#include "windows/messageloop.hpp"
#include "core/input.hpp"
#include "core/core.hpp"
#include "core/log.hpp"

//using KalaWindow::Graphics::Vulkan::Renderer_Vulkan;
using KalaWindow::Graphics::OpenGL::Renderer_OpenGL;
using KalaWindow::Graphics::OpenGL::Shader_OpenGL;
using KalaWindow::Graphics::Window;
using KalaWindow::Core::MessageLoop;
using KalaWindow::Core::Input;
using KalaWindow::Core::KalaWindowCore;
using KalaWindow::Core::Logger;
using KalaWindow::Core::LogType;

using std::make_unique;
using std::move;
using std::to_string;
using std::find_if;
using std::function;
using std::exception;

static bool enabledBeginPeriod = false;

//KalaWindow will dynamically update window idle state
static void UpdateIdleState(Window* window, bool& isIdle);

namespace KalaWindow::Graphics
{
	static unsigned int nextWindowID = 0;

	Window* Window::Initialize(
		const string& title,
		vec2 size)
	{
#ifdef _WIN32
		if (!enabledBeginPeriod)
		{
			timeBeginPeriod(1);
			enabledBeginPeriod = true;
		}
#endif //_WIN32

		unsigned int windowID = nextWindowID++;
		unique_ptr<Window> newWindow = make_unique<Window>(
			title,
			windowID,
			size);

		HINSTANCE newHInstance = GetModuleHandle(nullptr);

		WNDCLASSA wc = {};
		wc.lpfnWndProc = reinterpret_cast<WNDPROC>(MessageLoop::WindowProcCallback());
		wc.hInstance = newHInstance;
		wc.lpszClassName = "KalaWindowClass";

		if (!RegisterClassA(&wc))
		{
			DWORD err = GetLastError();
			string message{};
			if (err == ERROR_CLASS_ALREADY_EXISTS)
			{
				message = "Window class already exists with different definition.\n";
			}
			else
			{
				message = "RegisterClassA failed with error: " + to_string(err) + "\n";
			}

			KalaWindowCore::ForceClose(
				"Window Error",
				message);

			string title = "Window error";

			KalaWindowCore::ForceClose(title, message);

			return nullptr;
		}

		HWND newHwnd = CreateWindowExA(
			0,
			"KalaWindowClass",
			title.c_str(),
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			size.x,
			size.y,
			nullptr,
			nullptr,
			newHInstance,
			nullptr);

		if (!newHwnd)
		{
			DWORD errorCode = GetLastError();
			LPSTR errorMsg = nullptr;
			FormatMessageA(
				FORMAT_MESSAGE_ALLOCATE_BUFFER
				| FORMAT_MESSAGE_FROM_SYSTEM
				| FORMAT_MESSAGE_IGNORE_INSERTS,
				nullptr,
				errorCode,
				0,
				(LPSTR)&errorMsg,
				0,
				nullptr);

			string message = "CreateWindowExA failed with error "
				+ to_string(errorCode)
				+ ": "
				+ (errorMsg ? errorMsg : "Unknown");

			KalaWindowCore::ForceClose(
				"Window Error",
				message);

			if (errorMsg) LocalFree(errorMsg);

			string title = "Window error";

			KalaWindowCore::ForceClose(title, message);

			return nullptr;
		}

		WindowData newWindowStruct =
		{
			.hwnd = FromVar(newHwnd),
			.hInstance = FromVar(newHInstance),
			.wndProc = FromVar((WNDPROC)GetWindowLongPtr(newHwnd, GWLP_WNDPROC))
		};
		newWindow->SetWindowData(newWindowStruct);

		newWindow->SetInitializedState(true);

		createdWindows[title] = move(newWindow);
		runtimeWindows.push_back(createdWindows[title].get());

		return createdWindows[title].get();
	}

	Window* Window::FindWindowByName(const string& targetName)
	{
		auto it = find_if(
			runtimeWindows.begin(),
			runtimeWindows.end(),
			[targetName](const Window* win)
			{
				return win->title == targetName;
			});

		if (it != runtimeWindows.end()) return *it;

		return nullptr;
	}
	Window* Window::FindWindowByID(unsigned int targetID)
	{
		auto it = find_if(
			runtimeWindows.begin(),
			runtimeWindows.end(),
			[targetID](const Window* win)
			{
				return win->ID == targetID;
			});

		if (it != runtimeWindows.end()) return *it;

		return nullptr;
	}

	void Window::SetTitle(const string& newTitle)
	{
		HWND window = ToVar<HWND>(GetWindowData().hwnd);
		SetWindowTextA(window, newTitle.c_str());

		title = newTitle;
	}

	vec2 Window::GetSize()
	{
		WindowData& winData = GetWindowData();
		HWND hwnd = ToVar<HWND>(winData.hwnd);

		UINT dpi = GetDpiForWindow(hwnd);
		RECT rect{};
		GetClientRect(hwnd, &rect);

		int width = MulDiv(
			rect.right - rect.left, 
			dpi, 
			96);
		int height = MulDiv(
			rect.bottom - rect.top,
			dpi,
			96);

		return vec2
		{
			static_cast<float>(width),
			static_cast<float>(height)
		};
	}

	void Window::SetSize(vec2 newSize)
	{
		HWND window = ToVar<HWND>(GetWindowData().hwnd);

		SetWindowPos(
			window,
			nullptr,
			0,
			0,
			newSize.x,
			newSize.y,
			SWP_NOMOVE
			| SWP_NOZORDER);

		size = newSize;
	}

	vec2 Window::GetPosition()
	{
		HWND window = ToVar<HWND>(GetWindowData().hwnd);

		RECT rect{};
		if (GetWindowRect(window, &rect))
		{
			return vec2
			{ 
				static_cast<float>(rect.left),
				static_cast<float>(rect.top)
			};
		}

		return vec2{ 0, 0 };
	}

	void Window::SetPosition(vec2 newPosition)
	{
		HWND window = ToVar<HWND>(GetWindowData().hwnd);

		SetWindowPos(
			window,
			nullptr,
			newPosition.x,
			newPosition.y,
			0,
			0,
			SWP_NOSIZE
			| SWP_NOZORDER);
	}

	bool Window::IsFocused() const
	{
		HWND hwnd = ToVar<HWND>(window_windows.hwnd);

		return hwnd == GetForegroundWindow();
	}

	bool Window::IsMinimized() const
	{
		HWND hwnd = ToVar<HWND>(window_windows.hwnd);

		//IsIconic returns TRUE if the window is minimized (iconic state)
		return IsIconic(hwnd) == TRUE;
	}

	bool Window::IsVisible() const
	{
		HWND hwnd = ToVar<HWND>(window_windows.hwnd);

		return IsWindowVisible(hwnd) == TRUE;
	}

	void Window::SetWindowState(WindowState state)
	{
		HWND hwnd = ToVar<HWND>(window_windows.hwnd);

		switch (state)
		{
		case WindowState::WINDOW_NORMAL:
			ShowWindow(hwnd, SW_SHOWNORMAL);
			break;
		case WindowState::WINDOW_MAXIMIZE:
			ShowWindow(hwnd, SW_MAXIMIZE);
			break;
		case WindowState::WINDOW_MINIMIZE:
			ShowWindow(hwnd, SW_MINIMIZE);
			break;
		case WindowState::WINDOW_HIDE:
			ShowWindow(hwnd, SW_HIDE);
			break;
		case WindowState::WINDOW_SHOWNOACTIVATE:
			ShowWindow(hwnd, SW_SHOWNOACTIVATE);
			break;
		}

		UpdateWindow(hwnd);
	}

	void Window::Update()
	{
		if (!isInitialized)
		{
			Logger::Print(
				"Cannot run loop because window '" +
				title +
				"' has not been initialized!",
				"WINDOW_WINDOWS",
				LogType::LOG_ERROR,
				2);
			return;
		}

		UpdateIdleState(
			this,
			isIdle);

		MSG msg;
		
		if (isWindowFocusRequired
			&& isIdle
			&& !PeekMessage(&msg, nullptr, 0, 0, PM_NOREMOVE)) 
		{
			WaitMessage();
		}

		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg); //translate virtual-key messages (like WM_KEYDOWN) to character messages (WM_CHAR)
			DispatchMessage(&msg);  //send the message to the window procedure
		}
	}

	Window::~Window()
	{
		WindowData& win = GetWindowData();
		HWND winRef = ToVar<HWND>(win.hwnd);
		SetWindowState(WindowState::WINDOW_HIDE);

		OpenGLData& openGLData = GetOpenGLData();

		if (openGLData.hglrc)
		{
			wglMakeCurrent(nullptr, nullptr);
			wglDeleteContext(ToVar<HGLRC>(openGLData.hglrc));
			openGLData.hglrc = NULL;
		}
		if (win.wndProc) win.wndProc = NULL;
		if (openGLData.hdc)
		{
			ReleaseDC(
				ToVar<HWND>(win.hwnd),
				ToVar<HDC>(openGLData.hdc));
			openGLData.hdc = NULL;
		}

		//Renderer_Vulkan::DestroyWindowData(this);

		if (win.hwnd)
		{
			DestroyWindow(winRef);
			win.hwnd = NULL;
		}
		win.hInstance = NULL;
	}

	
}

void UpdateIdleState(Window* window, bool& isIdle)
{
	isIdle =
		!window->IsFocused()
		|| window->IsMinimized()
		|| !window->IsVisible();
}

#endif //_WIN32