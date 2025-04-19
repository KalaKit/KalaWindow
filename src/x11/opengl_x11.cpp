//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef KALAKIT_X11

#define KALAKIT_MODULE "OPENGL"

//kalawindow
#include "opengl.hpp"
#include <EGL/egl.h>

namespace KalaKit
{
	bool OpenGL::Initialize(int width, int height)
	{
		return false;
	}

	bool OpenGL::IsContextValid()
	{
		EGLContext current = eglGetCurrentContext();
		if (current == nullptr)
		{
			LOG_ERROR("Current OpenGL context is null!");
			return false;
		}

		return true;
	}
}

#endif // KALAKIT_X11