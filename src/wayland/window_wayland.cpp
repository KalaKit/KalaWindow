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

using std::strcmp;

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
		// SET UP DISPLAY
		//

        struct wl_display* newDisplay = wl_display_connect(nullptr);
        if (!newDisplay)
        {
            LOG_ERROR("Failed to connect to Wayland display!");
            return false;
        }
        
		uintptr_t rawDisplay = reinterpret_cast<uintptr_t>(newDisplay);
		KalaWindow::waylandDisplay = display_way(rawDisplay);

		//
		// GET STRUCTS FROM REGISTRY
		//

        struct wl_registry* registry = wl_display_get_registry(newDisplay);
        if (!registry)
        {
            LOG_ERROR("Failed to get Wayland registry!");
            wl_display_disconnect(newDisplay);
            return false;
        }

        static struct wl_compositor* compositor = nullptr;
		static struct wl_shm* shm = nullptr;
		static struct xdg_wm_base* xdgWmBase = nullptr;

        static const struct wl_registry_listener registryListener =
        {
            .global = [](
                void* data, 
                struct wl_registry* registry, 
                uint32_t name,
                const char* interface,
                uint32_t version)
            {
                if (strcmp(interface, "wl_compositor") == 0)
                {
                    //get access to the display server
                    compositor = (wl_compositor*)wl_registry_bind(
                        registry,
                        name,
                        &wl_compositor_interface,
                        1
                    );
                }
				else if (strcmp(interface, "wl_shm") == 0)
				{
					shm = static_cast<wl_shm*>(
						wl_registry_bind(
							registry,
							name,
							&wl_shm_interface,
							1
						)
					);
				}
				else if (strcmp(interface, "xdg_wm_base") == 0)
				{
					xdgWmBase = static_cast<xdg_wm_base*>(
						wl_registry_bind(
							registry,
							name,
							&xdg_wm_base_interface,
							1
						)
					);
				}
            },
            .global_remove = [](void*, struct wl_registry*, uint32_t) {}
        };

        wl_registry_add_listener(registry, &registryListener, nullptr);
        wl_display_roundtrip(newDisplay);

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

        wl_surface* newSurface = wl_compositor_create_surface(compositor);
        if (!newSurface)
        {
            LOG_ERROR("Failed to create Wayland surface!");
            wl_display_disconnect(newDisplay);
            return false;
        }

		uintptr_t rawSurface = reinterpret_cast<uintptr_t>(newSurface);
		KalaWindow::waylandSurface = surface_way(rawSurface);

		//
		// ADDS ROLE, TITLE AND DECORATIONS
		//

		struct xdg_surface* xdgSurface = xdg_wm_base_get_xdg_surface(xdgWmBase, newSurface);
		struct xdg_toplevel* xdgTopLevel = xdg_surface_get_toplevel(xdgSurface);

		//set window title
		xdg_toplevel_set_title(xdgTopLevel, title.c_str());

		//set window size
		xdg_toplevel_set_app_id(xdgTopLevel, "my-app-id");

		//add a listener to handle configure events
		static const struct xdg_surface_listener surface_listener =
		{
			.configure = [](
				void* data,
				struct xdg_surface* surface,
				uint32_t serial)
			{
				xdg_surface_ack_configure(surface, serial);
			}
		};
		xdg_surface_add_listener(xdgSurface, &surface_listener, newSurface);

		//and finally commit the surface
		wl_surface_commit(newSurface);

		//
		// CREATE A DUMMY BUFFER
		//

		int stride = width * 4;
		int size = stride * height;
		
		//create an in-memory file
		int fd = memfd_create(
			"wayland-shm", 
			MFD_CLOEXEC
			| MFD_ALLOW_SEALING);
		if (fd >= 0)
		{
			if (ftruncate(fd, size) < 0)
			{
				LOG_ERROR("FTruncate failed!");
				close(fd);
				wl_display_disconnect(newDisplay);
				return false;
			}
		}

		void* data = mmap(
			nullptr,
			size,
			PROT_READ
			| PROT_WRITE,
			MAP_SHARED,
			fd,
			0
		);
		if (data == MAP_FAILED)
		{
			LOG_ERROR("Failed to mmap shm file!");
			close(fd);
			wl_display_disconnect(newDisplay);
			return false;
		}

		memset(data, 0, size); //black pixels

		wl_shm_pool* pool = wl_shm_create_pool(shm, fd, size);
		wl_buffer* buffer = wl_shm_pool_create_buffer(
			pool,
			0,
			width,
			height,
			stride,
			WL_SHM_FORMAT_XRGB8888
		);

		//attach to surface
		wl_surface_attach(newSurface, buffer, 0, 0);
		wl_surface_commit(newSurface);

        wl_egl_window* newRenderTarget = wl_egl_window_create(
            newSurface,
            width,
            height
        );
        if (!newRenderTarget)
        {
            LOG_ERROR("Failed to create egl window!");
            wl_display_disconnect(newDisplay);
            return false;
        }
		uintptr_t rawRenderTarget = reinterpret_cast<uintptr_t>(newRenderTarget);
		KalaWindow::waylandRenderTarget = target_way(rawRenderTarget);

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
		}

		static wl_surface* surface{};
		if (surface == nullptr)
		{
			surface = reinterpret_cast<wl_surface*>(KalaWindow::waylandSurface.get());
		}

		//process pending events
		wl_display_dispatch_pending(display);

		//flush any outgoing requests to the wayland server
		wl_display_flush(display);

		//
		// RENDER LOOP CONTENT HERE
		//

		//finish render loop
		wl_surface_commit(surface);
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