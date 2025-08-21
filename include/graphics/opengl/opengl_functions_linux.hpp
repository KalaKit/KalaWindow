//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#ifdef __linux__

#include "KalaHeaders/api.hpp"
#include "OpenGL/glxext.h" //Linux-only OpenGL extension header

namespace KalaWindow::Graphics::OpenGL
{
	//add functions here...

	class LIB_API OpenGL_Functions_Linux
	{
	public:
		//Load all OpenGL general functions that are provided
		static void LoadAllLinuxFunctions();

		//Load a specific function, this won't be loaded again with LoadAllFunctions
		static void LoadLinuxFunction(const char* name);
	};
}

#endif //__linux__