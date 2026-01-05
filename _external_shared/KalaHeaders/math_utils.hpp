//------------------------------------------------------------------------------
// math_utils.hpp
//
// Copyright (C) 2026 Lost Empire Entertainment
//
// This is free source code, and you are welcome to redistribute it under certain conditions.
// Read LICENSE.md for more information.
//
// Provides:
//   - shorthands for math variables
//   - GLM-like containers as vec2, vec3, vec4, mat2, mat3, mat4, quat
//   - operators and helpers for vec, mat and quat types
//   - mat containers as column-major and scalar form
//------------------------------------------------------------------------------

#pragma once

#include <cmath>
#include <cstdint>
#include <algorithm>

#ifdef _WIN32
#include <basetsd.h>
#endif

using std::sinf;
using std::cosf;
using std::tanf;
using std::sqrtf;
using std::fabsf;
using std::atan2f;
using std::clamp;
using std::min;
using std::max;
using std::fmodf;
using std::powf;
using std::floorf;

//============================================================================
//
// DEFINE SHORTHANDS FOR SAFE MATH VARIABLES
//
//============================================================================

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

//static_cast
#ifndef scast
	#define scast static_cast
#endif

namespace KalaHeaders::KalaMath
{	
	//6-digit precision PI
	constexpr f32 PI = 3.131593f;

	//32-bit precision
	constexpr f32 epsilon = 1e-6f;

	//============================================================================
	//
	// GENERAL HELPERS
	//
	//============================================================================

	//Used for arithmetic division and prevents division by 0, returns result instead of mutating origin
	constexpr f32 safediv_a(
		f32 origin,
		f32 divisor)
	{
		const f32 safeDivisor = (fabsf(divisor) > epsilon) ? divisor : 1.0f;
		return origin / safeDivisor;
	}
	//Used for compound division and prevents division by 0, mutates origin instead of returning result
	constexpr void safediv_c(
		f32& origin,
		f32 divisor)
	{
		const f32 safeDivisor = (fabsf(divisor) > epsilon) ? divisor : 1.0f;
		origin /= safeDivisor;
	}

	//============================================================================
	//
	// VEC
	//
	//============================================================================

	template<size_t N>
	struct vec_storage;

	template<>
	struct vec_storage<2> { f32 x{}, y{}; };
	template<>
	struct vec_storage<3> { f32 x{}, y{}, z{}; };
	template<>
	struct vec_storage<4> { f32 x{}, y{}, z{}, w{}; };

	template <size_t N>
	struct vec : public vec_storage<N>
	{
		static_assert(N >= 2 && N <= 4, "vec can only have 2, 3, or 4 components.");

		constexpr vec() = default;

		//vec2

		constexpr vec(f32 _x)
			requires (N == 2)
			: vec_storage<N>{ _x, _x } {}
		constexpr vec(f32 _x, f32 _y)
			requires (N == 2)
			: vec_storage<N>{ _x, _y } {}
			
		constexpr vec(const vec<2>& _v)
			requires (N == 2)
			: vec_storage<N>{ _v.x, _v.y } {}
		constexpr vec(const vec<3>& _v)
			requires (N == 2)
			: vec_storage<N>{ _v.x, _v.y } {}
		constexpr vec(const vec<4>& _v)
			requires (N == 2)
			: vec_storage<N>{ _v.x, _v.y } {}
			
		template <size_t M>
		constexpr vec(const f32 (&_f)[M])
			requires (N == 2 && M >= 2)
			: vec_storage<N>{ _f[0], _f[1] } {}

		//vec3

		constexpr vec(f32 _x)
			requires (N == 3)
			: vec_storage<N>{ _x, _x, _x } {}
		constexpr vec(f32 _x, f32 _y, f32 _z)
			requires (N == 3)
			: vec_storage<N>{ _x, _y, _z } {}

		constexpr vec(const vec<2>& _v, f32 _z)
			requires (N == 3)
			: vec_storage<N>{ _v.x, _v.y, _z } {}
		constexpr vec(f32 _x, const vec<2>& _v)
			requires (N == 3)
			: vec_storage<N>{ _x, _v.x, _v.y } {}

		constexpr vec(const vec<2>& _v)
			requires (N == 3)
			: vec_storage<N>{ _v.x, _v.y, 0.0f } {}
		constexpr vec(const vec<3>& _v)
			requires (N == 3)
			: vec_storage<N>{ _v.x, _v.y, _v.z } {}
		constexpr vec(const vec<4>& _v)
			requires (N == 3)
			: vec_storage<N>{ _v.x, _v.y, _v.z } {}
			
		constexpr vec(const f32 (&_f)[2])
			requires (N == 3)
			: vec_storage<N>{ _f[0], _f[1], 0.0f } {}
		template <size_t M>
		constexpr vec(const f32 (&_f)[M])
			requires (N == 3 && M >= 3)
			: vec_storage<N>{ _f[0], _f[1], _f[2] } {}

		//vec4

		constexpr vec(f32 _x)
			requires (N == 4)
			: vec_storage<N>{ _x, _x, _x, _x } {}
		constexpr vec(f32 _x, f32 _y, f32 _z, f32 _w)
			requires (N == 4)
			: vec_storage<N>{ _x, _y, _z, _w } {}

		constexpr vec(const vec<2>& _v, f32 _z, f32 _w)
			requires (N == 4)
			: vec_storage<N>{ _v.x, _v.y, _z, _w } {}
		constexpr vec(f32 _x, const vec<2>& _v, f32 _w)
			requires (N == 4)
			: vec_storage<N>{ _x, _v.x, _v.y, _w } {}
		constexpr vec(f32 _x, f32 _y, const vec<2>& _v)
			requires (N == 4)
			: vec_storage<N>{ _x, _y, _v.x, _v.y } {}
		constexpr vec(const vec<2>& _v1, const vec<2>& _v2)
			requires (N == 4)
			: vec_storage<N>{ _v1.x, _v1.y, _v2.x, _v2.y } {}

		constexpr vec(const vec<3>& _v, f32 _w)
			requires (N == 4)
			: vec_storage<N>{ _v.x, _v.y, _v.z, _w } {}
		constexpr vec(f32 _x, const vec<3>& _v)
			requires (N == 4)
			: vec_storage<N>{ _x, _v.x, _v.y, _v.z } {}

		constexpr vec(const vec<2>& _v)
			requires (N == 4)
			: vec_storage<N>{ _v.x, _v.y, 0.0f, 0.0f } {}
		constexpr vec(const vec<3>& _v)
			requires (N == 4)
			: vec_storage<N>{ _v.x, _v.y, _v.z, 0.0f } {}
		constexpr vec(const vec<4>& _v)
			requires (N == 4)
			: vec_storage<N>{ _v.x, _v.y, _v.z, _v.w } {}
			
		constexpr vec(const f32 (&_f)[2])
			requires (N == 4)
			: vec_storage<N>{ _f[0], _f[1], 0.0f, 0.0f } {}
		constexpr vec(const f32 (&_f)[3])
			requires (N == 4)
			: vec_storage<N>{ _f[0], _f[1], _f[2], 0.0f } {}
		template <size_t M>
		constexpr vec(const f32 (&_f)[M])
			requires (N == 4 && M >= 4)
			: vec_storage<N>{ _f[0], _f[1], _f[2], _f[3] } {}

		using storage = vec_storage<N>;

		//============================================================================
		//
		// VEC ARITHMETIC OPERATORS
		//
		//============================================================================

		constexpr vec operator+(const vec& v) const 
		{ 
			if constexpr (N == 2) return { this->x + v.x, this->y + v.y };
			if constexpr (N == 3) return { this->x + v.x, this->y + v.y, this->z + v.z };
			if constexpr (N == 4) return { this->x + v.x, this->y + v.y, this->z + v.z, this->w + v.w };
		}
		constexpr vec operator+(f32 s) const
		{
			if constexpr (N == 2) return { this->x + s, this->y + s };
			if constexpr (N == 3) return { this->x + s, this->y + s, this->z + s };
			if constexpr (N == 4) return { this->x + s, this->y + s, this->z + s, this->w + s };
		}

		constexpr vec operator-(const vec& v) const
		{ 
			if constexpr (N == 2) return { this->x - v.x, this->y - v.y };
			if constexpr (N == 3) return { this->x - v.x, this->y - v.y, this->z - v.z };
			if constexpr (N == 4) return { this->x - v.x, this->y - v.y, this->z - v.z, this->w - v.w };
		}
		constexpr vec operator-(f32 s) const
		{
			if constexpr (N == 2) return { this->x - s, this->y - s };
			if constexpr (N == 3) return { this->x - s, this->y - s, this->z - s };
			if constexpr (N == 4) return { this->x - s, this->y - s, this->z - s, this->w - s };
		}

		constexpr vec operator*(const vec& v) const
		{ 
			if constexpr (N == 2) return { this->x * v.x, this->y * v.y };
			if constexpr (N == 3) return { this->x * v.x, this->y * v.y, this->z * v.z };
			if constexpr (N == 4) return { this->x * v.x, this->y * v.y, this->z * v.z, this->w * v.w };
		}
		constexpr vec operator*(f32 s) const
		{
			if constexpr (N == 2) return { this->x * s, this->y * s };
			if constexpr (N == 3) return { this->x * s, this->y * s, this->z * s };
			if constexpr (N == 4) return { this->x * s, this->y * s, this->z * s, this->w * s };
		}

		constexpr vec operator/(const vec& v) const
		{
			if constexpr (N == 2) return { safediv_a(this->x, v.x), safediv_a(this->y, v.y) };
			if constexpr (N == 3) return { safediv_a(this->x, v.x), safediv_a(this->y, v.y), safediv_a(this->z, v.z) };
			if constexpr (N == 4) return { safediv_a(this->x, v.x), safediv_a(this->y, v.y), safediv_a(this->z, v.z), safediv_a(this->w, v.w) };
		}
		constexpr vec operator/(f32 s) const
		{ 
			if constexpr (N == 2) return { safediv_a(this->x, s), safediv_a(this->y, s) };
			if constexpr (N == 3) return { safediv_a(this->x, s), safediv_a(this->y, s), safediv_a(this->z, s) };
			if constexpr (N == 4) return { safediv_a(this->x, s), safediv_a(this->y, s), safediv_a(this->z, s), safediv_a(this->w, s) };
		}

		constexpr bool operator<(f32 s) const
		{
			if constexpr (N == 2) return { this->x < s && this->y < s };
			if constexpr (N == 3) return { this->x < s && this->y < s && this->z < s };
			if constexpr (N == 4) return { this->x < s && this->y < s && this->z < s && this->w < s };
		}
		constexpr bool operator<(const vec& s) const
		{
			if constexpr (N == 2) return { this->x > s && this->y > s };
			if constexpr (N == 3) return { this->x > s && this->y > s && this->z > s };
			if constexpr (N == 4) return { this->x > s && this->y > s && this->z > s && this->w > s };
		}
		
		constexpr bool operator>(f32 s) const
		{
			if constexpr (N == 2) return { this->x < s.x && this->y < s.y };
			if constexpr (N == 3) return { this->x < s.x && this->y < s.y && this->z < s.z };
			if constexpr (N == 4) return { this->x < s.x && this->y < s.y && this->z < s.z && this->w < s.w };
		}
		constexpr bool operator>(const vec& s) const
		{
			if constexpr (N == 2) return { this->x > s.x && this->y > s.y };
			if constexpr (N == 3) return { this->x > s.x && this->y > s.y && this->z > s.z };
			if constexpr (N == 4) return { this->x > s.x && this->y > s.y && this->z > s.z && this->w > s.w };
		}

		constexpr bool operator==(const vec& v) const
		{
			if constexpr (N == 2) return 
				fabsf(this->x - v.x) < epsilon
				&& fabsf(this->y - v.y) < epsilon;
			if constexpr (N == 3) return
				fabsf(this->x - v.x) < epsilon
				&& fabsf(this->y - v.y) < epsilon
				&& fabsf(this->z - v.z) < epsilon;
			if constexpr (N == 4) return
				fabsf(this->x - v.x) < epsilon
				&& fabsf(this->y - v.y) < epsilon
				&& fabsf(this->z - v.z) < epsilon
				&& fabsf(this->w - v.w) < epsilon;
		}
		constexpr bool operator!=(const vec& v) const { return !(*this == v); }

		constexpr vec operator-() const
		{
			if constexpr (N == 2) return { -this->x, -this->y };
			if constexpr (N == 3) return { -this->x, -this->y, -this->z };
			if constexpr (N == 4) return { -this->x, -this->y, -this->z, -this->w };
		}

		//============================================================================
		//
		// VEC COMPOUND OPERATORS
		//
		//============================================================================

		constexpr vec& operator+=(f32 s)
		{
			if constexpr (N == 2) { this->x += s; this->y += s; return *this; }
			if constexpr (N == 3) { this->x += s; this->y += s; this->z += s; return *this; }
			if constexpr (N == 4) { this->x += s; this->y += s; this->z += s; this->w += s; return *this; }
		}
		constexpr vec& operator+=(const vec& v)
		{ 
			if constexpr (N == 2) { this->x += v.x; this->y += v.y; return *this; }
			if constexpr (N == 3) { this->x += v.x; this->y += v.y; this->z += v.z; return *this; }
			if constexpr (N == 4) { this->x += v.x; this->y += v.y; this->z += v.z; this->w += v.w; return *this; }
		}

		constexpr vec& operator-=(f32 s)
		{
			if constexpr (N == 2) { this->x += s; this->y += s; return *this; }
			if constexpr (N == 3) { this->x += s; this->y += s; this->z += s; return *this; }
			if constexpr (N == 4) { this->x += s; this->y += s; this->z += s; this->w += s; return *this; }
		}
		constexpr vec& operator-=(const vec& v)
		{ 
			if constexpr (N == 2) { this->x -= v.x; this->y -= v.y; return *this; }
			if constexpr (N == 3) { this->x -= v.x; this->y -= v.y; this->z -= v.z; return *this; }
			if constexpr (N == 4) { this->x -= v.x; this->y -= v.y; this->z -= v.z; this->w -= v.w; return *this; }
		}

		constexpr vec& operator*=(f32 s)
		{
			if constexpr (N == 2) { this->x *= s; this->y *= s; return *this; }
			if constexpr (N == 3) { this->x *= s; this->y *= s; this->z *= s; return *this; }
			if constexpr (N == 4) { this->x *= s; this->y *= s; this->z *= s; this->w *= s; return *this; }
		}
		constexpr vec& operator*=(const vec& v)
		{
			if constexpr (N == 2) { this->x *= v.x; this->y *= v.y; return *this; }
			if constexpr (N == 3) { this->x *= v.x; this->y *= v.y; this->z *= v.z; return *this; }
			if constexpr (N == 4) { this->x *= v.x; this->y *= v.y; this->z *= v.z; this->w *= v.w; return *this; }
		}

		constexpr vec& operator/=(f32 s)
		{
			if constexpr (N == 2) { safediv_c(this->x, s); safediv_c(this->y, s); return *this; }
			if constexpr (N == 3) { safediv_c(this->x, s); safediv_c(this->y, s); safediv_c(this->z, s); return *this; }
			if constexpr (N == 4) { safediv_c(this->x, s); safediv_c(this->y, s); safediv_c(this->z, s); safediv_c(this->w, s); return *this; }
		}
		constexpr vec& operator/=(const vec& v)
		{ 
			if constexpr (N == 2) { safediv_c(this->x, v.x); safediv_c(this->y, v.y); return *this; }
			if constexpr (N == 3) { safediv_c(this->x, v.x); safediv_c(this->y, v.y); safediv_c(this->z, v.z); return *this; }
			if constexpr (N == 4) { safediv_c(this->x, v.x); safediv_c(this->y, v.y); safediv_c(this->z, v.z); safediv_c(this->w, v.w); return *this; }
		}
		
		constexpr bool operator<=(f32 s) const
		{
			if constexpr (N == 2) return { this->x <= s && this->y <= s };
			if constexpr (N == 3) return { this->x <= s && this->y <= s && this->z <= s };
			if constexpr (N == 4) return { this->x <= s && this->y <= s && this->z <= s && this->w <= s };
		}
		constexpr bool operator<=(const vec& s) const
		{
			if constexpr (N == 2) return { this->x <= s.x && this->y <= s.y };
			if constexpr (N == 3) return { this->x <= s.x && this->y <= s.y && this->z <= s.z };
			if constexpr (N == 4) return { this->x <= s.x && this->y <= s.y && this->z <= s.z && this->w <= s.w };
		}

		constexpr bool operator>=(f32 s) const
		{
			if constexpr (N == 2) return { this->x >= s && this->y >= s };
			if constexpr (N == 3) return { this->x >= s && this->y >= s && this->z >= s };
			if constexpr (N == 4) return { this->x >= s && this->y >= s && this->z >= s && this->w >= s };
		}
		constexpr bool operator>=(const vec& s) const
		{
			if constexpr (N == 2) return { this->x >= s.x && this->y >= s.y };
			if constexpr (N == 3) return { this->x >= s.x && this->y >= s.y && this->z >= s.z };
			if constexpr (N == 4) return { this->x >= s.x && this->y >= s.y && this->z >= s.z && this->w >= s.w };
		}
	};

	template<typename F, size_t N>
	constexpr vec<N> apply_scalar(const vec<N>& v, f32 s, F func)
	{
		if constexpr (N == 2) return { func(v.x, s), func(v.y, s) };
		if constexpr (N == 3) return { func(v.x, s), func(v.y, s), func(v.z, s) };
		if constexpr (N == 4) return { func(v.x, s), func(v.y, s), func(v.z, s), func(v.w, s) };
	}

	template<typename F, size_t N>
	constexpr vec<N> apply_scalar(const vec<N>& v1, const vec<N>& v2, F func)
	{
		if constexpr (N == 2) return { func(v1.x, v2.x), func(v1.y, v2.y) };
		if constexpr (N == 3) return { func(v1.x, v2.x), func(v1.y, v2.y), func(v1.z, v2.z) };
		if constexpr (N == 4) return { func(v1.x, v2.x), func(v1.y, v2.y), func(v1.z, v2.z), func(v1.w, v2.w) };
	}

	//add

	template<size_t N>
		requires (N >= 2 && N <= 4)
	constexpr vec<N> operator+(f32 s, const vec<N>& v)
	{
		return apply_scalar(v, s, [](f32 a, f32 b) { return a + b; });
	}
	template<size_t N1, size_t N2>
		requires (N1 > N2 && N1 <= 4 && N2 >= 2)
	constexpr vec<N1> operator+(const vec<N1>& a, const vec<N2>& b)
	{
		vec<N1> r = a;

		r.x += b.x;
		r.y += b.y;
		if constexpr (N2 >= 3 && N1 >= 3) r.z += b.z;
		if constexpr (N2 == 4 && N1 == 4) r.w += b.w;

		return r;
	}

	template<size_t N1, size_t N2>
		requires (N1 > N2 && N1 <= 4 && N2 >= 2)
	constexpr vec<N1>& operator+=(vec<N1>& a, const vec<N2>& b)
	{
		a.x += b.x;
		a.y += b.y;
		if constexpr (N2 >= 3 && N1 >= 3) a.z += b.z;
		if constexpr (N2 == 4 && N1 == 4) a.w += b.w;

		return a;
	}

	//subtract

	template<size_t N>
		requires (N >= 2 && N <= 4)
	constexpr vec<N> operator-(f32 s, const vec<N>& v)
	{
		return apply_scalar(v, s, [](f32 a, f32 b) { return a - b; });
	}
	template<size_t N1, size_t N2>
		requires (N1 > N2 && N1 <= 4 && N2 >= 2)
	constexpr vec<N1> operator-(const vec<N1>& a, const vec<N2>& b)
	{
		vec<N1> r = a;

		r.x -= b.x;
		r.y -= b.y;
		if constexpr (N2 >= 3 && N1 >= 3) r.z -= b.z;
		if constexpr (N2 == 4 && N1 == 4) r.w -= b.w;

		return r;
	}

	template<size_t N1, size_t N2>
		requires (N1 > N2 && N1 <= 4 && N2 >= 2)
	constexpr vec<N1>& operator-=(vec<N1>& a, const vec<N2>& b)
	{
		a.x -= b.x;
		a.y -= b.y;
		if constexpr (N2 >= 3 && N1 >= 3) a.z -= b.z;
		if constexpr (N2 == 4 && N1 == 4) a.w -= b.w;

		return a;
	}

	//multiply

	template<size_t N>
		requires (N >= 2 && N <= 4)
	constexpr vec<N> operator*(f32 s, const vec<N>& v)
	{
		return apply_scalar(v, s, [](f32 a, f32 b) { return a * b; });
	}
	template<size_t N1, size_t N2>
		requires (N1 > N2 && N1 <= 4 && N2 >= 2)
	constexpr vec<N1> operator*(const vec<N1>& a, const vec<N2>& b)
	{
		vec<N1> r = a;

		r.x *= b.x;
		r.y *= b.y;
		if constexpr (N2 >= 3 && N1 >= 3) r.z *= b.z;
		if constexpr (N2 == 4 && N1 == 4) r.w *= b.w;

		return r;
	}

	template<size_t N1, size_t N2>
		requires (N1 > N2 && N1 <= 4 && N2 >= 2)
	constexpr vec<N1>& operator*=(vec<N1>& a, const vec<N2>& b)
	{
		a.x *= b.x;
		a.y *= b.y;
		if constexpr (N2 >= 3 && N1 >= 3) a.z *= b.z;
		if constexpr (N2 == 4 && N1 == 4) a.w *= b.w;

		return a;
	}

	//divide

	template<size_t N>
		requires (N >= 2 && N <= 4)
	constexpr vec<N> operator/(f32 s, const vec<N>& v)
	{
		return apply_scalar(v, s, [](f32 a, f32 b) { return safediv_a(a, b); });
	}
	template<size_t N1, size_t N2>
		requires (N1 > N2 && N1 <= 4 && N2 >= 2)
	constexpr vec<N1> operator/(const vec<N1>& a, const vec<N2>& b)
	{
		vec<N1> r = a;

		r.x = safediv_a(r.x, b.x);
		r.y = safediv_a(r.y, b.y);
		if constexpr (N2 >= 3 && N1 >= 3) r.z = safediv_a(r.z, b.z);
		if constexpr (N2 == 4 && N1 == 4) r.w = safediv_a(r.w, b.w);

		return r;
	}

	template<size_t N1, size_t N2>
		requires (N1 > N2 && N1 <= 4 && N2 >= 2)
	constexpr vec<N1>& operator/=(vec<N1>& a, const vec<N2>& b)
	{
		safediv_c(a.x, b.x);
		safediv_c(a.y, b.y);
		if constexpr (N2 >= 3 && N1 >= 3) safediv_c(a.z, b.z);
		if constexpr (N2 == 4 && N1 == 4) safediv_c(a.w, b.w);

		return a;
	}
	
	//define vec2, vec3 and vec4
	
	using vec2 = vec<2>; //Vector: x, y
	using vec3 = vec<3>; //Vector: x, y, z
	using vec4 = vec<4>; //Vector: x, y, z, w

	//right-handed, +Y up

	//Global right direction is +X
	inline const vec3 DIR_RIGHT = { 1, 0,  0 };
	//Global up direction is +Y
	inline const vec3 DIR_UP    = { 0, 1,  0 };
	//Global front direction is -Z
	inline const vec3 DIR_FRONT = { 0, 0, -1 };

	//Tilt up/down (rotate around X)
	inline const vec3 ROT_PITCH = { 1, 0, 0 };
	//Turn left/right (rotate around Y)
	inline const vec3 ROT_YAW   = { 0, 1, 0 };
	//Bank left/right (rotate around Z)
	inline const vec3 ROT_ROLL  = { 0, 0, 1 };

	//============================================================================
	//
	// MAT
	//
	//============================================================================

	template<size_t N>
	struct mat_storage;

	template<>
	struct mat_storage<2>
	{
		f32 m00 = 1.0f, m10{};
		f32 m01{},      m11 = 1.0f;
	};
	template<>
	struct mat_storage<3>
	{
		f32 m00 = 1.0f, m10{},      m20{};
		f32 m01{},      m11 = 1.0f, m21{};
		f32 m02{},      m12{},      m22 = 1.0f;
	};
	template<>
	struct mat_storage<4>
	{
		f32 m00 = 1.0f, m10{},      m20{},      m30{};
		f32 m01{},      m11 = 1.0f, m21{},      m31{};
		f32 m02{},      m12{},      m22 = 1.0f, m32{};
		f32 m03{},      m13{},      m23{},      m33 = 1.0f;
	};

	template <size_t N>
	struct mat : public mat_storage<N>
	{
		static_assert(N >= 2 && N <= 4, "mat can only have 2, 3, or 4 components.");

		constexpr mat() = default;

		constexpr mat(
			f32 _m)
			requires (N == 2)
			: mat_storage<N>{ _m, _m, _m, _m } {}
		constexpr mat(
			f32 _m00, f32 _m10,
			f32 _m01, f32 _m11)
			requires (N == 2)
			: mat_storage<N>{ _m00, _m10, _m01, _m11 } {}

		constexpr mat(
			f32 _m)
			requires (N == 3)
			: mat_storage<N>
			{
				_m, _m, _m,
				_m, _m, _m,
				_m, _m, _m
			} {}
		constexpr mat(
			f32 _m00, f32 _m10, f32 _m20,
			f32 _m01, f32 _m11, f32 _m21,
			f32 _m02, f32 _m12, f32 _m22)
			requires (N == 3)
			: mat_storage<N>{
				_m00, _m10, _m20,
				_m01, _m11, _m21,
				_m02, _m12, _m22
			} {}


		constexpr mat(
			f32 _m)
			requires (N == 4)
			: mat_storage<N>
			{
				_m, _m, _m, _m,
				_m, _m, _m, _m,
				_m, _m, _m, _m,
				_m, _m, _m, _m
			} {}
		constexpr mat(
			f32 _m00, f32 _m10, f32 _m20, f32 _m30,
			f32 _m01, f32 _m11, f32 _m21, f32 _m31,
			f32 _m02, f32 _m12, f32 _m22, f32 _m32,
			f32 _m03, f32 _m13, f32 _m23, f32 _m33)
			requires (N == 4)
			: mat_storage<N>{
				_m00, _m10, _m20, _m30,
				_m01, _m11, _m21, _m31,
				_m02, _m12, _m22, _m32,
				_m03, _m13, _m23, _m33
			} {}

		using storage = mat_storage<N>;

		//============================================================================
		//
		// MAT ARITHMETIC OPERATORS
		//
		//============================================================================

		constexpr mat operator+(const mat& m) const
		{
			mat r = *this;
			r += m;

			return r;
		}
		constexpr mat operator+(f32 s) const
		{
			mat r = *this;
			r += s;

			return r;
		}

		constexpr mat operator-(const mat& m) const
		{
			mat r = *this;
			r -= m;

			return r;
		}
		constexpr mat operator-(f32 s) const
		{
			mat r = *this;
			r -= s;

			return r;
		}

		constexpr mat operator*(const mat& m) const
		{
			mat r = *this;
			r *= m;

			return r;
		}
		constexpr mat operator*(f32 s) const
		{
			mat r = *this;
			r *= s;

			return r;
		}

		constexpr mat operator/(const mat& m) const { return *this * inverse(m); }
		constexpr mat operator/(f32 s) const
		{ 
			mat r = *this;
			r /= s;

			return r;
		}

		constexpr mat operator<(f32 s) const
		{
			if constexpr (N == 2)
			{
				this->m00 < s; this->m10 < s;
				this->m01 < s; this->m11 < s;
			}
			if constexpr (N == 3)
			{
				this->m00 < s; this->m10 < s; this->m20 < s;
				this->m01 < s; this->m11 < s; this->m21 < s;
				this->m02 < s; this->m12 < s; this->m22 < s;
			}
			if constexpr (N == 4)
			{
				this->m00 < s; this->m10 < s; this->m20 < s; this->m30 < s;
				this->m01 < s; this->m11 < s; this->m21 < s; this->m31 < s;
				this->m02 < s; this->m12 < s; this->m22 < s; this->m32 < s;
				this->m03 < s; this->m13 < s; this->m23 < s; this->m33 < s;
			}
		}
		constexpr mat operator>(f32 s) const
		{
			if constexpr (N == 2)
			{
				this->m00 > s; this->m10 > s;
				this->m01 > s; this->m11 > s;
			}
			if constexpr (N == 3)
			{
				this->m00 > s; this->m10 > s; this->m20 > s;
				this->m01 > s; this->m11 > s; this->m21 > s;
				this->m02 > s; this->m12 > s; this->m22 > s;
			}
			if constexpr (N == 4)
			{
				this->m00 > s; this->m10 > s; this->m20 > s; this->m30 > s;
				this->m01 > s; this->m11 > s; this->m21 > s; this->m31 > s;
				this->m02 > s; this->m12 > s; this->m22 > s; this->m32 > s;
				this->m03 > s; this->m13 > s; this->m23 > s; this->m33 > s;
			}
		}
		constexpr mat operator<(const mat& m) const
		{
			if constexpr (N == 2)
			{
				this->m00 < m.m00; this->m10 < m.m10;
				this->m01 < m.m01; this->m11 < m.m11;
			}
			if constexpr (N == 3)
			{
				this->m00 < m.m00; this->m10 < m.m10; this->m10 < m.m20;
				this->m01 < m.m01; this->m11 < m.m11; this->m10 < m.m21;
				this->m01 < m.m02; this->m11 < m.m12; this->m10 < m.m22;
			}
			if constexpr (N == 4)
			{
				this->m00 < m.m00; this->m10 < m.m10; this->m10 < m.m20; this->m10 < m.m30;
				this->m01 < m.m01; this->m11 < m.m11; this->m10 < m.m21; this->m10 < m.m31;
				this->m01 < m.m02; this->m11 < m.m12; this->m10 < m.m22; this->m10 < m.m32;
				this->m01 < m.m03; this->m11 < m.m13; this->m10 < m.m23; this->m10 < m.m33;
			}
		}
		constexpr mat operator>(const mat& m) const
		{
			if constexpr (N == 2)
			{
				this->m00 > m.m00; this->m10 > m.m10;
				this->m01 > m.m01; this->m11 > m.m11;
			}
			if constexpr (N == 3)
			{
				this->m00 > m.m00; this->m10 > m.m10; this->m10 > m.m20;
				this->m01 > m.m01; this->m11 > m.m11; this->m10 > m.m21;
				this->m01 > m.m02; this->m11 > m.m12; this->m10 > m.m22;
			}
			if constexpr (N == 4)
			{
				this->m00 > m.m00; this->m10 > m.m10; this->m10 > m.m20; this->m10 > m.m30;
				this->m01 > m.m01; this->m11 > m.m11; this->m10 > m.m21; this->m10 > m.m31;
				this->m01 > m.m02; this->m11 > m.m12; this->m10 > m.m22; this->m10 > m.m32;
				this->m01 > m.m03; this->m11 > m.m13; this->m10 > m.m23; this->m10 > m.m33;
			}
		}

		constexpr mat operator-() const
		{ 
			mat r = *this; 
			r *= -1.0f; 

			return r; 
		}

		constexpr bool operator==(const mat& m) const
		{
			if constexpr (N == 2)
				return
				(fabsf(this->m00 - m.m00) < epsilon)
				&& (fabsf(this->m01 - m.m01) < epsilon)
				&& (fabsf(this->m10 - m.m10) < epsilon)
				&& (fabsf(this->m11 - m.m11) < epsilon);

			if constexpr (N == 3)
				return
				(fabsf(this->m00 - m.m00) < epsilon)
				&& (fabsf(this->m01 - m.m01) < epsilon)
				&& (fabsf(this->m02 - m.m02) < epsilon)
				&& (fabsf(this->m10 - m.m10) < epsilon)
				&& (fabsf(this->m11 - m.m11) < epsilon)
				&& (fabsf(this->m12 - m.m12) < epsilon)
				&& (fabsf(this->m20 - m.m20) < epsilon)
				&& (fabsf(this->m21 - m.m21) < epsilon)
				&& (fabsf(this->m22 - m.m22) < epsilon);

			if constexpr (N == 4)
				return
				(fabsf(this->m00 - m.m00) < epsilon)
				&& (fabsf(this->m01 - m.m01) < epsilon)
				&& (fabsf(this->m02 - m.m02) < epsilon)
				&& (fabsf(this->m03 - m.m03) < epsilon)
				&& (fabsf(this->m10 - m.m10) < epsilon)
				&& (fabsf(this->m11 - m.m11) < epsilon)
				&& (fabsf(this->m12 - m.m12) < epsilon)
				&& (fabsf(this->m13 - m.m13) < epsilon)
				&& (fabsf(this->m20 - m.m20) < epsilon)
				&& (fabsf(this->m21 - m.m21) < epsilon)
				&& (fabsf(this->m22 - m.m22) < epsilon)
				&& (fabsf(this->m23 - m.m23) < epsilon)
				&& (fabsf(this->m30 - m.m30) < epsilon)
				&& (fabsf(this->m31 - m.m31) < epsilon)
				&& (fabsf(this->m32 - m.m32) < epsilon)
				&& (fabsf(this->m33 - m.m33) < epsilon);
		}
		constexpr bool operator!=(const mat& m) const { return !(*this == m); }

		//============================================================================
		//
		// MAT COMPOUND OPERATORS
		//
		//============================================================================

		constexpr mat& operator+=(const mat& m)
		{
			if constexpr (N == 2)
			{
				this->m00 += m.m00; this->m10 += m.m10;
				this->m01 += m.m01; this->m11 += m.m11;
			}
			if constexpr (N == 3)
			{
				this->m00 += m.m00; this->m10 += m.m10; this->m20 += m.m20;
				this->m01 += m.m01; this->m11 += m.m11; this->m21 += m.m21;
				this->m02 += m.m02; this->m12 += m.m12; this->m22 += m.m22;
			}
			if constexpr (N == 4)
			{
				this->m00 += m.m00; this->m10 += m.m10; this->m20 += m.m20; this->m30 += m.m30;
				this->m01 += m.m01; this->m11 += m.m11; this->m21 += m.m21; this->m31 += m.m31;
				this->m02 += m.m02; this->m12 += m.m12; this->m22 += m.m22; this->m32 += m.m32;
				this->m03 += m.m03; this->m13 += m.m13; this->m23 += m.m23; this->m33 += m.m33;
			}

			return *this;
		}
		constexpr mat& operator+=(f32 s)
		{
			if constexpr (N == 2)
			{
				this->m00 += s; this->m10 += s;
				this->m01 += s; this->m11 += s;
			}
			if constexpr (N == 3)
			{
				this->m00 += s; this->m10 += s; this->m20 += s;
				this->m01 += s; this->m11 += s; this->m21 += s;
				this->m02 += s; this->m12 += s; this->m22 += s;
			}
			if constexpr (N == 4)
			{
				this->m00 += s; this->m10 += s; this->m20 += s; this->m30 += s;
				this->m01 += s; this->m11 += s; this->m21 += s; this->m31 += s;
				this->m02 += s; this->m12 += s; this->m22 += s; this->m32 += s;
				this->m03 += s; this->m13 += s; this->m23 += s; this->m33 += s;
			}

			return *this;
		}

		constexpr mat& operator-=(const mat& m)
		{
			if constexpr (N == 2)
			{
				this->m00 -= m.m00; this->m10 -= m.m10;
				this->m01 -= m.m01; this->m11 -= m.m11;
			}
			if constexpr (N == 3)
			{
				this->m00 -= m.m00; this->m10 -= m.m10; this->m20 -= m.m20;
				this->m01 -= m.m01; this->m11 -= m.m11; this->m21 -= m.m21;
				this->m02 -= m.m02; this->m12 -= m.m12; this->m22 -= m.m22;
			}
			if constexpr (N == 4)
			{
				this->m00 -= m.m00; this->m10 -= m.m10; this->m20 -= m.m20; this->m30 -= m.m30;
				this->m01 -= m.m01; this->m11 -= m.m11; this->m21 -= m.m21; this->m31 -= m.m31;
				this->m02 -= m.m02; this->m12 -= m.m12; this->m22 -= m.m22; this->m32 -= m.m32;
				this->m03 -= m.m03; this->m13 -= m.m13; this->m23 -= m.m23; this->m33 -= m.m33;
			}

			return *this;
		}
		constexpr mat& operator-=(f32 s)
		{
			if constexpr (N == 2)
			{
				this->m00 -= s; this->m10 -= s;
				this->m01 -= s; this->m11 -= s;
			}
			if constexpr (N == 3)
			{
				this->m00 -= s; this->m10 -= s; this->m20 -= s;
				this->m01 -= s; this->m11 -= s; this->m21 -= s;
				this->m02 -= s; this->m12 -= s; this->m22 -= s;
			}
			if constexpr (N == 4)
			{
				this->m00 -= s; this->m10 -= s; this->m20 -= s; this->m30 -= s;
				this->m01 -= s; this->m11 -= s; this->m21 -= s; this->m31 -= s;
				this->m02 -= s; this->m12 -= s; this->m22 -= s; this->m32 -= s;
				this->m03 -= s; this->m13 -= s; this->m23 -= s; this->m33 -= s;
			}

			return *this;
		}

		constexpr mat& operator*=(const mat& m)
		{
			if constexpr (N == 2)
			{
				const f32 a00 = this->m00, a10 = this->m10;
				const f32 a01 = this->m01, a11 = this->m11;

				this->m00 = a00 * m.m00 + a01 * m.m10;
				this->m10 = a10 * m.m00 + a11 * m.m10;

				this->m01 = a00 * m.m01 + a01 * m.m11;
				this->m11 = a10 * m.m01 + a11 * m.m11;
			}
			if constexpr (N == 3)
			{
				const f32 a00 = this->m00, a10 = this->m10, a20 = this->m20;
				const f32 a01 = this->m01, a11 = this->m11, a21 = this->m21;
				const f32 a02 = this->m02, a12 = this->m12, a22 = this->m22;

				this->m00 = a00 * m.m00 + a01 * m.m10 + a02 * m.m20;
				this->m10 = a10 * m.m00 + a11 * m.m10 + a12 * m.m20;
				this->m20 = a20 * m.m00 + a21 * m.m10 + a22 * m.m20;

				this->m01 = a00 * m.m01 + a01 * m.m11 + a02 * m.m21;
				this->m11 = a10 * m.m01 + a11 * m.m11 + a12 * m.m21;
				this->m21 = a20 * m.m01 + a21 * m.m11 + a22 * m.m21;

				this->m02 = a00 * m.m02 + a01 * m.m12 + a02 * m.m22;
				this->m12 = a10 * m.m02 + a11 * m.m12 + a12 * m.m22;
				this->m22 = a20 * m.m02 + a21 * m.m12 + a22 * m.m22;
			}
			if constexpr (N == 4)
			{
				const f32 a00 = this->m00, a10 = this->m10, a20 = this->m20, a30 = this->m30;
				const f32 a01 = this->m01, a11 = this->m11, a21 = this->m21, a31 = this->m31;
				const f32 a02 = this->m02, a12 = this->m12, a22 = this->m22, a32 = this->m32;
				const f32 a03 = this->m03, a13 = this->m13, a23 = this->m23, a33 = this->m33;

				this->m00 = a00 * m.m00 + a01 * m.m10 + a02 * m.m20 + a03 * m.m30;
				this->m10 = a10 * m.m00 + a11 * m.m10 + a12 * m.m20 + a13 * m.m30;
				this->m20 = a20 * m.m00 + a21 * m.m10 + a22 * m.m20 + a23 * m.m30;
				this->m30 = a30 * m.m00 + a31 * m.m10 + a32 * m.m20 + a33 * m.m30;

				this->m01 = a00 * m.m01 + a01 * m.m11 + a02 * m.m21 + a03 * m.m31;
				this->m11 = a10 * m.m01 + a11 * m.m11 + a12 * m.m21 + a13 * m.m31;
				this->m21 = a20 * m.m01 + a21 * m.m11 + a22 * m.m21 + a23 * m.m31;
				this->m31 = a30 * m.m01 + a31 * m.m11 + a32 * m.m21 + a33 * m.m31;

				this->m02 = a00 * m.m02 + a01 * m.m12 + a02 * m.m22 + a03 * m.m32;
				this->m12 = a10 * m.m02 + a11 * m.m12 + a12 * m.m22 + a13 * m.m32;
				this->m22 = a20 * m.m02 + a21 * m.m12 + a22 * m.m22 + a23 * m.m32;
				this->m32 = a30 * m.m02 + a31 * m.m12 + a32 * m.m22 + a33 * m.m32;

				this->m03 = a00 * m.m03 + a01 * m.m13 + a02 * m.m23 + a03 * m.m33;
				this->m13 = a10 * m.m03 + a11 * m.m13 + a12 * m.m23 + a13 * m.m33;
				this->m23 = a20 * m.m03 + a21 * m.m13 + a22 * m.m23 + a23 * m.m33;
				this->m33 = a30 * m.m03 + a31 * m.m13 + a32 * m.m23 + a33 * m.m33;
			}

			return *this;
		}
		constexpr mat& operator*=(f32 s)
		{
			if constexpr (N == 2)
			{
				this->m00 *= s; this->m10 *= s;
				this->m01 *= s; this->m11 *= s;
			}
			if constexpr (N == 3)
			{
				this->m00 *= s; this->m10 *= s; this->m20 *= s;
				this->m01 *= s; this->m11 *= s; this->m21 *= s;
				this->m02 *= s; this->m12 *= s; this->m22 *= s;
			}
			if constexpr (N == 4)
			{
				this->m00 *= s; this->m10 *= s; this->m20 *= s; this->m30 *= s;
				this->m01 *= s; this->m11 *= s; this->m21 *= s; this->m31 *= s;
				this->m02 *= s; this->m12 *= s; this->m22 *= s; this->m32 *= s;
				this->m03 *= s; this->m13 *= s; this->m23 *= s; this->m33 *= s;
			}

			return *this;
		}

		constexpr mat& operator/=(const mat& m)
		{ 
			*this = *this * inverse(m);
			return *this;
		}
		constexpr mat& operator/=(f32 s)
		{
			if constexpr (N == 2)
			{
				this->m00 = safediv_c(this->m00, s); this->m10 = safediv_c(this->m10, s);
				this->m01 = safediv_c(this->m01, s); this->m11 = safediv_c(this->m11, s);
			}
			if constexpr (N == 3)
			{
				this->m00 = safediv_c(this->m00, s); this->m10 = safediv_c(this->m10, s); this->m20 = safediv_c(this->m20, s);
				this->m01 = safediv_c(this->m01, s); this->m11 = safediv_c(this->m11, s); this->m21 = safediv_c(this->m21, s);
				this->m02 = safediv_c(this->m02, s); this->m12 = safediv_c(this->m12, s); this->m22 = safediv_c(this->m22, s);
			}
			if constexpr (N == 4)
			{
				this->m00 = safediv_c(this->m00, s); this->m10 = safediv_c(this->m10, s); this->m20 = safediv_c(this->m20, s); this->m30 = safediv_c(this->m30, s);
				this->m01 = safediv_c(this->m01, s); this->m11 = safediv_c(this->m11, s); this->m21 = safediv_c(this->m21, s); this->m31 = safediv_c(this->m31, s);
				this->m02 = safediv_c(this->m02, s); this->m12 = safediv_c(this->m12, s); this->m22 = safediv_c(this->m22, s); this->m32 = safediv_c(this->m32, s);
				this->m03 = safediv_c(this->m03, s); this->m13 = safediv_c(this->m13, s); this->m23 = safediv_c(this->m23, s); this->m33 = safediv_c(this->m33, s);
			}

			return *this;
		}

		constexpr mat operator<=(f32 s) const
		{
			if constexpr (N == 2)
			{
				this->m00 <= s; this->m10 <= s;
				this->m01 <= s; this->m11 <= s;
			}
			if constexpr (N == 3)
			{
				this->m00 <= s; this->m10 <= s; this->m20 <= s;
				this->m01 <= s; this->m11 <= s; this->m21 <= s;
				this->m02 <= s; this->m12 <= s; this->m22 <= s;
			}
			if constexpr (N == 4)
			{
				this->m00 <= s; this->m10 <= s; this->m20 <= s; this->m30 <= s;
				this->m01 <= s; this->m11 <= s; this->m21 <= s; this->m31 <= s;
				this->m02 <= s; this->m12 <= s; this->m22 <= s; this->m32 <= s;
				this->m03 <= s; this->m13 <= s; this->m23 <= s; this->m33 <= s;
			}
		}
		constexpr mat operator>=(f32 s) const
		{
			if constexpr (N == 2)
			{
				this->m00 >= s; this->m10 >= s;
				this->m01 >= s; this->m11 >= s;
			}
			if constexpr (N == 3)
			{
				this->m00 >= s; this->m10 >= s; this->m20 >= s;
				this->m01 >= s; this->m11 >= s; this->m21 >= s;
				this->m02 >= s; this->m12 >= s; this->m22 >= s;
			}
			if constexpr (N == 4)
			{
				this->m00 >= s; this->m10 >= s; this->m20 >= s; this->m30 >= s;
				this->m01 >= s; this->m11 >= s; this->m21 >= s; this->m31 >= s;
				this->m02 >= s; this->m12 >= s; this->m22 >= s; this->m32 >= s;
				this->m03 >= s; this->m13 >= s; this->m23 >= s; this->m33 >= s;
			}
		}
		constexpr mat operator<=(const mat& m) const
		{
			if constexpr (N == 2)
			{
				this->m00 <= m.m00; this->m10 <= m.m10;
				this->m01 <= m.m01; this->m11 <= m.m11;
			}
			if constexpr (N == 3)
			{
				this->m00 <= m.m00; this->m10 <= m.m10; this->m10 <= m.m20;
				this->m01 <= m.m01; this->m11 <= m.m11; this->m10 <= m.m21;
				this->m01 <= m.m02; this->m11 <= m.m12; this->m10 <= m.m22;
			}
			if constexpr (N == 4)
			{
				this->m00 <= m.m00; this->m10 <= m.m10; this->m10 <= m.m20; this->m10 <= m.m30;
				this->m01 <= m.m01; this->m11 <= m.m11; this->m10 <= m.m21; this->m10 <= m.m31;
				this->m01 <= m.m02; this->m11 <= m.m12; this->m10 <= m.m22; this->m10 <= m.m32;
				this->m01 <= m.m03; this->m11 <= m.m13; this->m10 <= m.m23; this->m10 <= m.m33;
			}
		}
		constexpr mat operator>=(const mat& m) const
		{
			if constexpr (N == 2)
			{
				this->m00 >= m.m00; this->m10 >= m.m10;
				this->m01 >= m.m01; this->m11 >= m.m11;
			}
			if constexpr (N == 3)
			{
				this->m00 >= m.m00; this->m10 >= m.m10; this->m10 >= m.m20;
				this->m01 >= m.m01; this->m11 >= m.m11; this->m10 >= m.m21;
				this->m01 >= m.m02; this->m11 >= m.m12; this->m10 >= m.m22;
			}
			if constexpr (N == 4)
			{
				this->m00 >= m.m00; this->m10 >= m.m10; this->m10 >= m.m20; this->m10 >= m.m30;
				this->m01 >= m.m01; this->m11 >= m.m11; this->m10 >= m.m21; this->m10 >= m.m31;
				this->m01 >= m.m02; this->m11 >= m.m12; this->m10 >= m.m22; this->m10 >= m.m32;
				this->m01 >= m.m03; this->m11 >= m.m13; this->m10 >= m.m23; this->m10 >= m.m33;
			}
		}
	};

	//multiply mat by same vec

	template<size_t N>
		requires(N >= 2 && N <= 4)
	constexpr vec<N> operator*(const mat<N>& m, const vec<N>& v)
	{
		if constexpr (N == 2)
			return 
		{
			m.m00 * v.x + m.m10 * v.y,
			m.m01 * v.x + m.m11 * v.y
		};
		if constexpr (N == 3)
			return 
		{
			m.m00 * v.x + m.m10 * v.y + m.m20 * v.z,
			m.m01 * v.x + m.m11 * v.y + m.m21 * v.z,
			m.m02 * v.x + m.m12 * v.y + m.m22 * v.z
		};
		if constexpr (N == 4)
			return 
		{
			m.m00 * v.x + m.m10 * v.y + m.m20 * v.z + m.m30 * v.w,
			m.m01 * v.x + m.m11 * v.y + m.m21 * v.z + m.m31 * v.w,
			m.m02 * v.x + m.m12 * v.y + m.m22 * v.z + m.m32 * v.w,
			m.m03 * v.x + m.m13 * v.y + m.m23 * v.z + m.m33 * v.w
		};
	}
	
	//define mat2, mat3 and mat4
	
	using mat2 = mat<2>; //Matrix: m00 - m11
	using mat3 = mat<3>; //Matrix: m00 - m22
	using mat4 = mat<4>; //Matrix: m00 - m33
	
	//============================================================================
	//
	// QUAT
	//
	//============================================================================

	struct quat
	{
		f32 w = 1.0f, x{}, y{}, z{};
		
		constexpr quat() = default;
		
		constexpr quat(f32 _w, f32 _x, f32 _y, f32 _z)
			: w(_w), x(_x), y(_y), z(_z) {}
			
		constexpr explicit quat(const f32 (&arr)[4])
			: w(arr[0]), x(arr[1]), y(arr[2]), z(arr[3]) {}
			
		//============================================================================
		//
		// QUAT ARITHMETIC OPERATORS
		//
		//============================================================================

		//unary negation
		constexpr quat operator-() const
		{
			return { -w, -x, -y, -z };
		}
		
		//hamilton multiplication
		constexpr quat operator*(const quat& q) const
		{ 
			return 
			{
				w * q.w - x * q.x - y * q.y - z * q.z,
				w * q.x + x * q.w + y * q.z - z * q.y,
				w * q.y - x * q.z + y * q.w + z * q.x,
				w * q.z + x * q.y - y * q.x + z * q.w
			};
		}
		
		//scalar multiplication
		constexpr quat operator*(f32 s) const
		{
			return { w * s, x * s, y * s, z * s };
		}
		
		//scalar division 
		constexpr quat operator/(f32 s) const
		{
			return { w / s, x / s, y / s, z / s };
		}

		constexpr bool operator==(const quat& q) const
		{
			auto kabs = [](f32 v) -> f32
				{
					return v < 0.0f ? -v : v;
				};
			
			return
				kabs(w - q.w) < epsilon
				&& kabs(x - q.x) < epsilon
				&& kabs(y - q.y) < epsilon
				&& kabs(z - q.z) < epsilon;
		}
		constexpr bool operator!=(const quat& q) const { return !(*this == q); }
		
		//============================================================================
		//
		// QUAT COMPOUND OPERATORS
		//
		//============================================================================
		
		constexpr quat& operator*=(const quat& q)
		{
			*this = (*this) * q;
			return *this;
		}
	};
	
	//rotate vector by quaternion
	constexpr vec3 operator*(const quat& q, const vec3& v)
	{
		auto cross = [](
			const vec3& a,
			const vec3& b) -> vec3
		{
			return vec3
			{
				a.y * b.z - a.z * b.y,
				a.z * b.x - a.x * b.z,
				a.x * b.y - a.y * b.x
			};
		};
		
		const vec3 qv(q.x, q.y, q.z);
		const vec3 t = 2.0f * cross(qv, v);
		
		return v + q.w * t + cross(qv, t);
	}

	//============================================================================
	//
	// HELPER FUNCTIONS USING VEC, MAT AND QUAT
	//
	//============================================================================
	
	//Returns the inverse (congjugated) rotation of a quaternion,
	//assuming the quat input is already normalized
	constexpr quat inverse(const quat& q)
	{
		return { q.w, -q.x, -q.y, -q.z };
	}
	
	//Computes vec2 magnitude (distance from a)
	inline f32 length(const vec2 v)
	{
		return sqrtf(
			v.x * v.x
			+ v.y * v.y);
	}
	//Computes vec3 magnitude (distance from a)
	inline f32 length(const vec3& v)
	{
		return sqrtf(
			v.x * v.x
			+ v.y * v.y
			+ v.z * v.z);
	}
	//Computes vec4 magnitude (distance from a)
	inline f32 length(const vec4& v)
	{
		return sqrtf(
			v.x * v.x
			+ v.y * v.y
			+ v.z * v.z
			+ v.w * v.w);
	}
	
	//Computes quat magnitude (distance from a)
	inline f32 length(const quat& q)
	{
		return sqrtf(
			q.w * q.w
			+ q.x * q.x
			+ q.y * q.y
			+ q.z * q.z);
	}
	
	//
	// it is recommended to use isnear *only for equality checks*,
	// this means == and != only, use fabsf/linear + epsilon for <, >, <= and >=
	//
	
	//Returns true if f32 a is close to f32 b within epsilon range
	inline bool isnear(const f32 a, const f32 b = {})
	{
		return fabsf(a - b) <= epsilon;
	}
	
	//Returns true if vec2 a is close to vec2 b within epsilon range
	inline bool isnear(const vec2 a, const vec2 b = {})
	{
		return length(a - b) <= epsilon;
	}
	
	//Returns true if vec3 a is close to vec3 b within epsilon range
	inline bool isnear(const vec3& a, const vec3& b = {})
	{
		return length(a - b) <= epsilon;
	}
	
	//Returns true if vec4 a is close to vec4 b within epsilon range
	inline bool isnear(const vec4& a, const vec4& b = {})
	{
		return length(a - b) <= epsilon;
	}
	
	//Returns true if quat a is close to quat b within epsilon range
	inline bool isnear_q(const quat& a, const quat& b = {})
	{
		return (isnear(a.w, b.w)
			&& isnear(a.x, b.x)
			&& isnear(a.y, b.y)
			&& isnear(a.z, b.z))
			|| (isnear(a.w, -b.w)
			&& isnear(a.x, -b.x)
			&& isnear(a.y, -b.y)
			&& isnear(a.z, -b.z));
	}
	
	//Returns true if mat2 a is close to mat2 b within epsilon range
	inline bool isnear(const mat2& a, const mat2& b = {})
	{
		return isnear(a.m00, b.m00)
			&& isnear(a.m01, b.m01)
			&& isnear(a.m10, b.m10)
			&& isnear(a.m11, b.m11);
	}
	
	//Returns true if mat3 a is close to mat3 b within epsilon range
	inline bool isnear(const mat3& a, const mat3& b = {})
	{
		return isnear(a.m00, b.m00)
			&& isnear(a.m01, b.m01)
			&& isnear(a.m02, b.m02)

			&& isnear(a.m10, b.m10)
			&& isnear(a.m11, b.m11)
			&& isnear(a.m12, b.m12)

			&& isnear(a.m20, b.m20)
			&& isnear(a.m21, b.m21)
			&& isnear(a.m22, b.m22);
	}
	
	//Returns true if mat4 a is close to mat4 b within epsilon range
	inline bool isnear(const mat4& a, const mat4& b = {})
	{
		return isnear(a.m00, b.m00)
			&& isnear(a.m01, b.m01)
			&& isnear(a.m02, b.m02)
			&& isnear(a.m03, b.m03)

			&& isnear(a.m10, b.m10)
			&& isnear(a.m11, b.m11)
			&& isnear(a.m12, b.m12)
			&& isnear(a.m13, b.m13)

			&& isnear(a.m20, b.m20)
			&& isnear(a.m21, b.m21)
			&& isnear(a.m22, b.m22)
			&& isnear(a.m23, b.m23)

			&& isnear(a.m30, b.m30)
			&& isnear(a.m31, b.m31)
			&& isnear(a.m32, b.m32)
			&& isnear(a.m33, b.m33);
	}
	
	//Measures alignment between two vec2s
	inline f32 dot(
		const vec2 a,
		const vec2 b)
	{
		return
			a.x * b.x
			+ a.y * b.y;
	}
	//Measures alignment between two vec3s
	inline f32 dot(
		const vec3& a,
		const vec3& b)
	{
		return
			a.x * b.x
			+ a.y * b.y
			+ a.z * b.z;
	}
	//Measures alignment between two vec4s
	inline f32 dot(
		const vec4& a,
		const vec4& b)
	{
		return
			a.x * b.x
			+ a.y * b.y
			+ a.z * b.z
			+ a.w * b.w;
	}
	
	//Measures alignment between two quats
	inline f32 dot(
		const quat& a,
		const quat& b)
	{
		return
			a.w * b.w
			+ a.x * b.x
			+ a.y * b.y
			+ a.z * b.z;
	}
	
	//Returns true if float is range-normalized
	inline bool isnormalized_r(f32 v)
	{
		return v >= -epsilon && v <= 1.0f + epsilon;
	}
	//Returns true if vec2 is range-normalized
	inline bool isnormalized_r(vec2 v)
	{
		return isnormalized_r(v.x)
			&& isnormalized_r(v.y);
	}
	//Returns true if vec3 is range-normalized
	inline bool isnormalized_r(const vec3& v)
	{
		return isnormalized_r(v.x)
			&& isnormalized_r(v.y)
			&& isnormalized_r(v.z);
	}
	//Returns true if vec4 is range-normalized
	inline bool isnormalized_r(const vec4& v)
	{
		return isnormalized_r(v.x)
			&& isnormalized_r(v.y)
			&& isnormalized_r(v.z)
			&& isnormalized_r(v.w);
	}
	
	//Returns range-normalized float
	inline f32 normalize_r(f32 v) 
	{ 
		return 
			isnormalized_r(v)
			? v
			: clamp(v, 0.0f, 1.0f);
	}
	//Returns range-normalized vec2
	inline vec2 normalize_r(vec2 v) 
	{ 
		return vec2(
			normalize_r(v.x),
			normalize_r(v.y)); 
	}
	//Returns range-normalized vec3
	inline vec3 normalize_r(const vec3& v)
	{ 
		return vec3(
			normalize_r(v.x),
			normalize_r(v.y),
			normalize_r(v.z)); 
	}
	//Returns range-normalized vec4
	inline vec4 normalize_r(const vec4& v)
	{ 
		return vec4(
			normalize_r(v.x),
			normalize_r(v.y),
			normalize_r(v.z),
			normalize_r(v.w)); 
	}
	
	//Returns true if vec2 is unit-length normalized
	inline bool isnormalized(vec2 v)
	{
		f32 len2 = dot(v, v);
		return fabsf(len2 - 1.0f) <= epsilon;
	}
	//Returns true if vec3 is unit-length normalized
	inline bool isnormalized(const vec3& v)
	{
		f32 len2 = dot(v, v);
		return fabsf(len2 - 1.0f) <= epsilon;
	}
	//Returns true if vec4 is unit-length normalized
	inline bool isnormalized(const vec4& v)
	{
		f32 len2 = dot(v, v);
		return fabsf(len2 - 1.0f) <= epsilon;
	}
	
	//Returns true if quat is unit-length normalized
	inline bool isnormalized(const quat& q)
	{
		f32 len2 = dot(q, q);
		return fabsf(len2 - 1.0f) <= epsilon;
	}

	//Returns unit-length normalized vec2
	inline vec2 normalize(const vec2 v)
	{
		//skip normalize if already normalized
		if (isnormalized(v)) return v;
		
		f32 len = length(v);
		return (isnear(len)) 
			? vec2{}
			: v / len;
	}
	//Returns unit-length normalized vec3
	inline vec3 normalize(const vec3& v)
	{
		//skip normalize if already normalized
		if (isnormalized(v)) return v;
		
		f32 len = length(v);
		return (isnear(len)) 
			? vec3{}
			: v / len;
	}
	//Returns unit-length normalized vec4
	inline vec4 normalize(const vec4& v)
	{
		//skip normalize if already normalized
		if (isnormalized(v)) return v;
		
		f32 len = length(v);
		return (isnear(len)) 
			? vec4{}
			: v / len;
	}
	
	//Returns unit-length normalized quat
	inline quat normalize_q(const quat& q)
	{
		//skip normalize if already normalized
		if (isnormalized(q)) return q;
		
		f32 len = length(q);
		return (isnear(len)) 
			? quat{}
			: q / len;
	}
	
	//Wraps a rotation axis between 0 to 360 degrees
	inline f32 wrap(f32 deg)
	{
		deg = fmodf(deg, 360.0f);
		if (deg < 0.0f) deg += 360.0f;

		return deg;
	}

	//Convert degrees to radians
	inline f32 radians(f32 deg) { return deg * 0.017453f; }
	//Convert degrees to radians
	inline vec2 radians(const vec2 v)
	{
		return { radians(v.x), radians(v.y) };
	}
	//Convert degrees to radians
	inline vec3 radians(const vec3& v)
	{
		return { radians(v.x), radians(v.y), radians(v.z) };
	}
	//Convert degrees to radians
	inline vec4 radians(const vec4& v)
	{
		return { radians(v.x), radians(v.y), radians(v.z), radians(v.w) };
	}

	//Convert radians to degrees
	inline f32 degrees(f32 rad) { return rad * 57.295780f; }
	//Convert radians to degrees
	inline vec2 degrees(const vec2 v)
	{
		return { degrees(v.x), degrees(v.y) };
	}
	//Convert radians to degrees
	inline vec3 degrees(const vec3& v)
	{
		return { degrees(v.x), degrees(v.y), degrees(v.z) };
	}
	//Convert radians to degrees
	inline vec4 degrees(const vec4& v)
	{
		return { degrees(v.x), degrees(v.y), degrees(v.z), degrees(v.w) };
	}

	//Converts quat to 3D euler (degrees),
	//returns rotations as pitch-yaw-roll (XYZ), uses YXZ internally,
	//pitch, yaw and roll are clamped -360 to 360,
	//you are supposed to wrap or clamp pitch and roll as you prefer after this function
	inline vec3 toeuler3(const quat& q)
	{
		quat nq = normalize_q(q);
	
		//returns vec3 as identity if quat input is near identity
		if (isnear(q.w, 1.0f)
			&& isnear(q.x)
			&& isnear(q.y)
			&& isnear(q.z))
		{
			return {};
		}
	
		//get yaw
		
		//TODO: figure out a better solution so atan2f doesn't return -180 to 180
		
		f32 siny = 2.0f * (nq.w * nq.y + nq.x * nq.z);
		f32 cosy = 1.0f - 2.0f * (nq.y * nq.y + nq.x * nq.x);
		
		f32 yaw = atan2f(siny, cosy);
		
		//get pitch
		
		f32 sinp = 2.0f * (nq.w * nq.x - nq.y * nq.z);
		
		f32 pitch = (fabsf(sinp) >= 1.0f - epsilon)
			? copysign(PI / 2.0f, sinp)
			: asin(sinp);
		
		//get roll
		
		f32 sinr = 2.0f * (nq.w * nq.z + nq.x * nq.y);
		f32 cosr = 1.0f - 2.0f * (nq.z * nq.z + nq.x * nq.x);
		
		f32 roll = atan2f(sinr, cosr);
		
		//combine all together
		
		vec3 e = degrees(vec3{ pitch, yaw, roll });
		
		//clamp pitch -360 to 360
		e.x = clamp(e.x, -360.0f, 360.0f);
		//clamp yaw -360 to 360
		e.y = clamp(e.y, -360.0f, 360.0f);
		//clamp roll -360 to 360
		e.z = clamp(e.z, -360.0f, 360.0f);
		
		if (isnear(e.x)) e.x = 0.0f;
		if (isnear(e.y)) e.y = 0.0f;
		if (isnear(e.z)) e.z = 0.0f;

		return e;
	}
	
	//Converts 3D euler (degrees) to quaternion,
	//takes in rotations as pitch-yaw-roll (XYZ), uses YXZ internally,
	//pitch, yaw and roll are clamped -360 to 360,
	//you are supposed to wrap or clamp pitch and roll as you prefer before this function
	inline quat toquat(const vec3& euler)
	{
		//returns quat as identity if euler input is near 0
		if (isnear(euler)) return {};
		
		vec3 e = euler;
		
		//clamp pitch -360 to 360
		e.x = clamp(e.x, -360.0f, 360.0f);
		//clamp yaw -360 to 360
		e.y = clamp(e.y, -360.0f, 360.0f);
		//clamp roll -360 to 360
		e.z = clamp(e.z, -360.0f, 360.0f);
		
		if (isnear(e.x))  e.x = 0.0f;
		if (isnear(e.y))  e.y = 0.0f;
		if (isnear(e.z))  e.z = 0.0f;
		
		vec3 r = radians(e) * 0.5f;

		f32 cx = cosf(r.x), sx = sinf(r.x); //pitch
		f32 cy = cosf(r.y), sy = sinf(r.y); //yaw
		f32 cz = cosf(r.z), sz = sinf(r.z); //roll

		return
		{
			cx * cy * cz + sx * sy * sz, //w
			sx * cy * cz + cx * sy * sz, //x
			cx * sy * cz - sx * cy * sz, //y
			cx * cy * sz - sx * sy * cz  //z
		};
	}

	//Converts mat3 to quat
	inline quat toquat(const mat3& m)
	{
		const f32 trace = m.m00 + m.m11 + m.m22;
		quat q{};

		if (trace > 0.0f)
		{
			const f32 s = 0.5f / sqrtf(trace + 1.0f);
			q.w = 0.25f / s;
			q.x = (m.m21 - m.m12) * s;
			q.y = (m.m02 - m.m20) * s;
			q.z = (m.m10 - m.m01) * s;
		}
		else if (m.m00 > m.m11 && m.m00 > m.m22)
		{
			const f32 s = 2.0f * sqrtf(1.0f + m.m00 - m.m11 - m.m22);
			q.w = (m.m21 - m.m12) / s;
			q.x = 0.25f * s;
			q.y = (m.m10 + m.m01) / s;
			q.z = (m.m20 + m.m02) / s;
		}
		else if (m.m11 > m.m22)
		{
			const f32 s = 2.0f * sqrtf(1.0f + m.m11 - m.m00 - m.m22);
			q.w = (m.m02 - m.m20) / s;
			q.x = (m.m10 + m.m01) / s;
			q.y = 0.25f * s;
			q.z = (m.m21 + m.m12) / s;
		}
		else
		{
			const f32 s = 2.0f * sqrtf(1.0f + m.m22 - m.m00 - m.m11);
			q.w = (m.m10 - m.m01) / s;
			q.x = (m.m20 + m.m02) / s;
			q.y = (m.m21 + m.m12) / s;
			q.z = 0.25f * s;
		}

		return q;
	}
	//Converts mat4 to quat
	inline quat toquat(const mat4& m)
	{
		const f32 trace = m.m00 + m.m11 + m.m22;
		quat q{};

		if (trace > 0.0f)
		{
			const f32 s = 0.5f / sqrtf(trace + 1.0f);
			q.w = 0.25f / s;
			q.x = (m.m21 - m.m12) * s;
			q.y = (m.m02 - m.m20) * s;
			q.z = (m.m10 - m.m01) * s;
		}
		else if (m.m00 > m.m11 && m.m00 > m.m22)
		{
			const f32 s = 2.0f * sqrtf(1.0f + m.m00 - m.m11 - m.m22);
			q.w = (m.m21 - m.m12) / s;
			q.x = 0.25f * s;
			q.y = (m.m10 + m.m01) / s;
			q.z = (m.m20 + m.m02) / s;
		}
		else if (m.m11 > m.m22)
		{
			const f32 s = 2.0f * sqrtf(1.0f + m.m11 - m.m00 - m.m22);
			q.w = (m.m02 - m.m20) / s;
			q.x = (m.m10 + m.m01) / s;
			q.y = 0.25f * s;
			q.z = (m.m21 + m.m12) / s;
		}
		else
		{
			const f32 s = 2.0f * sqrtf(1.0f + m.m22 - m.m00 - m.m11);
			q.w = (m.m10 - m.m01) / s;
			q.x = (m.m20 + m.m02) / s;
			q.y = (m.m21 + m.m12) / s;
			q.z = 0.25f * s;
		}

		return q;
	}

	//Converts quat to mat3
	inline mat3 tomat3(const quat& q)
	{
		quat nq = normalize_q(q);
		
		const f32 xx = nq.x * nq.x;
		const f32 yy = nq.y * nq.y;
		const f32 zz = nq.z * nq.z;
		const f32 xy = nq.x * nq.y;
		const f32 xz = nq.x * nq.z;
		const f32 yz = nq.y * nq.z;
		const f32 wx = nq.w * nq.x;
		const f32 wy = nq.w * nq.y;
		const f32 wz = nq.w * nq.z;

		return 
		{
			1.0f - 2.0f * (yy + zz),  2.0f * (xy + wz),      2.0f * (xz - wy),
			2.0f * (xy - wz),         1.0f - 2.0f * (xx + zz), 2.0f * (yz + wx),
			2.0f * (xz + wy),         2.0f * (yz - wx),      1.0f - 2.0f * (xx + yy)
		};
	}
	//Converts quat to mat4
	inline mat4 tomat4(const quat& q)
	{
		quat nq = normalize_q(q);
		
		const f32 xx = nq.x * nq.x;
		const f32 yy = nq.y * nq.y;
		const f32 zz = nq.z * nq.z;
		const f32 xy = nq.x * nq.y;
		const f32 xz = nq.x * nq.z;
		const f32 yz = nq.y * nq.z;
		const f32 wx = nq.w * nq.x;
		const f32 wy = nq.w * nq.y;
		const f32 wz = nq.w * nq.z;

		return 
		{
			1.0f - 2.0f * (yy + zz), 2.0f * (xy + wz),        2.0f * (xz - wy),        0.0f,
			2.0f * (xy - wz),        1.0f - 2.0f * (xx + zz), 2.0f * (yz + wx),        0.0f,
			2.0f * (xz + wy),        2.0f * (yz - wx),        1.0f - 2.0f * (xx + yy), 0.0f,
			0.0f,                    0.0f,                    0.0f,                    1.0f
		};
	}

	//Converts a 2D matrix to 3D
	inline mat4 tomat4(const mat3& m)
	{
		return
		{
			m.m00, m.m10, 0.0f, 0.0f,
			m.m01, m.m11, 0.0f, 0.0f,
			0.0f,  0.0f,  1.0f, 0.0f,
			m.m02, m.m12, 0.0f, 1.0f
		};
	}
	
	//Vector pointing from one position to another
	inline vec2 direction(
		const vec2 a, 
		const vec2 b)
	{
		return normalize(b - a);
	}
	//Vector pointing from one position to another
	inline vec3 direction(
		const vec3& a, 
		const vec3& b)
	{
		return normalize(b - a);
	}

	//Returns the perpendicular magnitude in 2D
	inline f32 cross(
		const vec2 a,
		const vec2 b)
	{
		return
			a.x * b.y
			- a.y * b.x;
	}
	//Returns the normal vec3 in 3D
	inline vec3 cross(
		const vec3& a,
		const vec3& b)
	{
		return vec3
		{
			a.y * b.z - a.z * b.y,
			a.z * b.x - a.x * b.z,
			a.x * b.y - a.y * b.x
		};
	}
	
	//Restricts a vec2 to given ranges
	inline vec2 kclamp(
		const vec2 v,
		const vec2 min,
		const vec2 max)
	{
		return
		{
			clamp(v.x, min.x, max.x),
			clamp(v.y, min.y, max.y)
		};
	}
	//Restricts a vec3 to given ranges
	inline vec3 kclamp(
		const vec3& v,
		const vec3& min,
		const vec3& max)
	{
		return
		{
			clamp(v.x, min.x, max.x),
			clamp(v.y, min.y, max.y),
			clamp(v.z, min.z, max.z)
		};
	}
	//Restricts a vec4 to given ranges
	inline vec4 kclamp(
		const vec4& v,
		const vec4& min,
		const vec4& max)
	{
		return
		{
			clamp(v.x, min.x, max.x),
			clamp(v.y, min.y, max.y),
			clamp(v.z, min.z, max.z),
			clamp(v.w, min.w, max.w)
		};
	}

	//Turns position/orientation into a view transform,
	//use DIR_UP for fps style camera, or if youre using the camera from KalaGraphics
	inline mat4 view(
		const vec3& origin, 
		const vec3& target, 
		const vec3& up)
	{
		vec3 f = normalize(target - origin);
		vec3 s = normalize(cross(f, up));
		vec3 u = cross(s, f);

		return 
		{
			 s.x,             u.x,            -f.x,           0.0f,
			 s.y,             u.y,            -f.y,           0.0f,
			 s.z,             u.z,            -f.z,           0.0f,
			-dot(s, origin), -dot(u, origin), dot(f, origin), 1.0f
		};
	}

	//ortographic projection, bottom-left origin, Y-up projection,
	//viewport size is clamped internally from 100x100 to 8192x8192,
	//near and far are clamped internally from -10000.0 to 10000.0
	inline mat4 ortho(
		const vec2 viewport,
		f32 zNear = -1.0f,
		f32 zFar = 1.0f)
	{
		//ensure viewport is always valid
		vec2 correctVP = viewport;
		correctVP = kclamp(correctVP, 100.0f, 8192.0f);
		
		//ensure zNear and zFar are within safe bounds
		zNear = clamp(zNear, -10000.0f, 10000.0f);
		zFar = clamp(zFar, -10000.0f, 10000.0f);
		
		//ensure zNear is always smaller than zFar
		if (zNear >= zFar) zFar = zNear + 1;
		
		const f32 left = 0.0f;
		const f32 right = correctVP.x;
		const f32 bottom = 0.0f;
		const f32 top = correctVP.y;

		const f32 rl = right - left;
		const f32 tb = top - bottom;
		const f32 fn = zFar - zNear;

		mat4 m{};

		m.m00 = 2.0f / rl;            m.m10 = 0.0f;                 m.m20 = 0.0f;                 m.m30 = 0.0f;
		m.m01 = 0.0f;                 m.m11 = 2.0f / tb;            m.m21 = 0.0f;                 m.m31 = 0.0f;
		m.m02 = 0.0f;                 m.m12 = 0.0f;                 m.m22 = -2.0f / fn;           m.m32 = 0.0f;
		m.m03 = -(right + left) / rl; m.m13 = -(top + bottom) / tb; m.m23 = -(zFar + zNear) / fn; m.m33 = 1.0f;

		return m;
	}

	//perpective projection, bottom-left origin, Y-up projection,
	//viewport size is clamped internally from 100x100 to 8192x8192,
	//fov is clamped internally from 1.0 to 360.0 degrees,
	//near and far are clamped internally from epsilon to 1000000.0
	inline mat4 perspective(
		const vec2 viewport,
		f32 fovDeg = 90.0f,
		f32 zNear = 0.001f,
		f32 zFar = 512.0f)
	{
		//ensure viewport is always valid
		vec2 correctVP = viewport;
		correctVP = kclamp(correctVP, 100.0f, 8192.0f);
		
		//ensure fov is within safe bounds
		fovDeg = clamp(fovDeg, 1.0f, 360.0f);
		
		//ensure zNear and zFar are within safe bounds
		zNear = clamp(zNear, epsilon, 1000000.0f);
		zFar = clamp(zFar, epsilon, 1000000.0f);
		
		//ensure zNear is always smaller than zFar
		if (zNear >= zFar) zFar = zNear + 1;
		
		const f32 aspect = correctVP.x / correctVP.y;
		const f32 f = 1.0f / tanf(radians(fovDeg) * 0.5f);
		const f32 fn = zFar - zNear;

		mat4 m{};

		m.m00 = f / aspect; m.m10 = 0.0f; m.m20 = 0.0f;                 m.m30 = 0.0f;
		m.m01 = 0.0f;       m.m11 = f;    m.m21 = 0.0f;                 m.m31 = 0.0f;
		m.m02 = 0.0f;       m.m12 = 0.0f; m.m22 = -(zFar + zNear) / fn; m.m32 = -1.0f;
		m.m03 = 0.0f;       m.m13 = 0.0f; m.m23 = -(2.0f * zFar * zNear) / fn; m.m33 = 0.0f;

		return m;
	}

	//Returns a valid 2D uModel for vertex shaders
	inline mat4 createumodel(
		const vec2 pos,
		const f32 rotDeg,
		const vec2 size)
	{
		f32 r = radians(rotDeg);
		f32 c = cosf(r);
		f32 s = sinf(r);

		mat4 m{};

		m.m00 = c * size.x; m.m10 = -s * size.x; m.m20 = 0.0f; m.m30 = 0.0f;
		m.m01 = s * size.y; m.m11 =  c * size.y; m.m21 = 0.0f; m.m31 = 0.0f;
		m.m02 = 0.0f;       m.m12 = 0.0f;        m.m22 = 1.0f; m.m32 = 0.0f;
		m.m03 = pos.x;      m.m13 = pos.y;       m.m23 = 0.0f; m.m33 = 1.0f;

		return m;
	}

	//Returns a valid 3D uModel for vertex shaders
	inline mat4 createumodel(
		const vec3& pos, 
		const quat& rot, 
		const vec3& size)
	{
		quat q = normalize_q(rot);
		
		f32 xx = q.x * q.x;
		f32 yy = q.y * q.y;
		f32 zz = q.z * q.z;
		f32 xy = q.x * q.y;
		f32 xz = q.x * q.z;
		f32 yz = q.y * q.z;
		f32 wx = q.w * q.x;
		f32 wy = q.w * q.y;
		f32 wz = q.w * q.z;

		mat4 m{};

		m.m00 = (1.0f - 2.0f * (yy + zz)) * size.x;
		m.m10 = (2.0f * (xy - wz)) * size.x;
		m.m20 = (2.0f * (yz + wy)) * size.x;
		m.m30 = 0.0f;

		m.m01 = (2.0f * (xy + wz)) * size.y;
		m.m11 = (1.0f - 2.0f * (xx + zz)) * size.y;
		m.m21 = (2.0f * (yz - wx)) * size.y;
		m.m31 = 0.0f;

		m.m02 = (2.0f * (xz - wy)) * size.z;
		m.m12 = (2.0f * (yz + wx)) * size.z;
		m.m22 = (1.0f - 2.0f * (xx + yy)) * size.z;
		m.m32 = 0.0f;

		m.m03 = pos.x;
		m.m13 = pos.y;
		m.m23 = pos.z;
		m.m33 = 1.0f;

		return m;
	}

	//Linear interpolation between two floats by t
	inline f32 lerp(
		f32 a,
		f32 b,
		f32 t)
	{
		return a + (b - a) * t;
	}
	//Linear interpolation between two vec2s by t
	inline vec2 lerp(
		const vec2 a,
		const vec2 b,
		f32 t)
	{
		return
		{
			lerp(a.x, b.x, t),
			lerp(a.y, b.y, t)
		};
	}
	//Linear interpolation between two vec3s by t
	inline vec3 lerp(
		const vec3& a,
		const vec3& b,
		f32 t)
	{
		return
		{
			lerp(a.x, b.x, t),
			lerp(a.y, b.y, t),
			lerp(a.z, b.z, t)
		};
	}
	//Linear interpolation between two vec4s by t
	inline vec4 lerp(
		const vec4& a,
		const vec4& b,
		f32 t)
	{
		return
		{
			lerp(a.x, b.x, t),
			lerp(a.y, b.y, t),
			lerp(a.z, b.z, t),
			lerp(a.w, b.w, t)
		};
	}
	
	//Linear interpolation between two quats by t
	inline quat lerp(
		const quat& a,
		const quat& b,
		f32 t)
	{
		return normalize_q(
		{
			lerp(a.w, b.w, t),
			lerp(a.x, b.x, t),
			lerp(a.y, b.y, t),
			lerp(a.z, b.z, t)
		});
	}

	//Spherical linear interpolation between two non-normalized vec4s by t
	inline quat slerp(
		const quat& a,
		const quat& b,
		f32 t)
	{
		quat q1 = normalize_q(a);
		quat q2 = normalize_q(b);
		
		f32 dotAB = dot(q1, q2);
		
		//if dot < 0, flip one quaternion to ensure shortest rotation path
		if (dotAB < 0.0f)
		{
			q2 = q2 * -1.0f;
			dotAB = -dotAB;
		}
		
		//fall back to lerp if quats are extremely close
		if (dotAB >= 1.0f - epsilon)
		{
			return lerp(q1, q2, t);
		}
		
		f32 theta = acos(dotAB);
		f32 sinTheta = sinf(theta);
		
		f32 w1 = sinf((1.0f - t) * theta) / sinTheta;
		f32 w2 = sinf(t * theta) / sinTheta;

		return normalize_q(
		{
			q1.w * w1 + q2.w * w2,
			q1.x * w1 + q2.x * w2,
			q1.y * w1 + q2.y * w2,
			q1.z * w1 + q2.z * w2
		});
	}

	inline f32 smoothstep(
		const f32 edge0,
		const f32 edge1,
		const f32 x)
	{
		f32 t = clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);

		return t * t * (3.0f - 2.0f * t);
	}

	inline vec2 smoothstep(
		const vec2 edge0,
		const vec2 edge1,
		const vec2 x)
	{
		return
		{
			smoothstep(edge0.x, edge1.x, x.x),
			smoothstep(edge0.y, edge1.y, x.y)
		};
	}
	inline vec3 smoothstep(
		const vec3& edge0,
		const vec3& edge1,
		const vec3& x)
	{
		return
		{
			smoothstep(edge0.x, edge1.x, x.x),
			smoothstep(edge0.y, edge1.y, x.y),
			smoothstep(edge0.z, edge1.z, x.z)
		};
	}
	inline vec4 smoothstep(
		const vec4& edge0,
		const vec4& edge1,
		const vec4& x)
	{
		return
		{
			smoothstep(edge0.x, edge1.x, x.x),
			smoothstep(edge0.y, edge1.y, x.y),
			smoothstep(edge0.z, edge1.z, x.z),
			smoothstep(edge0.w, edge1.w, x.w)
		};
	}

	//Uses std::sqrtf and returns unit-accurate distance between two vec2s
	inline f32 distancesqrt(
		const vec2 a, 
		const vec2 b)
	{
		f32 dx = a.x - b.x;
		f32 dy = a.y - b.y;

		return sqrtf(dx * dx + dy * dy);
	}
	//Uses std::sqrtf and returns unit-accurate distance between two vec3s
	inline f32 distancesqrt(
		const vec3& a, 
		const vec3& b)
	{
		f32 dx = a.x - b.x;
		f32 dy = a.y - b.y;
		f32 dz = a.z - b.z;

		return sqrtf(dx * dx + dy * dy + dz * dz);
	}

	//Does not use std::sqrtf and returns squared distance between two vec2s
	inline f32 distancefast(
		const vec2 a, 
		const vec2 b)
	{
		f32 dx = a.x - b.x;
		f32 dy = a.y - b.y;

		return dx * dx + dy * dy;
	}
	//Does not use std::sqrtf and returns squared distance between two vec3s
	inline f32 distancefast(
		const vec3& a, 
		const vec3& b)
	{
		f32 dx = a.x - b.x;
		f32 dy = a.y - b.y;
		f32 dz = a.z - b.z;

		return dx * dx + dy * dy + dz * dz;
	}

	inline vec3 reflect(
		const vec3& I, 
		const vec3& N)
	{
		//N needs to be normalized, if not then I is returned
		if (!isnormalized(N)) return I;
		
		return I - 2.0f * dot(I, N) * N;
	}

	//Takes in non-normalized positions and returns angle in degrees (0..180),
	//uses vec2 a as the reference direction
	inline f32 angle(
		const vec2 a,
		const vec2 b)
	{
		vec2 na = normalize(a);
		vec2 nb = normalize(b);

		f32 d = clamp(dot(na, nb), -1.0f, 1.0f);
		return degrees(acos(d));
	}
	//Takes in non-normalized positions and returns angle in degrees (0..180),
	//uses vec3 a as the reference direction
	inline f32 angle(
		const vec3& a,
		const vec3& b,
		const vec3& axis)
	{
		vec3 na = normalize(a);
		vec3 nb = normalize(b);
		vec3 ax = normalize(axis);

		//project onto plane perpendicular to the axis
		na -= ax * dot(na, ax);
		nb -= ax * dot(nb, ax);

		na = normalize(na);
		nb = normalize(nb);

		f32 d = clamp(dot(na, nb), -1.0f, 1.0f);
		return degrees(acos(d));
	}

	//Takes in non-normalized positions and returns signed angle in degrees (-180..180),
	//uses vec2 a as the reference direction
	inline f32 angle_s(
		const vec2 a,
		const vec2 b)
	{
		vec2 na = normalize(a);
		vec2 nb = normalize(b);

		f32 rad = atan2(nb.y, nb.x) - atan2(na.y, na.x);
		f32 deg = degrees(rad);

		if (deg > 180.0f) deg -= 360.0f;
		if (deg < -180.0f) deg += 360.0f;

		return deg;
	}
	//Takes in non-normalized positions and returns signed angle in degrees (-180..180),
	//uses vec3 a as the reference direction
	inline f32 angle_s(
		const vec3& a,
		const vec3& b,
		const vec3& axis)
	{
		vec3 na = normalize(a);
		vec3 nb = normalize(b);
		vec3 ax = normalize(axis);

		//project onto plane perpendicular to the axis
		na -= ax * dot(na, ax);
		nb -= ax * dot(nb, ax);

		na = normalize(na);
		nb = normalize(nb);

		f32 rad = atan2(dot(cross(na, nb), ax), dot(na, nb));
		return degrees(rad);
	}

	//Takes in non-normalized positions and returns full angle in degrees (0..360),
	//uses vec2 a as the reference direction
	inline f32 angle_f(
		const vec2 a,
		const vec2 b)
	{
		vec2 na = normalize(a);
		vec2 nb = normalize(b);

		f32 rad = atan2(nb.y, nb.x) - atan2(na.y, na.x);
		f32 deg = degrees(rad);

		if (deg < 0.0f) deg += 360.0f;

		return deg;
	}
	//Takes in non-normalized positions and returns full angle in degrees (0..360),
	//uses vec3 a as the reference direction
	inline f32 angle_f(
		const vec3& a,
		const vec3& b,
		const vec3& axis)
	{
		f32 deg = angle_s(a, b, axis);
		if (deg < 0.0f) deg += 360.0f;
		return deg;
	}

	//Takes in a non-normalized axis and returns a quaternion that rotates around angle and axis
	inline quat angleaxis(
		f32 angle, 
		const vec3& axis)
	{
		vec3 na = normalize(axis);
		f32 half = angle * 0.5f;
		f32 s = sinf(half);

		return normalize_q(
			{
				cosf(half),
				na.x * s,
				na.y * s,
				na.z * s
			});
	}

	//Projects vec2 a onto vec2 b
	inline vec2 project(
		const vec2 a, 
		const vec2 b)
	{
		return (dot(a, b) / dot(b, b)) * b;
	}
	//Projects vec3 a onto vec3 b
	inline vec3 project(
		const vec3& a, 
		const vec3& b)
	{
		return (dot(a, b) / dot(b, b)) * b;
	}
	
	//Returns neutral vec2
	constexpr vec2 identity_vec2() { return vec2{}; }
	//Returns true if vec2 is true identity
	inline bool isidentity(const vec2& v) { return isnear(v); }
	
	//Returns neutral vec3
	constexpr vec3 identity_vec3() { return vec3{}; }
	//Returns true if vec3 is true identity
	inline bool isidentity(const vec3& v) { return isnear(v); }
	
	//Returns neutral vec4
	constexpr vec4 identity_vec4() { return vec4{}; }
	//Returns true if vec4 is true identity
	inline bool isidentity(const vec4& v) { return isnear(v); }
	
	//Returns neutral quat
	constexpr quat identity_quat() { return quat{}; }
	//Returns true if quat is true identity
	inline bool isidentity_q(const quat& q)
	{
		return isnear(q.w, 1.0f)
			&& isnear(q.x)
			&& isnear(q.y)
			&& isnear(q.z);
	}

	//Returns neutral mat2
	constexpr mat2 identity_mat2() { return mat2{}; }
	//Returns true if mat2 is true identity
	inline bool isidentity(const mat2& m)
	{
		return isnear(m.m00, 1.0f)
			&& isnear(m.m01)
			
			&& isnear(m.m10)
			&& isnear(m.m11, 1.0f);
	}
	
	//Returns neutral mat3
	constexpr mat3 identity_mat3() { return mat3{}; }
	//Returns true if mat3 is true identity
	inline bool isidentity(const mat3& m)
	{
		return isnear(m.m00, 1.0f)
			&& isnear(m.m01)
			&& isnear(m.m02)

			&& isnear(m.m10)
			&& isnear(m.m11, 1.0f)
			&& isnear(m.m12)

			&& isnear(m.m20)
			&& isnear(m.m21)
			&& isnear(m.m22, 1.0f);
	}
	
	//Returns neutral mat4
	constexpr mat4 identity_mat4() { return mat4{}; }
	//Returns true if mat4 is true identity
	inline bool isidentity(const mat4& m)
	{
		return isnear(m.m00, 1.0f)
			&& isnear(m.m01)
			&& isnear(m.m02)
			&& isnear(m.m03)

			&& isnear(m.m10)
			&& isnear(m.m11, 1.0f)
			&& isnear(m.m12)
			&& isnear(m.m13)

			&& isnear(m.m20)
			&& isnear(m.m21)
			&& isnear(m.m22, 1.0f)
			&& isnear(m.m23)

			&& isnear(m.m30)
			&& isnear(m.m31)
			&& isnear(m.m32)
			&& isnear(m.m33, 1.0f);
	}
	
	//============================================================================
	//
	// TRANSFORM
	//
	//============================================================================
	
	enum class PosTarget
	{
		POS_WORLD,    //position in world space
		POS_LOCAL,    //position relative to parent

		POS_COMBINED, //final position after combining world and local position
	};
	enum class RotTarget
	{
		ROT_WORLD,   //rotation in world space
		ROT_LOCAL,   //rotation relative to parent

		ROT_COMBINED //final rotation after combining world and local rotation
	};
	enum class SizeTarget
	{
		SIZE_WORLD,   //size in world space
		SIZE_LOCAL,   //size relative to parent

		SIZE_COMBINED //final position after combining world and local position
	};
	
	constexpr vec2 MIN_POS2 = vec2(-10000.0f);
	constexpr vec2 MAX_POS2 = vec2(10000.0f);
	
	constexpr vec2 MIN_SIZE2 = vec2(epsilon);
	constexpr vec2 MAX_SIZE2 = vec2(10000.0f);
	
	struct Transform2D
	{
		vec2 pos_world{};
		vec2 pos_local{};
		vec2 pos_combined{};

		f32 rot_world{};
		f32 rot_local{};
		f32 rot_combined{};

		vec2 size_world    = vec2(1.0f);
		vec2 size_local    = vec2(1.0f);
		vec2 size_combined = vec2(1.0f);
	};
	
	constexpr vec3 MIN_POS3 = vec3(-10000.0f);
	constexpr vec3 MAX_POS3 = vec3(10000.0f);
	
	constexpr vec3 MIN_SIZE3 = vec3(epsilon);
	constexpr vec3 MAX_SIZE3 = vec3(10000.0f);
	
	struct Transform3D
	{
		vec3 pos_world{};
		vec3 pos_local{};
		vec3 pos_combined{};

		quat rot_world{};
		quat rot_local{};
		quat rot_combined{};

		vec3 size_world    = vec3(1.0f);
		vec3 size_local    = vec3(1.0f);
		vec3 size_combined = vec3(1.0f);
	};
	
	//============================================================================
	//
	// TRANSFORM2D OPERATORS
	//
	//============================================================================
	
	//Updates target combined pos, rot and size relative to target local and parent combined values,
	//if parent is identity then target combined is target world
	inline void combine(
		Transform2D& target,
		const Transform2D& parent)
	{
		if (!isidentity(parent.pos_combined)
			|| !isnear(parent.rot_combined)
			|| !isnear(parent.size_combined, vec2(1.0f)))
		{
			target.rot_combined = 
				parent.rot_combined 
				+ target.rot_world 
				+ target.rot_local;
				
			target.size_combined = 
				parent.size_combined 
				* target.size_world 
				* target.size_local;

			f32 rads = radians(parent.rot_combined);
			mat3 rot_mat =
			{
				cosf(rads), -sinf(rads), 0.0f,
				sinf(rads),  cosf(rads), 0.0f,
				0.0f,        0.0f,       1.0f
			};

			vec3 rot_offset = vec3(rot_mat * vec3(target.pos_local, 1.0f));
			target.pos_combined =
				parent.pos_combined
				+ target.pos_world
				+ vec2(rot_offset.x, rot_offset.y);
		}
		else
		{
			target.pos_combined = target.pos_world;
			target.rot_combined = target.rot_world;
			target.size_combined = target.size_world;
		}
	}
	
	//Incrementally update position over time,
	//adding parent updates this position relative to parent
	inline void addpos(
		Transform2D& target,
		const Transform2D& parent,
		PosTarget type,
		const vec2 pos_delta)
	{
		//cannot set combined pos
		if (type == PosTarget::POS_COMBINED) return;
		
		vec2 base = (type == PosTarget::POS_WORLD)
			? target.pos_world
			: target.pos_local;
			
		vec2 pos_clamped = kclamp(
			base + pos_delta,
			MIN_POS2,
			MAX_POS2);
			
		switch (type)
		{
		default: return;
		case PosTarget::POS_WORLD: target.pos_world = pos_clamped; break;
		case PosTarget::POS_LOCAL: target.pos_local = pos_clamped; break;
		}
		
		combine(target, parent);
	}
	//Snaps to given position,
	//adding parent updates this position relative to parent
	inline void setpos(
		Transform2D& target,
		const Transform2D& parent,
		PosTarget type,
		const vec2 pos_new)
	{
		//cannot set combined pos
		if (type == PosTarget::POS_COMBINED) return;

		vec2 pos_clamped = kclamp(
			pos_new, 
			MIN_POS2, 
			MAX_POS2);

		switch (type)
		{
		default: return;
		case PosTarget::POS_WORLD: target.pos_world = pos_clamped; break;
		case PosTarget::POS_LOCAL: target.pos_local = pos_clamped; break;
		}

		combine(target, parent);
	}
	inline vec2 getpos(
		const Transform2D& target,
		PosTarget type)
	{
		switch (type)
		{
		default: return{};
		case PosTarget::POS_WORLD:    return target.pos_world; break;
		case PosTarget::POS_LOCAL:    return target.pos_local; break;
		case PosTarget::POS_COMBINED: return target.pos_combined; break;
		}
	}
	
	//Returns true local right direction of this transform
	inline vec2 getdirright(Transform2D& target)
	{
		float r = radians(target.rot_combined);
		return vec2(cosf(r), sinf(r));
	}
	//Returns true local up direction of this transform
	inline vec2 getdirup(Transform2D& target)
	{
		float r = radians(target.rot_combined);
		return vec2(-sinf(r), cosf(r));
	}
	
	//Takes in rotation in euler (degrees) and incrementally rotates over time,
	//adding parent updates this rotation relative to parent,
	//clamps between -360 and 360, you're expected to wrap according to your needs on your end
	inline void addrot(
		Transform2D& target,
		const Transform2D& parent,
		RotTarget type,
		f32 rot_delta)
	{
		//cannot set combined vec rot
		if (type == RotTarget::ROT_COMBINED) return;
		
		f32 base = (type == RotTarget::ROT_WORLD)
			? target.rot_world
			: target.rot_local;

		f32 clamped = base + rot_delta;
		clamped = clamp(clamped, -360.0f, 360.0f);

		switch (type)
		{
		default: return;
		case RotTarget::ROT_WORLD: target.rot_world = clamped; break;
		case RotTarget::ROT_LOCAL: target.rot_local = clamped; break;
		}

		combine(target, parent);
	}
	//Takes in rotation in euler (degrees) and snaps to given rotation,
	//adding parent updates this rotation relative to parent,
	//clamps between -360 and 360, you're expected to wrap according to your needs on your end
	inline void setrot(
		Transform2D& target,
		const Transform2D& parent,
		RotTarget type,
		const f32 rot_new)
	{
		//cannot set combined vec rot
		if (type == RotTarget::ROT_COMBINED) return;

		f32 clamped = clamp(rot_new, -360.0f, 360.0f);

		switch (type)
		{
		default: return;
		case RotTarget::ROT_WORLD: target.rot_world = clamped; break;
		case RotTarget::ROT_LOCAL: target.rot_local = clamped; break;
		}

		combine(target, parent);
	}
	//Returns rotation in euler (degrees)
	inline f32 getrot(
		const Transform2D& target,
		RotTarget type)
	{
		switch (type)
		{
		default: return{};
		case RotTarget::ROT_WORLD:    return target.rot_world; break;
		case RotTarget::ROT_LOCAL:    return target.rot_local; break;
		case RotTarget::ROT_COMBINED: return target.rot_combined; break;
		}
	}
	
	//Incrementally scales over time,
	//adding parent updates this size relative to parent
	inline void addsize(
		Transform2D& target,
		const Transform2D& parent,
		SizeTarget type,
		const vec2 size_delta)
	{
		//cannot set combined size
		if (type == SizeTarget::SIZE_COMBINED) return;

		vec2 base = (type == SizeTarget::SIZE_WORLD)
			? target.size_world
			: target.size_local;

		vec2 size_clamped = kclamp(
			base + size_delta,
			MIN_SIZE2,
			MAX_SIZE2);

		switch (type)
		{
		default: return;
		case SizeTarget::SIZE_WORLD: target.size_world = size_clamped; break;
		case SizeTarget::SIZE_LOCAL: target.size_local = size_clamped; break;
		}

		combine(target, parent);
	}
	//Snaps to given size,
	//adding parent updates this size relative to parent
	inline void setsize(
		Transform2D& target,
		const Transform2D& parent,
		SizeTarget type,
		const vec2 size_new)
	{
		//cannot set combined size
		if (type == SizeTarget::SIZE_COMBINED) return;

		vec2 size_clamped = kclamp(
			size_new, 
			MIN_SIZE2, 
			MAX_SIZE2);

		switch (type)
		{
		default: return;
		case SizeTarget::SIZE_WORLD: target.size_world = size_clamped; break;
		case SizeTarget::SIZE_LOCAL: target.size_local = size_clamped; break;
		}

		combine(target, parent);
	}
	inline vec2 getsize(
		const Transform2D& target,
		SizeTarget type)
	{
		switch (type)
		{
		default: return{};
		case SizeTarget::SIZE_WORLD:    return target.size_world;
		case SizeTarget::SIZE_LOCAL:    return target.size_local;
		case SizeTarget::SIZE_COMBINED: return target.size_combined;
		}
	}
	
	//============================================================================
	//
	// TRANSFORM3D OPERATORS
	//
	//============================================================================
	
	//Updates target combined pos, rot and size relative to target local and parent combined values,
	//if parent is identity then target combined is target world
	inline void combine(
		Transform3D& target,
		const Transform3D& parent)
	{
		if (!isidentity(parent.pos_combined)
			|| !isidentity_q(parent.rot_combined)
			|| !isnear(parent.size_combined, vec3(1.0f)))
		{
			target.rot_combined = 
				parent.rot_combined
				* target.rot_world
				* target.rot_local;
			target.size_combined =
				parent.size_combined
				* target.size_world
				* target.size_local;

			auto rotate = [](const mat4& m, const quat& q)
				{
					const f32 xx = q.x * q.x;
					const f32 yy = q.y * q.y;
					const f32 zz = q.z * q.z;
					const f32 xy = q.x * q.y;
					const f32 xz = q.x * q.z;
					const f32 yz = q.y * q.z;
					const f32 wx = q.w * q.x;
					const f32 wy = q.w * q.y;
					const f32 wz = q.w * q.z;

					mat4 r =
					{
						1 - 2 * (yy + zz), 2 * (xy + wz),     2 * (xz - wy),     0.0f,
						2 * (xy - wz),     1 - 2 * (xx + zz), 2 * (yz + wx),     0.0f,
						2 * (xz + wy),     2 * (yz - wx),     1 - 2 * (xx + yy), 0.0f,
						0.0f,              0.0f,              0.0f,              1.0f
					};

					return r * m;
				};

			mat4 rot_mat = rotate(mat4(1.0f), parent.rot_combined);

			vec4 rot_offset = rot_mat * vec4(target.pos_local, 1.0f);
			target.pos_combined =
				parent.pos_combined
				+ target.pos_world
				+ vec3(
					rot_offset.x,
					rot_offset.y,
					rot_offset.z);
		}
		else
		{
			target.pos_combined = target.pos_world;
			target.rot_combined = target.rot_world;
			target.size_combined = target.size_world;
		}
	}
	
	//Incrementally moves over time,
	//if parent is identity then target combined is target world
	inline void addpos(
		Transform3D& target,
		const Transform3D& parent,
		PosTarget type,
		const vec3& pos_delta)
	{
		//cannot set combined pos
		if (type == PosTarget::POS_COMBINED) return;

		vec3 base = (type == PosTarget::POS_WORLD)
			? target.pos_world
			: target.pos_local;

		vec3 pos_clamped = kclamp(
			base + pos_delta,
			MIN_POS3,
			MAX_POS3);

		switch (type)
		{
		default: return;
		case PosTarget::POS_WORLD: target.pos_world = pos_clamped; break;
		case PosTarget::POS_LOCAL: target.pos_local = pos_clamped; break;
		}

		combine(target, parent);
	}
	//Snaps to given position,
	//if parent is identity then target combined is target world
	inline void setpos(
		Transform3D& target,
		const Transform3D& parent,
		PosTarget type,
		const vec3& pos_new)
	{
		//cannot set combined pos
		if (type == PosTarget::POS_COMBINED) return;

		vec3 pos_clamped = kclamp(
			pos_new,
			MIN_POS3,
			MAX_POS3);

		switch (type)
		{
		default: return;
		case PosTarget::POS_WORLD: target.pos_world = pos_clamped; break;
		case PosTarget::POS_LOCAL: target.pos_local = pos_clamped; break;
		}

		combine(target, parent);
	}
	inline vec3 getpos(
		const Transform3D& target,
		PosTarget type)
	{
		switch (type)
		{
		default: return{};
		case PosTarget::POS_WORLD:    return target.pos_world;
		case PosTarget::POS_LOCAL:    return target.pos_local;
		case PosTarget::POS_COMBINED: return target.pos_combined;
		}
	}
	
	//Rotate towards target position,
	//if parent is identity then target combined is target world
	inline void lookat(
		Transform3D& target,
		const Transform3D& parent,
		RotTarget type,
		const vec3& targetPos)
	{
		//cannot set combined vec rot
		if (type == RotTarget::ROT_COMBINED) return;
		
		vec3 diff = targetPos - target.pos_combined;
		
		//cannot look at itself or targets too close to compute a direction
		if (isnear(diff)) return;
		
		vec3 forward = normalize(diff);
		
		//compute right and up
		
		vec3 right = cross(DIR_UP, forward);
		
		//ensures right axis is always valid by being perpendicular to forward
		//and never parallel to up vector if cross fails
		if (isnear(right)) right = cross(DIR_FRONT, forward);
		
		right = normalize(right);
		
		vec3 up = cross(forward, right);
		
		//build a quat from a rotation matrix
		
		quat q = normalize_q(toquat(
		{
			right.x,     right.y,    right.z,
			up.x,        up.y,       up.z,
			-forward.x, -forward.y, -forward.z
		}));
		
		switch (type)
		{
		default: return;
		case RotTarget::ROT_WORLD: target.rot_world = q; break;
		case RotTarget::ROT_LOCAL: target.rot_local = q; break;
		}
		
		combine(target, parent);
	}
	
	//Takes in rotation in euler (degrees) and incrementally rotates over time,
	//if parent is identity then target combined is target world,
	//clamps internally between -360 and 360, you're expected to wrap according to your needs on your end
	inline void addrot(
		Transform3D& target,
		const Transform3D& parent,
		RotTarget type,
		const vec3& rot_delta)
	{
		//cannot set combined vec rot
		if (type == RotTarget::ROT_COMBINED) return;

		vec3 current{};

		switch (type)
		{
		default: return;
		case RotTarget::ROT_WORLD: current = toeuler3(target.rot_world); break;
		case RotTarget::ROT_LOCAL: current = toeuler3(target.rot_local); break;
		}
			
		current = current + rot_delta;

		switch (type)
		{
		default: return;
		case RotTarget::ROT_WORLD: target.rot_world = toquat(current); break;
		case RotTarget::ROT_LOCAL: target.rot_local = toquat(current); break;
		}

		combine(target, parent);
	}
	//Takes in rotation in euler (degrees) and snaps to given rotation,
	//if parent is identity then target combined is target world,
	//clamps internally between -360 and 360, you're expected to wrap according to your needs on your end
	inline void setrot(
		Transform3D& target,
		const Transform3D& parent,
		RotTarget type,
		const vec3& rot_new)
	{
		//cannot set combined vec rot
		if (type == RotTarget::ROT_COMBINED) return;

		switch (type)
		{
		default: return;
		case RotTarget::ROT_WORLD: target.rot_world = toquat(rot_new); break;
		case RotTarget::ROT_LOCAL: target.rot_local = toquat(rot_new); break;
		}

		combine(target, parent);
	}
	//Takes in rotation in quaternion and snaps to given rotation,
	//if parent is identity then target combined is target world
	inline void setrot(
		Transform3D& target,
		const Transform3D& parent,
		RotTarget type,
		const quat& rot_new)
	{
		//cannot set combined vec rot
		if (type == RotTarget::ROT_COMBINED) return;

		quat rot_clamped = normalize_q(rot_new);

		switch (type)
		{
		default: return;
		case RotTarget::ROT_WORLD: target.rot_world = rot_clamped; break;
		case RotTarget::ROT_LOCAL: target.rot_local = rot_clamped; break;
		}
			
		combine(target, parent);
	}
	//Returns rotation in euler (degrees)
	inline vec3 getroteuler(
		const Transform3D& target,
		RotTarget type)
	{
		switch (type)
		{
		default: return{};
		case RotTarget::ROT_WORLD:    return toeuler3(target.rot_world); break;
		case RotTarget::ROT_LOCAL:    return toeuler3(target.rot_local); break;
		case RotTarget::ROT_COMBINED: return toeuler3(target.rot_combined); break;
		}
	}
	//Returns quaternion rotation
	inline quat getrotquat(
		const Transform3D& target,
		RotTarget type)
	{
		switch (type)
		{
		default: return{};
		case RotTarget::ROT_WORLD:    return target.rot_world; break;
		case RotTarget::ROT_LOCAL:    return target.rot_local; break;
		case RotTarget::ROT_COMBINED: return target.rot_combined; break;
		}
	}
	
	//Returns true local front direction of this transform
	inline vec3 getdirfront(Transform3D& target)
	{
		return target.rot_combined * DIR_FRONT;
	}
	//Returns true local right direction of this transform
	inline vec3 getdirright(Transform3D& target)
	{
		return target.rot_combined * DIR_RIGHT;
	}
	//Returns true local up direction of this transform
	inline vec3 getdirup(Transform3D& target)
	{
		return target.rot_combined * DIR_UP;
	}
	
	//Increments pitch over time with degrees,
	//if parent is identity then target combined is target world
	inline void addpitch(
		Transform3D& target,
		const Transform3D& parent,
		RotTarget type,
		float degrees)
	{
		addrot(target, parent, type, {degrees, 0, 0});
	}
	//Increments yaw over time with degrees,
	//if parent is identity then target combined is target world
	inline void addyaw(
		Transform3D& target,
		const Transform3D& parent,
		RotTarget type,
		float degrees)
	{
		addrot(target, parent, type, {0, degrees, 0});
	}
	//Increments roll over time with degrees,
	//if parent is identity then target combined is target world
	inline void addroll(
		Transform3D& target,
		const Transform3D& parent,
		RotTarget type,
		float degrees)
	{
		addrot(target, parent, type, {0, 0, degrees});
	}
	
	//Snaps pitch to given degrees,
	//if parent is identity then target combined is target world
	inline void setpitch(
		Transform3D& target,
		const Transform3D& parent,
		RotTarget type,
		float degrees)
	{
		vec3 e = getroteuler(target, type);
		e.x = degrees;
		setrot(target, parent, type, e);
	}
	//Snaps pitch to given degrees,
	//if parent is identity then target combined is target world
	inline void setyaw(
		Transform3D& target,
		const Transform3D& parent,
		RotTarget type,
		float degrees)
	{
		vec3 e = getroteuler(target, type);
		e.y = degrees;
		setrot(target, parent, type, e);
	}
	//Snaps pitch to given degrees,
	//if parent is identity then target combined is target world
	inline void setroll(
		Transform3D& target,
		const Transform3D& parent,
		RotTarget type,
		float degrees)
	{
		vec3 e = getroteuler(target, type);
		e.z = degrees;
		setrot(target, parent, type, e);
	}
	
	//Returns pitch as degrees for current transform
	inline f32 getpitch(
		Transform3D& target,
		RotTarget type)
	{
		return getroteuler(target, type).x;
	}
	//Returns yaw as degrees for current transform
	inline f32 getyaw(
		Transform3D& target,
		RotTarget type)
	{
		return getroteuler(target, type).y;
	}
	//Returns roll as degrees for current transform
	inline f32 getroll(
		Transform3D& target,
		RotTarget type)
	{
		return getroteuler(target, type).z;
	}
	
	//Incrementally scales over time,
	//if parent is identity then target combined is target world
	inline void addsize(
		Transform3D& target,
		const Transform3D& parent,
		SizeTarget type,
		const vec3& size_delta)
	{
		//cannot set combined size
		if (type == SizeTarget::SIZE_COMBINED) return;

		vec3 base = (type == SizeTarget::SIZE_WORLD)
			? target.size_world
			: target.size_local;

		vec3 size_clamped = kclamp(
			base + size_delta,
			MIN_SIZE3,
			MAX_SIZE3);

		switch (type)
		{
		default: return;
		case SizeTarget::SIZE_WORLD: target.size_world = size_clamped; break;
		case SizeTarget::SIZE_LOCAL: target.size_local = size_clamped; break;
		}

		combine(target, parent);
	}
	//Snaps to given scale,
	//if parent is identity then target combined is target world
	inline void setsize(
		Transform3D& target,
		const Transform3D& parent,
		SizeTarget type,
		const vec3& size_new)
	{
		//cannot set combined size
		if (type == SizeTarget::SIZE_COMBINED) return;

		vec3 size_clamped = kclamp(
			size_new,
			MIN_SIZE3,
			MAX_SIZE3);

		switch (type)
		{
		default: return;
		case SizeTarget::SIZE_WORLD: target.size_world = size_clamped; break;
		case SizeTarget::SIZE_LOCAL: target.size_local = size_clamped; break;
		}

		combine(target, parent);
	}
	inline vec3 getsize(
		Transform3D& target,
		SizeTarget type)
	{
		switch (type)
		{
		default: return{};
		case SizeTarget::SIZE_WORLD:    return target.size_world;
		case SizeTarget::SIZE_LOCAL:    return target.size_local;
		case SizeTarget::SIZE_COMBINED: return target.size_combined;
		}
	};
}