//---------------------------------------------------------------------------
// core_utils.hpp
//
// Copyright (C) 2026 Lost Empire Entertainment
//
// This is free source code, and you are welcome to redistribute it under certain conditions.
// Read LICENSE.md for more information.
//
// Provides:
//   - useful low level macros
//   - common container concepts
//   - helpers for getting enum or string from any enum to string or string to enum in any map or unordered map
//   - helpers for checking if raw array, array, vector, map or unordered map contains key or value
//   - helpers for removing duplicates from vector, map and unordered_map
//   - safe conversions between uintptr_t and pointers, integrals, enums
//---------------------------------------------------------------------------

#pragma once

//
// SKIP UNSUPPORTED C++ VERSION
//

#if __cplusplus < 202002L
	#error "UNSUPPORTED C++ VERSION! SUPPORTED: C++20 AND ABOVE"
#endif

//
// SKIP UNSUPPORTED PLATFORMS AND ARCHITECTURES
//

#if !defined(K_REDEFINE_GUARD_PLAT_ARCH)
	#define K_REDEFINE_GUARD_PLAT_ARCH

	#if defined(__APPLE__) || \
		defined(__FreeBSD__) || \
		defined(__OpenBSD__) || \
		defined(__NetBSD__) || \
		defined(__DragonFly__) || \
		defined(__CYGWIN__) || \
		defined(__ANDROID__)
		#error "UNSUPPORTED TARGET! SUPPORTED: _WIN32, __linux__"
	#elif !defined(_WIN32) && \
		!defined(__linux__)
		#error "UNSUPPORTED TARGET! SUPPORTED: _WIN32, __linux__"
	#elif !defined(_M_X64) && \
		!defined(__x86_64__)
		#error "UNSUPPORTED ARCHITECTURE! SUPPORTED: x64"
	#endif
#endif

//
// WINDOWS/LINUX MACROS
//

#if !defined(K_REDEFINE_GUARD_WIN_LIN)
	#define K_REDEFINE_GUARD_WIN_LIN

	#if defined(_WIN32)
		//any targeting windows
		#define KWIN_ANY

		//any msvc targeting windows
		#if defined(_MSC_VER)
			#define KWIN_MSVC
		//any posix targeting Windows
		#elif defined(__GNUC__)
			#define KWIN_GNU
		#else
			#error "UNKNOWN COMPILER DETECTED"
		#endif
	#endif

	#if defined(__linux__)
		//any targeting linux
		#define KLIN_ANY

		//any libc targeting linux
		#if defined(__GLIBC__)
			#define KLIN_GNU
		//any musl targeting linux
		#else
			#define KLIN_MUSL
		#endif
	#endif
#endif

//
// DEBUG MACRO
//

#if !defined(K_REDEFINE_GUARD_REL_DEB)
	#define K_REDEFINE_GUARD_REL_DEB

	#if !defined(KDEBUG)
		#if (defined(_MSC_VER) || \
			defined(__MINGW64__)) && \
			defined(_DEBUG)
			#define KDEBUG
		#elif defined(__linux__) && \
			!defined(NDEBUG)
			#define KDEBUG
		#endif
	#endif
#endif

//
// CAST SHORTHANDS
//

#if !defined(K_REDEFINE_GUARD_CASTS)
	#define K_REDEFINE_GUARD_CASTS

	#define rcast reinterpret_cast
	#define scast static_cast
	#define ccast const_cast
#endif

//
// COMPILER MACROS
//

#if !defined(KNORETURN)
	#define KNORETURN [[noreturn]]
#endif

#if !defined(KNODISCARD)
	#define KNODISCARD [[nodiscard]]
#endif

#include <cstdint>

//
// NUMERIC TYPE SHORTHANDS
//

#if !defined(KNUM)
	#define KNUM
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
#endif

//
// CROSS-PLATFORM IMPORT/EXPORT
//

#include <cstdint>

#ifdef _WIN32
	#if defined(LIB_STATIC) && defined(LIB_EXPORT)
		#error "Choose either LIB_STATIC or LIB_EXPORT, not both!"
	#endif

	#ifdef LIB_STATIC
		#define LIB_API
	#elif defined(LIB_EXPORT)
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

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <array>
#include <tuple>
#include <bit>
#include <type_traits>
#include <concepts>

namespace KalaHeaders::KalaCore
{
	using std::string;
	using std::string_view;
	using std::vector;
	using std::array;
	using std::unordered_map;
	using std::map;
	using std::unordered_set;
	using std::tuple_size;
	using std::false_type;
	using std::true_type;
	using std::assignable_from;
	using std::is_pointer_v;
	using std::is_integral_v;
	using std::is_array_v;
	using std::is_enum_v;
	using std::convertible_to;
	using std::equality_comparable;
	using std::equality_comparable_with;
	using std::constructible_from;
	using std::same_as;
	using std::remove_cv_t;
	using std::remove_pointer_t;
	using std::remove_cvref_t;
	using std::remove_reference_t;
	using std::remove_extent_t;
	using std::underlying_type_t;
	using std::hash;
	using std::bit_cast;

	//
	// CONCEPTS FOR COMMON CONTAINERS
	//

	template<typename T>
	concept AnyEnum = is_enum_v<T>;

	//String, string_view, char* or charArrayName[N]
	template<typename T>
	concept AnyString =
		same_as<remove_cvref_t<T>, string>
		|| same_as<remove_cvref_t<T>, string_view>
		|| (is_pointer_v<remove_cvref_t<T>>
			&& same_as
			<
				remove_cv_t<remove_pointer_t<remove_cvref_t<T>>>,
				char
			>)
		|| (is_array_v<remove_reference_t<T>>
			&& same_as
			<
				remove_cv_t<remove_extent_t<remove_reference_t<T>>>,
				char
			>);

	//Enables enums to be used as keys in unordered containers
	template<typename T>
		requires AnyEnum<T>
	struct EnumHash
	{
		constexpr size_t operator()(T e) const noexcept
		{
			using U = underlying_type_t<T>;
			return scast<size_t>(scast<U>(e));
		}
	};

	//Type X and Y can be compared with each other
	template<typename X, typename Y>
	concept IsComparable = equality_comparable_with<X, Y>;

	//Type X can be assigned as the value of type Y
	template<typename X, typename Y>
	concept IsAssignable = assignable_from<X, Y>;

	//Type X can be constructoed to type Y
	template<typename X, typename Y>
	concept IsConstrutible = constructible_from<X, Y>;

	//Type T supports equality comparison and hashing
	template<typename T>
	concept IsHashable =
		IsComparable<T, T>
		&& requires (T v)
	{
		{ hash<T>{}(v) } -> convertible_to<size_t>;
	};

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

	template<typename T>
	struct IsUnorderedMap : false_type {};

	template<typename K, typename V, typename H, typename E, typename A>
	struct IsUnorderedMap<unordered_map<K, V, H, E, A>> : true_type {};

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

	//Any map or unordered map that stores enums in K and string types in V
	template<typename M>
	concept AnyEnumAndStringMap =
		AnyMap<M>
		&& AnyEnum<typename M::key_type>
		&& AnyString<typename M::mapped_type>;

	//
	// CONVERT BETWEEN ENUM AND STRING
	//

	//Converts string type to known enum type,
	//assumes map or unordered map key is known enum type and value is string type,
	//returns error string on failure
	template<AnyString S, AnyEnumAndStringMap M>
	KNODISCARD
	inline constexpr string StringToEnum(
		S&& value,
		const M& map,
		typename M::key_type& outValue)
	{
		string_view sv{ value };

		for (const auto& [k, v] : map)
		{
			if (v == sv)
			{
				outValue = k;
				return "";
			}
		}

		return "StringToEnum failed because target was not found!";
	}

	//Converts known enum type to string_view,
	//assumes map or unordered map key known enum type and value is string type,
	//returns error string on failure
	template<AnyEnumAndStringMap M>
	KNODISCARD
	inline constexpr string EnumToString(
		typename M::key_type key,
		const M& map,
		string_view& outValue)
	{
		auto it = map.find(key);
		if (it == map.end())
		{
			return "EnumToString failed because key was not found!";
		}

		outValue = it->second;
		return "";
	}

	//
	// GET VALUE FROM MAP
	//

	//Get all keys by value from map or unordered_map,
	//if append is true then the output vector won't be cleared,
	//returns error string on failure
	template<AnyMap T, typename K, typename V>
		requires (
		IsComparable<typename T::mapped_type, V>
		&& IsAssignable<K&, typename T::key_type>)
	KNODISCARD
	inline string GetMapKeys(
		const T& map, 
		const V& value, 
		vector<K>& outValue,
		bool append = false)
	{
		if (!append) outValue.clear();

		bool foundValues{};
		for (const auto& [k, v] : map)
		{
			if (v == value)
			{
				outValue.push_back(k);
				foundValues = true;
			}
		}

		return foundValues
			? ""
			: "GetMapKeys failed because map was empty!";
	}

	//Get value by key from map or unordered_map,
	//returns error string on failure
	template<AnyMap T, typename K, typename V>
		requires (
			IsComparable<typename T::key_type, K>
			&& IsAssignable<V&, typename T::mapped_type>)
	KNODISCARD
	inline constexpr string GetMapValue(
		const T& map, 
		const K& key, 
		V& outValue)
	{
		auto it = map.find(key);
		if (it == map.end())
		{
			return "GetMapValue failed because map did not contain key!";
		}

		outValue = it->second;
		return "";
	}

	//
	// CHECK IF CONTAINER CONTAINS VALUE
	//

	//Returns true if raw array of type T contains the requested value 
	template<AnyRawArray A, typename T>
		requires IsComparable<AnyRawArrayElement<A>, T>
	KNODISCARD
	inline constexpr bool ContainsValue(
		const A& container, 
		const T& value)
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
	KNODISCARD
	inline constexpr bool ContainsValue(
		const A& container, 
		const T& value)
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
	KNODISCARD
	inline constexpr bool ContainsValue(
		const V& container, 
		const T& value)
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
	KNODISCARD
	inline constexpr bool ContainsKey(
		const M& container, 
		const T& key)
	{
		return container.contains(key);
	}

	//Returns true if map or unordered map with value of type T contains the requested value 
	template<AnyMap M, typename T>
		requires IsComparable<typename M::mapped_type, T>
	KNODISCARD
	inline constexpr bool ContainsValue(
		const M& container, 
		const T& value)
	{
		for (const auto& [k, v] : container)
		{
			if (v == value) return true;
		}
		return false;
	}

	//
	// REMOVE DUPLICATES FROM CONTAINER
	//

	//Returns true if the vector contains duplicate values
	template <AnyVector T>
		requires IsHashable<typename T::value_type>
	inline constexpr bool HasDuplicates(const T& v)
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

	//Remove all duplicate values the from vector that appear more than once, order is preserved
	template <AnyVector T>
		requires IsHashable<typename T::value_type>
	inline constexpr void RemoveDuplicates(T& v)
	{
		if (v.size() < 2) return;

		unordered_set<typename T::value_type> seen{};
		seen.reserve(v.size());

		erase_if(v, [&](const auto& x)
		{
			return !seen.insert(x).second;
		});
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
	KNODISCARD
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
	KNODISCARD
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
	KNODISCARD
	inline constexpr T ToVar(uintptr_t h)
		requires AnyEnum<T>
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
	KNODISCARD
	inline constexpr uintptr_t FromVar(T* h)
	{
		return rcast<uintptr_t>(h);
	}

	//Converts an integral handle to an uintptr_t.
	//Use cases:
	//  - integers
	//  - bitmask flags
	//  - opaque handles
	template<typename T>
	KNODISCARD
	inline constexpr uintptr_t FromVar(T h)
		requires is_integral_v<T>
	{
		return scast<uintptr_t>(h);
	}

	//Converts an enum handle to an uintptr_t.
	//Use cases:
	//  - enums
	//  - enum-based bitmask flags
	//  - strongly typed API handles
	template<typename T>
	KNODISCARD
	inline constexpr uintptr_t FromVar(T h)
		requires AnyEnum<T>
	{
		return scast<uintptr_t>(scast<underlying_type_t<T>>(h));
	}	
}
