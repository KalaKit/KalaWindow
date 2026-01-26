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
//   - Common container concepts
//   - Helpers for checking if raw array, array, vector, map or unordered map contains key or value
//   - Helpers for removing duplicates from vector, map and unordered_map
//   - Safe conversions between uintptr_t and pointers, integrals, enums
//------------------------------------------------------------------------------

#pragma once

#include <string>
#include <unordered_set>
#include <vector>
#include <unordered_map>
#include <map>
#include <array>
#include <tuple>
#include <cstdint>
#include <bit>
#include <type_traits>
#include <concepts>

using std::string_view;
using std::unordered_set;
using std::vector;
using std::array;
using std::unordered_map;
using std::map;
using std::tuple_size;
using std::false_type;
using std::true_type;
using std::is_pointer_v;
using std::is_integral_v;
using std::is_array_v;
using std::is_enum_v;
using std::convertible_to;
using std::equality_comparable;
using std::equality_comparable_with;
using std::same_as;
using std::remove_cvref_t;
using std::remove_reference_t;
using std::remove_extent_t;
using std::underlying_type_t;
using std::hash;
using std::bit_cast;

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
inline constexpr T bcast(const U& v) noexcept
{
	return bit_cast<T>(v);
}

namespace KalaHeaders::KalaCore
{
	//
	// CONCEPTS FOR COMMON CONTAINERS
	//

	//Type X and Y can be compared with each other
	template<typename X, typename Y>
	concept IsComparable = equality_comparable_with<X, Y>;

	//Type T supports equality comparison and hashing
	template<typename T>
	concept IsHashable =
		IsComparable<T, T>
		&& requires (T v)
	{
		{ hash<T>{}(v) } -> convertible_to<size_t>;
	};

	template<typename T>
	struct IsUnorderedMap : false_type{};

	template<typename K, typename V, typename H, typename E, typename A>
	struct IsUnorderedMap<unordered_map<K, V, H, E, A>> : true_type{};

	//Raw array of type T and size N (T arrayName[N])
	template<typename T>
	concept AnyRawArray = is_array_v<remove_reference_t<T>>;

	//Element of type T of a raw array and size N (T arrayName[N])
	template<typename A>
	using AnyRawArrayElement = remove_extent_t<remove_reference_t<A>>;

	//Regular array of type T and size N (arrayName array<T, N>)
	template<typename T>
	concept AnyArray =
		requires
	{
		typename remove_cvref_t<T>::value_type;
		tuple_size<remove_cvref_t<T>>::value;
	}&& same_as
		<
			remove_cvref_t<T>,
			array
			<
				typename remove_cvref_t<T>::value_type,
				tuple_size<remove_cvref_t<T>>::value
			>
		>;

	//Vector of type T (vector<T>)
	template<typename T>
	concept AnyVector =
		same_as<remove_cvref_t<T>,
		vector<typename remove_cvref_t<T>::value_type>>;

	//Map or unordered map with key of type K and value of type V (map<K, V>/unordered_map<K, V>)
	template<typename T>
	concept AnyMap =
		requires(
			remove_cvref_t<T>& m, 
			typename remove_cvref_t<T>::key_type k)
	{
		typename remove_cvref_t<T>::key_type;
		typename remove_cvref_t<T>::mapped_type;

		{ m.find(k) };
		{ m.end() };

		{ m.begin()->second };
	};

	//
	// CHECK IF CONTAINER CONTAINS VALUE
	//

	//Returns true if raw array of type T contains the requested value 
	template<AnyRawArray A, typename T>
		requires IsComparable<AnyRawArrayElement<A>, T>
	bool ContainsValue(const A& container, const T& value)
	{
		using Element = AnyRawArrayElement<A>;

		for (const Element& e : container)
		{
			if (e == value) return true;
		}
		return false;
	}

	//Returns true if array of type T contains the requested value 
	template<AnyArray A, typename T>
		requires IsComparable<typename A::value_type, T>
	bool ContainsValue(const A& container, const T& value)
	{
		using Element = A::value_type;

		for (const Element& e : container)
		{
			if (e == value) return true;
		}
		return false;
	}

	//Returns true if vector of type T contains the requested value 
	template<AnyVector V, typename T>
		requires IsComparable<typename V::value_type, T>
	bool ContainsValue(const V& container, const T& value)
	{
		using Element = V::value_type;

		for (const Element& e : container)
		{
			if (e == value) return true;
		}
		return false;
	}

	//Returns true if map or unordered map with value of type T contains the requested key 
	template<AnyMap M, typename T>
		requires IsComparable<typename M::key_type, T>
	bool ContainsKey(const M& container, const T& key)
	{
		return container.contains(key);
	}

	//Returns true if map or unordered map with value of type T contains the requested value 
	template<AnyMap M, typename T>
		requires IsComparable<typename M::mapped_type, T>
	bool ContainsValue(const M& container, const T& value)
	{
		for (const auto& [k, v] : container)
		{
			if (v == value) return true;
		}
		return false;
	}

	//
	// CHECK IF CONTAINER HAS DUPLICATES
	//

	//Returns true if any value appears more than once in the vector
	template <AnyVector T>
		requires IsHashable<typename T::value_type>
	inline constexpr bool ContainsDuplicates(const T& v)
	{
		if (v.size() < 2) return false;

		unordered_set<typename T::value_type> seen{};
		seen.reserve(v.size());

		for (const auto& x : v)
		{
			if (!seen.insert(x).second) return true;
		}

		return false;
	}

	//Returns true if any value appears more than once in the map or unordered map
	template <AnyMap T>
		requires IsHashable<typename T::mapped_type>
	inline constexpr bool ContainsDuplicates(const T& m)
	{
		if (m.size() < 2) return false;

		unordered_set<typename T::mapped_type> seen{};
		seen.reserve(m.size());

		for (const auto& [key, value] : m)
		{
			if (!seen.insert(value).second) return true;
		}

		return false;
	}

	//
	// REMOVE DUPLICATES FROM CONTAINER
	//

	//Remove all duplicate values the from vector that appear more than once, order is preserved
	template <AnyVector T>
		requires IsHashable<typename T::value_type>
	inline constexpr void RemoveDuplicates(T& v)
	{
		if (v.size() < 2) return;

		unordered_map<typename T::value_type, typename T::size_type> counts{};
		counts.reserve(v.size());

		for (const auto& x : v) ++counts[x];

		vector<typename T::value_type> result{};
		result.reserve(v.size());

		for (const auto& x : v) if (counts[x] == 1) result.push_back(x);

		v = std::move(result);
	}

	//Remove all duplicate values from the map or unordered map that appear more than once, key order is preserved for maps
	template <AnyMap T>
		requires IsHashable<typename T::mapped_type>
	inline constexpr void RemoveDuplicates(T& m)
	{
		if (m.size() < 2) return;

		using Value = typename T::mapped_type;
		using Count = typename T::size_type;

		if constexpr (IsUnorderedMap<remove_cvref_t<T>>::value)
		{
			unordered_map<Value, Count> counts{};

			for (const auto& [k, v] : m) ++counts[v];

			for (auto it = m.begin(); it != m.end();)
			{
				if (counts[it->second] > 1) it = m.erase(it);
				else ++it;
			}
		}
		else
		{
			map<Value, Count> counts{};

			for (const auto& [k, v] : m) ++counts[v];

			for (auto it = m.begin(); it != m.end();)
			{
				if (counts[it->second] > 1) it = m.erase(it);
				else ++it;
			}
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
	template<typename T>
	inline constexpr T ToVar(uintptr_t h)
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
	template<typename T>
	inline constexpr T ToVar(uintptr_t h)
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
	template<typename T>
	inline constexpr T ToVar(uintptr_t h)
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
	template<typename T>
	inline constexpr uint64_t FromVar(T* h)
	{
		return rcast<uint64_t>(h);
	}

	//Converts an integral handle to an uintptr_t.
	//Use cases:
	//  - integers
	//  - bitmask flags
	//  - opaque handles
	template<typename T>
	inline constexpr uint64_t FromVar(T h)
		requires is_integral_v<T>
	{
		return scast<uint64_t>(h);
	}

	//Converts an enum handle to an uintptr_t.
	//Use cases:
	//  - enums
	//  - enum-based bitmask flags
	//  - strongly typed API handles
	template<typename T>
	inline constexpr uint64_t FromVar(T h)
		requires is_enum_v<T>
	{
		return scast<uint64_t>(scast<underlying_type_t<T>>(h));
	}	
}