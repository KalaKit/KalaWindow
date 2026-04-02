//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef __linux__

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/X.h>

#include <memory>
#include <sstream>

#include "core_utils.hpp"
#include "log_utils.hpp"

#include "graphics/kw_window.hpp"
#include "core/kw_core.hpp"
#include "core/kw_input.hpp"
#include "graphics/kw_window_global.hpp"
#include "opengl/kw_opengl.hpp"
#include "vulkan/kw_vulkan.hpp"

using KalaHeaders::KalaCore::ToVar;
using KalaHeaders::KalaCore::FromVar;

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;

using KalaWindow::Core::KalaWindowCore;
using KalaWindow::Core::Input;
using KalaWindow::OpenGL::OpenGL_Context;
using KalaWindow::Vulkan::Vulkan_Context;
using KalaWindow::Graphics::ProcessWindow;
using KalaWindow::Graphics::X11GlobalData;
using KalaWindow::Graphics::WindowMode;
using KalaWindow::Graphics::WindowState;

using std::make_unique;
using std::unique_ptr;
using std::to_string;
using std::ostringstream;

constexpr u16 MAX_TITLE_LENGTH = 50;

//KalaWindow will dynamically update window idle state
static void UpdateIdleState(ProcessWindow* window, bool& isIdle)
{
	isIdle =
		!window->IsForegroundWindow()
		|| window->IsMinimized()
		|| !window->IsVisible();
}

namespace KalaWindow::Graphics
{
	static KalaWindowRegistry<ProcessWindow> registry{};

	KalaWindowRegistry<ProcessWindow>& ProcessWindow::GetRegistry() { return registry; }

    ProcessWindow* ProcessWindow::Initialize(
		string_view title,
		ProcessWindow* parentWindow,
		DpiContext context)
    {
		if (!Window_Global::IsInitialized())
		{
			KalaWindowCore::ForceClose(
				"Window error",
				"Failed to create window '" + string(title) + "' because global window context has not been created!");

			return nullptr;
		}

        const X11GlobalData& globalData = Window_Global::GetGlobalData();

        if (!globalData.display)
        {
            KalaWindowCore::ForceClose(
                "Window error",
                "Failed to initialize window because the attached display was invalid!");
        }

        u32 newID = KalaWindowCore::GetGlobalID() + 1;
		KalaWindowCore::SetGlobalID(newID);

		unique_ptr<ProcessWindow> newWindow = make_unique<ProcessWindow>();
		ProcessWindow* windowPtr = newWindow.get();

        Display* display = ToVar<Display*>(globalData.display);

        Window root = ToVar<Window>(globalData.window_root);
        Window parent = parentWindow
            ? ToVar<Window>(parentWindow->windowData.window)
            : 0;

        XIM xim = ToVar<XIM>(globalData.xim);

        bool isChild = parentWindow;

        Atom atom_wm_delete = ToVar<Atom>(globalData.atom_wm_delete);

        Window window = XCreateSimpleWindow(
            display,
            root,
            800,
            800,
            800,
            800,
            1,
            BlackPixel(display, DefaultScreen(display)),
            WhitePixel(display, DefaultScreen(display)));

        //disable background pix map for cleaner redraw
        XSetWindowBackgroundPixmap(display, window, None);

        if (isChild)
        {
            //assign parent window
            XSetTransientForHint(
                display,
                window,
                parent);
        }

        XIC xic = XCreateIC(
            xim,
            XNInputStyle,
            XIMPreeditNothing | XIMStatusNothing,
            XNClientWindow, window,
            XNFocusWindow, window,
            nullptr);

        if (!xic)
        {
            KalaWindowCore::ForceClose(
                "Window error",
                "Failed to create XIC for window '" + string(title) + "'");
        }

        Atom net_wm_pid = ToVar<Atom>(globalData.atom_net_wm_pid);

        //set task manager title via PID
        pid_t pid = getpid();
        XChangeProperty(
            display,
            window,
            net_wm_pid,
            XA_CARDINAL,
            32,
            PropModeReplace,
            rcast<unsigned char*>(&pid),
            1);

        XSetWMProtocols(
            display,
            window,
            &atom_wm_delete,
            1);

        //allow events
        XSelectInput(
            display,
            window,
            ExposureMask
            | StructureNotifyMask
            | PropertyChangeMask
            | FocusChangeMask
            | EnterWindowMask
            | LeaveWindowMask
            | KeyPressMask
            | KeyReleaseMask
            | ButtonPressMask
            | ButtonReleaseMask
            | PointerMotionMask);

        WindowData newWindowStruct{};

        //set window dpi aware state here...

        newWindowStruct.window = FromVar(window);
        newWindowStruct.xic = FromVar(xic);

        windowPtr->windowData = newWindowStruct;

        windowPtr->SetTitle(title);
		windowPtr->ID = newID;

        windowPtr->SetWindowClass(title);

		windowPtr->isInitialized = true;

        windowPtr->pos = vec2(800);
        windowPtr->size = vec2(800);

		windowPtr->oldPos = vec2(800);
		windowPtr->oldSize = vec2(800);

        //show window
        XMapWindow(
            display,
            window);

		//allow files to be dragged to this window
		//DragAcceptFiles(newHwnd, TRUE);

		registry.AddContent(newID, std::move(newWindow));

		Log::Print(
			"Created new window '" + string(title) + "' with ID '" + to_string(newID) + "'!",
			"KW_WINDOW",
			LogType::LOG_SUCCESS);

		return windowPtr;
    }

    bool ProcessWindow::IsInitialized() const { return isInitialized; }

	u32 ProcessWindow::GetID() const { return ID; }

    void ProcessWindow::Update()
	{
		if (!isInitialized)
		{
			Log::Print(
				"Cannot run window loop because window '" + GetTitle() + "' has not been initialized!",
				"KW_WINDOW",
				LogType::LOG_ERROR,
				2);

			return;
		}

        UpdateIdleState(
			this,
			isIdle);
    }

	void ProcessWindow::SetLastDraggedFiles(const vector<string>& files) { lastDraggedFiles = files; };
	const vector<string>& ProcessWindow::GetLastDraggedFiles() const { return lastDraggedFiles; };
	void ProcessWindow::ClearLastDraggedFiles() { lastDraggedFiles.clear(); };

    void ProcessWindow::SetTitle(string_view newValue) const
    {
        if (newValue.empty())
		{
			Log::Print(
				"Window title cannot be empty!",
				"KW_WINDOW",
				LogType::LOG_ERROR,
				2);

			return;
		}

		if (newValue.length() > MAX_TITLE_LENGTH)
		{
			Log::Print(
				"Window title exceeded max allowed length of '" + to_string(MAX_TITLE_LENGTH) + "'! Title has been truncated.",
				"KW_WINDOW",
				LogType::LOG_ERROR,
                2);

            return;
		}

        const X11GlobalData& globalData = Window_Global::GetGlobalData();

        if (!globalData.display)
        {
            KalaWindowCore::ForceClose(
                "Window error",
                "Failed to set window title because the attached display was invalid!");
        }
        if (!windowData.window)
        {
            KalaWindowCore::ForceClose(
                "Window error",
                "Failed to set window title because the attached window was invalid!");
        }

        string value(newValue);
        Display* display = ToVar<Display*>(globalData.display);

        Atom net_wm_name = ToVar<Atom>(globalData.atom_net_wm_name);
        Atom utf8 = ToVar<Atom>(globalData.atom_utf8);

        XStoreName(
            display, 
            windowData.window, 
            value.c_str());

        XChangeProperty(
            display,
            windowData.window,
            net_wm_name,
            utf8,
            8,
            PropModeReplace,
            rcast<const unsigned char*>(value.c_str()),
            value.size());
    }
    string ProcessWindow::GetTitle() const
    {
        const X11GlobalData& globalData = Window_Global::GetGlobalData();

        if (!globalData.display)
        {
			KalaWindowCore::ForceClose(
				"Window error",
				"Failed to get window title because the attached display was invalid!");

            return {};
        }
        if (!windowData.window)
        {
			KalaWindowCore::ForceClose(
				"Window error",
				"Failed to get window title because the attached window was invalid!");

            return {};
        }

        Display* display = ToVar<Display*>(globalData.display);
        Window window = ToVar<Window>(windowData.window);

        Atom atom_utf8 = ToVar<Atom>(globalData.atom_utf8);
        Atom atom_net_wm_name = ToVar<Atom>(globalData.atom_net_wm_name);

        Atom actualType{};
        int actualFormat{};
        unsigned long nItems{};
        unsigned long bytesAfter{};
        unsigned char* prop{};

        string title{};

        if (XGetWindowProperty(
            display,
            window,
            atom_net_wm_name,
            0,
            (~0L),
            False,
            atom_utf8,
            &actualType,
            &actualFormat,
            &nItems,
            &bytesAfter,
            &prop) == Success)
        {
            if (prop)
            {
                title.assign(rcast<char*>(prop), nItems);
                XFree(prop);
                
                return title;
            }
        }

        //fallback

        char* name{};
        if (XFetchName(
            display,
            window,
            &name) > 0
            && name)
        {
            title = name;
            XFree(name);
        }

        return title;
    }

    void ProcessWindow::SetIcon(u32 texture) const {}
    u32 ProcessWindow::GetIcon() const { return iconID; }
	void ProcessWindow::ClearIcon() const {}

    void ProcessWindow::SetTaskbarOverlayIcon(
		u32 texture,
		string_view tooltip) const {}
	u32 ProcessWindow::GetTaskbarOverlayIcon() const { return overlayIconID; }
	void ProcessWindow::ClearTaskbarOverlayIcon() const {}

    void ProcessWindow::BringToFocus() {}

    void ProcessWindow::SetClientRectSize(vec2 newSize)
    { 
        vec2 winSize = kclamp(newSize, minSize, maxSize);

        const X11GlobalData& globalData = Window_Global::GetGlobalData();

        if (!globalData.display)
        {
            KalaWindowCore::ForceClose(
                "Window error",
                "Failed to set window client rect size because the attached display was invalid!");
        }
        if (!windowData.window)
        {
            KalaWindowCore::ForceClose(
                "Window error",
                "Failed to set window client rect size because the attached window was invalid!");
        }

        Display* display = ToVar<Display*>(globalData.display);
        Window window = ToVar<Window>(windowData.window);

        XResizeWindow(
            display,
            window,
            scast<int>(winSize.x),
            scast<int>(winSize.y));

        XFlush(display);
    }
    vec2 ProcessWindow::GetClientRectSize() const { return size; }

    void ProcessWindow::SetOuterSize(vec2 newSize) { SetClientRectSize(newSize); }
    vec2 ProcessWindow::GetOuterSize() const { return GetClientRectSize(); }

    void ProcessWindow::SetPosition(vec2 newPosition)
    { 
        vec2 winPos = newPosition;

        const X11GlobalData& globalData = Window_Global::GetGlobalData();

        if (!globalData.display)
        {
            KalaWindowCore::ForceClose(
                "Window error",
                "Failed to set window position because the attached display was invalid!");
        }
        if (!windowData.window)
        {
            KalaWindowCore::ForceClose(
                "Window error",
                "Failed to set window position because the attached window was invalid!");
        }

        Display* display = ToVar<Display*>(globalData.display);
        Window window = ToVar<Window>(windowData.window);

        XMoveWindow(
            display,
            window,
            scast<int>(winPos.x),
            scast<int>(winPos.y));

        XFlush(display);
    }
    vec2 ProcessWindow::GetPosition() { static vec2 pos{}; return pos; }

    void ProcessWindow::SetMaxSize(vec2 newMaxSize)
    { 
        maxSize = kclamp(newMaxSize, minSize + 1.0f, 10000.0f);

        if (size > maxSize) SetClientRectSize(maxSize);

        const X11GlobalData& globalData = Window_Global::GetGlobalData();

        if (!globalData.display)
        {
            KalaWindowCore::ForceClose(
                "Window error",
                "Failed to set window max size because the attached display was invalid!");
        }
        if (!windowData.window)
        {
            KalaWindowCore::ForceClose(
                "Window error",
                "Failed to set window max size because the attached window was invalid!");
        }

        Display* display = ToVar<Display*>(globalData.display);
        Window window = ToVar<Window>(windowData.window);

        XSizeHints hints{};
        hints.flags = PMinSize | PMaxSize;
        hints.min_width = (int)minSize.x;
        hints.min_height = (int)minSize.y;
        hints.max_width = (int)maxSize.x;
        hints.max_height = (int)maxSize.y;

        ostringstream oss{};
        oss << "min_x: " << hints.min_width 
            << ", min_y: " << hints.min_height
            << ", max_x: " << hints.max_width
            << ", max_y: " << hints.max_height;
        Log::Print(oss.str());

        XSetWMNormalHints(display, window, &hints);
        XFlush(display);
    }
	vec2 ProcessWindow::GetMaxSize() const { return maxSize; }

	void ProcessWindow::SetMinSize(vec2 newMinSize)
    { 
        minSize = kclamp(newMinSize, 1.0f, maxSize - 1.0f);

        if (size < minSize) SetClientRectSize(minSize);

        const X11GlobalData& globalData = Window_Global::GetGlobalData();

        if (!globalData.display)
        {
            KalaWindowCore::ForceClose(
                "Window error",
                "Failed to set window min size because the attached display was invalid!");
        }
        if (!windowData.window)
        {
            KalaWindowCore::ForceClose(
                "Window error",
                "Failed to set window min size because the attached window was invalid!");
        }

        Display* display = ToVar<Display*>(globalData.display);
        Window window = ToVar<Window>(windowData.window);

        XSizeHints hints{};
        hints.flags = PMinSize | PMaxSize;
        hints.min_width = (int)minSize.x;
        hints.min_height = (int)minSize.y;
        hints.max_width = (int)maxSize.x;
        hints.max_height = (int)maxSize.y;

        XSetNormalHints(display, window, &hints);
        XFlush(display);
    }
	vec2 ProcessWindow::GetMinSize() const { return minSize; }

    void ProcessWindow::SetAlwaysOnTopState(bool state)
    { 
        const X11GlobalData& globalData = Window_Global::GetGlobalData();

        if (!globalData.display)
        {
            KalaWindowCore::ForceClose(
                "Window error",
                "Failed to set window always on top state because the attached display was invalid!");
        }
        if (!windowData.window)
        {
            KalaWindowCore::ForceClose(
                "Window error",
                "Failed to set window always on top state because the attached window was invalid!");
        }

        Display* display = ToVar<Display*>(globalData.display);
        Window window = ToVar<Window>(windowData.window);

        Atom atom_net_wm_state       = ToVar<Atom>(globalData.atom_net_wm_state);
        Atom atom_net_wm_state_above = ToVar<Atom>(globalData.atom_net_wm_state_above);
    
        XEvent event{};
        event.xclient.type = ClientMessage;
        event.xclient.window = window;
        event.xclient.message_type = atom_net_wm_state;
        event.xclient.format = 32;
        event.xclient.data.l[0] = state;
        event.xclient.data.l[1] = atom_net_wm_state_above;
        event.xclient.data.l[2] = 0;
        event.xclient.data.l[3] = 0;
        event.xclient.data.l[4] = 0;

        XSendEvent(
            display,
            DefaultRootWindow(display),
            False,
            SubstructureRedirectMask
            | SubstructureNotifyMask,
            &event);

        XFlush(display);
    }
    bool ProcessWindow::IsAlwaysOnTop() const
    { 
        bool isAbove{};

        const X11GlobalData& globalData = Window_Global::GetGlobalData();

        if (!globalData.display)
        {
            KalaWindowCore::ForceClose(
                "Window error",
                "Failed to get window always on top state because the attached display was invalid!");
        }
        if (!windowData.window)
        {
            KalaWindowCore::ForceClose(
                "Window error",
                "Failed to get window always on top state because the attached window was invalid!");
        }

        Display* display = ToVar<Display*>(globalData.display);
        Window window = ToVar<Window>(windowData.window);

        Atom atom_net_wm_state       = ToVar<Atom>(globalData.atom_net_wm_state);
        Atom atom_net_wm_state_above = ToVar<Atom>(globalData.atom_net_wm_state_above);

        Atom actualType{};
        int actualFormat{};
        unsigned long nItems{}, bytesAfter{};
        unsigned char* data{};

        if (XGetWindowProperty(
            display,
            window,
            atom_net_wm_state,
            0,
            1024,
            False,
            XA_ATOM,
            &actualType,
            &actualFormat,
            &nItems,
            &bytesAfter,
            &data) != Success)
        {
            return false;
        }

        if (data)
        {
            Atom* atoms = (Atom*)data;
            for (unsigned long i = 0; i < nItems; i++)
            {
                if (atoms[i] == atom_net_wm_state_above)
                {
                    isAbove = true;
                    break;
                }
            }
            XFree(data);
        }

        return isAbove;
    }

    void ProcessWindow::SetResizableState(bool state)
    { 
        bool resizable = state;

        const X11GlobalData& globalData = Window_Global::GetGlobalData();

        if (!globalData.display)
        {
            KalaWindowCore::ForceClose(
                "Window error",
                "Failed to set window resizable state because the attached display was invalid!");
        }
        if (!windowData.window)
        {
            KalaWindowCore::ForceClose(
                "Window error",
                "Failed to set window resizable state because the attached window was invalid!");
        }
        
        Display* display = ToVar<Display*>(globalData.display);
        Window window = ToVar<Window>(windowData.window);

        XSizeHints hints{};
        long supplied{};

        if (!XGetWMNormalHints(
            display,
            window,
            &hints,
            &supplied))
        {
            memset(&hints, 0, sizeof(hints));
        }

        if (resizable)
        {
            hints.flags |= PMinSize | PMaxSize;

            hints.min_width = 1;
            hints.min_height = 1;

            hints.max_width = scast<int>(maxSize.x);
            hints.max_height = scast<int>(maxSize.y);
        }
        else
        {
            hints.flags |= PMinSize | PMaxSize;

            hints.min_width = hints.max_width = scast<int>(size.x);
            hints.min_height = hints.max_height = scast<int>(size.y);
        }

        XSetWMNormalHints(display, window, &hints);
        XFlush(display);
    }
    bool ProcessWindow::IsResizable() const
    {
        const X11GlobalData& globalData = Window_Global::GetGlobalData();

        if (!globalData.display)
        {
            KalaWindowCore::ForceClose(
                "Window error",
                "Failed to get window resizable state because the attached display was invalid!");
        }
        if (!windowData.window)
        {
            KalaWindowCore::ForceClose(
                "Window error",
                "Failed to get window resizable state because the attached window was invalid!");
        }

        Display* display = ToVar<Display*>(globalData.display);
        Window window = ToVar<Window>(windowData.window);

        XSizeHints hints{};
        long supplied{};

        if (!XGetWMNormalHints(
            display,
            window,
            &hints,
            &supplied))
        {
            //assume resizable if hints are missing
            return true;
        }

        if ((hints.flags & PMinSize)
            && (hints.flags & PMaxSize))
        {
            return !(
                hints.min_width == hints.max_width
                && hints.min_height == hints.max_height);
        }

        return false;
    }

    void ProcessWindow::SetWindowClass(string_view newValue)
    {
		if (newValue.empty())
		{
			Log::Print(
				"Class value cannot be empty!",
				"KW_WINDOW",
				LogType::LOG_ERROR,
				2);

			return;
		}

		if (newValue.length() > MAX_TITLE_LENGTH)
		{
			Log::Print(
				"Class value exceeded max allowed length of '" + to_string(MAX_TITLE_LENGTH) + "'! Title has been truncated.",
				"KW_WINDOW",
				LogType::LOG_ERROR,
                2);

            return;
		}

        const X11GlobalData& globalData = Window_Global::GetGlobalData();

        if (!globalData.display)
        {
            KalaWindowCore::ForceClose(
                "Window error",
                "Failed to set window class because the attached display was invalid!");
        }
        if (!windowData.window)
        {
            KalaWindowCore::ForceClose(
                "Window error",
                "Failed to set window class because the attached window was invalid!");
        }
        
        string value(newValue);

        XClassHint classHint{};
            classHint.res_name = const_cast<char*>(value.c_str());
            classHint.res_class = const_cast<char*>(value.c_str());

        XSetClassHint(
            ToVar<Display*>(globalData.display),
            windowData.window,
            &classHint);
    }

    bool ProcessWindow::IsIdle() const { return isIdle; }

    bool ProcessWindow::IsHovered() const { return isWindowHovered; }
    bool ProcessWindow::IsForegroundWindow() const { return isFocused; }
    bool ProcessWindow::IsFocused() const { return isFocused; }
    bool ProcessWindow::IsFullscreen() { return isFullscreen; }
    bool ProcessWindow::IsMinimized() const { return !isVisible; }
    bool ProcessWindow::IsVisible() const { return isVisible; }

    void ProcessWindow::SetResizingState(bool newState) { isResizing = newState; }
	bool ProcessWindow::IsResizing() const { return isResizing; }

    void ProcessWindow::SetWindowMode(WindowMode mode)
    {
        const X11GlobalData& globalData = Window_Global::GetGlobalData();

        if (!globalData.display)
        {
            KalaWindowCore::ForceClose(
                "Window error",
                "Failed to set window mode because the attached display was invalid!");
        }
        if (!windowData.window)
        {
            KalaWindowCore::ForceClose(
                "Window error",
                "Failed to set window mode because the attached window was invalid!");
        }

        Display* display = ToVar<Display*>(globalData.display);
        Window window = ToVar<Window>(windowData.window);

        Atom atom_net_wm_state            = ToVar<Atom>(globalData.atom_net_wm_state);
        Atom atom_net_wm_state_fullscreen = ToVar<Atom>(globalData.atom_net_wm_state_fullscreen);

        long action{};

        switch (mode)
        {
            default:
            case WindowMode::WINDOWMODE_WINDOWED:
                action = 0;
                break;
            case WindowMode::WINDOWMODE_BORDERLESS:
            case WindowMode::WINDOWMODE_EXCLUSIVE:
                action = 1;
                SetPosition(oldPos);
                SetClientRectSize(oldSize);
                break;
        }

        XEvent event{};
        event.xclient.type = ClientMessage;
        event.xclient.window = window;
        event.xclient.message_type = atom_net_wm_state;
        event.xclient.format = 32;
        event.xclient.data.l[0] = action;
        event.xclient.data.l[1] = atom_net_wm_state_fullscreen;
        event.xclient.data.l[2] = 0;
        event.xclient.data.l[3] = 0;
        event.xclient.data.l[4] = 0;

        XSendEvent(
            display,
            DefaultRootWindow(display),
            False,
            SubstructureRedirectMask
            | SubstructureNotifyMask,
            &event);

        if (mode == WindowMode::WINDOWMODE_WINDOWED)
        {
            SetPosition(oldPos);
            SetClientRectSize(oldSize);
        }

        XFlush(display);

        windowMode = mode;
    }
    WindowMode ProcessWindow::GetWindowMode() { return windowMode; }

    void ProcessWindow::SetWindowState(WindowState state)
    {
        const X11GlobalData& globalData = Window_Global::GetGlobalData();

        if (!globalData.display)
        {
            KalaWindowCore::ForceClose(
                "Window error",
                "Failed to set window state because the attached display was invalid!");
        }
        if (!windowData.window)
        {
            KalaWindowCore::ForceClose(
                "Window error",
                "Failed to set window state because the attached window was invalid!");
        }

        Display* display = ToVar<Display*>(globalData.display);
        Window window = ToVar<Window>(windowData.window);

        switch (state)
        {
            default:
            case WindowState::WINDOW_NORMAL:
            case WindowState::WINDOW_SHOWNOACTIVATE:
            {
                Atom atom_net_wm_state            = ToVar<Atom>(globalData.atom_net_wm_state);
                Atom atom_net_wm_state_fullscreen = ToVar<Atom>(globalData.atom_net_wm_state_fullscreen);
                Atom atom_net_wm_state_horizontal = ToVar<Atom>(globalData.atom_net_wm_state_horizontal);
                Atom atom_net_wm_state_vertical   = ToVar<Atom>(globalData.atom_net_wm_state_vertical);
                
                XEvent event{};
                event.xclient.type = ClientMessage;
                event.xclient.window = window;
                event.xclient.message_type = atom_net_wm_state;
                event.xclient.format = 32;
                event.xclient.data.l[0] = 0; //remove hints
                event.xclient.data.l[1] = atom_net_wm_state_vertical;
                event.xclient.data.l[2] = atom_net_wm_state_horizontal;
                event.xclient.data.l[3] = atom_net_wm_state_fullscreen;
                event.xclient.data.l[4] = 0;

                XSendEvent(
                    display,
                    DefaultRootWindow(display),
                    False,
                    SubstructureRedirectMask
                    | SubstructureNotifyMask,
                    &event);

                XMapWindow(display, window);

                SetPosition(oldPos);
                SetClientRectSize(oldSize);

                break;
            }
            case WindowState::WINDOW_MAXIMIZE:
            {
                SetPosition(oldPos);
                SetClientRectSize(oldSize);

                Atom atom_net_wm_state            = ToVar<Atom>(globalData.atom_net_wm_state);
                Atom atom_net_wm_state_horizontal = ToVar<Atom>(globalData.atom_net_wm_state_horizontal);
                Atom atom_net_wm_state_vertical   = ToVar<Atom>(globalData.atom_net_wm_state_vertical);
                
                XEvent event{};
                event.xclient.type = ClientMessage;
                event.xclient.window = window;
                event.xclient.message_type = atom_net_wm_state;
                event.xclient.format = 32;
                event.xclient.data.l[0] = 1; //add hints
                event.xclient.data.l[1] = atom_net_wm_state_vertical;
                event.xclient.data.l[2] = atom_net_wm_state_horizontal;
                event.xclient.data.l[3] = 0;
                event.xclient.data.l[4] = 0;

                XSendEvent(
                    display,
                    DefaultRootWindow(display),
                    False,
                    SubstructureRedirectMask
                    | SubstructureNotifyMask,
                    &event);

                XMapWindow(display, window);
                break;
            }
            case WindowState::WINDOW_MINIMIZE:
            {
                SetPosition(oldPos);
                SetClientRectSize(oldSize);

                int screen = DefaultScreen(display);
                XIconifyWindow(display, window, screen);
                break;
            }
            case WindowState::WINDOW_HIDE:
            {
                SetPosition(oldPos);
                SetClientRectSize(oldSize);

                XUnmapWindow(display, window);
                break;
            }
        }

        XFlush(display);

        windowState = state;
    }
    WindowState ProcessWindow::GetWindowState() const { return windowState; }

    void ProcessWindow::TriggerResize() { if (resizeCallback) resizeCallback(); }
	void ProcessWindow::SetResizeCallback(const function<void()>& callback) { resizeCallback = callback; }

	void ProcessWindow::TriggerRedraw() { if (redrawCallback) redrawCallback(); }
	void ProcessWindow::SetRedrawCallback(const function<void()>& callback) { redrawCallback = callback; }

    void ProcessWindow::SetWindowData(const WindowData& newWindowStruct) { windowData = newWindowStruct; }
	const WindowData& ProcessWindow::GetWindowData() const { return windowData; }

    u32 ProcessWindow::GetInputID() const { return inputID; }
	void ProcessWindow::SetInputID(u32 newValue) { inputID = newValue; }

	u32 ProcessWindow::GetContextID() const { return contextID; }
	void ProcessWindow::SetContextID(u32 newValue) { contextID = newValue; }

	u32 ProcessWindow::GetMenuBarID() const { return 0; }
	void ProcessWindow::SetMenuBarID(u32 newValue) {}

	void ProcessWindow::SetShutdownCallback(function<void()> newValue) { shutdownCallback = newValue; }

    void ProcessWindow::UpdateFullscreenState()
    {
        Atom actualType{};
        int actualFormat{};
        unsigned long nItems{}, bytesAfter{};
        unsigned char* data{};

        const X11GlobalData& globalData = Window_Global::GetGlobalData();

        if (!globalData.display)
        {
            KalaWindowCore::ForceClose(
                "Window error",
                "Failed to update window fullscreen state because the attached display was invalid!");
        }
        if (!windowData.window)
        {
            KalaWindowCore::ForceClose(
                "Window error",
                "Failed to update window fullscreen state because the attached window was invalid!");
        }

        Display* display = ToVar<Display*>(globalData.display);
        Window window = ToVar<Window>(windowData.window);

        Atom netWmState = ToVar<Atom>(globalData.atom_net_wm_state);
        Atom netWmStateFullscreen = ToVar<Atom>(globalData.atom_net_wm_state_fullscreen);

        XGetWindowProperty(
            display,
            window,
            netWmState,
            0,
            1024,
            False,
            XA_ATOM,
            &actualType,
            &actualFormat,
            &nItems,
            &bytesAfter,
            &data);

        isFullscreen = false;

        if (data)
        {
            Atom* atoms = (Atom*)data;

            for (unsigned long i = 0; i < nItems; i++)
            {
                if (atoms[i] == netWmStateFullscreen)
                {
                    isFullscreen = true;
                    break;
                }
            }

            XFree(data);
        }
    }

    void ProcessWindow::CloseWindow()
    {
        if (shutdownCallback) shutdownCallback();
		
		KalaWindowRegistry<OpenGL_Context>::RemoveAllWindowContent(ID);
        KalaWindowRegistry<Vulkan_Context>::RemoveAllWindowContent(ID);

		KalaWindowRegistry<Input>::RemoveAllWindowContent(ID);
		
		KalaWindowRegistry<ProcessWindow>::RemoveContent(ID);
    }

    ProcessWindow::~ProcessWindow()
    {
        string title = GetTitle();

		Log::Print(
			"Destroying window '" + title + "' with ID '" + to_string(ID) + "'.",
			"KW_WINDOW",
			LogType::LOG_INFO);

		inputID = 0;
		contextID = 0;
		menuBarID = 0;

        const X11GlobalData& globalData = Window_Global::GetGlobalData();
        if (globalData.display)
        {
            Display* display = ToVar<Display*>(globalData.display);
            Window window = ToVar<Window>(windowData.window);

            XIC xic = ToVar<XIC>(windowData.xic);

            XDestroyWindow(display, window);
            XDestroyIC(xic);
        }
    }
}

#endif //__linux__