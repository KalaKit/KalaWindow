//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef KALAKIT_WAYLAND

#define KALAKIT_MODULE "WINDOW"

#include <cstring>
#include <wayland-egl.h>
#include <xdg-shell-client-protocol.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

//external
#include "crashHandler.hpp"
#include "magic_enum.hpp"

//kalawindow
#include "window.hpp"
#include "input.hpp"
#include "opengl.hpp"
#include "opengl_loader.hpp"
#include "internal/window_wayland.hpp"

using std::strcmp;
using std::to_string;

namespace KalaKit
{
    bool KalaWindow::Initialize(
        const string& title,
        int width,
        int height,
		bool initializeOpenGL)
    {
        if (isInitialized)
        {
            LOG_ERROR("Window is already initialized!");
            return false;
        }

        //initialize the crash handler first
        KalaCrashHandler::Initialize();

		//
		// CREATE A NEW WAYLAND DISPLAY
		//

        wl_display* newDisplay = Window_Wayland::CreateDisplay();
		if (!newDisplay)
        {
            LOG_ERROR("Failed to connect to Wayland display!");
			return false;
        }

		//
		// GET STRUCTS FROM REGISTRY
		//

        Window_Wayland::SetupRegistry(newDisplay);
		wl_compositor* compositor = Window_Wayland::GetCompositor();
		wl_shm* shm = Window_Wayland::GetSHM();
		xdg_wm_base* xdgWmBase = Window_Wayland::GetWMBase();

		if (!compositor)
		{
			LOG_ERROR("Failed to get wl_compositor from Wayland registry!");
			wl_display_disconnect(newDisplay);
			return false;
		}
		if (!shm)
		{
			LOG_ERROR("Failed to get shm from Wayland registry!");
			wl_display_disconnect(newDisplay);
			return false;
		}
		if (!xdgWmBase)
		{
			LOG_ERROR("Failed to get xdg wm base from Wayland registry!");
			wl_display_disconnect(newDisplay);
			return false;
		}

        //
		// CREATE THE DRAWABLE AREA
		//

        wl_surface* newSurface = Window_Wayland::CreateSurface();
		if (!newSurface)
        {
            LOG_ERROR("Failed to create Wayland surface!");
            wl_display_disconnect(newDisplay);
            return false;
        }

		//
		// ADDS ROLE, TITLE AND DECORATIONS
		//

		Window_Wayland::AddDecorations(title, newDisplay, newSurface);

		//
		// CREATE A BUFFER SO CONTENT IS SHOWN
		//

		bool createdBuffer = Window_Wayland::CreateBuffer(
			width,
			height,
			newDisplay,
			newSurface
		);
		if (!createdBuffer)
		{
			LOG_ERROR("Failed to create Wayland buffer!");
            wl_display_disconnect(newDisplay);
			return false;
		}

		//
		// REST OF THE INITIALIZATION
		//

        //initialize input
        KalaInput::Initialize();

		if (initializeOpenGL)
		{
        	//initialize opengl
        	if (!OpenGL::Initialize()) return false;

        	//and finally set opengl viewport size
        	OpenGLLoader::glViewportPtr(0, 0, width, height);
		}

        isInitialized = true;

        LOG_SUCCESS("Window successfully initialized");
        return true;
    }

    void KalaWindow::Update()
	{
		if (!isInitialized)
		{
			LOG_ERROR("Cannot run loop because KalaWindow is not initialized!");
			return;
		}

		static wl_display* display{};
		if (display == nullptr)
		{
			display = reinterpret_cast<wl_display*>(KalaWindow::waylandDisplay.get());
			if (display == nullptr)
			{
				LOG_ERROR("Failed to get wayland display! Shutting down...");
				SetShouldCloseState(true);
				return;
			}
			else
			{
				LOG_DEBUG("Update loop has valid wayland display!");
			}
		}

		//process pending events and check if the display is still valid
		int display_status = wl_display_dispatch_pending(display);
		if (debugType == DebugType::DEBUG_ALL
			|| debugType == DebugType::DEBUG_WAYLAND_DISPLAY_CHECK)
		{
			LOG_DEBUG("Update loop display status: '" + to_string(display_status) + "'");
		}
		if (display_status == -1)
		{
			LOG_ERROR("Display connection lost! Shutting down...");
			SetShouldCloseState(true);
			return;
		}

		//flush any outgoing requests to the wayland server
		int display_flush_status = wl_display_flush(display);
		if (debugType == DebugType::DEBUG_ALL
			|| debugType == DebugType::DEBUG_WAYLAND_DISPLAY_CHECK)
		{
			LOG_DEBUG("Update loop display flush status: '" + to_string(display_flush_status) + "'");
		}
		if (display_flush_status == -1)
		{
			LOG_ERROR("Failed to flush wayland display! Shutting down...");
		}

		//
		// RENDER LOOP CONTENT HERE
		//

		if (wl_display_dispatch(display) == -1)
		{
			LOG_ERROR("Wayland display dispatch failed! Shutting down...");
			SetShouldCloseState(true);
		}
	}

    void KalaWindow::SwapBuffers(const OPENGLCONTEXT& context)
    {

    }

	void KalaWindow::SetWindowFocusRequiredState(bool newWindowFocusRequiredState)
	{
		if (!isInitialized)
		{
			LOG_ERROR("Cannot set window focus required state because KalaWindow is not initialized!");
			return;
		}


		isWindowFocusRequired = newWindowFocusRequiredState;
	}

	DebugType KalaWindow::GetDebugType()
	{
		return debugType;
	}
	void KalaWindow::SetDebugType(DebugType newDebugType)
	{
		debugType = newDebugType;
	}

	void KalaWindow::SetWindowTitle(const string& title)
	{
        /*
		if (!isInitialized)
		{
			LOG_ERROR("Cannot set window title because KalaWindow is not initialized!");
			return;
		}

		if (debugType == DebugType::DEBUG_ALL
			|| debugType == DebugType::DEBUG_WINDOW_TITLE)
		{
			LOG_DEBUG("New window title: " << title << "");
		}

		SetWindowTextA(window, title.c_str());
        */
	}

	void KalaWindow::SetWindowState(WindowState state)
	{
        /*
		if (!isInitialized)
		{
			LOG_ERROR("Cannot set window state because KalaWindow is not initialized!");
			return;
		}

		if (debugType == DebugType::DEBUG_ALL
			|| debugType == DebugType::DEBUG_WINDOW_TITLE)
		{
			string type = ToString(state);
			LOG_DEBUG("New window type: " << type << "");
		}

		switch (state)
		{
		case WindowState::WINDOW_RESET:
			ShowWindow(window->handle, SW_RESTORE);
			break;
		case WindowState::WINDOW_MINIMIZED:
			ShowWindow(window->handle, SW_MINIMIZE);
			break;
		case WindowState::WINDOW_MAXIMIZED:
			ShowWindow(window->handle, SW_MAXIMIZE);
			break;
		}
        */
	}

	bool KalaWindow::IsWindowBorderless()
	{
        /*
		if (!isInitialized)
		{
			LOG_ERROR("Cannot get window borderless state because KalaWindow is not initialized!");
			return false;
		}

		LONG style = GetWindowLong(window->handle, GWL_STYLE);
		return (style & WS_OVERLAPPEDWINDOW);
        */
       return false;
	}
	void KalaWindow::SetWindowBorderlessState(bool newWindowBorderlessState)
	{
        /*
		if (!isInitialized)
		{
			LOG_ERROR("Cannot set window borderless state because KalaWindow is not initialized!");
			return;
		}

		if (debugType == DebugType::DEBUG_ALL
			|| debugType == DebugType::DEBUG_WINDOW_BORDERLESS_STATE)
		{
			string type = newWindowBorderlessState ? "true" : "false";
			LOG_DEBUG("New window borderless state: " << type << "");
		}

		if (!newWindowBorderlessState)
		{
			//save original style and placement
			originalStyle = GetWindowLong(window->handle, GWL_STYLE);
			GetWindowPlacement(window, &originalPlacement);

			//set style to borderless
			SetWindowLong(window, GWL_STYLE, WS_POPUP);

			//resize to full monitor
			MONITORINFO mi = { sizeof(mi) };
			GetMonitorInfo(MonitorFromWindow(window->handle, MONITOR_DEFAULTTONEAREST), &mi);
			SetWindowPos(
				window,
				HWND_TOP,
				mi.rcMonitor.left, mi.rcMonitor.top,
				mi.rcMonitor.right - mi.rcMonitor.left,
				mi.rcMonitor.bottom - mi.rcMonitor.top,
				SWP_FRAMECHANGED
				| SWP_NOOWNERZORDER
				| SWP_SHOWWINDOW);

			isWindowBorderless = true;
		}
		else
		{
			//restore original style
			SetWindowLong(window->handle, GWL_STYLE, originalStyle);

			//restore previous size/position
			SetWindowPlacement(window->handle, &originalPlacement);
			SetWindowPos(
				window,
				nullptr,
				0,
				0,
				0,
				0,
				SWP_NOMOVE
				| SWP_NOSIZE
				| SWP_FRAMECHANGED
				| SWP_NOOWNERZORDER
				| SWP_NOZORDER
				| SWP_SHOWWINDOW);

			isWindowBorderless = true;

		}
        */
	}

	bool KalaWindow::IsWindowHidden()
	{
        /*
		if (!isInitialized)
		{
			LOG_ERROR("Cannot get window hidden state because KalaWindow is not initialized!");
			return false;
		}

		return !IsWindowVisible(window->handle);
        */
       return false;
	}
	void KalaWindow::SetWindowHiddenState(bool newWindowHiddenState)
	{
        /*
		if (!isInitialized)
		{
			LOG_ERROR("Cannot set window hidden state because KalaWindow is not initialized!");
			return;
		}

		if (debugType == DebugType::DEBUG_ALL
			|| debugType == DebugType::DEBUG_WINDOW_HIDDEN_STATE)
		{
			string type = newWindowHiddenState ? "true" : "false";
			LOG_DEBUG("New window hidden state: " << type << "");
		}

		ShowWindow(window, newWindowHiddenState ? SW_HIDE : SW_SHOW);
        */
	}

	POS KalaWindow::GetWindowPosition()
	{
        /*
		if (!isInitialized)
		{
			LOG_ERROR("Cannot get window position because KalaWindow is not initialized!");
			return { 0, 0 };
		}

		RECT rect{};
		GetWindowRect(window->handle, &rect);

		POS pos{};
		pos.x = rect.left;
		pos.y = rect.top;
		return pos;
        */
       return {0, 0};
	}
	void KalaWindow::SetWindowPosition(int width, int height)
	{
        /*
		if (!isInitialized)
		{
			LOG_ERROR("Cannot set window position because KalaWindow is not initialized!");
			return;
		}

		if (debugType == DebugType::DEBUG_ALL
			|| debugType == DebugType::DEBUG_WINDOW_SET_POSITION)
		{
			string sizeX = to_string(width);
			string sizeY = to_string(height);

			LOG_DEBUG("New window position: (" << width << ", " << height);
		}

		SetWindowPos(
			window->handle,
			nullptr,
			width,
			height,
			0,
			0,
			SWP_NOSIZE
			| SWP_NOZORDER
			| SWP_NOOWNERZORDER);
        */
	}

	POS KalaWindow::GetWindowFullSize()
	{
        /*
		if (!isInitialized)
		{
			LOG_ERROR("Cannot get window full size because KalaWindow is not initialized!");
			return { 0, 0 };
		}

		RECT rect{};
		GetWindowRect(window->handle, &rect);

		POS size{};
		size.x = rect.right - rect.left;
		size.y = rect.bottom - rect.top;
		return size;
        */
       return {0, 0};
	}
	void KalaWindow::SetWindowFullSize(int width, int height)
	{
        /*
		if (!isInitialized)
		{
			LOG_ERROR("Cannot set window full size because KalaWindow is not initialized!");
			return;
		}

		if (debugType == DebugType::DEBUG_ALL
			|| debugType == DebugType::DEBUG_WINDOW_SET_FULL_SIZE)
		{
			string sizeX = to_string(width);
			string sizeY = to_string(height);

			LOG_DEBUG("New window full size: (" << width << ", " << height);
		}

		SetWindowPos(
			window->handle,
			nullptr,
			0,
			0,
			width,
			height,
			SWP_NOMOVE
			| SWP_NOZORDER
			| SWP_NOOWNERZORDER);
        */
	}

	POS KalaWindow::GetWindowContentSize()
	{
        /*
		if (!isInitialized)
		{
			LOG_ERROR("Cannot get window content size because KalaWindow is not initialized!");
			return { 0, 0 };
		}

		RECT rect{};
		GetClientRect(window->handle, &rect);

		POS size{};
		size.x = rect.right - rect.left;
		size.y = rect.bottom - rect.top;
		return size;
        */
       return {0, 0};
	}
	void KalaWindow::SetWindowContentSize(int width, int height)
	{
        /*
		if (!isInitialized)
		{
			LOG_ERROR("Cannot set window content size because KalaWindow is not initialized!");
			return;
		}

		if (debugType == DebugType::DEBUG_ALL
			|| debugType == DebugType::DEBUG_WIŃDOW_SET_CONTENT_SIZE)
		{
			string sizeX = to_string(width);
			string sizeY = to_string(height);

			LOG_DEBUG("New window content size: (" << width << ", " << height);
		}

		RECT rect = { 0, 0, width, height };
		AdjustWindowRect(
			&rect,
			GetWindowLong(
				window->handle,
				GWL_STYLE),
			FALSE);

		SetWindowPos(
			window->handle,
			nullptr,
			0,
			0,
			rect.right - rect.left,
			rect.bottom - rect.top,
			SWP_NOMOVE
			| SWP_NOZORDER
			| SWP_NOOWNERZORDER);
        */
	}

	POS KalaWindow::GetWindowMaxSize()
	{
		if (!isInitialized)
		{
			LOG_ERROR("Cannot get window max size because KalaWindow is not initialized!");
			return { 0, 0 };
		}

		POS point{};
		point.x = maxWidth;
		point.y = maxHeight;
		return point;
	}
	POS KalaWindow::GetWindowMinSize()
	{
		if (!isInitialized)
		{
			LOG_ERROR("Cannot get window min size because KalaWindow is not initialized!");
			return { 0, 0 };
		}

		POS point{};
		point.x = minWidth;
		point.y = minHeight;
		return point;
	}
	void KalaWindow::SetMinMaxSize(
		int newMaxWidth,
		int newMaxHeight,
		int newMinWidth,
		int newMinHeight)
	{
		if (!isInitialized)
		{
			LOG_ERROR("Cannot set window max and min size because KalaWindow is not initialized!");
			return;
		}

		if (debugType == DebugType::DEBUG_ALL
			|| debugType == DebugType::DEBUG_WINDOW_SET_MINMAX_SIZE)
		{
			LOG_DEBUG("Set new max size: " << maxWidth << ", " << maxHeight 
				<< ", min size:" << minWidth << ", " << minHeight);
		}

		maxWidth = newMaxWidth;
		maxHeight = newMaxHeight;
		minWidth = newMinWidth;
		minHeight = newMinHeight;
	}

	bool KalaWindow::CanExit()
	{
		return canExit;
	}

	string KalaWindow::ToString(WindowState state)
	{
		return string(magic_enum::enum_name(state));
	}

	bool KalaWindow::ShouldClose()
	{
		return shouldClose;
	}
	void KalaWindow::SetShouldCloseState(bool newShouldCloseState)
	{
		shouldClose = newShouldCloseState;
	}

	PopupResult KalaWindow::CreatePopup(
		const string& title,
		const string& message,
		PopupAction action,
		PopupType type)
	{
        /*
		int flags = 0;

		switch (action)
		{
		case PopupAction::POPUP_ACTION_OK: flags |= MB_OK; break;
		case PopupAction::POPUP_ACTION_OK_CANCEL: flags |= MB_OKCANCEL; break;
		case PopupAction::POPUP_ACTION_YES_NO: flags |= MB_YESNO; break;
		case PopupAction::POPUP_ACTION_YES_NO_CANCEL: flags |= MB_YESNOCANCEL; break;
		case PopupAction::POPUP_ACTION_RETRY_CANCEL: flags |= MB_RETRYCANCEL; break;
		default:
			flags |= MB_OK;
			break;
		}

		switch (type)
		{
		case PopupType::POPUP_TYPE_INFO: flags |= MB_ICONINFORMATION; break;
		case PopupType::POPUP_TYPE_WARNING: flags |= MB_ICONWARNING; break;
		case PopupType::POPUP_TYPE_ERROR: flags |= MB_ICONERROR; break;
		case PopupType::POPUP_TYPE_QUESTION: flags |= MB_ICONQUESTION; break;
		default:
			flags |= MB_ICONINFORMATION;
			break;
		}

		int result = MessageBox(
			nullptr,
			message.c_str(),
			title.c_str(),
			flags);

		//cast the result directly to your strongly-typed enum
		return static_cast<PopupResult>(result);
        */
       return PopupResult::POPUP_RESULT_NONE;
	}

	void KalaWindow::SetExitState(
		bool setExitAllowedState,
		const string& title,
		const string& info)
	{
		if (!isInitialized)
		{
			LOG_ERROR("Cannot set exit state because KalaWindow is not initialized!");
			return;
		}

		canExit = setExitAllowedState;
		exitTitle = title;
		exitInfo = info;
	}
}

#endif // KALAKIT_WAYLAND