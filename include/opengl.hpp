//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

//kalawindow
#include "platform.hpp"
#include "opengl_typedefs.hpp"

namespace KalaKit
{
	class KALAWINDOW_API OpenGL
	{
	public:
		static inline bool isInitialized;

		/// <summary>
		/// Creates a context of OpenGL 3.3 and attaches to the window.
		/// </summary>
		static bool Initialize(int width = 0, int height = 0);

		static bool IsContextValid();

		static const char* GetGLErrorString(GLenum err)
		{
			switch (err)
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

	private:
		/// <summary>
		/// Checks whether user has OpenGL 3.3 or higher.
		/// </summary>
		/// <returns></returns>
		static bool IsCorrectVersion();
	};
}