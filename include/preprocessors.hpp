//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#if defined(KALAKIT_WINDOWS)
    #pragma message(">>> KALAKIT_WINDOWS is defined")
#elif defined(KALAKIT_X11)
    #pragma message(">>> KALAKIT_X11 is defined")
#elif defined(KALAKIT_WAYLAND)
    #pragma message(">>> KALAKIT_WAYLAND is defined")
#else
    #error ">>> No KALAKIT platform macro defined!"
#endif

//
// LOG PREPROCESSORS
//

#ifndef WRITE_LOG
	#define WRITE_LOG(type, msg) std::cout << "[KALAKIT_" << KALAKIT_MODULE << " | " << type << "] " << msg << "\n"
#endif

//log types
#ifdef KALAWINDOW_DEBUG
	#define LOG_DEBUG(msg) WRITE_LOG("DEBUG", msg)
#else
	#define LOG_DEBUG(msg)
#endif
#define LOG_SUCCESS(msg) WRITE_LOG("SUCCESS", msg)
#define LOG_ERROR(msg) WRITE_LOG("ERROR", msg)

#include <iostream>

//
// DLL EXPORT/IMPORT MACRO
//

#ifdef KALAKIT_WINDOWS
	#ifdef KALAWINDOW_DLL_EXPORT
		#define KALAWINDOW_API __declspec(dllexport)
	#else
		#define KALAWINDOW_API __declspec(dllimport)
	#endif
#elif KALAKIT_X11 || KALAKIT_WAYLAND
	#define KALAWINDOW_API __attribute__((visibility("default")))
#else
	#define KALAWINDOW_API
#endif

//
// DEFINE COMMON VARIABLES
//

#if KALAKIT_WINDOWS
	#include <Windows.h>
	#define POINT POS
	struct WINDOW
	{
		HINSTANCE hInstance;
		HWND hWnd;
	};
#elif KALAKIT_X11
	#include <X11/Xlib.h>
	struct POS
	{
		int x;
		int y;
	};
	struct WINDOW
	{
		Display* display;
		::Window window;
	};
#elif KALAKIT_WAYLAND
	#include <wayland-client.h>
	struct POS
	{
		int x;
		int y;
	};
	struct WINDOW
	{
		struct wl_display* display;
		struct wl_surface* surface;
	};
#endif