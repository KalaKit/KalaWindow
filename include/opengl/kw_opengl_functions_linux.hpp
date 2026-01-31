//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#ifdef __linux__

#include "KalaHeaders/core_utils.hpp"

namespace KalaWindow::OpenGL::OpenGLFunctions
{
	struct GL_Linux
	{
		//add functions here...
	};

	class LIB_API OpenGL_Functions_Linux
	{
	public:
		static const GL_Linux* GetGLLinux();

		//Load all OpenGL general functions that are provided
		static void LoadAllLinuxFunctions();
	};
}

#endif //__linux__