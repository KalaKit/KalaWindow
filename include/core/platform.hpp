//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <iostream>
#include <cstdint>

using std::is_pointer_v;
using std::cout;

//
// LOG PREPROCESSORS
//

#ifndef WRITE_LOG
#define WRITE_LOG(type, msg) cout << "[KALAKIT_" << KALAKIT_MODULE << " | " << type << "] " << msg << "\n"
#endif

//log types
#ifdef _DEBUG
#define LOG_DEBUG(msg) WRITE_LOG("DEBUG", msg)
#else
#define LOG_DEBUG(msg)
#endif
#define LOG_SUCCESS(msg) WRITE_LOG("SUCCESS", msg)
#define LOG_INFO(msg) WRITE_LOG("INFO", msg)
#define LOG_WARNING(msg) WRITE_LOG("WARNING", msg)
#define LOG_ERROR(msg) WRITE_LOG("ERROR", msg)

//
// DLL EXPORT/IMPORT MACRO
//

#ifdef _WIN32
#ifdef KALAWINDOW_DLL_EXPORT
#define KALAWINDOW_API __declspec(dllexport)
#else
#define KALAWINDOW_API __declspec(dllimport)
#endif
#elif __linux__
#define KALAWINDOW_API __attribute__((visibility("default")))
#else
#define KALAWINDOW_API
#endif

//
// DECLARE PLATFORM VARIABLES
//

typedef void* kwindow;

struct kvec2 { float x, y; };
struct kvec3 { float x, y, z; };
struct kvec4 { float x, y, z, w; };

struct kmat2 { kvec2 columns[2]; };
struct kmat3 { kvec3 columns[3]; };
struct kmat4 { kvec4 columns[4]; };

//
// CONVERT TO PLATFORM-AGNOSTIC VARIABLES AND BACK
//

//Convert an uintptr_t to a platform-specific or renderer-specific type.
//Usage example: ToVar<yourTargetType>(yourInput);
template<typename var> static constexpr var ToVar(uintptr_t h)
{
	if constexpr (is_pointer_v<var>)
	{
		return reinterpret_cast<var>(h);
	}
	else return static_cast<var>(h);
}
//Convert a platform-specific or renderer-specific type to an uintptr_t for use elsewhere.
//Usage example: FromVar<yourTargetType>(yourInput);
template<typename var> static constexpr uintptr_t FromVar(var h)
{
	if constexpr (is_pointer_v<var>)
	{
		return reinterpret_cast<uintptr_t>(h);
	}
	else return static_cast<uintptr_t>(h);
}