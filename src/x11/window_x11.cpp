//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef KALAKIT_X11

#define KALAKIT_MODULE "WINDOW"

#include <memory>
#include <thread>
#include <chrono>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

//external
#include "crashHandler.hpp"
#include "magic_enum.hpp"

//kalawindow
#include "window.hpp"
#include "input.hpp"
#include "opengl.hpp"
#include "opengl_loader.hpp"
#include "internal/window_x11.hpp"

using std::make_unique;
using std::this_thread::sleep_for;
using std::chrono::milliseconds;

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

		//initialize FreeType
		KalaWindow::freeType = make_unique<FreeType>();

        Display* newDisplay = XOpenDisplay(nullptr);
        if (!newDisplay)
        {
            LOG_ERROR("Failed to open X11 display!");
            return false;
        }
		Window_X11::newDisplay = newDisplay;

        ::Window root = DefaultRootWindow(newDisplay);

        XSetWindowAttributes swa;
        swa.event_mask =
            ExposureMask
            | KeyPressMask
            | StructureNotifyMask;

        ::Window newWindow = XCreateWindow(
            newDisplay,
            root,
            0, 0,
            width, height,
            0,
            CopyFromParent,
            InputOutput,
            CopyFromParent,
            CWEventMask,
            &swa
        );

        if (!newWindow)
        {
            LOG_ERROR("Failed to create X11 window!");
            XCloseDisplay(newDisplay);
            return false;
        }
		Window_X11::newWindow = newWindow;

        XStoreName(newDisplay, newWindow, title.c_str());
        XMapWindow(newDisplay, newWindow); //show the window

		XSizeHints* sizeHints = XAllocSizeHints();
		if (sizeHints)
		{
			sizeHints->flags = PMinSize | PMaxSize;

			sizeHints->min_width = 800;
			sizeHints->min_height = 600;

			sizeHints->max_width = 7680;
			sizeHints->max_height = 4320;

			XSetWMNormalHints(
				Window_X11::newDisplay,
				Window_X11::newWindow,
				sizeHints
			);
			XFree(sizeHints);
		}

		GC newGC = XCreateGC(newDisplay, newWindow, 0, nullptr);
		if (!newGC)
		{
			LOG_ERROR("Failed to create graphics context (GC)!");
			XDestroyWindow(newDisplay, newWindow);
			XCloseDisplay(newDisplay);
			return false;
		}
		Window_X11::newGC = newGC;
		Colormap colorMap = DefaultColormap(
			newDisplay,
			DefaultScreen(newDisplay)
		);

		XColor color_green;
		XParseColor(
			newDisplay, 
			colorMap, 
			"#00CC66", 
			&color_green
		);
		XAllocColor(
			newDisplay,
			colorMap,
			&color_green
		);
		XSetForeground(
			newDisplay,
			newGC,
			color_green.pixel
		);

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
        while (XPending(Window_X11::newDisplay))
		{
			XEvent ev;
			XNextEvent(Window_X11::newDisplay, &ev);

			if (ev.type == ConfigureNotify)
			{
				Window_X11::width = ev.xconfigure.width;
				Window_X11::height = ev.xconfigure.height;
			}

			// HANDLE EVENTS HERE (INPUT ETC)
		}

		XFillRectangle(
			Window_X11::newDisplay,
			Window_X11::newWindow,
			Window_X11::newGC,
			0, 0,
			Window_X11::width, Window_X11::height
		);

		XFlush(Window_X11::newDisplay);

		//60 fps
		sleep_for(milliseconds(16));
	}

	void KalaWindow::SwapOpenGLBuffers()
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
			ShowWindow(newWindow, SW_RESTORE);
			break;
		case WindowState::WINDOW_MINIMIZED:
			ShowWindow(newWindow, SW_MINIMIZE);
			break;
		case WindowState::WINDOW_MAXIMIZED:
			ShowWindow(newWindow, SW_MAXIMIZE);
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

		LONG style = GetWindowLong(newWindow, GWL_STYLE);
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
			originalStyle = GetWindowLong(newWindow, GWL_STYLE);
			GetWindowPlacement(window, &originalPlacement);

			//set style to borderless
			SetWindowLong(window, GWL_STYLE, WS_POPUP);

			//resize to full monitor
			MONITORINFO mi = { sizeof(mi) };
			GetMonitorInfo(MonitorFromWindow(newWindow, MONITOR_DEFAULTTONEAREST), &mi);
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
			SetWindowLong(newWindow, GWL_STYLE, originalStyle);

			//restore previous size/position
			SetWindowPlacement(newWindow, &originalPlacement);
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

		return !IsWindowVisible(newWindow);
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
		GetWindowRect(newWindow, &rect);

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
			newWindow,
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
		GetWindowRect(newWindow, &rect);

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
			newWindow,
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
		GetClientRect(newWindow, &rect);

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
				newWindow,
				GWL_STYLE),
			FALSE);

		SetWindowPos(
			newWindow,
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
		string zenityCommand = "zenity ";

		//dialog type
		switch (action)
		{
		case PopupAction::POPUP_ACTION_OK:
			zenityCommand += "--info ";
			break;
		case PopupAction::POPUP_ACTION_OK_CANCEL:
		case PopupAction::POPUP_ACTION_RETRY_CANCEL:
			zenityCommand += "--question ";
			zenityCommand += "--ok-label='OK' --cancel-label='Cancel' ";
			break;
		case PopupAction::POPUP_ACTION_YES_NO:
		case PopupAction::POPUP_ACTION_YES_NO_CANCEL:
			zenityCommand += "--question ";
			break;
		default:
			zenityCommand += "--info ";
			break;
		}

		//icon type
		switch (type)
		{
		case PopupType::POPUP_TYPE_WARNING:
			zenityCommand += "--icon=dialog-warning ";
			break;
		case PopupType::POPUP_TYPE_ERROR:
			zenityCommand += "--icon=dialog-error ";
			break;
		case PopupType::POPUP_TYPE_QUESTION:
			zenityCommand += "--icon=dialog-question ";
			break;
		case PopupType::POPUP_TYPE_INFO:
		default:
        	zenityCommand += "--icon=dialog-information ";
        	break;
		}

		zenityCommand += "--title=\"" + title + "\" ";
		zenityCommand += "--text=\"" + message + "\" ";

		int result = system(zenityCommand.c_str());

		//what input user chose
		switch (action)
		{
		case PopupAction::POPUP_ACTION_OK:
			return result == 0 ? PopupResult::POPUP_RESULT_OK : PopupResult::POPUP_RESULT_NONE;
		case PopupAction::POPUP_ACTION_OK_CANCEL:
		case PopupAction::POPUP_ACTION_RETRY_CANCEL:
			return result == 0 ? PopupResult::POPUP_RESULT_OK : PopupResult::POPUP_RESULT_CANCEL;
		case PopupAction::POPUP_ACTION_YES_NO:
		case PopupAction::POPUP_ACTION_YES_NO_CANCEL:
			return result == 0 ? PopupResult::POPUP_RESULT_YES : PopupResult::POPUP_RESULT_NO;
		default:
        	return PopupResult::POPUP_RESULT_NONE;
		}
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

#endif // KALAKIT_X11