//------------------------------------------------------------------------------
// core_utils.hpp
//
// Copyright (C) 2025 Lost Empire Entertainment
//
// This is free source code, and you are welcome to redistribute it under certain conditions.
// Read LICENSE.md for more information.
//
// Provides:
//   - Cross-platform export/import macro (LIB_API)
//   - Win32 machine level function calling convenction (LIB_APIENTRY)
//   - Function inlining control (FORCE_INLINE, NO_INLINE)
//   - Deprecation marker (DEPRECATED)
//   - Debug-only assertion (DEBUG_ASSERT)
//   - Fixed-width integer shorthands (u8, i64, f32, etc.)
//   - Safe conversions between uintptr_t and pointers, integrals, enums
//------------------------------------------------------------------------------

#pragma once

#include <cstdint>
#include <type_traits>

#ifdef _WIN32
#include <basetsd.h>
#endif

using std::is_pointer_v;
using std::is_integral_v;
using std::is_enum_v;
using std::underlying_type_t;

//
// CROSS-PLATFORM IMPORT/EXPORT
//

#ifdef _WIN32
#ifdef LIB_EXPORT
#define LIB_API  __declspec(dllexport)
#else
#define LIB_API  __declspec(dllimport)
#endif
#elif __linux__
#define LIB_API  __attribute__((visibility("default")))
#else
#define LIB_API 
#endif

//
// WIN32 MACHINE LEVEL FUNCTION CALLING CONVENCTION
//

#ifdef _WIN32
#define LIB_APIENTRY __stdcall
#elif __linux__
#define LIB_APIENTRY
#endif

//
// FOR PERFORMANCE-CRITICAL CODE
//

#if defined(_MSC_VER)
#define FORCE_INLINE __forceinline
#define NO_INLINE    __declspec(noinline)
#elif defined(__GNUC__) || defined(__clang__)
#define FORCE_INLINE inline __attribute__((always_inline))
#define NO_INLINE    __attribute__((noinline))
#else
#define FORCE_INLINE inline
#define NO_INLINE
#endif

//
// MARK FEATURE AS DEPRECATED
//

#if defined(_MSC_VER)
#define DEPRECATED(msg) __declspec(deprecated(msg))
#elif defined(__GNUC__) || defined(__clang__)
#define DEPRECATED(msg) __attribute__((deprecated(msg)))
#else
#define DEPRECATED(msg)
#endif

//
// DEBUG-ONLY ASSERT WITHOUT ADDING RUNTIME COST IN RELEASE
//

#if defined(NDEBUG)
#define DEBUG_ASSERT(x) ((void)0)
#else
#include <cassert>
#define DEBUG_ASSERT(x) assert(x)
#endif

//
// DEFINE SHORTHANDS FOR SAFE MATH VARIABLES
//

//64-bit signed int (signed size_t)
//Min: -9 quintillion
//Max: 9 quintillion
#ifdef _WIN32
using sst = SSIZE_T;
#else
using sst = ssize_t;
#endif

//64-bit unsigned int
//Min: 0
//Max: 18 quintillion
using st = size_t;

//8-bit unsigned int
//Min: 0
//Max: 255
using u8 = uint8_t;

//16-bit unsigned int
//Min: 0
//Max: 65,535
using u16 = uint16_t;

//32-bit unsigned int
//Min: 0
//Max: 4,294,967,295
using u32 = uint32_t;

//64-bit unsigned int
//Replaces handles and pointers (uintptr_t)
//Min: 0
//Max: 18 quintillion
using u64 = uint64_t;

//8-bit int
//Min: -128
//Max: 127
using i8 = int8_t;

//16-bit int
//Min: -32,768
//Max: 32,767
using i16 = int16_t;

//32-bit int
//Min: -2,147,483,648
//Max: 2,147,483,647
using i32 = int32_t;

//64-bit int
//Min: -9 quintillion
//Max: 9 quintillion
using i64 = int64_t;

//32-bit float
//6 decimal precision
using f32 = float;

//64-bit float
//15 decimal precision
using f64 = double;

//
// CONVERT TO PLATFORM-AGNOSTIC VARIABLES AND BACK
//

//Converts an uintptr_t to a pointer.
//Requires <T> where T is the pointer you want to convert back to.
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
//Requires <T> where T is the integral handle you want to convert back to.
//Use cases:
//  - integers
//  - bitmask flags
//  - opaque handles
template<typename T> static constexpr T ToVar(uintptr_t h)
	requires is_integral_v<T>
{
	return static_cast<T>(h);
}

//Converts an uintptr_t to an enum handle
//Requires <T> where T is the enum type you want to convert back to.
//Use cases:
//  - enums
//  - enum-based bitmask flags
//  - strongly typed API handles
template<typename T> static constexpr T ToVar(uintptr_t h)
	requires is_enum_v<T>
{
	return static_cast<T>(static_cast<underlying_type_t<T>>(h));
}

//Converts a pointer to a uintptr_t.
//Use cases:
//  - structs
//  - classes
//  - functions
//  - arrays
template<typename T> static constexpr u64 FromVar(T* h)
{
	return reinterpret_cast<u64>(h);
}

//Converts an integral handle to an uintptr_t.
//Use cases:
//  - integers
//  - bitmask flags
//  - opaque handles
template<typename T> static u64 FromVar(T h)
	requires is_integral_v<T>
{
	return static_cast<u64>(h);
}

//Converts an enum handle to an uintptr_t.
//Use cases:
//  - enums
//  - enum-based bitmask flags
//  - strongly typed API handles
template<typename T> static u64 FromVar(T h)
	requires is_enum_v<T>
{
	return static_cast<u64>(static_cast<underlying_type_t<T>>(h));
}