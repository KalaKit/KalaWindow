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
	class LIB_API OpenGL_Functions_Linux
	{
	public:
		//Load all OpenGL Linux functions that are provided
		static void LoadFunctions();
	};
}

#endif //__linux__