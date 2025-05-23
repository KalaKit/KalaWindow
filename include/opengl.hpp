﻿//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#ifdef _WIN32
	#ifdef KALAWINDOW_DLL_EXPORT
		#define KALAWINDOW_API __declspec(dllexport)
	#else
		#define KALAWINDOW_API __declspec(dllimport)
	#endif
#else
	#define KALAWINDOW_API
#endif

#include "opengl_typedefs.hpp"

namespace KalaKit
{
	class KALAWINDOW_API OpenGL
	{
	public:
		/// <summary>
		/// Creates a context of OpenGL 3.3 and attaches to the window.
		/// </summary>
		static bool Initialize();

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
		static inline HGLRC realContext;

		/// <summary>
		/// Checks whether user has OpenGL 3.3 or higher.
		/// </summary>
		/// <returns></returns>
		static bool IsCorrectVersion();
	};
}