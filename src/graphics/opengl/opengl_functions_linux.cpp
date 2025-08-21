//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef __linux__

#include <vector>
#include <string>

#include "KalaHeaders/logging.hpp"
#include "KalaHeaders/core_types.hpp"

#include "graphics/opengl/opengl_functions_linux.hpp"
#include "core/core.hpp"
#include "core/global_handles.hpp"

using KalaHeaders::Log;
using KalaHeaders::LogType;

using KalaWindow::Core::KalaWindowCore;
using KalaWindow::Core::GlobalHandle;
using namespace KalaWindow::Graphics::OpenGLFunctions;

using std::vector;
using std::string;

struct LinuxGLFunction
{
    const char* name;
    void** target;
};

static inline vector<LinuxGLFunction> loadedLinuxFunctions{};

LinuxGLFunction functions[] =
{
    //add functions here...
};

namespace KalaWindow::Graphics::OpenGL
{
	void OpenGL_Functions_Linux::LoadAllLinuxFunctions()
	{
        //add functions here...

        /*
        for (auto& check : checks)
        {
            if (!check.ptr)
            {
                KalaWindowCore::ForceClose(
                    "OpenGL Linux function error",
                    "Failed to load function '" + string(check.name) + "'");
            }
        }
        */
	}

	void OpenGL_Functions_Linux::LoadLinuxFunction(const char* name)
	{
        //check if already loaded
        auto it = find_if(
            loadedLinuxFunctions.begin(),
            loadedLinuxFunctions.end(),
            [name](const LinuxGLFunction& rec) { return strcmp(rec.name, name) == 0; });

        //already loaded
        if (it != loadedLinuxFunctions.end())
        {
            Log::Print(
                "Function '" + string(name) + "' is already loaded!",
                "OPENGL LINUX FUNCTION",
                LogType::LOG_ERROR);

            return;
        }

        //find entry in registry
        LinuxGLFunction* entry = nullptr;
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
                "OPENGL LINUX FUNCTION",
                LogType::LOG_ERROR);

            return;
        }

        //try to load
        void* ptr = nullptr;

        ptr = reinterpret_cast<void*>(glXGetProcAddress(
            reinterpret_cast<const GLubyte*>(name)));
        if (!ptr)
        {
            void* module = ToVar<void*>(GlobalHandle::GetOpenGLHandle());
            ptr = reinterpret_cast<void*>(GetProcAddress(module, name));
        }

        if (!ptr)
        {
            KalaWindowCore::ForceClose(
                "OpenGL Linux function error",
                "Failed to load OpenGL error '" + string(name) + "'!");
        }

        //assign into the real extern global
        *entry->target = ptr;

        loadedLinuxFunctions.push_back(LinuxGLFunction
            {
                entry->name,
                entry->target
            });
	}
}

#endif //#ifdef __linux__