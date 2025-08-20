//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#ifdef __linux__

#include <vector>

#include "KalaHeaders/api.hpp"
#include "OpenGL/glxext.h" //Linux-only OpenGL extension header

namespace KalaWindow::Graphics::OpenGL
{
	using std::vector;

	struct GLFunction
	{
		string name;
		void* ptr;
	};

	//add functions here...

	class LIB_API OpenGL_Functions_Linux
	{
	public:
		//Load all OpenGL general functions that are provided
		static void LoadAllFunctions();

		//Load a specific function, this won't be loaded again with LoadAllFunctions
		static void LoadFunction(void** target, const char* name);
	private:
		static inline vector<GLFunction> loadedFunctions{};
	};
}

#endif //__linux__