//Copyright(C) 2025 Lost Empire Entertainment
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

namespace KalaKit
{
	class KALAWINDOW_API OpenGL
	{
	public:
		/// <summary>
		/// Creates a context of OpenGL 3.3 and attaches to the window.
		/// </summary>
		static bool Initialize();
	private:
		/// <summary>
		/// Checks whether user has OpenGL 3.3 or higher.
		/// </summary>
		/// <returns></returns>
		static bool IsCorrectVersion();
	};
}