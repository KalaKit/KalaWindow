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

#include <vector>

#include "enums.hpp"

namespace KalaKit
{
	using std::vector;

	class KALAWINDOW_API Graphics
	{
	public:
		/// <summary>
		/// Creates a context of OpenGL 3.3 and attaches to the window.
		/// </summary>
		static bool CreateOpenGLContext();
		/// <summary>
		/// Loads user-chosen OpenGL functions.
		/// </summary>
		static void LoadCoreOpenGLFunctions(const vector<OpenGLFunction>& functions);

		struct OpenGLFunctionEntry
		{
			OpenGLFunction id; //Function enum value
			const char* name;  //Actual OpenGL function name
			void** target;     //Pointer to where the loaded function should be stored

			OpenGLFunctionEntry(
				OpenGLFunction i,
				const char* n,
				void** t) :
				id(i),
				name(n),
				target(t) {}
		};
	private:
		template <typename T>
		static T LoadOpenGLFunction(const char* name);

		static const vector<OpenGLFunctionEntry> openGLFunctionTable;
	};
}