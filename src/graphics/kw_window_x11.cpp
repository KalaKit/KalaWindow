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
using std::string;
using std::ostringstream;

//KalaWindow will dynamically update window idle state
static void UpdateIdleState(ProcessWindow* window, bool& isIdle)
{
	isIdle =
		!window->IsForegroundWindow()
		|| window->IsMinimized()
		|| !window->IsVisible();
}

static void ForceClose(
    string&& action,
    string&& reason)
{
    KalaWindowCore::ForceClose(
        "KalaWindow window error",
        "Failed to " + std::move(action) + " because " + std::move(reason) + "!");
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
				"Failed to create window because global window has not been initialized!",
				"KW_WINDOW",
				LogType::LOG_ERROR,
				2);

			return nullptr;
		}

		if (title.empty()
            || title.size() > MAX_NAME_LENGTH)
		{
			Log::Print(
				"Failed to create window because its title is empty or too long!",
				"KW_WINDOW",
				LogType::LOG_ERROR,
				2);

			return nullptr;
		}

		if (size < MIN_WINDOW_SIZE)
		{
			Log::Print(
				"Failed to create window '" + title + "' because its size is too small!",
				"KW_WINDOW",
				LogType::LOG_ERROR,
				2);

			return nullptr;
		}
		if (size > MAX_WINDOW_SIZE)
		{
			Log::Print(
				"Failed to create window '" + title + "' because its size is too big!",
				"KW_WINDOW",
				LogType::LOG_ERROR,
				2);

			return nullptr;
		}

        const X11GlobalData& globalData = Window_Global::GetGlobalData();
        if (!globalData.display)
        {
            ForceClose(
                "create window '" + title + "'",
                "the attached display was invalid!");
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
            ForceClose(
                "create window '" + title + "'",
                "XCreateWindow failed!");
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
            ForceClose(
                "create window '" + title + "'",
                "XCreateIC failed!");
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
            ForceClose(
                "create window '" + title + "'",
                "first XChangeProperty failed!");
        }

        Atom atom_wm_delete = ToVar<Atom>(globalData.atom_wm_delete);

        status = XSetWMProtocols(
            display,
            window,
            &atom_wm_delete,
            1);

        if (status == 0)
        {
            ForceClose(
                "create window '" + title + "'",
                "XSetWMProtocols failed!");
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

        windowPtr->SetTitle(string(title));
		windowPtr->ID = newID;

        windowPtr->SetWindowClass(string(title));

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
				ForceClose(
					"create child window '" + title + "'",
                    "parent window was invalid!");
			}

            Window parentWindowRef = ToVar<Window>(parentWindow->GetWindowData().window);

            if (!parentWindowRef)
            {
				ForceClose(
					"create child window '" + title + "'",
                    "parent window '" + to_string(parentWindow->GetID()) + "' handle is invalid!");
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
                ForceClose(
                    "create window '" + title + "'",
                    "second XChangeProperty failed!");
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
			"Created new window '" + title + "' with ID '" + to_string(newID) + "'!",
			"KW_WINDOW",
			LogType::LOG_SUCCESS);

		return windowPtr;
    }

	u32 ProcessWindow::GetID() const { return ID; }
    u32 ProcessWindow::GetInputID() const { return inputID; }
	u32 ProcessWindow::GetGraphicsContextID() const { return graphicsContextID; }

    void ProcessWindow::Update()
	{
        UpdateIdleState(
			this,
			isIdle);
    }

	const vector<string>& ProcessWindow::GetLastDraggedFiles() const { return lastDraggedFiles; };
	void ProcessWindow::SetLastDraggedFiles(vector<string>&& files)
    {
        if (files.empty())
        {
            Log::Print(
                "Failed to set window '" + to_string(ID) + " dragged files because they were empty!",
                "KW_WINDOW",
                LogType::LOG_SUCCESS);
        }
        
        lastDraggedFiles = std::move(files);
    };
	void ProcessWindow::ClearLastDraggedFiles() { lastDraggedFiles.clear(); };

    string ProcessWindow::GetTitle() const
    {
        const X11GlobalData& globalData = Window_Global::GetGlobalData();
        if (!globalData.display
            || !windowData.window)
        {
			ForceClose(
				"get window '" + to_string(ID) + "' title",
                "the display or window handle was invalid!");
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
    void ProcessWindow::SetTitle(string&& newValue) const
    {
        const X11GlobalData& globalData = Window_Global::GetGlobalData();
        if (!globalData.display
            || !windowData.window)
        {
			ForceClose(
				"set window '" + to_string(ID) + "' title",
                "the display or window handle was invalid!");
        }

		if (newValue.empty()
            || newValue.length() > MAX_NAME_LENGTH)
		{
			Log::Print(
				"Failed to set window '" + to_string(ID) + "' title "
                "because the new title is empty or too long!",
				"KW_WINDOW",
				LogType::LOG_ERROR,
				2);

			return;
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

		if (Window_Global::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set window '" + to_string(ID) + "' title to '" + newValue + "'",
				"KW_WINDOW",
				LogType::LOG_VERBOSE);
		}
    }

    void ProcessWindow::BringToFocus()
    {
        const X11GlobalData& globalData = Window_Global::GetGlobalData();
        if (!globalData.display
            || !windowData.window)
        {
			ForceClose(
				"bring window '" + to_string(ID) + "' to focus",
                "the display or window handle was invalid!");
        }

        //skip if already focused
        if (isFocused) return;

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

    vec2 ProcessWindow::GetSize() const { return size; }
    void ProcessWindow::SetSize(vec2 newSize)
    {
        const X11GlobalData& globalData = Window_Global::GetGlobalData();
        if (!globalData.display
            || !windowData.window)
        {
			ForceClose(
				"set window '" + to_string(ID) + "' size",
                "the display or window handle was invalid!");
        }

		vec2 oldSize = size;
		if (isnear(oldSize, newSize))
        {
			Log::Print(
				"Failed to set window '" + to_string(ID) + "' size because it is already the same size!",
				"KW_WINDOW",
				LogType::LOG_ERROR,
				2);

            return;
        }

        if (newSize > maxSize)
		{
			Log::Print(
				"Failed to set window '" + to_string(ID) + "' size because it cannot be bigger than window max size!",
				"KW_WINDOW",
				LogType::LOG_ERROR,
				2);

			return;
		}
        if (newSize < minSize)
		{
			Log::Print(
				"Failed to set window '" + to_string(ID) + "' size because it cannot be smaller than window min size!",
				"KW_WINDOW",
				LogType::LOG_ERROR,
				2);

			return;
		}

        Display* display = ToVar<Display*>(globalData.display);
        Window window = ToVar<Window>(windowData.window);

        XResizeWindow(
            display,
            window,
            scast<int>(newSize.x),
            scast<int>(newSize.y));

        XFlush(display);

		string val = to_string(newSize.x) + "x" + to_string(newSize.y);
		if (Window_Global::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set window '" + to_string(ID) + "' size to '" + val + "'",
				"KW_WINDOW",
				LogType::LOG_VERBOSE);
		}
    }

	vec2 ProcessWindow::GetMaxSize() const { return maxSize; }
    void ProcessWindow::SetMaxSize(vec2 newSize)
    { 
        const X11GlobalData& globalData = Window_Global::GetGlobalData();
        if (!globalData.display
            || !windowData.window)
        {
			ForceClose(
				"set window '" + to_string(ID) + "' max size",
                "the display or window handle was invalid!");
        }

		if (isnear(maxSize, newSize))
        {
			Log::Print(
				"Failed to set window '" + to_string(ID) + "' max size because it is already the same size!",
				"KW_WINDOW",
				LogType::LOG_ERROR,
				2);

            return;
        }

        if (newSize > MAX_WINDOW_SIZE)
		{
			Log::Print(
				"Failed to set window '" + to_string(ID)
                + "' max size because it is too big!",
				"KW_WINDOW",
				LogType::LOG_ERROR,
				2);

			return;
		}
        if (newSize < minSize)
		{
			Log::Print(
				"Failed to set window '" + to_string(ID)
                + "' max size because it cannot be smaller than window min size!",
				"KW_WINDOW",
				LogType::LOG_ERROR,
				2);

			return;
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
                "Failed to set window '" + to_string(ID) + "' max size because XGetWMNormalHints failed!",
                "KW_WINDOW",
                LogType::LOG_ERROR,
                2);
        }

		string val = to_string(newSize.x) + "x" + to_string(newSize.y);

		if (Window_Global::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set window '" + to_string(ID) + "' max size to '" + val + "'",
				"KW_WINDOW",
				LogType::LOG_VERBOSE);
		}
    }

	vec2 ProcessWindow::GetMinSize() const { return minSize; }
	void ProcessWindow::SetMinSize(vec2 newSize)
    { 
        const X11GlobalData& globalData = Window_Global::GetGlobalData();
        if (!globalData.display
            || !windowData.window)
        {
			ForceClose(
				"set window '" + to_string(ID) + "' min size",
                "the display or window handle was invalid!");
        }

		if (isnear(minSize, newSize))
        {
			Log::Print(
				"Failed to set window '" + to_string(ID) + "' min size because it is already the same size!",
				"KW_WINDOW",
				LogType::LOG_ERROR,
				2);

            return;
        }

        if (newSize > maxSize)
		{
			Log::Print(
				"Failed to set window '" + to_string(ID) 
                + "' min size because it cannot be bigger than window max size!",
				"KW_WINDOW",
				LogType::LOG_ERROR,
				2);

			return;
		}
        if (newSize < MIN_WINDOW_SIZE)
		{
			Log::Print(
				"Failed to set window '" + to_string(ID) 
                + "' min size because it is too small!",
				"KW_WINDOW",
				LogType::LOG_ERROR,
				2);

			return;
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
                "Failed to set window '" + to_string(ID) + "' min size because XGetWMNormalHints failed!",
                "KW_WINDOW",
                LogType::LOG_ERROR,
                2);
        }

		string val = to_string(newSize.x) + "x" + to_string(newSize.y);

		if (Window_Global::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set window '" + to_string(ID) + "' min size to '" + val + "'",
				"KW_WINDOW",
				LogType::LOG_VERBOSE);
		}
    }

    vec2 ProcessWindow::GetPosition() { static vec2 pos{}; return pos; }
    void ProcessWindow::SetPosition(vec2 newPosition)
    { 
        const X11GlobalData& globalData = Window_Global::GetGlobalData();
        if (!globalData.display
            || !windowData.window)
        {
			ForceClose(
				"set window '" + to_string(ID) + "' pos",
                "the display or window handle was invalid!");
        }

        Display* display = ToVar<Display*>(globalData.display);
        Window window = ToVar<Window>(windowData.window);

        XMoveWindow(
            display,
            window,
            scast<int>(newPosition.x),
            scast<int>(newPosition.y));

        XFlush(display);

		string val = to_string(newPosition.x) + "x" + to_string(newPosition.y);

		if (Window_Global::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set window '" + to_string(ID) + "' position to '" + val + "'",
				"KW_WINDOW",
				LogType::LOG_VERBOSE);
		}
    }

    bool ProcessWindow::IsAlwaysOnTop() const
    {
        const X11GlobalData& globalData = Window_Global::GetGlobalData();
        if (!globalData.display
            || !windowData.window)
        {
			ForceClose(
				"get window '" + to_string(ID) + "' always on top state",
                "the display or window handle was invalid!");
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

        bool isAbove{};
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
    void ProcessWindow::SetAlwaysOnTopState(bool state)
    { 
        const X11GlobalData& globalData = Window_Global::GetGlobalData();
        if (!globalData.display
            || !windowData.window)
        {
			ForceClose(
				"set window '" + to_string(ID) + "' always on top state",
                "the display or window handle was invalid!");
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

		string val = state ? "true" : "false";

		if (Window_Global::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set window '" + to_string(ID) + "' always on top state to '" + val + "'",
				"KW_WINDOW",
				LogType::LOG_VERBOSE);
		}
    }

    bool ProcessWindow::IsResizable() const
    {
        const X11GlobalData& globalData = Window_Global::GetGlobalData();
        if (!globalData.display
            || !windowData.window)
        {
			ForceClose(
				"get window '" + to_string(ID) + "' resizable state",
                "the display or window handle was invalid!");
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
    void ProcessWindow::SetResizableState(bool state)
    {
        const X11GlobalData& globalData = Window_Global::GetGlobalData();
        if (!globalData.display
            || !windowData.window)
        {
			ForceClose(
				"set window '" + to_string(ID) + "' resizable state",
                "the display or window handle was invalid!");
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

        if (state)
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

		string val = state ? "true" : "false";

		if (Window_Global::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set window '" + to_string(ID) + "' resizable state to '" + val + "'",
				"KW_WINDOW",
				LogType::LOG_VERBOSE);
		}
    }

    pair<string, string> ProcessWindow::GetWindowClass() const
    {
        const X11GlobalData& globalData = Window_Global::GetGlobalData();
        if (!globalData.display
            || !windowData.window)
        {
			ForceClose(
				"get window '" + to_string(ID) + "' class",
                "the display or window handle was invalid!");
        }

        XClassHint classHint{};
        Status status = XGetClassHint(
            ToVar<Display*>(globalData.display),
            ToVar<Window>(windowData.window),
            &classHint);

        if (status == 0
            || !classHint.res_name
            || !classHint.res_class)
        {
			Log::Print(
				"Failed to get window '" + to_string(ID) + "' class value "
                "because XClassHint failed or it had no name or class value!",
				"KW_WINDOW",
				LogType::LOG_ERROR,
				2);

            return { "", "" };
        }

        pair<string, string> result
        { 
            classHint.res_name,
            classHint.res_class
        };

        XFree(classHint.res_name);
        XFree(classHint.res_class);

        return result;
    }
    void ProcessWindow::SetWindowClass(string&& newValue)
    {
        const X11GlobalData& globalData = Window_Global::GetGlobalData();
        if (!globalData.display
            || !windowData.window)
        {
			ForceClose(
				"set window '" + to_string(ID) + "' class",
                "the display or window handle was invalid!");
        }

		if (newValue.empty()
            || newValue.length() > MAX_NAME_LENGTH)
		{
			Log::Print(
				"Failed to set window '" + to_string(ID) + "' class value "
                "because it was empty or too long!",
				"KW_WINDOW",
				LogType::LOG_ERROR,
				2);

			return;
		}

        string nameClassValue = string(newValue);

        XClassHint classHint{};
            classHint.res_name = ccast<char*>(nameClassValue.c_str());
            classHint.res_class = ccast<char*>(nameClassValue.c_str());

        XSetClassHint(
            ToVar<Display*>(globalData.display),
            windowData.window,
            &classHint);

		if (Window_Global::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set window '" + to_string(ID) + "' class to '" + newValue + "'",
				"KW_WINDOW",
				LogType::LOG_VERBOSE);
		}
    }

    bool ProcessWindow::IsIdle() const { return isIdle; }

    bool ProcessWindow::IsHovered() const { return isWindowHovered; }
    bool ProcessWindow::IsForegroundWindow() const { return isFocused; }
    bool ProcessWindow::IsFocused() const { return isFocused; }
    bool ProcessWindow::IsFullscreen() { return isFullscreen; }
    bool ProcessWindow::IsMinimized() const { return isMinimized; }
    bool ProcessWindow::IsVisible() const { return isVisible; }
	bool ProcessWindow::IsResizing() const { return isResizing; }

    WindowMode ProcessWindow::GetWindowMode() { return windowMode; }
    void ProcessWindow::SetWindowMode(WindowMode mode)
    {
        const X11GlobalData& globalData = Window_Global::GetGlobalData();
        if (!globalData.display
            || !windowData.window)
        {
			ForceClose(
				"set window '" + to_string(ID) + "' mode",
                "the display or window handle was invalid!");
        }

        Display* display = ToVar<Display*>(globalData.display);
        Window window = ToVar<Window>(windowData.window);

        Atom atom_net_wm_state            = ToVar<Atom>(globalData.atom_net_wm_state);
        Atom atom_net_wm_state_fullscreen = ToVar<Atom>(globalData.atom_net_wm_state_fullscreen);

        long action{};

        string windowModeVal{};

        switch (mode)
        {
        case WindowMode::WINDOWMODE_WINDOWED:
            windowModeVal = "windowed";

            action = 0;
            break;
        case WindowMode::WINDOWMODE_BORDERLESS:
        case WindowMode::WINDOWMODE_EXCLUSIVE:
            windowModeVal = "borderless";

            action = 1;
            SetPosition(oldPos);
            SetSize(oldSize);
            break;
        default: break;
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

		if (Window_Global::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set window '" + to_string(ID) + "' mode to '" + windowModeVal + "'",
				"KW_WINDOW",
				LogType::LOG_VERBOSE);
		}
    }

    WindowState ProcessWindow::GetWindowState() const { return windowState; }
    void ProcessWindow::SetWindowState(WindowState state)
    {
        const X11GlobalData& globalData = Window_Global::GetGlobalData();
        if (!globalData.display
            || !windowData.window)
        {
			ForceClose(
				"set window '" + to_string(ID) + "' state",
                "the display or window handle was invalid!");
        }

        Display* display = ToVar<Display*>(globalData.display);
        Window window = ToVar<Window>(windowData.window);

        string windowStateVal{};

        switch (state)
        {
        case WindowState::WINDOW_NORMAL:
        case WindowState::WINDOW_SHOWNOACTIVATE:
        {
            windowStateVal = "normal";

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
            windowStateVal = "maximize";

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
            windowStateVal = "minimize";

            SetPosition(oldPos);
            SetSize(oldSize);

            int screen = DefaultScreen(display);
            XIconifyWindow(display, window, screen);
            break;
        }
        case WindowState::WINDOW_HIDE:
        {
            windowStateVal = "hide";

            SetPosition(oldPos);
            SetSize(oldSize);

            XUnmapWindow(display, window);
            break;
        }
        default: break;
        }

        XFlush(display);

        windowState = state;

		if (Window_Global::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set window '" + to_string(ID) + "' state to '" + windowStateVal + "'",
				"KW_WINDOW",
				LogType::LOG_VERBOSE);
		}
    }

	const WindowData& ProcessWindow::GetWindowData() const { return windowData; }
    void ProcessWindow::SetWindowData(WindowData&& newWindowStruct)
    {
        if (!windowData.window
            || !windowData.xic)
        {
			Log::Print(
				"Failed to set window '" + to_string(ID) + "' data "
                "because the window handle or xic was invalid!",
				"KW_WINDOW",
				LogType::LOG_ERROR,
				2);

			return;
        }

        windowData = std::move(newWindowStruct);
    }

	void ProcessWindow::ResizeCallback() { if (resizeCallback) resizeCallback(); }
	void ProcessWindow::SetResizeCallback(function<void()>&& newValue)
	{
		if (!newValue)
		{
			Log::Print(
				"Failed to assign window '" + to_string(ID) + "' resize callback because it was empty!",
				"KW_WINDOW",
				LogType::LOG_ERROR,
				2);

			return;
		}

		resizeCallback = std::move(newValue);
	}

	void ProcessWindow::SetShutdownCallback(function<void()>&& newValue)
	{
		if (!newValue)
		{
			Log::Print(
				"Failed to assign window '" + to_string(ID) + "' shutdown callback because it was empty!",
				"KW_WINDOW",
				LogType::LOG_ERROR,
				2);

			return;
		}

		shutdownCallback = std::move(newValue);
	}
    
    void ProcessWindow::UpdateFullscreenAndMinimizedState()
    {
        const X11GlobalData& globalData = Window_Global::GetGlobalData();
        if (!globalData.display
            || !windowData.window)
        {
			ForceClose(
				"update window '" + to_string(ID) + "' fullscreen and minimized state",
                "the display or window handle was invalid!");
        }

        Atom actualType{};
        int actualFormat{};
        unsigned long nItems{}, bytesAfter{};
        unsigned char* data{};

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

			Log::Print(
                "\n======================================================================"
                "\nFINISHED SHUTDOWN"
                "\n======================================================================\n",
                true);

			exit(0);
        }
    }
}

#endif //__linux__