//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#ifdef _WIN32
	#ifdef KALAUTILS_DLL_EXPORT
		#define KALAUTILS_API __declspec(dllexport)
	#else
		#define KALAUTILS_API __declspec(dllimport)
	#endif
#else
	#define KALAUTILS_API
#endif

namespace KalaKit
{

}