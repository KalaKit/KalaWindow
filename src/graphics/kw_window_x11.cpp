//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#if defined(__linux__) && defined(KW_USE_X11)

#include <X11/Xlib.h>
#include <X11/Xatom.h>
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
		const string& title,
		vec2 size,
		ProcessWindow* parentWindow ,
		DpiContext context)
    {
		if (!Window_Global::IsInitialized())
		{
			KalaWindowCore::ForceClose(
				"Window error",
				"Failed to create window '" + title + "' because global window context has not been created!");

			return nullptr;
		}

        const X11GlobalData& globalData = Window_Global::GetGlobalData();

        u32 newID = KalaWindowCore::GetGlobalID() + 1;
		KalaWindowCore::SetGlobalID(newID);

		unique_ptr<ProcessWindow> newWindow = make_unique<ProcessWindow>();
		ProcessWindow* windowPtr = newWindow.get();

		Log::Print(
			"Creating window '" + title + "' with ID '" + to_string(newID) + "'.",
			"WINDOW",
			LogType::LOG_DEBUG);

        Display* display = ToVar<Display*>(globalData.display);

        Window parent = parentWindow
            ? ToVar<Window>(parentWindow->windowData.window)
            : ToVar<Window>(globalData.window_root);

        Atom wmDelete = ToVar<Atom>(globalData.atom_wmDelete);

        Window window = XCreateSimpleWindow(
            display,
            parent,
            100,
            100,
            size.x,
            size.y,
            1,
            BlackPixel(display, DefaultScreen(display)),
            WhitePixel(display, DefaultScreen(display)));

        //allow events
        XSelectInput(
            display,
            window,
            ExposureMask
            | KeyPressMask
            | StructureNotifyMask);

        XSetWMProtocols(
            display,
            window,
            &wmDelete,
            1);

        //show window
        XMapWindow(
            display,
            window);

        WindowData newWindowStruct{};

        //set window dpi aware state here...

        newWindowStruct.window = FromVar(window);

        windowPtr->windowData = newWindowStruct;

		windowPtr->SetTitle(title);
		windowPtr->ID = newID;
		windowPtr->SetClientRectSize(size);

		windowPtr->isInitialized = true;

		windowPtr->oldPos = windowPtr->GetPosition();
		windowPtr->oldSize = windowPtr->GetClientRectSize();

		//allow files to be dragged to this window
		//DragAcceptFiles(newHwnd, TRUE);

		registry.AddContent(newID, std::move(newWindow));

		Log::Print(
			"Created window '" + title + "' with ID '" + to_string(newID) + "'!",
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

        XEvent event{};

        const X11GlobalData& globalData = Window_Global::GetGlobalData();

        Display* display = ToVar<Display*>(globalData.display);
        Atom wmDelete = ToVar<Atom>(globalData.atom_wmDelete);

        XNextEvent(display, &event);

        switch (event.type)
        {
            case Expose: break;
            case KeyPress: CloseWindow(); break;
            case ClientMessage:
            {
                if ((Atom)event.xclient.data.l[0] == wmDelete)
                {
                    CloseWindow();
                    break;
                }
            }
        }
    }

	void ProcessWindow::SetLastDraggedFiles(const vector<string>& files) { lastDraggedFiles = files; };
	const vector<string>& ProcessWindow::GetLastDraggedFiles() const { return lastDraggedFiles; };
	void ProcessWindow::ClearLastDraggedFiles() { lastDraggedFiles.clear(); };

    void ProcessWindow::SetTitle(const string& newTitle) const {}
    const string& ProcessWindow::GetTitle() const { static string title{}; return title; }

    void ProcessWindow::SetIcon(u32 texture) const {}
    u32 ProcessWindow::GetIcon() const { return iconID; }
	void ProcessWindow::ClearIcon() const {}

    void ProcessWindow::SetTaskbarOverlayIcon(
		u32 texture,
		const string& tooltip) const {}
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