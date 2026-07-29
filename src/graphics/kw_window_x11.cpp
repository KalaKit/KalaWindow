//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef __linux__

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/X.h>

#include <unistd.h>
#include <memory>
#include <sstream>

#include "core_utils.hpp"
#include "log_utils.hpp"

#include "graphics/kw_window.hpp"
#include "core/kw_core.hpp"
#include "core/kw_input.hpp"
#include "graphics/kw_window_global.hpp"
#include "graphics/kw_vulkan.hpp"

using KalaHeaders::KalaCore::ToVar;
using KalaHeaders::KalaCore::FromVar;

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;

using KalaWindow::Core::KalaWindowCore;
using KalaWindow::Core::MAX_NAME_LENGTH;
using KalaWindow::Core::Input;
using KalaWindow::Graphics::VulkanContext;
using KalaWindow::Graphics::ProcessWindow;
using KalaWindow::Graphics::X11GlobalData;
using KalaWindow::Graphics::WindowMode;
using KalaWindow::Graphics::WindowState;

using std::make_unique;
using std::unique_ptr;
using std::to_string;
using std::ostringstream;

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
		string&& title,
		vec2 pos,
		vec2 size,
		ProcessWindow* parentWindow,
		DpiContext context)
    {
		if (!Window_Global::IsInitialized())
		{
			Log::Print(
				"Cannot initialize window because global window has not been initialized!",
				"KW_WINDOW",
				LogType::LOG_ERROR,
				2);

			return nullptr;
		}

		if (title.empty())
		{
			Log::Print(
				"Window title cannot be empty!",
				"KW_WINDOW",
				LogType::LOG_ERROR,
				2);

			return nullptr;
		}

		if (title.length() > MAX_NAME_LENGTH)
		{
			Log::Print(
				"Window title cannot be over '" + to_string(MAX_NAME_LENGTH) + "' characters long!",
				"KW_WINDOW",
				LogType::LOG_ERROR,
				2);

			return nullptr;
		}

		if (size < 100.0f)
		{
			Log::Print(
				"Cannot set window '" + string(title) + "' size less than 100x100!",
				"KW_WINDOW",
				LogType::LOG_ERROR,
				2);

			return nullptr;
		}

        const X11GlobalData& globalData = Window_Global::GetGlobalData();

        if (!globalData.display)
        {
            KalaWindowCore::ForceClose(
                "KalaWindow window error",
                "Failed to initialize window because the attached display was invalid!");
        }

        u32 newID = KalaWindowCore::GetGlobalID() + 1;
		KalaWindowCore::SetGlobalID(newID);

		unique_ptr<ProcessWindow> newWindow = make_unique<ProcessWindow>();
		ProcessWindow* windowPtr = newWindow.get();

        Display* display = ToVar<Display*>(globalData.display);

        Window root = ToVar<Window>(globalData.window_root);

        XIM xim = ToVar<XIM>(globalData.xim);

        XSetWindowAttributes attrs{};
        attrs.background_pixmap = None;
        attrs.border_pixel = 0;

        Window window = XCreateWindow(
            display,
            root,
            pos.x,
            pos.y,
            size.x,
            size.y,
            0,
            CopyFromParent,
            InputOutput,
            CopyFromParent,
            CWBackPixmap | CWBorderPixel,
            &attrs);

        if (window == None)
        {
            KalaWindowCore::ForceClose(
                "KalaWindow window error",
                "Failed to create window '" + string(title) + "' because XCreateWindow failed!");
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
                "KalaWindow window error",
                "Failed to create window '" + string(title) + "' because XCreateIC failed!");
        }

        //set task manager title via PID
        long pid = scast<long>(getpid());
        Status status = XChangeProperty(
            display,
            window,
            ToVar<Atom>(globalData.atom_net_wm_pid),
            ToVar<Atom>(globalData.atom_cardinal),
            32,
            PropModeReplace,
            rcast<unsigned char*>(&pid),
            1);

        if (status == 0)
        {
            KalaWindowCore::ForceClose(
                "KalaWindow window error",
                "Failed to create window '" + string(title) + "' because first XChangeProperty failed!");
        }

        Atom atom_wm_delete = ToVar<Atom>(globalData.atom_wm_delete);

        status = XSetWMProtocols(
            display,
            window,
            &atom_wm_delete,
            1);

        if (status == 0)
        {
            KalaWindowCore::ForceClose(
                "KalaWindow window error",
                "Failed to create window '" + string(title) + "' because XSetWMProtocols failed!");
        }

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

        //flush now and detect errors via ErrorHandler
        XSync(display, False);

        WindowData newWindowStruct{};

        //set window dpi aware state here...

        newWindowStruct.window = FromVar(window);
        newWindowStruct.xic = FromVar(xic);

        windowPtr->windowData = newWindowStruct;

        windowPtr->SetTitle(std::move(title));
		windowPtr->ID = newID;

        windowPtr->SetWindowClass(windowPtr->GetTitle());

        windowPtr->pos = pos;
        windowPtr->size = size;

		windowPtr->oldPos = pos;
		windowPtr->oldSize = size;

		//todo: add allow files to be dragged to this window
		//DragAcceptFiles(newHwnd, TRUE);

        if (parentWindow)
        {
            const vector<ProcessWindow*>& content = registry.GetAllContent();

			if (find(content.begin(),
				content.end(),
				parentWindow)
				== content.end())
			{
				KalaWindowCore::ForceClose(
					"KalaWindow window error",
					"Failed to create child window '" + string(title) + "' because parent window pointer was not found!");
			}

            Window parentWindowRef = ToVar<Window>(parentWindow->GetWindowData().window);

            if (!parentWindowRef)
            {
				KalaWindowCore::ForceClose(
					"KalaWindow window error",
					"Failed to create child window '" + string(title) + "' because parent window handle is invalid!");
            }

			parentWindow->childIDs.push_back(newID);
			windowPtr->parentID = parentWindow->ID;
        }

        //do not display child window in taskbar
        if (windowPtr->parentID != UINT32_MAX)
        {
            Atom skipTaskbar = ToVar<Atom>(globalData.atom_net_wm_state_skip_taskbar);
            status = XChangeProperty(
                display,
                window,
                ToVar<Atom>(globalData.atom_net_wm_state),
                XA_ATOM,
                32,
                PropModeAppend,
                (unsigned char*)&skipTaskbar,
                1);

            if (status == 0)
            {
                KalaWindowCore::ForceClose(
                    "KalaWindow window error",
                    "Failed to create window '" + string(title) + "' because second XChangeProperty failed!");
            }
        }

        //show window
        XMapWindow(
            display,
            window);

        //flush now and detect errors via ErrorHandler
        XSync(display, False);

        windowPtr->BringToFocus();

		registry.AddContent(newID, std::move(newWindow));

		Log::Print(
			"Created new window '" + string(title) + "' with ID '" + to_string(newID) + "'!",
			"KW_WINDOW",
			LogType::LOG_SUCCESS);

		return windowPtr;
    }

	u32 ProcessWindow::GetID() const { return ID; }

    void ProcessWindow::Update()
	{
        UpdateIdleState(
			this,
			isIdle);
    }

	void ProcessWindow::SetLastDraggedFiles(vector<string>&& files) { lastDraggedFiles = std::move(files); };
	const vector<string>& ProcessWindow::GetLastDraggedFiles() const { return lastDraggedFiles; };
	void ProcessWindow::ClearLastDraggedFiles() { lastDraggedFiles.clear(); };

    void ProcessWindow::SetTitle(string&& newValue) const
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

		if (newValue.length() > MAX_NAME_LENGTH)
		{
			Log::Print(
				"Window title cannot be over '" + to_string(MAX_NAME_LENGTH) + "' characters long!",
				"KW_WINDOW",
				LogType::LOG_ERROR,
				2);

			return;
		}

        const X11GlobalData& globalData = Window_Global::GetGlobalData();

        if (!globalData.display)
        {
            KalaWindowCore::ForceClose(
                "KalaWindow window error",
                "Failed to set window title because the attached display was invalid!");
        }
        if (!windowData.window)
        {
            KalaWindowCore::ForceClose(
                "KalaWindow window error",
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
				"KalaWindow window error",
				"Failed to get window title because the attached display was invalid!");

            return {};
        }
        if (!windowData.window)
        {
			KalaWindowCore::ForceClose(
				"KalaWindow window error",
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
		string&& tooltip) const {}
	u32 ProcessWindow::GetTaskbarOverlayIcon() const { return overlayIconID; }
	void ProcessWindow::ClearTaskbarOverlayIcon() const {}

    void ProcessWindow::BringToFocus()
    {
        Display* display = ToVar<Display*>(Window_Global::GetGlobalData().display);
        Window window = ToVar<Window>(windowData.window);

        Atom atom_net_active_window = ToVar<Atom>(Window_Global::GetGlobalData().atom_net_active_window);

        XEvent xev{};
        xev.type = ClientMessage;
        xev.xclient.window = window;
        xev.xclient.message_type = atom_net_active_window;
        xev.xclient.format = 32;
        xev.xclient.data.l[0] = 2;
        xev.xclient.data.l[1] = CurrentTime;

        XSendEvent(
            display,
            window,
            False,
            SubstructureRedirectMask
            | SubstructureNotifyMask,
            &xev);

        XSetICFocus(ToVar<XIC>(windowData.xic));
        XFlush(display);
    }

    void ProcessWindow::SetSize(vec2 newSize)
    {
		vec2 oldSize = GetSize();
		if (isnear(oldSize, newSize)) return;

		if (newSize < 100.0f)
		{
			Log::Print(
				"Cannot set window '" + GetTitle() + "' size less than 100x100!",
				"KW_WINDOW",
				LogType::LOG_ERROR,
				2);

			return;
		}

        newSize = kclamp(newSize, minSize, maxSize);

        const X11GlobalData& globalData = Window_Global::GetGlobalData();

        if (!globalData.display)
        {
            KalaWindowCore::ForceClose(
                "KalaWindow window error",
                "Failed to set window client rect size because the attached display was invalid!");
        }
        if (!windowData.window)
        {
            KalaWindowCore::ForceClose(
                "KalaWindow window error",
                "Failed to set window client rect size because the attached window was invalid!");
        }

        Display* display = ToVar<Display*>(globalData.display);
        Window window = ToVar<Window>(windowData.window);

        XResizeWindow(
            display,
            window,
            scast<int>(newSize.x),
            scast<int>(newSize.y));

        XFlush(display);
    }
    vec2 ProcessWindow::GetSize() const { return size; }

    void ProcessWindow::SetOuterSize(vec2 newSize) { SetSize(newSize); }
    vec2 ProcessWindow::GetOuterSize() const { return outerSize; }

    void ProcessWindow::SetPosition(vec2 newPosition)
    { 
        vec2 winPos = newPosition;

        const X11GlobalData& globalData = Window_Global::GetGlobalData();

        if (!globalData.display)
        {
            KalaWindowCore::ForceClose(
                "KalaWindow window error",
                "Failed to set window position because the attached display was invalid!");
        }
        if (!windowData.window)
        {
            KalaWindowCore::ForceClose(
                "KalaWindow window error",
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
        maxSize = kclamp(newMaxSize, minSize + 1.0f, MAX_WINDOW_SIZE);

        if (size > maxSize) SetSize(maxSize);

        const X11GlobalData& globalData = Window_Global::GetGlobalData();

        if (!globalData.display)
        {
            KalaWindowCore::ForceClose(
                "KalaWindow window error",
                "Failed to set window max size because the attached display was invalid!");
        }
        if (!windowData.window)
        {
            KalaWindowCore::ForceClose(
                "KalaWindow window error",
                "Failed to set window max size because the attached window was invalid!");
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
            hints = {};
        }

        hints.flags |= PMinSize | PMaxSize;
        hints.min_width = (int)minSize.x;
        hints.min_height = (int)minSize.y;
        hints.max_width = (int)maxSize.x;
        hints.max_height = (int)maxSize.y;

        XSetWMNormalHints(display, window, &hints);
        XFlush(display);

        XSizeHints verify{};
        if (!XGetWMNormalHints(
            display,
            window,
            &verify,
            &supplied))
        {
            Log::Print(
                "Failed to set max size!",
                "KW_WINDOW",
                LogType::LOG_ERROR,
                2);
        }
    }
	vec2 ProcessWindow::GetMaxSize() const { return maxSize; }

	void ProcessWindow::SetMinSize(vec2 newMinSize)
    { 
        minSize = kclamp(newMinSize, MIN_WINDOW_SIZE, maxSize - 1.0f);

        if (size < minSize) SetSize(minSize);

        const X11GlobalData& globalData = Window_Global::GetGlobalData();

        if (!globalData.display)
        {
            KalaWindowCore::ForceClose(
                "KalaWindow window error",
                "Failed to set window min size because the attached display was invalid!");
        }
        if (!windowData.window)
        {
            KalaWindowCore::ForceClose(
                "KalaWindow window error",
                "Failed to set window min size because the attached window was invalid!");
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
            hints = {};
        }

        hints.flags |= PMinSize | PMaxSize;
        hints.min_width = (int)minSize.x;
        hints.min_height = (int)minSize.y;
        hints.max_width = (int)maxSize.x;
        hints.max_height = (int)maxSize.y;

        XSetWMNormalHints(display, window, &hints);
        XFlush(display);

        XSizeHints verify{};
        if (!XGetWMNormalHints(
            display,
            window,
            &verify,
            &supplied))
        {
            Log::Print(
                "Failed to set min size!",
                "KW_WINDOW",
                LogType::LOG_ERROR,
                2);
        }
    }
	vec2 ProcessWindow::GetMinSize() const { return minSize; }

    void ProcessWindow::SetAlwaysOnTopState(bool state)
    { 
        const X11GlobalData& globalData = Window_Global::GetGlobalData();

        if (!globalData.display)
        {
            KalaWindowCore::ForceClose(
                "KalaWindow window error",
                "Failed to set window always on top state because the attached display was invalid!");
        }
        if (!windowData.window)
        {
            KalaWindowCore::ForceClose(
                "KalaWindow window error",
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
                "KalaWindow window error",
                "Failed to get window always on top state because the attached display was invalid!");
        }
        if (!windowData.window)
        {
            KalaWindowCore::ForceClose(
                "KalaWindow window error",
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
                "KalaWindow window error",
                "Failed to set window resizable state because the attached display was invalid!");
        }
        if (!windowData.window)
        {
            KalaWindowCore::ForceClose(
                "KalaWindow window error",
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
                "KalaWindow window error",
                "Failed to get window resizable state because the attached display was invalid!");
        }
        if (!windowData.window)
        {
            KalaWindowCore::ForceClose(
                "KalaWindow window error",
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

		if (newValue.length() > MAX_NAME_LENGTH)
		{
			Log::Print(
				"Class value exceeded max allowed length of '" + to_string(MAX_NAME_LENGTH) + "'! Title has been truncated.",
				"KW_WINDOW",
				LogType::LOG_ERROR,
                2);

            return;
		}

        const X11GlobalData& globalData = Window_Global::GetGlobalData();

        if (!globalData.display)
        {
            KalaWindowCore::ForceClose(
                "KalaWindow window error",
                "Failed to set window class because the attached display was invalid!");
        }
        if (!windowData.window)
        {
            KalaWindowCore::ForceClose(
                "KalaWindow window error",
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
    bool ProcessWindow::IsMinimized() const { return isMinimized; }
    bool ProcessWindow::IsVisible() const { return isVisible; }

    void ProcessWindow::SetResizingState(bool newState) { isResizing = newState; }
	bool ProcessWindow::IsResizing() const { return isResizing; }

    void ProcessWindow::SetWindowMode(WindowMode mode)
    {
        const X11GlobalData& globalData = Window_Global::GetGlobalData();

        if (!globalData.display)
        {
            KalaWindowCore::ForceClose(
                "KalaWindow window error",
                "Failed to set window mode because the attached display was invalid!");
        }
        if (!windowData.window)
        {
            KalaWindowCore::ForceClose(
                "KalaWindow window error",
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
                SetSize(oldSize);
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
            SetSize(oldSize);
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
                "KalaWindow window error",
                "Failed to set window state because the attached display was invalid!");
        }
        if (!windowData.window)
        {
            KalaWindowCore::ForceClose(
                "KalaWindow window error",
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
                SetSize(oldSize);

                break;
            }
            case WindowState::WINDOW_MAXIMIZE:
            {
                SetPosition(oldPos);
                SetSize(oldSize);

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
                SetSize(oldSize);

                int screen = DefaultScreen(display);
                XIconifyWindow(display, window, screen);
                break;
            }
            case WindowState::WINDOW_HIDE:
            {
                SetPosition(oldPos);
                SetSize(oldSize);

                XUnmapWindow(display, window);
                break;
            }
        }

        XFlush(display);

        windowState = state;
    }
    WindowState ProcessWindow::GetWindowState() const { return windowState; }

    void ProcessWindow::SetWindowData(WindowData&& newWindowStruct) { windowData = std::move(newWindowStruct); }
	const WindowData& ProcessWindow::GetWindowData() const { return windowData; }

    u32 ProcessWindow::GetInputID() const { return inputID; }
	void ProcessWindow::SetInputID(u32 newValue) { inputID = newValue; }

	u32 ProcessWindow::GetGraphicsContextID() const { return graphicsContextID; }
	void ProcessWindow::SetGraphicsContextID(u32 newValue) { graphicsContextID = newValue; }

	u32 ProcessWindow::GetMenuBarID() const { return 0; }
	void ProcessWindow::SetMenuBarID(u32 newValue) {}

    void ProcessWindow::UpdateFullscreenAndMinimizedState()
    {
        Atom actualType{};
        int actualFormat{};
        unsigned long nItems{}, bytesAfter{};
        unsigned char* data{};

        const X11GlobalData& globalData = Window_Global::GetGlobalData();

        if (!globalData.display)
        {
            KalaWindowCore::ForceClose(
                "KalaWindow window error",
                "Failed to update window fullscreen state because the attached display was invalid!");
        }
        if (!windowData.window)
        {
            KalaWindowCore::ForceClose(
                "KalaWindow window error",
                "Failed to update window fullscreen state because the attached window was invalid!");
        }

        Display* display = ToVar<Display*>(globalData.display);
        Window window = ToVar<Window>(windowData.window);

        Atom netWmState = ToVar<Atom>(globalData.atom_net_wm_state);
        Atom netWmStateFullscreen = ToVar<Atom>(globalData.atom_net_wm_state_fullscreen);
        Atom netWmStateHidden = ToVar<Atom>(globalData.atom_net_wm_state_hidden);

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

        bool wasMinimized = isMinimized;

        isFullscreen = false;
        isMinimized = false;

        if (data)
        {
            Atom* atoms = (Atom*)data;

            for (unsigned long i = 0; i < nItems; i++)
            {
                if (atoms[i] == netWmStateFullscreen) isFullscreen = true;
                if (atoms[i] == netWmStateHidden) isMinimized = true;

                if (isFullscreen
                    && isMinimized)
                {
                    break;
                }
            }

            XFree(data);
        }

        if (!wasMinimized
            && isMinimized)
        {
            for (u32 childID : childIDs)
            {
                ProcessWindow* pw = ProcessWindow::GetRegistry().GetContent(childID);
                if (pw)
                {
                    pw->SetWindowState(WindowState::WINDOW_MINIMIZE);
                }
            }
        }
        else if (wasMinimized
                 && !isMinimized)
        {
            Window root = ToVar<Window>(globalData.window_root);

            for (u32 childID : childIDs)
            {
                ProcessWindow* pw = ProcessWindow::GetRegistry().GetContent(childID);
                if (pw)
                {
                    Window childWindow = ToVar<Window>(pw->GetWindowData().window);

                    XMapWindow(display, childWindow);
                    
                    pw->BringToFocus();
                }
            }
        }
    }

	void ProcessWindow::SetResizeCallback(function<void()>&& newValue)
	{
		if (!newValue)
		{
			Log::Print(
				"Cannot assign empty function to resize callback!",
				"KW_WINDOW",
				LogType::LOG_ERROR,
				2);

			return;
		}

		resizeCallback = std::move(newValue);
	}
	void ProcessWindow::ResizeCallback() { if (resizeCallback) resizeCallback(); }

	void ProcessWindow::SetShutdownCallback(function<void()>&& newValue)
	{
		if (!newValue)
		{
			Log::Print(
				"Cannot assign empty function to shutdown callback!",
				"KW_WINDOW",
				LogType::LOG_ERROR,
				2);

			return;
		}

		shutdownCallback = std::move(newValue);
	}

    void ProcessWindow::Destroy()
    {
		if (registry.GetAllContent().size() == 1)
		{
			Log::Print(
                "\n======================================================================"
                "\nSHUTTING DOWN"
                "\n======================================================================\n",
                true);

			Log::Print(
				"Shutting down because the last process window is being destroyed.",
				"KW_WINDOW",
				LogType::LOG_INFO);
		}

		if (shutdownCallback) shutdownCallback();

		vector<u32> children = std::move(childIDs);

		for (const auto& w : children)
		{
			if (Window_Global::IsVerboseLoggingEnabled())
			{
				Log::Print(
					"Destroying child window '" + to_string(w) + "' of parent window '" + to_string(ID) + "'",
					"KW_WINDOW",
					LogType::LOG_VERBOSE);
			}

			ProcessWindow* pw = ProcessWindow::GetRegistry().GetContent(w);
			if (pw) pw->Destroy();
		}
		if (parentID != UINT32_MAX)
		{
			if (Window_Global::IsVerboseLoggingEnabled())
			{
				Log::Print(
					"Destroying child window '" + to_string(ID) + "' of parent window '" + to_string(parentID) + "'",
					"KW_WINDOW",
					LogType::LOG_VERBOSE);
			}

			ProcessWindow* pw = ProcessWindow::GetRegistry().GetContent(parentID);
			if (pw)
			{
				auto it = find(pw->childIDs.begin(), pw->childIDs.end(), ID);
				if (it != pw->childIDs.end()) pw->childIDs.erase(it);
			}
		}

        KalaWindowRegistry<VulkanContext>::RemoveAllWindowContent(ID);

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
		graphicsContextID = 0;
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

        if (registry.GetAllContent().empty())
        {
			const X11GlobalData& globalData = Window_Global::GetGlobalData();
			if (globalData.display)
			{
				XIM xim = ToVar<XIM>(globalData.xim);
				XCloseIM(xim);

				Display* display = ToVar<Display*>(globalData.display);
				if (display) XCloseDisplay(display);
			}

			exit(0);
        }
    }
}

#endif //__linux__