//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <iostream>
#include <cstdint>

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
// TEMPLATE FOR ALL OS VARIABLES
//

template <typename Tag>
struct StrongHandle
{
	uintptr_t value;
	
	//default constructor
	StrongHandle() : value(0) {} 

	explicit StrongHandle(uintptr_t v) : value(v) {}

	bool operator==(const StrongHandle& other) const { return value == other.value; }
	bool operator!=(const StrongHandle& other) const { return value != other.value; }

	uintptr_t get() const { return value; }
};

//
// DECLARE PLATFORM VARIABLES
//

typedef void* OPENGLCONTEXT;
struct POS
{
	int x;
	int y;
};

//windows structs

struct Window_Win{};
using window_win = StrongHandle<Window_Win>;     // Window or drawable region on Windows (HWND)

struct Context_Win{};
using context_win = StrongHandle<Context_Win>;   // Drawing surface/context for a window on Windows (HDC)

//x11 structs

struct Window_X11{};
using window_x11 = StrongHandle<Window_X11>;     // Window or drawable region on X11 (Window)

struct Display_X11{};
using display_x11 = StrongHandle<Display_X11>;   // Session/context for communicating with the X server on X11 (Display)

struct DrawContext_X11{};
using target_x11 = StrongHandle<DrawContext_X11>; // Graphics context for drawing on X11 (GC)

//wayland structs

struct Surface_Way{};
using surface_way = StrongHandle<Surface_Way>;   // Window or drawable region on Wayland (wl_surface*)

struct Display_Way{};
using display_way = StrongHandle<Display_Way>;   // Session/context for communicating with the Wayland server on Wayland (wl_display*)

struct RenderTarget_Way{};
using target_way = StrongHandle<RenderTarget_Way>; // Render target buffer for drawing on Wayland (eglSurface)
