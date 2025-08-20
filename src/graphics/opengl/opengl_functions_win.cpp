//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef _WIN32

#include <windows.h>
#include <string>

#include "KalaHeaders/core_types.hpp"

#include "graphics/opengl/opengl_functions_win.hpp"
#include "core/core.hpp"
#include "core/global_handles.hpp"

using KalaWindow::Core::KalaWindowCore;
using KalaWindow::Core::GlobalHandle;
using namespace KalaWindow::Graphics::OpenGLFunctions;

using std::string;

struct WinFunctionCheck
{
    const char* name;
    const void* ptr;
};

WinFunctionCheck checks[] =
{
    { "wglCreateContextAttribsARB", wglCreateContextAttribsARB },
    { "wglChoosePixelFormatARB",    wglChoosePixelFormatARB },
    { "wglSwapIntervalEXT",         wglSwapIntervalEXT }
};

namespace KalaWindow::Graphics::OpenGLFunctions
{
	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = nullptr;
	PFNWGLCHOOSEPIXELFORMATARBPROC    wglChoosePixelFormatARB    = nullptr;
	PFNWGLSWAPINTERVALEXTPROC         wglSwapIntervalEXT         = nullptr;

	void OpenGL_Functions_Windows::LoadAllWinFunctions()
	{
        wglCreateContextAttribsARB = reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(wglGetProcAddress("wglCreateContextAttribsARB"));
        wglChoosePixelFormatARB =    reinterpret_cast<PFNWGLCHOOSEPIXELFORMATARBPROC>   (wglGetProcAddress("wglChoosePixelFormatARB"));
        wglSwapIntervalEXT =         reinterpret_cast<PFNWGLSWAPINTERVALEXTPROC>        (wglGetProcAddress("wglSwapIntervalEXT"));

        for (auto& check : checks)
        {
            if (!check.ptr)
            {
                KalaWindowCore::ForceClose(
                    "OpenGL Windows function error",
                    "Failed to load function '" + string(check.name) + "'");
            }
        }
	}

    void OpenGL_Functions_Windows::LoadWinFunction(void** target, const char* name)
    {
        //check if already loaded
        auto it = find_if(
            loadedWinFunctions.begin(),
            loadedWinFunctions.end(),
            [name](const WinGLFunction& rec) { return rec.name == name; });

        //already loaded - return existing one
        if (it != loadedWinFunctions.end())
        {
            *target = it->ptr;
            return;
        }

        //try to load
        *target = reinterpret_cast<void*>(wglGetProcAddress(name));
        if (!*target)
        {
            HMODULE module = ToVar<HMODULE>(GlobalHandle::GetOpenGLHandle());
            *target = reinterpret_cast<void*>(GetProcAddress(module, name));
        }

        if (!*target)
        {
            KalaWindowCore::ForceClose(
                "OpenGL Core function error",
                "Failed to load OpenGL error '" + string(name) + "'!");
        }

        loadedWinFunctions.push_back(
            {
                name,
                *target
            });
    }
}

#endif //_WIN32