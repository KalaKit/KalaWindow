//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef _WIN32

#define KALAKIT_MODULE "WINDOW"

#include <windows.h>
#include <ShlObj.h>
#include <algorithm>

#ifdef KALAWINDOW_SUPPORT_VULKAN
#include <vulkan/vulkan.h>
#include "graphics/vulkan/vulkan.hpp"
#endif //KALAWINDOW_SUPPORT_VULKAN

#include "graphics/window.hpp"
#include "windows/messageloop.hpp"
#include "core/input.hpp"
#include "graphics/glyph.hpp"

using KalaWindow::Core::MessageLoop;
using KalaWindow::Core::Input;

using std::make_unique;
using std::move;
using std::to_string;
using std::find_if;

namespace KalaWindow::Graphics
{
	static unsigned int nextWindowID = 0;

	Window* Window::Initialize(
		const string& title,
		int width,
		int height)
	{
		unsigned int windowID = nextWindowID++;
		unique_ptr<Window> newWindow = make_unique<Window>(
			title,
			windowID,
			width,
			height);

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
			LOG_ERROR(message);

			string title = "Window initialize error!";

			if (newWindow->CreatePopup(
				title,
				message,
				PopupAction::POPUP_ACTION_OK,
				PopupType::POPUP_TYPE_ERROR)
				== PopupResult::POPUP_RESULT_OK)
			{
				Render::Shutdown();
			}

			return nullptr;
		}

		HWND newHwnd = CreateWindowExA(
			0,
			"KalaWindowClass",
			title.c_str(),
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT,
			width, height,
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

			LOG_ERROR(message);

			if (errorMsg) LocalFree(errorMsg);

			string title = "Window initialize error!";

			if (newWindow->CreatePopup(
				title,
				message,
				PopupAction::POPUP_ACTION_OK,
				PopupType::POPUP_TYPE_ERROR)
				== PopupResult::POPUP_RESULT_OK)
			{
				Render::Shutdown();
			}

			return nullptr;
		}

		Window_OpenGLData oData{};
		Window_VulkanData vData{};
		WindowStruct_Windows newWindowStruct =
		{
			.hwnd = newHwnd,
			.hInstance = newHInstance,
			.wndProc = reinterpret_cast<void*>((WNDPROC)GetWindowLongPtr(newHwnd, GWLP_WNDPROC)),
			.openglData = oData,
			.vulkanData = vData
		};
		newWindow->SetWindow_Windows(newWindowStruct);

		ShowWindow(newHwnd, SW_SHOW);
		UpdateWindow(newHwnd);

		newWindow->SetInitializedState(true);

		windows.push_back(move(newWindow));

		Window* window = FindWindowByID(windowID);

#ifdef KALAWINDOW_SUPPORT_VULKAN
		Renderer_Vulkan::CreateSyncObjects(window);
#endif //KALAWINDOW_SUPPORT_VULKAN

		return FindWindowByID(windowID);
	}

	Window* Window::FindWindowByName(const string& targetName)
	{
		auto it = find_if(
			windows.begin(),
			windows.end(),
			[targetName](const unique_ptr<Window>& win)
			{
				return win->title == targetName;
			});

		if (it != windows.end()) return it->get();

		return nullptr;
	}
	Window* Window::FindWindowByID(unsigned int targetID)
	{
		auto it = find_if(
			windows.begin(),
			windows.end(),
			[targetID](const unique_ptr<Window>& win)
			{
				return win->ID == targetID;
			});

		if (it != windows.end()) return it->get();

		return nullptr;
	}

	bool Window::IsFocused(Window* window) const
	{
		WindowStruct_Windows& win = window->GetWindow_Windows();
		HWND hwnd = reinterpret_cast<HWND>(win.hwnd);

		return hwnd == GetForegroundWindow();
	}

	bool Window::IsMinimized(Window* window) const
	{
		WindowStruct_Windows& win = window->GetWindow_Windows();
		HWND hwnd = reinterpret_cast<HWND>(win.hwnd);

		//IsIconic returns TRUE if the window is minimized (iconic state)
		return IsIconic(hwnd) == TRUE;
	}

	bool Window::IsVisible(Window* window) const
	{
		WindowStruct_Windows& win = window->GetWindow_Windows();
		HWND hwnd = reinterpret_cast<HWND>(win.hwnd);

		return IsWindowVisible(hwnd) == TRUE;
	}

	PopupResult Window::CreatePopup(
		const string& title,
		const string& message,
		PopupAction action,
		PopupType type)
	{
		int flags = 0;

		switch (action)
		{
		case PopupAction::POPUP_ACTION_OK:            flags |= MB_OK; break;
		case PopupAction::POPUP_ACTION_OK_CANCEL:     flags |= MB_OKCANCEL; break;
		case PopupAction::POPUP_ACTION_YES_NO:        flags |= MB_YESNO; break;
		case PopupAction::POPUP_ACTION_YES_NO_CANCEL: flags |= MB_YESNOCANCEL; break;
		case PopupAction::POPUP_ACTION_RETRY_CANCEL:  flags |= MB_RETRYCANCEL; break;
		default:                                      flags |= MB_OK; break;
		}

		switch (type)
		{
		case PopupType::POPUP_TYPE_INFO:     flags |= MB_ICONINFORMATION; break;
		case PopupType::POPUP_TYPE_WARNING:  flags |= MB_ICONWARNING; break;
		case PopupType::POPUP_TYPE_ERROR:    flags |= MB_ICONERROR; break;
		case PopupType::POPUP_TYPE_QUESTION: flags |= MB_ICONQUESTION; break;
		default:                             flags |= MB_ICONINFORMATION; break;
		}

		int result = MessageBox(
			nullptr,
			message.c_str(),
			title.c_str(),
			flags);

		switch (result)
		{
		case IDOK:     return PopupResult::POPUP_RESULT_OK;
		case IDCANCEL: return PopupResult::POPUP_RESULT_CANCEL;
		case IDYES:    return PopupResult::POPUP_RESULT_YES;
		case IDNO:     return PopupResult::POPUP_RESULT_NO;
		case IDRETRY:  return PopupResult::POPUP_RESULT_RETRY;
		default:       return PopupResult::POPUP_RESULT_NONE;
		}
	}

	void Window::Update(Window* targetWindow)
	{
		if (!targetWindow->IsInitialized())
		{
			LOG_ERROR("Cannot run loop because window '" << targetWindow->GetTitle() << "' has not been initialized!");
			return;
		}

		targetWindow->UpdateIdleState(targetWindow);

		MSG msg;
		
		if (targetWindow->IsIdle()
			&& !PeekMessage(&msg, nullptr, 0, 0, PM_NOREMOVE)) 
		{
			WaitMessage();
		}

		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg); //translate virtual-key messages (like WM_KEYDOWN) to character messages (WM_CHAR)
			DispatchMessage(&msg);  //send the message to the window procedure
		}

		Input::EndFrameUpdate();
	}

	void Window::UpdateIdleState(Window* window)
	{
		isIdle =
			!IsFocused(window)
			|| IsMinimized(window)
			|| !IsVisible(window);
	}

	void Window::DeleteWindow(Window* window)
	{
		if (window == nullptr)
		{
			LOG_ERROR("Cannot destroy window because it is nullptr!");
			return;
		}

		WindowStruct_Windows& win = window->GetWindow_Windows();
		HWND winRef = reinterpret_cast<HWND>(win.hwnd);
		ShowWindow(winRef, SW_HIDE);

		GlyphSystem::ClearGlyphs(window);

#ifdef KALAWINDOW_SUPPORT_OPENGL
		if (win.openglData.hglrc)
		{
			wglMakeCurrent(nullptr, nullptr);
			wglDeleteContext(reinterpret_cast<HGLRC>(win.openglData.hglrc));
			win.openglData.hglrc = nullptr;
		}
		if (win.wndProc) win.wndProc = nullptr;
		if (win.openglData.hdc)
		{
			ReleaseDC(
				reinterpret_cast<HWND>(win.hwnd),
				reinterpret_cast<HDC>(win.openglData.hdc));
			win.openglData.hdc = nullptr;
		}
		if (win.wndProc) win.wndProc = nullptr;
#elif KALAWINDOW_SUPPORT_VULKAN
		if (win.vulkanData.surface)
		{
			vkDestroySurfaceKHR(
				reinterpret_cast<VkInstance>(Renderer_Vulkan::instancePtr),
				reinterpret_cast<VkSurfaceKHR>(win.vulkanData.surface),
				nullptr);
			win.vulkanData.surface = NULL;

			Renderer_Vulkan::DestroySyncObjects(window);
		}
#endif //KALAWINDOW_SUPPORT_VULKAN
		if (win.hwnd)
		{
			DestroyWindow(winRef);
			win.hwnd = nullptr;
		}
		win.hInstance = nullptr;

		for (size_t i = 0; i < Window::windows.size(); ++i)
		{
			if (Window::windows[i].get() == window)
			{
				Window::windows.erase(Window::windows.begin() + i);
				break;
			}
		}
	}
}

#endif //_WIN32