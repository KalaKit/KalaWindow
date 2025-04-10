//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef KALAKIT_X11

#define KALAKIT_MODULE "WINDOW"

//external
#include "crashHandler.hpp"

//kalawindow
#include "window.hpp"
#include "input.hpp"
#include "opengl.hpp"
#include "opengl_loader.hpp"

namespace KalaKit
{
    bool KalaWindow::Initialize(
        const string& title,
        int width,
        int height)
    {
        if (isInitialized)
        {
            LOG_ERROR("Window is already initialized!");
            return false;
        }

        //initialize the crash handler first
        KalaCrashHandler::Initialize();

        Display* display = XOpenDisplay(nullptr);
        if (!display)
        {
            LOG_ERROR("Failed to open X11 display!");
            return false;
        }
        window.display = display;

        ::Window root = DefaultRootWindow(window.display);

        XSSetWindowAttributes swa;
        swa.event_mast =
            ExposureMask
            | KeyPressMask
            | StructureNotifyMask;

        ::Window win = XCreateWindow(
            window.display,
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

        if (!win)
        {
            LOG_ERROR("Failed to create X11 window!");
            XCloseDisplay(window.display);
            return false;
        }
        window.handle = win;

        XStoreName(window.display, window.handle, title.c_str());
        XMapWindow(window.display, window.handle); //show the window

        //initialize input
        KalaInput::Initialize();

        //initialize opengl
        bool openglInitialized = OpenGL::Initialize();
        if (!openglInitialized) return false;

        //and finally set opengl viewport size
        OpenGLLoader::glViewportPtr(0, 0, width, height);

        isInitialized = true;

        LOG_SUCCESS("Window successfully initialized");
        return true;
    }
}

#endif // KALAKIT_X11