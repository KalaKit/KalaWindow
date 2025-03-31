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

#ifdef _WIN32
#include <Windows.h>
#endif

namespace KalaKit
{
#ifdef _WIN32
	using POS = POINT; // Position, cursor location or offset.
	using SIZE = RECT;   // Size or bounds of a region on screen.
	using WINDOW = HWND;  // The current window that is created and running.
#else
	struct POS // Position, cursor location or offset.
	{
		int x, y;
	};
	struct SIZE // Size or bounds of a region on screen.
	{
		int left;
		int top;
		int right;
		int bottom;
	};

	#if X11_ENABLED
		#include <X11/Xlib.h>
		struct WINDOW // The current window that is created and running.
		{
			::Display* display;
			::Window window;
		};
	#else
		struct wl_display;
		struct wl_surface;

		struct WINDOW // The current window that is created and running.
		{
			wl_display* display;
			wl_surface* window;
		};
	#endif
#endif

	class KALAUTILS_API OSUtils
	{
	public:
		static void DummyFunction();
	};
}