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
//   - Safe conversions between uintptr_t and pointers, integrals, enums
//------------------------------------------------------------------------------

#pragma once

#include <cstdint>
#include <type_traits>

using std::is_pointer_v;
using std::is_integral_v;
using std::is_enum_v;
using std::underlying_type_t;
using std::uint64_t;

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
template<typename T> static constexpr uint64_t FromVar(T* h)
{
	return reinterpret_cast<uint64_t>(h);
}

//Converts an integral handle to an uintptr_t.
//Use cases:
//  - integers
//  - bitmask flags
//  - opaque handles
template<typename T> static uint64_t FromVar(T h)
	requires is_integral_v<T>
{
	return static_cast<uint64_t>(h);
}

//Converts an enum handle to an uintptr_t.
//Use cases:
//  - enums
//  - enum-based bitmask flags
//  - strongly typed API handles
template<typename T> static uint64_t FromVar(T h)
	requires is_enum_v<T>
{
	return static_cast<uint64_t>(static_cast<underlying_type_t<T>>(h));
}