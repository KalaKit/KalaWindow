//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef KALAKIT_WAYLAND

#pragma once

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GL/gl.h>

//kalawindow
#include "platform.hpp"

namespace KalaKit
{
    class OpenGL_Wayland
    {
    public:
        static inline EGLDisplay eglDisplay = EGL_NO_DISPLAY;
        static inline EGLSurface eglSurface = EGL_NO_SURFACE;
        static inline EGLContext eglContext = EGL_NO_CONTEXT;
        static inline EGLConfig eglConfig = nullptr;
    };
}

#endif // KALAKIT_WAYLAND