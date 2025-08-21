//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef _WIN32

#include <windows.h>
#include <vector>
#include <string>

#include "KalaHeaders/logging.hpp"
#include "KalaHeaders/core_types.hpp"

#include "graphics/opengl/opengl_functions_win.hpp"
#include "core/core.hpp"
#include "core/global_handles.hpp"

using KalaHeaders::Log;
using KalaHeaders::LogType;

using KalaWindow::Core::KalaWindowCore;
using KalaWindow::Core::GlobalHandle;
using namespace KalaWindow::Graphics::OpenGLFunctions;

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
    { "wglCreateContextAttribsARB", reinterpret_cast<void**>(&wglCreateContextAttribsARB) },
    { "wglChoosePixelFormatARB",    reinterpret_cast<void**>(&wglChoosePixelFormatARB) },
    { "wglSwapIntervalEXT",         reinterpret_cast<void**>(&wglSwapIntervalEXT) }
};

namespace KalaWindow::Graphics::OpenGLFunctions
{
	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = nullptr;
	PFNWGLCHOOSEPIXELFORMATARBPROC    wglChoosePixelFormatARB    = nullptr;
	PFNWGLSWAPINTERVALEXTPROC         wglSwapIntervalEXT         = nullptr;

	void OpenGL_Functions_Windows::LoadAllWinFunctions()
	{
        /*
        for (auto& check : checks)
        {
            if (!check.ptr)
            {
                KalaWindowCore::ForceClose(
                    "OpenGL Windows function error",
                    "Failed to load function '" + string(check.name) + "'");
            }
        }
        */
	}

    void OpenGL_Functions_Windows::LoadWinFunction(const char* name)
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
                "OPENGL WIN FUNCTION",
                LogType::LOG_ERROR);

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
                "OPENGL WIN FUNCTION",
                LogType::LOG_ERROR);

            return;
        }

        //try to load
        void* ptr = nullptr;

        ptr = reinterpret_cast<void*>(wglGetProcAddress(name));
        if (!ptr)
        {
            HMODULE module = ToVar<HMODULE>(GlobalHandle::GetOpenGLHandle());
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
    }
}

#endif //_WIN32