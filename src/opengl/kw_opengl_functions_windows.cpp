//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef _WIN32

#include <windows.h>
#include <string>

#include "KalaHeaders/log_utils.hpp"
#include "KalaHeaders/core_utils.hpp"

#include "opengl/kw_opengl_functions_windows.hpp"
#include "opengl/kw_opengl.hpp"
#include "core/kw_core.hpp"

using KalaHeaders::KalaCore::ToVar;
using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;

using KalaWindow::Core::KalaWindowCore;
using namespace KalaWindow::OpenGL::OpenGLFunctions;

using std::string;

struct WinGLFunction
{
    const char* name;
    void** target;
};

namespace KalaWindow::OpenGL::OpenGLFunctions
{
    static GL_Windows glWindows{};

    const GL_Windows* OpenGL_Functions_Windows::GetGLWindows()
    {
        return &glWindows;
    }

    WinGLFunction functions[] =
    {
        { "wglCreateContextAttribsARB",   reinterpret_cast<void**>(&glWindows.wglCreateContextAttribsARB) },
        { "wglChoosePixelFormatARB",      reinterpret_cast<void**>(&glWindows.wglChoosePixelFormatARB) },
        { "wglSwapIntervalEXT",           reinterpret_cast<void**>(&glWindows.wglSwapIntervalEXT) },
        { "wglGetPixelFormatAttribfvARB", reinterpret_cast<void**>(&glWindows.wglGetPixelFormatAttribfvARB) },
        { "wglGetPixelFormatAttribivARB", reinterpret_cast<void**>(&glWindows.wglGetPixelFormatAttribivARB) }
    };

	void OpenGL_Functions_Windows::LoadAllWindowsFunctions()
	{
        for (auto& entry : functions)
        {
            void* ptr = reinterpret_cast<void*>(wglGetProcAddress(entry.name));
            if (!ptr)
            {
                HMODULE module = ToVar<HMODULE>(OpenGL_Global::GetOpenGLLibrary());
                ptr = reinterpret_cast<void*>(GetProcAddress(module, entry.name));
            }

            if (!ptr)
            {
                KalaWindowCore::ForceClose(
                    "OpenGL Windows function error",
                    "Failed to load OpenGL function '" + string(entry.name) + "'!");
            }

            //assign into GL_Windows dispatch table
            *entry.target = ptr;

            Log::Print(
                "Loaded '" + string(entry.name) + "'!",
                "OPENGL_WINDOWS",
                LogType::LOG_DEBUG);
        }
	}
}

#endif //_WIN32