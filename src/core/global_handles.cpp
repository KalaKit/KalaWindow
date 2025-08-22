//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef _WIN32
#include <Windows.h>
#else
#include <dlfcn.h>
#endif

#include <string>

#include "KalaHeaders/core_types.hpp"

#include "core/core.hpp"
#include "core/global_handles.hpp"

using std::string;

namespace KalaWindow::Core
{
	void GlobalHandle::SetOpenGLHandle()
	{
#ifdef _WIN32
		HMODULE handle = GetModuleHandleA("opengl32.dll");
		if (!handle)
		{
			KalaWindowCore::ForceClose(
				"OpenGL error",
				"Failed to get handle for 'opengl32.dll'");

			return;
		}

		openGL32Lib = FromVar(handle);
#else
		void* handle = dlopen("libGL.so.1", RTLD_LAZY | RTLD_LOCAL);
		if (!handle)
		{
			string message = "Failed to get handle for 'libGL.so.1'! Reason: " + string(dlerror());

			KalaWindowCore::ForceClose(
				"OpenGL error",
				message);

			return;
		}

		openGL32 = FromVar(handle);
#endif
	}
}