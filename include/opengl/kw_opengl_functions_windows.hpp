//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#ifdef _WIN32

#include "KalaHeaders/core_utils.hpp"

#include "OpenGL/wglext.h" //Windows-only OpenGL extension header

namespace KalaWindow::OpenGL::OpenGLFunctions
{
	//Creates an OpenGL rendering context with specific attributes (version, profile)
	LIB_API extern PFNWGLCREATECONTEXTATTRIBSARBPROC   wglCreateContextAttribsARB;

	//Chooses a pixel format that matches specified attributes
	LIB_API extern PFNWGLCHOOSEPIXELFORMATARBPROC      wglChoosePixelFormatARB;

	//Sets the swap interval for buffer swaps (vsync control)
	LIB_API extern PFNWGLSWAPINTERVALEXTPROC           wglSwapIntervalEXT;

	//Retrieves float pixel format attributes for a given pixel format
	LIB_API extern PFNWGLGETPIXELFORMATATTRIBFVARBPROC wglGetPixelFormatAttribfvARB;

	//Retrieves integer pixel format attributes for a given pixel format
	LIB_API extern PFNWGLGETPIXELFORMATATTRIBIVARBPROC wglGetPixelFormatAttribivARB;

	class LIB_API OpenGL_Functions_Windows
	{
	public:
		//Load all OpenGL Windows functions that are provided
		static void LoadAllWindowsFunctions();

		//Load a specific function, this won't be loaded again with LoadAllFunctions
		static void LoadWindowsFunction(const char* name);
	};
}

#endif //_WIN32