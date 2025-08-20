//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef __linux__

#include <string>

#include "KalaHeaders/core_types.hpp"

#include "graphics/opengl/opengl_functions_linux.hpp"
#include "core/core.hpp"
#include "core/global_handles.hpp"

using KalaWindow::Core::KalaWindowCore;
using KalaWindow::Core::GlobalHandle;
using namespace KalaWindow::Graphics::OpenGLFunctions;

using std::string;

struct LinuxFunctionCheck
{
    const char* name;
    const void* ptr;
};

LinuxFunctionCheck checks[] =
{
    //add functions here...
};

namespace KalaWindow::Graphics::OpenGL
{
	void OpenGL_Functions_Linux::LoadAllLinuxFunctions()
	{
        //add functions here...

        for (auto& check : checks)
        {
            if (!check.ptr)
            {
                KalaWindowCore::ForceClose(
                    "OpenGL Linux function error",
                    "Failed to load function '" + string(check.name) + "'");
            }
        }
	}

	void OpenGL_Functions_Linux::LoadLinuxFunction(void** target, const char* name)
	{
        //check if already loaded
        auto it = find_if(
            loadedLinuxFunctions.begin(),
            loadedLinuxFunctions.end(),
            [name](const LinuxGLFunction& rec) { return rec.name == name; });

        //already loaded - return existing one
        if (it != loadedLinuxFunctions.end())
        {
            *target = it->ptr;
            return;
        }

        //try to load
        *target = reinterpret_cast<T>(glXGetProcAddress(
            reinterpret_cast<const GLubyte*>(name)));
        if (!*target)
        {
            void* module = ToVar<void*>(GlobalHandle::GetOpenGLHandle());
            *target = reinterpret_cast<void*>(GetProcAddress(module, name));
        }

        if (!*target)
        {
            KalaWindowCore::ForceClose(
                "OpenGL Core function error",
                "Failed to load OpenGL error '" + string(name) + "'!");
        }

        loadedLinuxFunctions.push_back(
            {
                name,
                *target
            });
	}
}

#endif //#ifdef __linux__