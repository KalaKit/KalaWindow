//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include "graphics/kw_window.hpp"

#if defined(KLIN_ANY)

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/X.h>

#include <unistd.h>
#include <memory>
#include <sstream>
#include <algorithm>

#include "core_utils.hpp"
#include "log_utils.hpp"

#include "core/kw_core.hpp"
#include "core/kw_input.hpp"
#include "graphics/kw_window_global.hpp"
#include "graphics/kw_vulkan.hpp"
#include "core/kw_messageloop_x11.hpp"

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
using KalaWindow::Core::MessageLoop;

using std::make_unique;
using std::unique_ptr;
using std::to_string;
using std::string;
using std::ostringstream;

static int XRESULT{};

static constexpr int SUCCESS_XGETWINDOWPROPERTY = 0;
static constexpr int SUCCESS_XSENDEVENT = 1;

static void ForceClose(
    string&& action,
    string&& reason)
{
    KalaWindowCore::ForceClose(
        "KalaWindow window error",
        "Failed to " + std::move(action) + " because " + std::move(reason));
}

namespace KalaWindow::Graphics
{
	static KalaWindowRegistry<ProcessWindow> registry{};

	KalaWindowRegistry<ProcessWindow>& ProcessWindow::GetRegistry() { return registry; }

    ProcessWindow* ProcessWindow::Initialize(
		string&& title,
		vec2 pos,
		vec2 size,
		ProcessWindow* parentWindow)
    {
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

        string newTitle = std::move(title);

		if (!Window_Global::IsInitialized()) Window_Global::Initialize();
        if (!VulkanContext::IsInitialized()) VulkanContext::Initialize();

		if (size < MIN_WINDOW_SIZE)
		{
			Log::Print(
				"Failed to create window '" + newTitle + "' because its size is too small!",
				"KW_WINDOW",
				LogType::LOG_ERROR,
				2);

			return nullptr;
		}
		if (size > MAX_WINDOW_SIZE)
		{
			Log::Print(
				"Failed to create window '" + newTitle + "' because its size is too big!",
				"KW_WINDOW",
				LogType::LOG_ERROR,
				2);

			return nullptr;
		}

        const X11GlobalData& globalData = Window_Global::GetGlobalData();
        if (!globalData.display)
        {
            ForceClose(
                "create window '" + newTitle + "'",
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

        //flush now and detect errors via ErrorHandler
        XSync(display, False);

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
                "create window '" + newTitle + "'",
                "XCreateIC failed!");
        }

        //set task manager title via PID
        u32 pid = scast<u32>(getpid());
        Atom pidAtom = ToVar<Atom>(globalData.atom_net_wm_pid);

        if (pidAtom == None)
        {
            ForceClose(
                "create window '" + newTitle + "'",
                "pidAtom was invalid!");
        }

        XChangeProperty(
            display,
            window,
            pidAtom,
            XA_CARDINAL,
            32,
            PropModeReplace,
            rcast<unsigned char*>(&pid),
            1);

        Atom atom_wm_delete = ToVar<Atom>(globalData.atom_wm_delete);

        if (display == nullptr 
            || window == None
            || atom_wm_delete == None)
        {
            Log::Print(
                "PRE-CHECK FAILED: display=" + to_string(display != nullptr)
                + " window=" + to_string(window)
                + " atom=" + to_string(atom_wm_delete),
                "KW_WINDOW",
                LogType::LOG_ERROR,
                2);
        }

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

        newWindowStruct.window = FromVar(window);
        newWindowStruct.xic = FromVar(xic);

        windowPtr->windowData = newWindowStruct;

        windowPtr->SetTitle(string(newTitle));
		windowPtr->ID = newID;

        windowPtr->SetWindowClass(string(newTitle));

        windowPtr->pos = pos;
        windowPtr->size = size;

		windowPtr->oldPos = pos;
		windowPtr->oldSize = size;

        if (parentWindow)
        {
            const vector<ProcessWindow*>& content = registry.GetAllContent();

			if (find(content.begin(),
				content.end(),
				parentWindow)
				== content.end())
			{
				ForceClose(
					"create child window '" + newTitle + "'",
                    "parent window was invalid!");
			}

            Window parentWindowRef = ToVar<Window>(parentWindow->GetWindowData().window);

            if (!parentWindowRef)
            {
				ForceClose(
					"create child window '" + newTitle + "'",
                    "parent window '" + to_string(parentWindow->GetID()) + "' handle is invalid!");
            }

			parentWindow->childIDs.push_back(newID);
			windowPtr->parentID = parentWindow->ID;
        }

        //do not display child window in taskbar
        if (windowPtr->parentID != UINT32_MAX)
        {
            Atom skipTaskbar = ToVar<Atom>(globalData.atom_net_wm_state_skip_taskbar);

            XChangeProperty(
                display,
                window,
                ToVar<Atom>(globalData.atom_net_wm_state),
                XA_ATOM,
                32,
                PropModeAppend,
                (unsigned char*)&skipTaskbar,
                1);
        }

        Atom xDndAware = ToVar<Atom>(globalData.atom_xDndAware);
        unsigned long version = 5; //XDND version

        //declare as XDND drop target
        XChangeProperty(
            display,
            window,
            xDndAware,
            XA_ATOM,
            32,
            PropModeReplace,
            rcast<const unsigned char*>(&version),
            1);

        Atom xDndTypeList = ToVar<Atom>(globalData.atom_xDndTypeList);
        Atom textUri      = ToVar<Atom>(globalData.atom_textUri);

        //advertise file-drop MIME type
        XChangeProperty(
            display,
            window,
            xDndTypeList,
            XA_ATOM,
            32,
            PropModeReplace,
            rcast<const unsigned char*>(&textUri),
            1);

        Atom netWmWindowType       = ToVar<Atom>(globalData.atom_net_wm_window_type);
        Atom netWmWindowTypeNormal = ToVar<Atom>(globalData.atom_net_wm_window_type_normal);

        //declare window type
        XChangeProperty(
            display,
            window,
            netWmWindowType,
            XA_ATOM,
            32,
            PropModeReplace,
            rcast<const unsigned char*>(&netWmWindowTypeNormal),
            1);

        //show window
        XMapWindow(
            display,
            window);

        //flush now and detect errors via ErrorHandler
        XSync(display, False);

        windowPtr->BringToFocus();

		string err = registry.AddContent(newID, std::move(newWindow));
		if (!err.empty())
		{
			KalaWindowCore::ForceClose(
				"KalaWindow window error",
				"Failed to initialize window! Reason: " + err);
		}

        if (!Input::Initialize(windowPtr->ID))
		{
			KalaWindowCore::ForceClose(
				"KalaWindow window error",
				"Failed to initialize window because input initialization failed!");
		}
        if (!VulkanContext::InitializeInstance(windowPtr->ID))
		{
			KalaWindowCore::ForceClose(
				"KalaWindow window error",
				"Failed to initialize window because Vulkan context initialization failed!");
		}

		Log::Print(
			"Created new window '" + newTitle + "' with ID '" + to_string(newID) + "'!",
			"KW_WINDOW",
			LogType::LOG_SUCCESS);

		return windowPtr;
    }

    void ProcessWindow::Update(
        const function<void()>& earlyGlobalUpdate,
        const function<void()>& globalUpdate,
        const function<void()>& lateGlobalUpdate)
	{
        if (earlyGlobalUpdate) earlyGlobalUpdate();

        //X11 requires a message loop update that is separate from each process window
        MessageLoop::Update();

        for (ProcessWindow* pw : registry.GetAllContent())
        {
            if (!pw)
            {
                KalaWindowCore::ForceClose(
                    "KalaWindow window error",
                    "Failed to update a window during window early global update because it was invalid!");
            }

            if (pw->earlyUpdateCallback) pw->earlyUpdateCallback();

            pw->UpdateIdleState();

            if (pw->updateCallback) pw->updateCallback();
        }

        if (globalUpdate) globalUpdate();

        for (ProcessWindow* pw : registry.GetAllContent())
        {
            //ensure each window is still valid after user callback
            if (!pw)
            {
                KalaWindowCore::ForceClose(
                    "KalaWindow window error",
                    "Failed to update a window during global update because it was invalid!");
            }

            u32 inputID = pw->GetInputID();
            Input* input{};
            string err = Input::GetRegistry().GetContent(inputID, input);
            if (!err.empty())
            {
                KalaWindowCore::ForceClose(
                    "KalaWindow window error",
                    "Failed to update input '" + to_string(inputID) 
                    + "' under window '" + to_string(pw->GetID()) 
                    + "' during window global update! Reason: " + err);
            }

            input->EndFrameUpdate();

            if (pw->lateUpdateCallback) pw->lateUpdateCallback();
        }

        if (lateGlobalUpdate) lateGlobalUpdate();
    }

	u32 ProcessWindow::GetID() const { return ID; }
    u32 ProcessWindow::GetInputID() const { return inputID; }
	u32 ProcessWindow::GetGraphicsContextID() const { return graphicsContextID; }

    void ProcessWindow::SetDraggedFilesCallback(function<void(const vector<path>&, vec2)>&& newValue)
    {
        if (!newValue)
		{
			Log::Print(
				"Failed to assign window '" + to_string(ID) + "' dragged files callback because it was empty!",
				"KW_WINDOW",
				LogType::LOG_ERROR,
				2);

			return;
		}

		draggedFilesCallback = std::move(newValue);
    }
	const vector<path>& ProcessWindow::GetLastDraggedFiles() const { return lastDraggedFiles; };
	void ProcessWindow::ClearLastDraggedFiles() { lastDraggedFiles.clear(); };

    string ProcessWindow::GetTitle() const
    {
        const X11GlobalData& globalData = Window_Global::GetGlobalData();
        if (!globalData.display)
        {
			ForceClose(
				"get window '" + to_string(ID) + "' title",
                "the display handle was invalid!");
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

        XRESULT = XGetWindowProperty(
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
            &prop);

        if (XRESULT != SUCCESS_XGETWINDOWPROPERTY)
        {
			ForceClose(
				"get window '" + to_string(ID) + " title",
                "XGetWindowProperty failed! Result code: " + to_string(XRESULT));
        }

        if (!prop)
        {
            Log::Print(
                "Failed to get window '" + to_string(ID) + "' title because prop was invalid!",
                "KW_WINDOW",
                LogType::LOG_ERROR,
                2);
        }

        title.assign(rcast<char*>(prop), nItems);
        XFree(prop);

        return title;
    }
    void ProcessWindow::SetTitle(string&& newValue) const
    {
        const X11GlobalData& globalData = Window_Global::GetGlobalData();
        if (!globalData.display)
        {
			ForceClose(
				"set window '" + to_string(ID) + "' title",
                "the display handle was invalid!");
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
        if (!globalData.display)
        {
			ForceClose(
				"bring window '" + to_string(ID) + "' to focus",
                "the display handle was invalid!");
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

        XRESULT = XSendEvent(
            display,
            window,
            False,
            SubstructureRedirectMask
            | SubstructureNotifyMask,
            &xev);

        if (XRESULT != SUCCESS_XSENDEVENT)
        {
            ForceClose(
                "bring window '" + to_string(ID) + "' to focus",
                "XSendEvent failed! Result code: " + to_string(XRESULT));
        }

        XSetICFocus(ToVar<XIC>(windowData.xic));
        XFlush(display);
    }

    vec2 ProcessWindow::GetSize() const { return size; }
    void ProcessWindow::SetSize(vec2 newSize)
    {
        const X11GlobalData& globalData = Window_Global::GetGlobalData();
        if (!globalData.display)
        {
			ForceClose(
				"set window '" + to_string(ID) + "' size",
                "the display handle was invalid!");
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
        if (!globalData.display)
        {
			ForceClose(
				"set window '" + to_string(ID) + "' max size",
                "the display handle was invalid!");
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

        XGetWMNormalHints(
            display,
            window,
            &hints,
            &supplied);

        hints.flags |= PMinSize | PMaxSize;
        hints.min_width = (int)minSize.x;
        hints.min_height = (int)minSize.y;
        hints.max_width = (int)maxSize.x;
        hints.max_height = (int)maxSize.y;

        XSetWMNormalHints(display, window, &hints);
        XFlush(display);

        XSizeHints verify{};
        XGetWMNormalHints(
            display,
            window,
            &verify,
            &supplied);

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
        if (!globalData.display)
        {
			ForceClose(
				"set window '" + to_string(ID) + "' min size",
                "the display handle was invalid!");
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

        XGetWMNormalHints(
            display,
            window,
            &hints,
            &supplied);

        hints.flags |= PMinSize | PMaxSize;
        hints.min_width = (int)minSize.x;
        hints.min_height = (int)minSize.y;
        hints.max_width = (int)maxSize.x;
        hints.max_height = (int)maxSize.y;

        XSetWMNormalHints(display, window, &hints);
        XFlush(display);

        XSizeHints verify{};
        XGetWMNormalHints(
            display,
            window,
            &verify,
            &supplied);

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
        if (!globalData.display)
        {
			ForceClose(
				"set window '" + to_string(ID) + "' pos",
                "the display handle was invalid!");
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
        if (!globalData.display)
        {
			ForceClose(
				"get window '" + to_string(ID) + "' always on top state",
                "the display handle was invalid!");
        }

        Display* display = ToVar<Display*>(globalData.display);
        Window window = ToVar<Window>(windowData.window);

        Atom atom_net_wm_state       = ToVar<Atom>(globalData.atom_net_wm_state);
        Atom atom_net_wm_state_above = ToVar<Atom>(globalData.atom_net_wm_state_above);

        Atom actualType{};
        int actualFormat{};
        unsigned long nItems{}, bytesAfter{};
        unsigned char* data{};

        XRESULT = XGetWindowProperty(
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
            &data);

        if (XRESULT != SUCCESS_XGETWINDOWPROPERTY)
        {
			ForceClose(
				"get window '" + to_string(ID) + " always on top state",
                "XGetWindowProperty failed! Result code: " + to_string(XRESULT));
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
        if (!globalData.display)
        {
			ForceClose(
				"set window '" + to_string(ID) + "' always on top state",
                "the display handle was invalid!");
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

        XRESULT = XSendEvent(
            display,
            DefaultRootWindow(display),
            False,
            SubstructureRedirectMask
            | SubstructureNotifyMask,
            &event);

        if (XRESULT != SUCCESS_XSENDEVENT)
        {
			ForceClose(
				"set window '" + to_string(ID) + "' always on top state",
                "XSendEvent failed! Result code: " + to_string(XRESULT));
        }

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
        if (!globalData.display)
        {
			ForceClose(
				"get window '" + to_string(ID) + "' resizable state",
                "the display handle was invalid!");
        }

        Display* display = ToVar<Display*>(globalData.display);
        Window window = ToVar<Window>(windowData.window);

        Atom netWmAllowedActions = ToVar<Atom>(globalData.atom_net_wm_allowed_actions);
        Atom netWmActionResize = ToVar<Atom>(globalData.atom_net_wm_action_resize);

        Atom actualType{};
        int actualFormat{};
        unsigned long nItems{}, bytesAfter{};
        unsigned char* prop{};

        XRESULT = XGetWindowProperty(
            display,
            window,
            netWmAllowedActions,
            0L,
            256L,
            False,
            XA_ATOM,
            &actualType,
            &actualFormat,
            &nItems,
            &bytesAfter,
            &prop);

        if (XRESULT != SUCCESS_XGETWINDOWPROPERTY
            || actualType != XA_ATOM
            || actualFormat != 32)
        {
            if (prop) XFree(prop);

            //WM doesnt support EWMH - not resizable as far as we can tell
            return false;
        }

        Atom* allowedActions = rcast<Atom*>(prop);
        bool resizable{};
        for (unsigned long i = 0; i < nItems; ++i)
        {
            if (allowedActions[i] == netWmActionResize)
            {
                resizable = true;
                break;
            }
        }

        XFree(prop);
        return resizable;
    }
    void ProcessWindow::SetResizableState(bool state)
    {
        const X11GlobalData& globalData = Window_Global::GetGlobalData();
        if (!globalData.display)
        {
			ForceClose(
				"set window '" + to_string(ID) + "' resizable state",
                "the display handle was invalid!");
        }
        
        Display* display = ToVar<Display*>(globalData.display);
        Window window = ToVar<Window>(windowData.window);

        XSizeHints hints{};
        long supplied{};

        XGetWMNormalHints(
            display,
            window,
            &hints,
            &supplied);

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
        if (!globalData.display)
        {
			ForceClose(
				"get window '" + to_string(ID) + "' class",
                "the display handle was invalid!");
        }

        XClassHint classHint{};
        XGetClassHint(
            ToVar<Display*>(globalData.display),
            ToVar<Window>(windowData.window),
            &classHint);

        if (!classHint.res_name
            || !classHint.res_class)
        {
			Log::Print(
				"Failed to get window '" + to_string(ID) + "' class value "
                "because XClassHint failed or it had no name or class value! Result code: " + to_string(XRESULT),
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
        if (!globalData.display)
        {
			ForceClose(
				"set window '" + to_string(ID) + "' class",
                "the display handle was invalid!");
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

    WindowMode ProcessWindow::GetWindowMode() { return windowMode; }
    void ProcessWindow::SetWindowMode(WindowMode mode)
    {
        const X11GlobalData& globalData = Window_Global::GetGlobalData();
        if (!globalData.display)
        {
			ForceClose(
				"set window '" + to_string(ID) + "' mode",
                "the display handle was invalid!");
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

        XRESULT = XSendEvent(
            display,
            DefaultRootWindow(display),
            False,
            SubstructureRedirectMask
            | SubstructureNotifyMask,
            &event);

        if (XRESULT != SUCCESS_XSENDEVENT)
        {
			ForceClose(
				"set window '" + to_string(ID) + "' mode",
                "XSendEvent failed! Result code: " + to_string(XRESULT));
        }

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
        if (!globalData.display)
        {
			ForceClose(
				"set window '" + to_string(ID) + "' state",
                "the display handle was invalid!");
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

            XRESULT = XSendEvent(
                display,
                DefaultRootWindow(display),
                False,
                SubstructureRedirectMask
                | SubstructureNotifyMask,
                &event);

            if (XRESULT != SUCCESS_XSENDEVENT)
            {
                ForceClose(
                    "set window '" + to_string(ID) + "' state",
                    "XSendEvent failed! Result code: " + to_string(XRESULT));
            }

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

            XRESULT = XSendEvent(
                display,
                DefaultRootWindow(display),
                False,
                SubstructureRedirectMask
                | SubstructureNotifyMask,
                &event);

            if (XRESULT != SUCCESS_XSENDEVENT)
            {
                ForceClose(
                    "set window '" + to_string(ID) + "' state",
                    "XSendEvent failed! Result code: " + to_string(XRESULT));
            }

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

    void ProcessWindow::SetEarlyUpdateCallback(function<void()>&& newValue)
    {
		if (!newValue)
		{
			Log::Print(
				"Assigned empty early update callback to window '" + to_string(ID) + "'.",
				"KW_WINDOW",
				LogType::LOG_WARNING);
		}

		earlyUpdateCallback = std::move(newValue);
    }
    void ProcessWindow::SetUpdateCallback(function<void()>&& newValue)
    {
		if (!newValue)
		{
			Log::Print(
				"Assigned empty update callback to window '" + to_string(ID) + "'.",
				"KW_WINDOW",
				LogType::LOG_WARNING);
		}

		updateCallback = std::move(newValue);
    }
    void ProcessWindow::SetLateUpdateCallback(function<void()>&& newValue)
    {
		if (!newValue)
		{
			Log::Print(
				"Assigned late update callback to window '" + to_string(ID) + "'.",
				"KW_WINDOW",
				LogType::LOG_WARNING);
		}

		lateUpdateCallback = std::move(newValue);
    }

	void ProcessWindow::SetResizeCallback(function<void()>&& newValue)
	{
		if (!newValue)
		{
			Log::Print(
				"Assigned empty resize callback to window '" + to_string(ID) + "'.",
				"KW_WINDOW",
				LogType::LOG_WARNING);
		}

		resizeCallback = std::move(newValue);
	}

	void ProcessWindow::SetShutdownCallback(function<void()>&& newValue)
	{
		if (!newValue)
		{
			Log::Print(
				"Assigned empty shutdown callback to window '" + to_string(ID) + "'.",
				"KW_WINDOW",
				LogType::LOG_WARNING);
		}

		shutdownCallback = std::move(newValue);
	}

    void ProcessWindow::UpdateIdleState()
    {
        isIdle =
            !IsForegroundWindow()
            || IsMinimized()
            || !IsVisible();
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

        XRESULT = XGetWindowProperty(
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

        if (XRESULT != SUCCESS_XGETWINDOWPROPERTY)
        {
			ForceClose(
				"update window '" + to_string(ID) + " fullscreen and minimized state",
                "XGetWindowProperty failed! Result code: " + to_string(XRESULT));
        }

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
                ProcessWindow* w{};
                string err = ProcessWindow::GetRegistry().GetContent(childID, w);
                if (!err.empty())
                {
                    KalaWindowCore::ForceClose(
                        "KalaWindow message loop error",
                        "Failed to minimize child window '" + to_string(childID) 
                        + "' under parent '" + to_string(w->ID) + "'! Reason: " + err);
                }

                w->SetWindowState(WindowState::WINDOW_MINIMIZE);
            }
        }
        else if (wasMinimized
                 && !isMinimized)
        {
            for (u32 childID : childIDs)
            {
                ProcessWindow* w{};
                string err = ProcessWindow::GetRegistry().GetContent(childID, w);
                if (!err.empty())
                {
                    KalaWindowCore::ForceClose(
                        "KalaWindow message loop error",
                        "Failed to bring child window '" + to_string(childID) 
                        + "' under parent '" + to_string(w->ID) + "' to focus! Reason: " + err);
                }

                Window childWindow = ToVar<Window>(w->GetWindowData().window);

                XMapWindow(display, childWindow);
                
                w->BringToFocus();
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

			ProcessWindow* pw{};
            string err = ProcessWindow::GetRegistry().GetContent(w, pw);
			if (!err.empty())
            {
				KalaWindowCore::ForceClose(
					"KalaWindow window error",
					"Failed to destroy child window '" + to_string(w) 
					+ "' under parent '" + to_string(ID) + "'! Reason: " + err);
            }

            pw->Destroy();
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

			ProcessWindow* pw{};
            string err = ProcessWindow::GetRegistry().GetContent(parentID, pw);
			if (!err.empty())
			{
				KalaWindowCore::ForceClose(
					"KalaWindow window error",
					"Failed to destroy child window '" + to_string(ID) 
					+ "' under parent '" + to_string(parentID) + "'! Reason: " + err);
			}

            auto it = find(pw->childIDs.begin(), pw->childIDs.end(), ID);
            if (it != pw->childIDs.end()) pw->childIDs.erase(it);
		}

        string err = VulkanContext::GetRegistry().DestroyContent(graphicsContextID);
		if (!err.empty())
		{
			KalaWindowCore::ForceClose(
				"KalaWindow window error",
				"Failed to destroy window '" + to_string(ID) + "' Vulkan content! Reason: " + err);
		}
		err = Input::GetRegistry().DestroyContent(inputID);
		if (!err.empty())
		{
			KalaWindowCore::ForceClose(
				"KalaWindow window error",
				"Failed to destroy window '" + to_string(ID) + "' input content! Reason: " + err);
		}
		
		err = registry.DestroyContent(ID);
		if (!err.empty())
		{
			KalaWindowCore::ForceClose(
				"KalaWindow window error",
				"Failed to destroy window '" + to_string(ID) + "'! Reason: " + err);
		}
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

            //shuts down x11 libnotify and libcanberra
            Window_Global::Shutdown();

			Log::Print(
                "\n======================================================================"
                "\nFINISHED SHUTDOWN"
                "\n======================================================================\n",
                true);

			exit(0);
        }
    }
}

#endif //KLIN_ANY