//------------------------------------------------------------------------------
// api.hpp
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
//------------------------------------------------------------------------------

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