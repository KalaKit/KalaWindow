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

#include <Windows.h>
#include <string>

namespace KalaKit
{
	using std::string;

	class KALAWINDOW_API MessageLoop
	{
	public:
		static LRESULT CALLBACK WindowProcCallback(
			HWND hwnd,
			UINT msg,
			WPARAM wParam,
			LPARAM lParam);
	private:
		/// <summary>
		/// Handle all update loop messages.
		/// </summary>
		static bool ProcessMessage(const MSG& msg);
	};
}