//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef KALAWINDOW_SUPPORT_OPENGL

#define KALAKIT_MODULE "OPENGL"

#include "graphics/opengl/opengl.hpp"
#include "graphics/opengl/opengl_typedefs.hpp"

namespace KalaWindow::Graphics
{
	const char* Renderer_OpenGL::GetGLErrorString(unsigned int err)
	{
		GLenum glErr = static_cast<GLenum>(err);

		switch (glErr)
		{
		case GL_INVALID_ENUM: return "GL_INVALID_ENUM";
		case GL_INVALID_VALUE: return "GL_INVALID_VALUE";
		case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION";
		case GL_STACK_OVERFLOW: return "GL_STACK_OVERFLOW";
		case GL_STACK_UNDERFLOW: return "GL_STACK_UNDERFLOW";
		case GL_OUT_OF_MEMORY: return "GL_OUT_OF_MEMORY";
		case GL_INVALID_FRAMEBUFFER_OPERATION: return "GL_INVALID_FRAMEBUFFER_OPERATION";
		default: return "Unknown error";
		}
	}

	bool Renderer_OpenGL::IsContextValid(Window* targetWindow)
	{
		WindowStruct_Windows& win = targetWindow->GetWindow_Windows();
		HGLRC hglrc = reinterpret_cast<HGLRC>(win.hglrc);

		HGLRC current = wglGetCurrentContext();
		if (current == nullptr)
		{
			LOG_ERROR("Current OpenGL context is null!");
			return false;
		}

		if (current != hglrc)
		{
			LOG_ERROR("Current OpenGL context does not match stored context!");
			return false;
		}

		return true;
	}
}

#endif //KALAWINDOW_SUPPORT_OPENGL