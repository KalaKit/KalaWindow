//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include "core/kw_registry.hpp"
#include <X11/X.h>
#if defined(__linux__) && defined(KW_USE_X11)

#include <X11/Xlib.h>

#include <vector>

#include "KalaHeaders/core_utils.hpp"

#include "core/kw_messageloop_x11.hpp"
#include "graphics/kw_window_global.hpp"
#include "graphics/kw_window.hpp"

using KalaWindow::Graphics::Window_Global;
using KalaWindow::Graphics::X11GlobalData;
using KalaWindow::Graphics::ProcessWindow;
using KalaWindow::Graphics::KalaWindowRegistry;
using KalaWindow::Graphics::WindowData;

using KalaHeaders::KalaCore::ToVar;

using std::vector;

static void DispatchEvents(
    const XEvent& event,
    const X11GlobalData& globalData);

namespace KalaWindow::Core
{
    void MessageLoop::Update()
    {
        const X11GlobalData& globalData = Window_Global::GetGlobalData();

        Display* display = ToVar<Display*>(globalData.display);

        while (XPending(display))
        {
            XEvent event{};
            XNextEvent(display, &event);

            DispatchEvents(event, globalData);
        }
    }
}

void DispatchEvents(
    const XEvent& event,
    const X11GlobalData& globalData)
{
    const vector<ProcessWindow*>& activeWindows = KalaWindowRegistry<ProcessWindow>::runtimeContent;

    Atom wmDelete = ToVar<Atom>(globalData.atom_wmDelete);
    Window target = event.xany.window;

    for (const auto& w : activeWindows)
    {
        if (!w
            || !w->IsInitialized())
        {
            continue;
        }

        const WindowData& wdata = w->GetWindowData();

        Window window = ToVar<Window>(wdata.window);

        if (target != window) continue;

        switch (event.type)
        {
            //TODO: add input events here

            case ClientMessage:
                if ((Atom)event.xclient.data.l[0] == wmDelete) w->CloseWindow();
                break;
        }

        break;
    }
}

#endif //__linux__ && KW_USE_X11