//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#if defined(__linux__) && defined(KW_USE_X11)

#include <X11/X.h>
#include <X11/extensions/Xrandr.h>

#include "glcorearb.h"

#include <vector>

#include "core_utils.hpp"
#include "math_utils.hpp"

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

using KalaHeaders::KalaMath::vec2;

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

        Atom atom_wm_delete = ToVar<Atom>(globalData.atom_wm_delete);

        Atom atom_net_wm_state = ToVar<Atom>(globalData.atom_net_wm_state);

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
                //same idea as WM_SIZE
                case ConfigureNotify:
                {
                    vec2 newPos = vec2(event.xconfigure.x, event.xconfigure.y);
                    vec2 newSize = vec2(event.xconfigure.width, event.xconfigure.height);

                    vec2 oldSize = w->size;

                    w->pos = newPos;
                    w->size = newSize;

                    if (w->IsResizable()
                        && oldSize != newSize)
                    {
                        vec2 winSize = w->GetClientRectSize();

                        if (OpenGL_Global::IsInitialized())
                        {
                            const GL_Core* coreFunc = OpenGL_Functions_Core::GetGLCore();

                            coreFunc->glViewport(
                                0,
                                0,
                                (GLsizei)winSize.x,
					            (GLsizei)winSize.y);
                        }
                    }

                    break;
                }

                case Expose:
                {
                    if (event.xexpose.count == 0) w->TriggerRedraw();
                    break;
                }

                case ClientMessage:
                {
                    if ((Atom)event.xclient.data.l[0] == atom_wm_delete) w->CloseWindow();
                    break;
                }

                case PropertyNotify:
                {
                    if (event.xproperty.atom == atom_net_wm_state)
                    {
                        w->UpdateFullscreenState();
                    }
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

                case MapNotify:
                {   
                    w->isVisible = true;
                    break;
                }
                case UnmapNotify:
                {   
                    w->isVisible = false;
                    break;
                }

                //mouse is hovering over window
                case EnterNotify:   break;
                //mouse is no longer hovering over window
                case LeaveNotify:   break;

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