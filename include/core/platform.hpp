//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <cstdint>
#include <type_traits>

#include "glm/glm.hpp"

using std::is_pointer_v;
using std::is_integral_v;
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat2;
using glm::mat3;
using glm::mat4;

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
// CONVERT TO PLATFORM-AGNOSTIC VARIABLES AND BACK
//

//Converts an uintptr_t to a pointer.
//Requires <T> where T is the variable type you want to convert back to.
//Use cases:
//  - structs
//  - classes
//  - functions
//  - arrays
template<typename T> static constexpr T ToVar(uintptr_t h)
requires is_pointer_v<T>
{
	return reinterpret_cast<T>(h);
}

//Converts an uintptr_t to an integral handle
//Requires <T> where T is the variable type you want to convert back to.
//Use cases:
//  - integers
//  - enums
//  - bitmask flags
//  - opaque handles
template<typename T> static constexpr T ToVar(uintptr_t h)
requires is_integral_v<T>
{
	return static_cast<T>(h);
}

//Converts a pointer to a uintptr_t.
//Use cases:
//  - structs
//  - classes
//  - functions
//  - arrays
template<typename T> static constexpr uintptr_t FromVar(T* h)
{
	return reinterpret_cast<uintptr_t>(h);
}

//Converts an integral handle to an uintptr_t.
//Use cases:
//  - integers
//  - enums
//  - bitmask flags
//  - opaque handles
template<typename T> static uintptr_t FromVar(T h)
requires is_integral_v<T>
{
	return static_cast<uintptr_t>(h);
}