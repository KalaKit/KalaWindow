//------------------------------------------------------------------------------
// core_utils.hpp
//
// Copyright (C) 2026 Lost Empire Entertainment
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
//   - Shorthands for casters
//   - Helpers for removing duplicates from vector, map and unordered_map
//   - Safe conversions between uintptr_t and pointers, integrals, enums
//------------------------------------------------------------------------------

#pragma once

#include <cstdint>
#include <type_traits>
#include <bit>
#include <unordered_set>
#include <vector>
#include <unordered_map>
#include <map>
#include <concepts>
#include <functional>

using std::is_pointer_v;
using std::is_integral_v;
using std::is_enum_v;
using std::underlying_type_t;
using std::uint64_t;
using std::bit_cast;
using std::unordered_set;
using std::vector;
using std::unordered_map;
using std::map;
using std::equality_comparable;
using std::hash;
using std::convertible_to;

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
// SHORTHANDS FOR CASTERS
//

//reinterpret_cast
#ifndef rcast
	#define rcast reinterpret_cast
#endif

//static_cast
#ifndef scast
	#define scast static_cast
#endif

//dynamic_cast
#ifndef dcast
	#define dcast dynamic_cast
#endif

//const_cast
#ifndef ccast
	#define ccast const_cast
#endif

//bit_cast
template<typename T, typename U>
constexpr T bcast(const U& v) noexcept
{
	return bit_cast<T>(v);
}

namespace KalaHeaders::KalaCore
{
	//
	// REMOVE DUPLICATES FROM CONTAINER
	//

	template<typename T>
	concept Hashable =
		requires(T v)
		{
			{ hash<T>{}(v) } -> convertible_to<size_t>;
		};

	//Returns true if any value appears more than once in the vector
	template <typename T>
	requires equality_comparable<T> && Hashable<T>
	bool ContainsDuplicates(const vector<T>& v)
	{
		if (v.size() < 2) return false;

		unordered_set<T> seen{};
		seen.reserve(v.size());

		for (const auto& x : v)
		{
			if (!seen.insert(x).second) return true;
		}

		return false;
	}

	//Returns true if any value appears more than once in the map
	template <typename K, typename T>
	requires equality_comparable<T> && Hashable<T>
	bool ContainsDuplicates(const map<K, T>& m)
	{
		if (m.size() < 2) return false;

		unordered_set<T> seen{};
		seen.reserve(m.size());

		for (const auto& [key, value] : m)
		{
			if (!seen.insert(value).second) return true;
		}

		return false;
	}

	//Returns true if any value appears more than once in the unordered map
	template <typename K, typename T>
	requires equality_comparable<T> && Hashable<T>
	bool ContainsDuplicates(const unordered_map<K, T>& m)
	{
		if (m.size() < 2) return false;

		unordered_set<T> seen{};
		seen.reserve(m.size());

		for (const auto& [key, value] : m)
		{
			if (!seen.insert(value).second) return true;
		}

		return false;
	}

	//Remove all duplicates from vector that appear more than once, order is preserved
	template <typename T>
	requires equality_comparable<T> && Hashable<T>
	void RemoveDuplicates(vector<T>& v)
	{
		if (v.size() < 2) return;

		unordered_map<T, size_t> counts{};

		for (const auto& x : v) ++counts[x];

		vector<T> result{};
		result.reserve(v.size());

		for (const auto& x : v) if (counts[x] == 1) result.push_back(x);

		v = std::move(result);
	}

	//Remove all duplicates from map that appear more than once, key order is preserved
	template <typename K, typename T>
	requires equality_comparable<T> && Hashable<T>
	void RemoveDuplicates(map<K, T>& m)
	{
		if (m.size() < 2) return;

		unordered_map<T, size_t> counts{};
		counts.reserve(m.size());

		for (const auto& [key, value] : m) ++counts[value];

		for (auto it = m.begin(); it != m.end();)
		{
			if (counts[it->second] > 1) it = m.erase(it);
			else ++it;
		}
	}

	//Remove all duplicates from unordered map that appear more than once
	template <typename K, typename T>
	requires equality_comparable<T> && Hashable<T>
	void RemoveDuplicates(unordered_map<K, T>& m)
	{
		if (m.size() < 2) return;

		unordered_map<T, size_t> counts{};
		counts.reserve(m.size());

		for (const auto& [key, value] : m) ++counts[value];

		for (auto it = m.begin(); it != m.end();)
		{
			if (counts[it->second] > 1) it = m.erase(it);
			else ++it;
		}
	}

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
		return rcast<T>(h);
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
		return scast<T>(h);
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
		return scast<T>(scast<underlying_type_t<T>>(h));
	}

	//Converts a pointer to a uintptr_t.
	//Use cases:
	//  - structs
	//  - classes
	//  - functions
	//  - arrays
	template<typename T> static constexpr uint64_t FromVar(T* h)
	{
		return rcast<uint64_t>(h);
	}

	//Converts an integral handle to an uintptr_t.
	//Use cases:
	//  - integers
	//  - bitmask flags
	//  - opaque handles
	template<typename T> static uint64_t FromVar(T h)
		requires is_integral_v<T>
	{
		return scast<uint64_t>(h);
	}

	//Converts an enum handle to an uintptr_t.
	//Use cases:
	//  - enums
	//  - enum-based bitmask flags
	//  - strongly typed API handles
	template<typename T> static uint64_t FromVar(T h)
		requires is_enum_v<T>
	{
		return scast<uint64_t>(scast<underlying_type_t<T>>(h));
	}	
}