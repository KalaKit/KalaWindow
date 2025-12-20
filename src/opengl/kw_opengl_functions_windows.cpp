//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef _WIN32

#include <windows.h>
#include <vector>
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
using KalaWindow::OpenGL::OpenGL_Global;

using std::vector;
using std::string;

struct WinGLFunction
{
    const char* name;
    void** target;
};

static inline vector<WinGLFunction> loadedWinFunctions{};

WinGLFunction functions[] =
{
    { "wglCreateContextAttribsARB",   reinterpret_cast<void**>(&wglCreateContextAttribsARB) },
    { "wglChoosePixelFormatARB",      reinterpret_cast<void**>(&wglChoosePixelFormatARB) },
    { "wglSwapIntervalEXT",           reinterpret_cast<void**>(&wglSwapIntervalEXT) },
    { "wglGetPixelFormatAttribfvARB", reinterpret_cast<void**>(&wglGetPixelFormatAttribfvARB) },
    { "wglGetPixelFormatAttribivARB", reinterpret_cast<void**>(&wglGetPixelFormatAttribivARB) }
};

namespace KalaWindow::OpenGL::OpenGLFunctions
{
	PFNWGLCREATECONTEXTATTRIBSARBPROC   wglCreateContextAttribsARB = nullptr;
	PFNWGLCHOOSEPIXELFORMATARBPROC      wglChoosePixelFormatARB    = nullptr;
	PFNWGLSWAPINTERVALEXTPROC           wglSwapIntervalEXT         = nullptr;
    PFNWGLGETPIXELFORMATATTRIBFVARBPROC wglGetPixelFormatAttribfvARB = nullptr;
    PFNWGLGETPIXELFORMATATTRIBIVARBPROC wglGetPixelFormatAttribivARB = nullptr;

	void OpenGL_Functions_Windows::LoadAllWindowsFunctions()
	{
        for (const auto& func : functions)
        {
            LoadWindowsFunction(func.name);
        }
	}

    void OpenGL_Functions_Windows::LoadWindowsFunction(const char* name)
    {
        //check if already loaded
        auto it = find_if(
            loadedWinFunctions.begin(),
            loadedWinFunctions.end(),
            [name](const WinGLFunction& rec) { return strcmp(rec.name, name) == 0; });

        //already loaded
        if (it != loadedWinFunctions.end())
        {
            Log::Print(
                "Function '" + string(name) + "' is already loaded!",
                "OPENGL_WINDOWS",
                LogType::LOG_ERROR,
                2);

            return;
        }

        //find entry in registry
        WinGLFunction* entry = nullptr;
        for (auto& f : functions)
        {
            if (strcmp(f.name, name) == 0)
            {
                entry = &f;
                break;
            }
        }
        if (!entry)
        {
            Log::Print(
                "Function '" + string(name) + "' does not exist!",
                "OPENGL_WINDOWS",
                LogType::LOG_ERROR,
                2);

            return;
        }

        //try to load
        void* ptr = nullptr;

        ptr = reinterpret_cast<void*>(wglGetProcAddress(name));
        if (!ptr)
        {
            HMODULE module = ToVar<HMODULE>(OpenGL_Global::GetOpenGLLibrary());
            ptr = reinterpret_cast<void*>(GetProcAddress(module, name));
        }

        if (!ptr)
        {
            KalaWindowCore::ForceClose(
                "OpenGL Windows function error",
                "Failed to load OpenGL error '" + string(name) + "'!");
        }

        //assign into the real extern global
        *entry->target = ptr;

        loadedWinFunctions.push_back(WinGLFunction
            {
                entry->name,
                entry->target
            });

        Log::Print(
            "Loaded '" + string(name) + "'!",
            "OPENGL_WINDOWS",
            LogType::LOG_DEBUG);
    }
}

#endif //_WIN32