//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

//
// PLATFORM PREPROCESSORS
//

#ifdef _WIN32
	#ifndef KALAKIT_WINDOWS
		#define KALAKIT_WINDOWS
	#endif
#elif __linux__
	#include <X11/Xlib.h>
	#include <wayland-client.h>
	#if defined(X11_FOUND)
		#ifndef KALAKIT_X11
			#define KALAKIT_X11
		#endif

	#elif defined(WAYLAND_FOUND)
		#ifndef KALAKIT_WAYLAND
			#define KALAKIT_WAYLAND
		#endif
	#else
		#error "Unknown UNIX version! Must be X11 or Wayland!"
	#endif
#else
	#error "Unsupported OS version! Must be WIN32 or UNIX!"
#endif

//
// LOG PREPROCESSORS
//

#ifndef WRITE_LOG
	#define WRITE_LOG(type, msg) std::cout << "[KALAKIT_" << KALAKIT_MODULE << " | " << type << "] " << msg << "\n"
#endif

//log types
#if KALAWINDOW_DEBUG
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