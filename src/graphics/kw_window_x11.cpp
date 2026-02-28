//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.


#if defined(__linux__) && defined(KW_USE_X11)

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/X.h>

#include <memory>

#include "KalaHeaders/core_utils.hpp"
#include "KalaHeaders/log_utils.hpp"

#include "graphics/kw_window.hpp"
#include "core/kw_core.hpp"
#include "core/kw_input.hpp"
#include "graphics/kw_window_global.hpp"
#include "opengl/kw_opengl.hpp"

using KalaHeaders::KalaCore::ToVar;
using KalaHeaders::KalaCore::FromVar;

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;

using KalaWindow::Core::KalaWindowCore;
using KalaWindow::Core::ShutdownState;
using KalaWindow::Core::Input;
using KalaWindow::OpenGL::OpenGL_Context;
using KalaWindow::Graphics::ProcessWindow;
using KalaWindow::Graphics::X11GlobalData;

using std::make_unique;
using std::unique_ptr;
using std::to_string;

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
		vec2 size,
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

        u32 newID = KalaWindowCore::GetGlobalID() + 1;
		KalaWindowCore::SetGlobalID(newID);

		unique_ptr<ProcessWindow> newWindow = make_unique<ProcessWindow>();
		ProcessWindow* windowPtr = newWindow.get();

		Log::Print(
			"Creating window '" + string(title) + "' with ID '" + to_string(newID) + "'.",
			"WINDOW",
			LogType::LOG_DEBUG);

        Display* display = ToVar<Display*>(globalData.display);

        Window root = ToVar<Window>(globalData.window_root);
        Window parent = parentWindow
            ? ToVar<Window>(parentWindow->windowData.window)
            : 0;

        bool isChild = parentWindow;

        Atom wmDelete = ToVar<Atom>(globalData.atom_wmDelete);

        Window window = XCreateSimpleWindow(
            display,
            root,
            100,
            100,
            size.x,
            size.y,
            1,
            BlackPixel(display, DefaultScreen(display)),
            WhitePixel(display, DefaultScreen(display)));

        if (isChild)
        {
            //assign parent window
            XSetTransientForHint(
                display,
                window,
                parent);
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
            &wmDelete,
            1);

        //allow events
        XSelectInput(
            display,
            window,
            ExposureMask
            | KeyPressMask
            | StructureNotifyMask);

        WindowData newWindowStruct{};

        //set window dpi aware state here...

        newWindowStruct.window = FromVar(window);

        windowPtr->windowData = newWindowStruct;

        windowPtr->SetTitle(title);
		windowPtr->ID = newID;
		windowPtr->SetClientRectSize(size);

        windowPtr->SetWindowClass(title);

		windowPtr->isInitialized = true;

		windowPtr->oldPos = windowPtr->GetPosition();
		windowPtr->oldSize = windowPtr->GetClientRectSize();

        //show window
        XMapWindow(
            display,
            window);

		//allow files to be dragged to this window
		//DragAcceptFiles(newHwnd, TRUE);

		registry.AddContent(newID, std::move(newWindow));

		Log::Print(
			"Created window '" + string(title) + "' with ID '" + to_string(newID) + "'!",
			"WINDOW",
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
				"WINDOW",
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
				"WINDOW",
				LogType::LOG_ERROR,
				2);

			return;
		}

		if (newValue.length() > MAX_TITLE_LENGTH)
		{
			Log::Print(
				"Window title exceeded max allowed length of '" + to_string(MAX_TITLE_LENGTH) + "'! Title has been truncated.",
				"WINDOW",
				LogType::LOG_ERROR,
                2);

            return;
		}

        const X11GlobalData& globalData = Window_Global::GetGlobalData();
        if (globalData.display
            && windowData.window)
        {
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
    }
    const string& ProcessWindow::GetTitle() const { static string title{}; return title; }

    void ProcessWindow::SetIcon(u32 texture) const {}
    u32 ProcessWindow::GetIcon() const { return iconID; }
	void ProcessWindow::ClearIcon() const {}

    void ProcessWindow::SetTaskbarOverlayIcon(
		u32 texture,
		string_view tooltip) const {}
	u32 ProcessWindow::GetTaskbarOverlayIcon() const { return overlayIconID; }
	void ProcessWindow::ClearTaskbarOverlayIcon() const {}

    void ProcessWindow::BringToFocus() {}

    void ProcessWindow::SetClientRectSize(vec2 newSize) {}
    vec2 ProcessWindow::GetClientRectSize() const { static vec2 rect{}; return rect; }

    void ProcessWindow::SetOuterSize(vec2 newSize) {}
    vec2 ProcessWindow::GetOuterSize() const { static vec2 outer{}; return outer; }

    void ProcessWindow::SetPosition(vec2 newPosition) const {}
    vec2 ProcessWindow::GetPosition() { static vec2 pos{}; return pos; }

    void ProcessWindow::SetMaxSize(vec2 newMaxSize) { maxSize = newMaxSize; }
	vec2 ProcessWindow::GetMaxSize() const { return maxSize; }

	void ProcessWindow::SetMinSize(vec2 newMinSize) { minSize = newMinSize; }
	vec2 ProcessWindow::GetMinSize() const { return minSize; }

	void ProcessWindow::SetFocusRequired(bool newFocusRequired) { isWindowFocusRequired = newFocusRequired; }
	bool ProcessWindow::IsFocusRequired() const { return isWindowFocusRequired; }

    void ProcessWindow::SetAlwaysOnTopState(bool state) const {}
    bool ProcessWindow::IsAlwaysOnTop() const { static bool top{}; return top; }

    void ProcessWindow::SetResizableState(bool state) const {}
    bool ProcessWindow::IsResizable() const { bool res{}; return res; }

    void ProcessWindow::SetWindowClass(string_view newValue)
    {
		if (newValue.empty())
		{
			Log::Print(
				"Class value cannot be empty!",
				"WINDOW",
				LogType::LOG_ERROR,
				2);

			return;
		}

		if (newValue.length() > MAX_TITLE_LENGTH)
		{
			Log::Print(
				"Class value exceeded max allowed length of '" + to_string(MAX_TITLE_LENGTH) + "'! Title has been truncated.",
				"WINDOW",
				LogType::LOG_ERROR,
                2);

            return;
		}

        const X11GlobalData& globalData = Window_Global::GetGlobalData();
        if (globalData.display
            && windowData.window)
        {
            string value(newValue);

            XClassHint classHint{};
                classHint.res_name = const_cast<char*>(value.c_str());
                classHint.res_class = const_cast<char*>(value.c_str());

            XSetClassHint(
                ToVar<Display*>(globalData.display),
                windowData.window,
                &classHint);
        }
    }

    bool ProcessWindow::IsIdle() const { return isIdle; }

    bool ProcessWindow::IsForegroundWindow() const { static bool fore{}; return fore; }
    bool ProcessWindow::IsFocused() const { static bool focu{}; return focu; }
    bool ProcessWindow::IsFullscreen() { static bool full{}; return full; }
    bool ProcessWindow::IsMinimized() const { static bool mini{}; return mini; }
    bool ProcessWindow::IsVisible() const { static bool visi{}; return visi; }

    void ProcessWindow::SetResizingState(bool newState) { isResizing = newState; }
	bool ProcessWindow::IsResizing() const { return isResizing; }

    void ProcessWindow::SetWindowMode(WindowMode mode) {}
    WindowMode ProcessWindow::GetWindowMode() { static WindowMode mode{}; return mode; }

    void ProcessWindow::SetWindowState(WindowState state) {}
    WindowState ProcessWindow::GetWindowState() const { static WindowState state{}; return state; }

    void ProcessWindow::TriggerResize() { if (resizeCallback) resizeCallback(); }
	void ProcessWindow::SetResizeCallback(const function<void()>& callback) { resizeCallback = callback; }

	void ProcessWindow::TriggerRedraw() { if (redrawCallback) redrawCallback(); }
	void ProcessWindow::SetRedrawCallback(const function<void()>& callback) { redrawCallback = callback; }

    void ProcessWindow::SetWindowData(const WindowData& newWindowStruct) { windowData = newWindowStruct; }
	const WindowData& ProcessWindow::GetWindowData() const { return windowData; }

    u32 ProcessWindow::GetInputID() const { return inputID; }
	void ProcessWindow::SetInputID(u32 newValue) { inputID = newValue; }

	u32 ProcessWindow::GetGLID() const { return glID; }
	void ProcessWindow::SetGLID(u32 newValue) { glID = newValue; }

	u32 ProcessWindow::GetMenuBarID() const { return menuBarID; }
	void ProcessWindow::SetMenuBarID(u32 newValue) { menuBarID = newValue; }

	void ProcessWindow::SetCleanExternalContent(function<void(u32)> newValue) { cleanExternalContent = newValue; }

    void ProcessWindow::CloseWindow()
    {
        if (cleanExternalContent) cleanExternalContent(ID);
		
		KalaWindowRegistry<OpenGL_Context>::RemoveAllWindowContent(ID);

		KalaWindowRegistry<Input>::RemoveAllWindowContent(ID);
		
		KalaWindowRegistry<ProcessWindow>::RemoveContent(ID);

        if (KalaWindowRegistry<ProcessWindow>::createdContent.size() == 0)
        {
            KalaWindowCore::Shutdown(ShutdownState::SHUTDOWN_CLEAN);
        }
    }

    ProcessWindow::~ProcessWindow()
    {
        const X11GlobalData& globalData = Window_Global::GetGlobalData();
        if (globalData.display)
        {
            Display* display = ToVar<Display*>(globalData.display);
            Window window = ToVar<Window>(windowData.window);

            XDestroyWindow(display, window);
        }
    }
}

#endif //__linux__ and KW_USE_X11