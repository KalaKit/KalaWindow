//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#if defined(__linux__) && defined(KW_USE_X11)

#include <X11/X.h>
#include <X11/extensions/Xrandr.h>

#include <vector>

#include "KalaHeaders/core_utils.hpp"

#include "core/kw_messageloop_x11.hpp"
#include "core/kw_registry.hpp"
#include "graphics/kw_window_global.hpp"
#include "graphics/kw_window.hpp"
#include "opengl/kw_opengl.hpp"
#include "opengl/kw_opengl_functions_core.hpp"

using KalaWindow::Graphics::Window_Global;
using KalaWindow::Graphics::X11GlobalData;
using KalaWindow::Graphics::ProcessWindow;
using KalaWindow::Graphics::KalaWindowRegistry;
using KalaWindow::Graphics::WindowData;
using KalaWindow::OpenGL::OpenGL_Global;
using KalaWindow::OpenGL::OpenGLFunctions::GL_Core;
using KalaWindow::OpenGL::OpenGLFunctions::OpenGL_Functions_Core;

using KalaHeaders::KalaCore::ToVar;

using std::vector;

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

            DispatchEvents(event);
        }
    }

    void MessageLoop::DispatchEvents(const XEvent& event)
    {
        const X11GlobalData& globalData = Window_Global::GetGlobalData();

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
                case Expose:
                {
                    if (event.xexpose.count == 0) w->TriggerRedraw();
                    break;
                }

                case ConfigureNotify:
                {
                    if (w->IsResizable())
                    {
                        int width = event.xconfigure.width;
                        int height = event.xconfigure.height;

                        if (OpenGL_Global::IsInitialized()
                            && width > 0
                            && height > 0)
                        {
                            const GL_Core* coreFunc = OpenGL_Functions_Core::GetGLCore();

                            coreFunc->glViewport(
                                0,
                                0,
                                width,
                                height);
                        }

                        w->TriggerResize();
                        w->TriggerRedraw();
                    }

                    break;
                }

                case ClientMessage:
                {
                    if ((Atom)event.xclient.data.l[0] == wmDelete) w->CloseWindow();
                    break;
                }

                case FocusIn:
                {
                    w->isFocused = true;
                    break;
                }
                case FocusOut:
                {
                    w->isFocused = false;
                    break;
                }

                case KeyPress:      break;
                case KeyRelease:    break;
                case ButtonPress:   break;
                case ButtonRelease: break;
                case MotionNotify:  break;
            }

            break;
        }
    }
}

#endif //__linux__ && KW_USE_X11