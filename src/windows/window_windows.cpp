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

	unique_ptr<Window> Window::Initialize(
		const string& title,
		kvec2 size)
	{
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
			LOG_ERROR(message);

			string title = "Window initialize error!";

			if (newWindow->CreatePopup(
				title,
				message,
				PopupAction::POPUP_ACTION_OK,
				PopupType::POPUP_TYPE_ERROR)
				== PopupResult::POPUP_RESULT_OK)
			{
				Render::Shutdown(ShutdownState::SHUTDOWN_FAILURE);
			}

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
				Render::Shutdown(ShutdownState::SHUTDOWN_FAILURE);
			}

			return nullptr;
		}

		Window_OpenGLData oData{};
		Window_VulkanData vData{};

		WindowStruct_Windows newWindowStruct =
		{
			.hwnd = FromVar<HWND>(newHwnd),
			.hInstance = FromVar<HINSTANCE>(newHInstance),
			.wndProc = FromVar<WNDPROC>((WNDPROC)GetWindowLongPtr(newHwnd, GWLP_WNDPROC)),
			.openglData = oData,
			.vulkanData = vData
		};
		newWindow->SetWindow_Windows(newWindowStruct);

		ShowWindow(newHwnd, SW_SHOW);
		UpdateWindow(newHwnd);

		newWindow->SetInitializedState(true);

		windows.push_back(newWindow.get());

		return newWindow;
	}

	Window* Window::FindWindowByName(const string& targetName)
	{
		auto it = find_if(
			windows.begin(),
			windows.end(),
			[targetName](const Window* win)
			{
				return win->title == targetName;
			});

		if (it != windows.end()) return *it;

		return nullptr;
	}
	Window* Window::FindWindowByID(unsigned int targetID)
	{
		auto it = find_if(
			windows.begin(),
			windows.end(),
			[targetID](const Window* win)
			{
				return win->ID == targetID;
			});

		if (it != windows.end()) return *it;

		return nullptr;
	}

	void Window::SetTitle(const string& newTitle)
	{
		HWND window = ToVar<HWND>(this->GetWindow_Windows().hwnd);
		SetWindowTextA(window, newTitle.c_str());

		this->title = newTitle;
	}

	void Window::SetSize(kvec2 newSize)
	{
		HWND window = ToVar<HWND>(this->GetWindow_Windows().hwnd);

		SetWindowPos(
			window,
			nullptr,
			0,
			0,
			newSize.x,
			newSize.y,
			SWP_NOMOVE
			| SWP_NOZORDER);

		this->size = newSize;
	}

	kvec2 Window::GetPosition()
	{
		HWND window = ToVar<HWND>(this->GetWindow_Windows().hwnd);

		RECT rect{};
		if (GetWindowRect(window, &rect))
		{
			return kvec2
			{ 
				static_cast<float>(rect.left),
				static_cast<float>(rect.top)
			};
		}

		return kvec2{ 0, 0 };
	}

	void Window::SetPosition(kvec2 newPosition)
	{
		HWND window = ToVar<HWND>(this->GetWindow_Windows().hwnd);

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

	bool Window::IsFocused(Window* window) const
	{
		WindowStruct_Windows& win = window->GetWindow_Windows();
		HWND hwnd = ToVar<HWND>(win.hwnd);

		return hwnd == GetForegroundWindow();
	}

	bool Window::IsMinimized(Window* window) const
	{
		WindowStruct_Windows& win = window->GetWindow_Windows();
		HWND hwnd = ToVar<HWND>(win.hwnd);

		//IsIconic returns TRUE if the window is minimized (iconic state)
		return IsIconic(hwnd) == TRUE;
	}

	bool Window::IsVisible(Window* window) const
	{
		WindowStruct_Windows& win = window->GetWindow_Windows();
		HWND hwnd = ToVar<HWND>(win.hwnd);

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
		HWND winRef = ToVar<HWND>(win.hwnd);
		ShowWindow(winRef, SW_HIDE);

		GlyphSystem::ClearGlyphs(window);

#ifdef KALAWINDOW_SUPPORT_OPENGL
		if (win.openglData.hglrc)
		{
			wglMakeCurrent(nullptr, nullptr);
			wglDeleteContext(ToVar<HGLRC>(win.openglData.hglrc));
			win.openglData.hglrc = NULL;
		}
		if (win.wndProc) win.wndProc = NULL;
		if (win.openglData.hdc)
		{
			ReleaseDC(
				ToVar<HWND>(win.hwnd),
				ToVar<HDC>(win.openglData.hdc));
			win.openglData.hdc = NULL;
		}
		if (win.wndProc) win.wndProc = NULL;
#elif KALAWINDOW_SUPPORT_VULKAN
		Renderer_Vulkan::DestroyWindowData(window);
#endif //KALAWINDOW_SUPPORT_VULKAN
		if (win.hwnd)
		{
			DestroyWindow(winRef);
			win.hwnd = NULL;
		}
		win.hInstance = NULL;

		for (size_t i = 0; i < Window::windows.size(); ++i)
		{
			if (Window::windows[i] == window)
			{
				Window::windows.erase(Window::windows.begin() + i);
				break;
			}
		}
	}
}

#endif //_WIN32