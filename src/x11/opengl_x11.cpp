//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef KALAKIT_X11

#define KALAKIT_MODULE "OPENGL"

//kalawindow
#include "opengl.hpp"

namespace KalaKit
{
    OPENGLCONTEXT OpenGL::GetOpenGLContext()
	{
    	return eglGetCurrentContext();
	}
}

#endif // KALAKIT_X11