//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#ifdef KALAWINDOW_SUPPORT_OPENGL
#ifdef _WIN32

#include <cstdint>
#include <windows.h>

using std::uintptr_t;

#define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB 0x2092
#define WGL_CONTEXT_PROFILE_MASK_ARB 0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001

//Creates an OpenGL rendering context with specific attributes (version, profile)
static inline HGLRC(WINAPI* wglCreateContextAttribsARB)(
	HDC hDC,
	HGLRC hShareContext,
	const int* attribList) = nullptr;

//Chooses a pixel format that matches specified attributes
static inline BOOL(WINAPI* wglChoosePixelFormatARB)(
    HDC hDC,
    const int* attribIList,
    const FLOAT* attribFList,
    UINT maxFormats,
    int* formats,
    UINT* numFormats) = nullptr;

//Sets the swap interval for buffer swaps (vsync control)
static inline BOOL(WINAPI* wglSwapIntervalEXT)(
    int interval) = nullptr;

#endif //_WIN32
#endif //KALAWINDOW_SUPPORT_OPENGL