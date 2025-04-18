//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#ifdef KALAKIT_WINDOWS
#include <Windows.h>
#endif

//kalawindow
#include "platform.hpp"

namespace KalaKit
{
	using std::string;

	class KALAWINDOW_API MessageLoop
	{
#ifdef KALAKIT_WINDOWS

		//
		// WINDOWS MESSAGE LOOP
		//

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

		static LRESULT CursorTest(
			HWND hwnd,
			UINT msg,
			WPARAM wParam,
			LPARAM lParam);

#elif KALAKIT_X11

		//
		// X11 MESSAGE LOOP
		//

#endif
	};
}