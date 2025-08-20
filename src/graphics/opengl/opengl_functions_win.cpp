//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef _WIN32

#include <windows.h>
#include <string>

#include "graphics/opengl/opengl_functions_win.hpp"
#include "core/core.hpp"

using std::string;

static void VerifyFunctions();

namespace KalaWindow::Graphics::OpenGLFunctions
{
	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = nullptr;
	PFNWGLCHOOSEPIXELFORMATARBPROC    wglChoosePixelFormatARB    = nullptr;
	PFNWGLSWAPINTERVALEXTPROC         wglSwapIntervalEXT         = nullptr;

	void OpenGL_Functions_Windows::LoadFunctions()
	{
        wglCreateContextAttribsARB = reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(wglGetProcAddress("wglCreateContextAttribsARB"));
        wglChoosePixelFormatARB =    reinterpret_cast<PFNWGLCHOOSEPIXELFORMATARBPROC>   (wglGetProcAddress("wglChoosePixelFormatARB"));
        wglSwapIntervalEXT =         reinterpret_cast<PFNWGLSWAPINTERVALEXTPROC>        (wglGetProcAddress("wglSwapIntervalEXT"));

        VerifyFunctions();
	}
}

void VerifyFunctions()
{
    using KalaWindow::Core::KalaWindowCore;

    using namespace KalaWindow::Graphics::OpenGLFunctions;

    struct FunctionCheck
    {
        const char* name;
        const void* ptr;
    };

    FunctionCheck checks[] =
    {
        { "wglCreateContextAttribsARB", wglCreateContextAttribsARB },
        { "wglChoosePixelFormatARB",    wglChoosePixelFormatARB },
        { "wglSwapIntervalEXT",         wglSwapIntervalEXT }
    };

    for (auto& check : checks)
    {
        if (!check.ptr)
        {
            KalaWindowCore::ForceClose(
                "OpenGL Windows Function error",
                "Failed to load function '" + string(check.name) + "'");
        }
    }
}

#endif //_WIN32