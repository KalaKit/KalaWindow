//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#ifdef _WIN32

#include "KalaHeaders/api.hpp"
#include "OpenGL/wglext.h" //Windows-only OpenGL extension header

namespace KalaWindow::Graphics::OpenGLFunctions
{
	//Creates an OpenGL rendering context with specific attributes (version, profile)
	extern PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;

	//Chooses a pixel format that matches specified attributes
	extern PFNWGLCHOOSEPIXELFORMATARBPROC    wglChoosePixelFormatARB;

	//Sets the swap interval for buffer swaps (vsync control)
	extern PFNWGLSWAPINTERVALEXTPROC         wglSwapIntervalEXT;

	class LIB_API OpenGL_Functions_Windows
	{
	public:
		//Load all OpenGL Windows functions that are provided
		static void LoadAllWinFunctions();

		//Load a specific function, this won't be loaded again with LoadAllFunctions
		static void LoadWinFunction(void** target, const char* name);
	};
}

#endif //_WIN32