//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#ifdef _WIN32

#include <cstdint>
#include <windows.h>

using std::uintptr_t;

#define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB 0x2092
#define WGL_CONTEXT_PROFILE_MASK_ARB 0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001

//Creates an OpenGL rendering context with specific attributes (version, profile)
KALAWINDOW_API extern HGLRC(WINAPI* wglCreateContextAttribsARB)(
	HDC hDC,
	HGLRC hShareContext,
	const int* attribList);

//Chooses a pixel format that matches specified attributes
KALAWINDOW_API extern BOOL(WINAPI* wglChoosePixelFormatARB)(
    HDC hDC,
    const int* attribIList,
    const FLOAT* attribFList,
    UINT maxFormats,
    int* formats,
    UINT* numFormats);

//Sets the swap interval for buffer swaps (vsync control)
KALAWINDOW_API extern BOOL(WINAPI* wglSwapIntervalEXT)(
    int interval);

#endif //_WIN32