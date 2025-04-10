//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef KALAKIT_WAYLAND

#define KALAKIT_MODULE "OPENGL"

//kalawindow
#include "opengl.hpp"
#include <EGL/egl.h>

namespace KalaKit
{
	bool OpenGL::Initialize()
	{
		return false;
	}

    OPENGLCONTEXT OpenGL::GetOpenGLContext()
	{
    	return eglGetCurrentContext();
	}

	bool OpenGL::IsContextValid()
	{
		EGLContext current = eglGetCurrentContext();
		if (current == nullptr)
		{
			LOG_ERROR("Current OpenGL context is null!");
			return false;
		}

		if (current != realContext)
		{
			LOG_ERROR("Current OpenGL context does not match stored context!");
			return false;
		}

		return true;
	}
}

#endif // KALAKIT_WAYLAND