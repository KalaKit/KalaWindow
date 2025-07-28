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
// DEFINE SHORTHANDS FOR SAFE MATH VARIABLES
//

using u8 = uint8_t;   //8-bit unsigned int (0 - 255)
using u16 = uint16_t; //16-bit unsigned int (0 - 65,535)
using u32 = uint32_t; //32-bit unsigned int (0 - 4,294,967,295)
using u64 = uint64_t; //64-bit unsigned int (0 - 18 quintillion)

using i8 = int8_t;    //8-bit int (-128 - 127)
using i16 = int16_t;  //16-bit int (-32,768 - 32,767)
using i32 = int32_t;  //32-bit int (-2,147,483,648 - 2,147,483,647)
using i64 = int64_t;  //64-bit int (-9 quintillion to 9 quintillion)

using f32 = float;    //32-bit float (~7 decimal precision)
using f64 = double;   //64-bit float (~15 decimal precision)

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