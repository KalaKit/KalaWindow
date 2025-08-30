//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#ifdef _WIN32
	#ifdef IMGUI_EXPORT
		#define IMGUI_API   __declspec(dllexport)
	#else
		#define IMGUI_API   __declspec(dllimport)
#endif
#elif __linux__
	#define IMGUI_API   __attribute__((visibility("default")))
#else
	#define IMGUI_API  
#endif