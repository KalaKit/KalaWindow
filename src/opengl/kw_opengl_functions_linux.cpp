//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef __linux__

#include <string>
#include <dlfcn.h>

#include "KalaHeaders/log_utils.hpp"
#include "KalaHeaders/core_utils.hpp"

#include "opengl/kw_opengl_functions_linux.hpp"
#include "opengl/kw_opengl.hpp"
#include "core/kw_core.hpp"

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;

using KalaWindow::Core::KalaWindowCore;
using namespace KalaWindow::OpenGL::OpenGLFunctions;
using KalaWindow::OpenGL::OpenGL_Global;

using std::string;

struct LinuxGLFunction
{
    const char* name;
    void** target;
};

namespace KalaWindow::OpenGL::OpenGLFunctions
{
    static GL_Linux glLinux{};

    const GL_Linux* OpenGL_Functions_Linux::GetGLLinux()
    {
        return &glLinux;
    }

    LinuxGLFunction functions[] =
    {
        //add functions here...
    };

	void OpenGL_Functions_Linux::LoadAllLinuxFunctions()
	{
        for (auto& entry : functions)
        {
            void* ptr = reinterpret_cast<void*>(glXGetProcAddress(
                reinterpret_cast<const GLubyte*>(entry.name)));

            if (!ptr)
            {
                void* module = ToVar<void*>(OpenGL_Global::GetOpenGLHandle());
                ptr = dlsym(module, entry.name);
            }

            if (!ptr)
            {
                KalaWindowCore::ForceClose(
                    "OpenGL Linux function error",
                    "Failed to load OpenGL function '" + string(entry.name) + "'!");
            }

            //assign into GL_Linux dispatch table
            *entry.target = ptr;

            Log::Print(
                "Loaded '" + string(entry.name) + "'!",
                "OPENGL_LINUX",
                LogType::LOG_DEBUG);
        }
	}
}

#endif //#ifdef __linux__