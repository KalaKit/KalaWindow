//------------------------------------------------------------------------------
// math_utils.hpp
//
// Copyright (C) 2025 Lost Empire Entertainment
//
// This is free source code, and you are welcome to redistribute it under certain conditions.
// Read LICENSE.md for more information.
//
// Provides:
//   - shorthands for math variables
//   - GLM-like containers as vec2, vec3, vec4, mat2, mat3, mat4, quat (vec4)
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

using std::sin;
using std::cos;
using std::tan;
using std::sqrt;
using std::abs;
using std::clamp;
using std::min;
using std::max;
using std::fmod;
using std::fabsf;

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

//6-digit precision PI
constexpr f32 PI = 3.141593f;

//32-bit precision
constexpr f32 epsilon = 1e-6f;
//64-bit precision
constexpr f64 epsilon64 = 1e-12f;

namespace KalaHeaders
{
	//
	// GENERAL HELPERS
	//

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

	//
	// VEC
	//

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
		static_assert(N >= 2 && N <= 4, "kvec can only have 2, 3, or 4 components.");

		constexpr vec() = default;

		//vec2

		constexpr vec(f32 _x)
			requires (N == 2)
			: vec_storage<N>{ _x, _x } {}
		constexpr vec(f32 _x, f32 _y)
			requires (N == 2)
			: vec_storage<N>{ _x, _y } {}

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

		constexpr vec(const vec<3>& _v, f32 _w)
			requires (N == 4)
			: vec_storage<N>{ _v.x, _v.y, _v.z, _w } {}
		constexpr vec(f32 _x, const vec<3>& _v)
			requires (N == 4)
			: vec_storage<N>{ _x, _v.x, _v.y, _v.z } {}

		using storage = vec_storage<N>;

		//
		// ARITHMETIC OPERATORS
		//

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
		constexpr bool operator>(f32 s) const
		{
			if constexpr (N == 2) return { this->x > s && this->y > s };
			if constexpr (N == 3) return { this->x > s && this->y > s && this->z > s };
			if constexpr (N == 4) return { this->x > s && this->y > s && this->z > s && this->w > s };
		}
		constexpr bool operator<(const vec& s) const
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

		constexpr vec operator-() const
		{ 
			if constexpr (N == 2) return { -this->x, -this->y };
			if constexpr (N == 3) return { -this->x, -this->y, -this->z };
			if constexpr (N == 4) return { -this->x, -this->y, -this->z, -this->w };
		}

		constexpr bool operator==(const vec& v) const
		{
			if constexpr (N == 2) return 
				abs(this->x - v.x) < epsilon
				&& abs(this->y - v.y) < epsilon;
			if constexpr (N == 3) return
				abs(this->x - v.x) < epsilon
				&& abs(this->y - v.y) < epsilon
				&& abs(this->z - v.z) < epsilon;
			if constexpr (N == 4) return
				abs(this->x - v.x) < epsilon
				&& abs(this->y - v.y) < epsilon
				&& abs(this->z - v.z) < epsilon
				&& abs(this->w - v.w) < epsilon;
		}
		constexpr bool operator!=(const vec& v) const { return !(*this == v); }

		//
		// COMPOUND OPERATORS
		//

		constexpr vec& operator+=(const vec& v)
		{ 
			if constexpr (N == 2) { this->x += v.x; this->y += v.y; return *this; }
			if constexpr (N == 3) { this->x += v.x; this->y += v.y; this->z += v.z; return *this; }
			if constexpr (N == 4) { this->x += v.x; this->y += v.y; this->z += v.z; this->w += v.w; return *this; }
		}
		constexpr vec& operator+=(f32 s)
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
		constexpr vec& operator-=(f32 s)
		{
			if constexpr (N == 2) { this->x -= s; this->y -= s; return *this; }
			if constexpr (N == 3) { this->x -= s; this->y -= s; this->z -= s; return *this; }
			if constexpr (N == 4) { this->x -= s; this->y -= s; this->z -= s; this->w -= s; return *this; }
		}

		constexpr vec& operator*=(const vec& v)
		{
			if constexpr (N == 2) { this->x *= v.x; this->y *= v.y; return *this; }
			if constexpr (N == 3) { this->x *= v.x; this->y *= v.y; this->z *= v.z; return *this; }
			if constexpr (N == 4) { this->x *= v.x; this->y *= v.y; this->z *= v.z; this->w *= v.w; return *this; }
		}
		constexpr vec& operator*=(f32 s)
		{
			if constexpr (N == 2) { this->x *= s; this->y *= s; return *this; }
			if constexpr (N == 3) { this->x *= s; this->y *= s; this->z *= s; return *this; }
			if constexpr (N == 4) { this->x *= s; this->y *= s; this->z *= s; this->w *= s; return *this; }
		}

		constexpr vec& operator/=(const vec& v)
		{ 
			if constexpr (N == 2) { safediv_c(this->x, v.x); safediv_c(this->y, v.y); return *this; }
			if constexpr (N == 3) { safediv_c(this->x, v.x); safediv_c(this->y, v.y); safediv_c(this->z, v.z); return *this; }
			if constexpr (N == 4) { safediv_c(this->x, v.x); safediv_c(this->y, v.y); safediv_c(this->z, v.z); safediv_c(this->w, v.w); return *this; }
		}
		constexpr vec& operator/=(f32 s)
		{ 
			if constexpr (N == 2) { safediv_c(this->x, s); safediv_c(this->y, s); return *this; }
			if constexpr (N == 3) { safediv_c(this->x, s); safediv_c(this->y, s); safediv_c(this->z, s); return *this; }
			if constexpr (N == 4) { safediv_c(this->x, s); safediv_c(this->y, s); safediv_c(this->z, s); safediv_c(this->w, s); return *this; }
		}

		constexpr bool operator<=(f32 s) const
		{
			if constexpr (N == 2) return { this->x <= s && this->y <= s };
			if constexpr (N == 3) return { this->x <= s && this->y <= s && this->z <= s };
			if constexpr (N == 4) return { this->x <= s && this->y <= s && this->z <= s && this->w <= s };
		}
		constexpr bool operator>=(f32 s) const
		{
			if constexpr (N == 2) return { this->x >= s && this->y >= s };
			if constexpr (N == 3) return { this->x >= s && this->y >= s && this->z >= s };
			if constexpr (N == 4) return { this->x >= s && this->y >= s && this->z >= s && this->w >= s };
		}
		constexpr bool operator<=(const vec& s) const
		{
			if constexpr (N == 2) return { this->x <= s.x && this->y <= s.y };
			if constexpr (N == 3) return { this->x <= s.x && this->y <= s.y && this->z <= s.z };
			if constexpr (N == 4) return { this->x <= s.x && this->y <= s.y && this->z <= s.z && this->w <= s.w };
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
	constexpr vec<N> operator+(const vec<N>& v, f32 s)
	{
		return apply_scalar(v, s, [](f32 a, f32 b) { return a + b; });
	}
	template<size_t N>
		requires (N >= 2 && N <= 4)
	constexpr vec<N> operator+(f32 s, const vec<N>& v)
	{
		return apply_scalar(v, s, [](f32 a, f32 b) { return a + b; });
	}
	template<size_t N>
		requires (N >= 2 && N <= 4)
	constexpr vec<N> operator+(const vec<N> v1, const vec<N>& v2)
	{
		return apply_scalar(v1, v2, [](f32 a, f32 b) { return a + b; });
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

	template<size_t N>
		requires (N >= 2 && N <= 4)
	constexpr vec<N>& operator+=(vec<N>& v, f32 s)
	{
		v = apply_scalar(v, s, [](f32 a, f32 b) { return a + b; });
		return v;
	}
	template<size_t N>
		requires (N >= 2 && N <= 4)
	constexpr vec<N>& operator+=(vec<N>& v1, const vec<N>& v2)
	{
		v1 = apply_scalar(v1, v2, [](f32 a, f32 b) { return a + b; });
		return v1;
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
	constexpr vec<N> operator-(const vec<N>& v, f32 s)
	{
		return apply_scalar(v, s, [](f32 a, f32 b) { return a - b; });
	}
	template<size_t N>
		requires (N >= 2 && N <= 4)
	constexpr vec<N> operator-(f32 s, const vec<N>& v)
	{
		return apply_scalar(v, s, [](f32 a, f32 b) { return a - b; });
	}
	template<size_t N>
		requires (N >= 2 && N <= 4)
	constexpr vec<N> operator-(const vec<N> v1, const vec<N>& v2)
	{
		return apply_scalar(v1, v2, [](f32 a, f32 b) { return a - b; });
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

	template<size_t N>
		requires (N >= 2 && N <= 4)
	constexpr vec<N>& operator-=(vec<N>& v, f32 s)
	{
		v = apply_scalar(v, s, [](f32 a, f32 b) { return a - b; });
		return v;
	}
	template<size_t N>
		requires (N >= 2 && N <= 4)
	constexpr vec<N>& operator-=(vec<N>& v1, const vec<N>& v2)
	{
		v1 = apply_scalar(v1, v2, [](f32 a, f32 b) { return a - b; });
		return v1;
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
	constexpr vec<N> operator*(const vec<N>& v, f32 s)
	{
		return apply_scalar(v, s, [](f32 a, f32 b) { return a * b; });
	}
	template<size_t N>
		requires (N >= 2 && N <= 4)
	constexpr vec<N> operator*(f32 s, const vec<N>& v)
	{
		return apply_scalar(v, s, [](f32 a, f32 b) { return a * b; });
	}
	template<size_t N>
		requires (N >= 2 && N <= 4)
	constexpr vec<N> operator*(const vec<N> v1, const vec<N>& v2)
	{
		return apply_scalar(v1, v2, [](f32 a, f32 b) { return a * b; });
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

	template<size_t N>
		requires (N >= 2 && N <= 4)
	constexpr vec<N>& operator*=(vec<N>& v, f32 s)
	{
		v = apply_scalar(v, s, [](f32 a, f32 b) { return a * b; });
		return v;
	}
	template<size_t N>
		requires (N >= 2 && N <= 4)
	constexpr vec<N>& operator*=(vec<N>& v1, const vec<N>& v2)
	{
		v1 = apply_scalar(v1, v2, [](f32 a, f32 b) { return a * b; });
		return v1;
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
	constexpr vec<N> operator/(const vec<N>& v, f32 s)
	{
		return apply_scalar(v, s, [](f32 a, f32 b) { return safediv_a(a, b); });
	}
	template<size_t N>
		requires (N >= 2 && N <= 4)
	constexpr vec<N> operator/(f32 s, const vec<N>& v)
	{
		return apply_scalar(v, s, [](f32 a, f32 b) { return safediv_a(a, b); });
	}
	template<size_t N>
		requires (N >= 2 && N <= 4)
	constexpr vec<N> operator/(const vec<N> v1, const vec<N>& v2)
	{
		return apply_scalar(v1, v2, [](f32 a, f32 b) { return safediv_a(a, b); });
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

	template<size_t N>
		requires (N >= 2 && N <= 4)
	constexpr vec<N>& operator/=(vec<N>& v, f32 s)
	{
		v = apply_scalar(v, s, [](f32 a, f32 b) { return safediv_c(a, b); });
		return v;
	}
	template<size_t N>
		requires (N >= 2 && N <= 4)
	constexpr vec<N>& operator/=(vec<N>& v1, const vec<N>& v2)
	{
		v1 = apply_scalar(v1, v2, [](f32 a, f32 b) { return safediv_c(a, b); });
		return v1;
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

	//
	// MAT
	//

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

		//
		// ARITHMETIC OPERATORS
		//

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
				(abs(this->m00 - m.m00) < epsilon)
				&& (abs(this->m01 - m.m01) < epsilon)
				&& (abs(this->m10 - m.m10) < epsilon)
				&& (abs(this->m11 - m.m11) < epsilon);

			if constexpr (N == 3)
				return
				(abs(this->m00 - m.m00) < epsilon)
				&& (abs(this->m01 - m.m01) < epsilon)
				&& (abs(this->m02 - m.m02) < epsilon)
				&& (abs(this->m10 - m.m10) < epsilon)
				&& (abs(this->m11 - m.m11) < epsilon)
				&& (abs(this->m12 - m.m12) < epsilon)
				&& (abs(this->m20 - m.m20) < epsilon)
				&& (abs(this->m21 - m.m21) < epsilon)
				&& (abs(this->m22 - m.m22) < epsilon);

			if constexpr (N == 4)
				return
				(abs(this->m00 - m.m00) < epsilon)
				&& (abs(this->m01 - m.m01) < epsilon)
				&& (abs(this->m02 - m.m02) < epsilon)
				&& (abs(this->m03 - m.m03) < epsilon)
				&& (abs(this->m10 - m.m10) < epsilon)
				&& (abs(this->m11 - m.m11) < epsilon)
				&& (abs(this->m12 - m.m12) < epsilon)
				&& (abs(this->m13 - m.m13) < epsilon)
				&& (abs(this->m20 - m.m20) < epsilon)
				&& (abs(this->m21 - m.m21) < epsilon)
				&& (abs(this->m22 - m.m22) < epsilon)
				&& (abs(this->m23 - m.m23) < epsilon)
				&& (abs(this->m30 - m.m30) < epsilon)
				&& (abs(this->m31 - m.m31) < epsilon)
				&& (abs(this->m32 - m.m32) < epsilon)
				&& (abs(this->m33 - m.m33) < epsilon);
		}
		constexpr bool operator!=(const mat& m) const { return !(*this == m); }

		//
		// COMPOUND OPERATORS
		//

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

	using vec2 = vec<2>; //Vector: x, y
	using vec3 = vec<3>; //Vector: x, y, z
	using vec4 = vec<4>; //Vector: x, y, z, w

	using quat = vec<4>; //Quaternion: x, y, z, w

	using mat2 = mat<2>; //Matrix: m00 - m11
	using mat3 = mat<3>; //Matrix: m00 - m22
	using mat4 = mat<4>; //Matrix: m00 - m33

	const vec3 GRAVITY = { 0.0f, -9.81f, 0.0f };

	const vec3 DIR_RIGHT = { 1, 0,  0 };
	const vec3 DIR_UP    = { 0, 1,  0 };
	const vec3 DIR_FRONT = { 0, 0, -1 };

	const vec3 ROT_PITCH = { 1, 0, 0 };
	const vec3 ROT_YAW   = { 0, 1, 0 };
	const vec3 ROT_ROLL  = { 0, 0, 1 };

	//
	// HELPER FUNCTIONS USING DEFINED mat AND vec CONTAINERS
	//

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

	//Converts 2D euler (degrees) to quaternion
	inline quat toquat(const vec2 euler)
	{
		f32 half = radians(euler.y * 0.5f);
		f32 cz = cos(half);
		f32 sz = sin(half);

		//rotation around Z axis
		return { 0.0f, 0.0f, sz, cz };
	}
	//Converts 3D euler (degrees) to quaternion
	inline quat toquat(const vec3& euler)
	{
		vec3 r = radians(euler) * 0.5f;

		f32 cx = cos(r.x), sx = sin(r.x);
		f32 cy = cos(r.y), sy = sin(r.y);
		f32 cz = cos(r.z), sz = sin(r.z);

		return
		{
			sx * cy * cz - cx * sy * sz, //x
			cx * sy * cz + sx * cy * sz, //y
			cx * cy * sz - sx * sy * cz, //z
			cx * cy * cz + sx * sy * sz  //w
		};
	}

	//Converts quat to 2D euler (degrees)
	inline f32 toeuler2(const quat& q)
	{
		return degrees(atan2(
			2.0f * (q.w * q.z + q.x * q.y),
			1.0f - 2.0f * (q.y * q.y + q.z * q.z)));
	}
	//Converts quat to 3D euler (degrees)
	inline vec3 toeuler3(const quat& q)
	{
		f32 sinr_cosp = 2.0f * (q.w * q.x + q.y * q.z);
		f32 cosr_cosp = 1.0f - 2.0f * (q.x * q.x + q.y * q.y);
		f32 pitch = atan2(sinr_cosp, cosr_cosp);

		f32 sinp = 2.0f * (q.w * q.y - q.z * q.x);
		f32 yaw = (abs(sinp) >= 1.0f)
			? copysign(PI / 2.0f, sinp)
			: asin(sinp);

		f32 siny_cosp = 2.0f * (q.w * q.z + q.x * q.y);
		f32 cosy_cosp = 1.0f - 2.0f * (q.y * q.y + q.z * q.z);
		f32 roll = atan2(siny_cosp, cosy_cosp);

		return degrees(vec3{ pitch, yaw, roll });
	}

	//Converts mat3 to quat
	inline quat toquat(const mat3& m)
	{
		const f32 trace = m.m00 + m.m11 + m.m22;
		quat q{};

		if (trace > 0.0f)
		{
			const f32 s = 0.5f / sqrt(trace + 1.0f);
			q.w = 0.25f / s;
			q.x = (m.m21 - m.m12) * s;
			q.y = (m.m02 - m.m20) * s;
			q.z = (m.m10 - m.m01) * s;
		}
		else if (m.m00 > m.m11 && m.m00 > m.m22)
		{
			const f32 s = 2.0f * sqrt(1.0f + m.m00 - m.m11 - m.m22);
			q.w = (m.m21 - m.m12) / s;
			q.x = 0.25f * s;
			q.y = (m.m10 + m.m01) / s;
			q.z = (m.m20 + m.m02) / s;
		}
		else if (m.m11 > m.m22)
		{
			const f32 s = 2.0f * sqrt(1.0f + m.m11 - m.m00 - m.m22);
			q.w = (m.m02 - m.m20) / s;
			q.x = (m.m10 + m.m01) / s;
			q.y = 0.25f * s;
			q.z = (m.m21 + m.m12) / s;
		}
		else
		{
			const f32 s = 2.0f * sqrt(1.0f + m.m22 - m.m00 - m.m11);
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
			const f32 s = 0.5f / sqrt(trace + 1.0f);
			q.w = 0.25f / s;
			q.x = (m.m21 - m.m12) * s;
			q.y = (m.m02 - m.m20) * s;
			q.z = (m.m10 - m.m01) * s;
		}
		else if (m.m00 > m.m11 && m.m00 > m.m22)
		{
			const f32 s = 2.0f * sqrt(1.0f + m.m00 - m.m11 - m.m22);
			q.w = (m.m21 - m.m12) / s;
			q.x = 0.25f * s;
			q.y = (m.m10 + m.m01) / s;
			q.z = (m.m20 + m.m02) / s;
		}
		else if (m.m11 > m.m22)
		{
			const f32 s = 2.0f * sqrt(1.0f + m.m11 - m.m00 - m.m22);
			q.w = (m.m02 - m.m20) / s;
			q.x = (m.m10 + m.m01) / s;
			q.y = 0.25f * s;
			q.z = (m.m21 + m.m12) / s;
		}
		else
		{
			const f32 s = 2.0f * sqrt(1.0f + m.m22 - m.m00 - m.m11);
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
		const f32 xx = q.x * q.x;
		const f32 yy = q.y * q.y;
		const f32 zz = q.z * q.z;
		const f32 xy = q.x * q.y;
		const f32 xz = q.x * q.z;
		const f32 yz = q.y * q.z;
		const f32 wx = q.w * q.x;
		const f32 wy = q.w * q.y;
		const f32 wz = q.w * q.z;

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
		const f32 xx = q.x * q.x;
		const f32 yy = q.y * q.y;
		const f32 zz = q.z * q.z;
		const f32 xy = q.x * q.y;
		const f32 xz = q.x * q.z;
		const f32 yz = q.y * q.z;
		const f32 wx = q.w * q.x;
		const f32 wy = q.w * q.y;
		const f32 wz = q.w * q.z;

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

	//Wraps a rotation axis between 0 to 360 degrees
	inline f32 wrap(f32 deg)
	{
		deg = fmod(deg, 360.0f);
		if (deg < 0.0f) deg += 360.0f;

		return deg;
	}

	//ortographic projection, bottom-left origin, Y-up projection
	inline mat4 ortho(const vec2 viewport)
	{
		const float left = 0.0f;
		const float right = viewport.x;
		const float bottom = 0.0f;
		const float top = viewport.y;
		const float zNear = -1.0f;
		const float zFar = 1.0f;

		const float rl = right - left;
		const float tb = top - bottom;
		const float fn = zFar - zNear;

		mat4 m{};

		m.m00 = 2.0f / rl;            m.m10 = 0.0f;                 m.m20 = 0.0f;                 m.m30 = 0.0f;
		m.m01 = 0.0f;                 m.m11 = 2.0f / tb;            m.m21 = 0.0f;                 m.m31 = 0.0f;
		m.m02 = 0.0f;                 m.m12 = 0.0f;                 m.m22 = -2.0f / fn;           m.m32 = 0.0f;
		m.m03 = -(right + left) / rl; m.m13 = -(top + bottom) / tb; m.m23 = -(zFar + zNear) / fn; m.m33 = 1.0f;

		return m;
	}

	//perpective projection, bottom-left origin, Y-up projection
	inline mat4 perspective(
		const vec2 viewport,
		f32 fovDeg,
		f32 zNear,
		f32 zFar)
	{
		const f32 aspect = viewport.x / viewport.y;
		const f32 f = 1.0f / tan(radians(fovDeg) * 0.5f);
		const f32 fn = zFar - zNear;

		mat4 m{};

		m.m00 = f / aspect; m.m10 = 0.0f; m.m20 = 0.0f;                 m.m30 = 0.0f;
		m.m01 = 0.0f;       m.m11 = f;    m.m21 = 0.0f;                 m.m31 = 0.0f;
		m.m02 = 0.0f;       m.m12 = 0.0f; m.m22 = -(zFar + zNear) / fn; m.m32 = -(2.0f * zFar * zNear) / fn;
		m.m03 = 0.0f;       m.m13 = 0.0f; m.m23 = -1.0f;                m.m33 = 0.0f;

		return m;
	}

	//Computes vec2 magnitude (distance from a)
	inline f32 length(const vec2 v)
	{
		return sqrt(
			v.x * v.x
			+ v.y * v.y);
	}
	//Computes vec3 magnitude (distance from a)
	inline f32 length(const vec3& v)
	{
		return sqrt(
			v.x * v.x
			+ v.y * v.y
			+ v.z * v.z);
	}
	//Computes vec4 magnitude (distance from a)
	inline f32 length(const vec4& v)
	{
		return sqrt(
			v.x * v.x
			+ v.y * v.y
			+ v.z * v.z
			+ v.w * v.w);
	}

	//Returns unit-length version of vec2
	inline vec2 normalize(const vec2 v)
	{
		f32 len = length(v);
		return (len != 0.0f) ? v / len : vec2{};
	}
	//Returns unit-length version of vec3
	inline vec3 normalize(const vec3& v)
	{
		f32 len = length(v);
		return (len != 0.0f) ? v / len : vec3{};
	}
	//Returns unit-length version of vec4
	inline vec4 normalize(const vec4& v)
	{
		f32 len = length(v);
		return (len != 0.0f) ? v / len : vec4{};
	}
	//Normalizes a quaternion
	inline quat normalize_quat(const quat& q)
	{
		f32 len = sqrt(
			q.x * q.x
			+ q.y * q.y
			+ q.z * q.z
			+ q.w * q.w);

		return q / len;
	}

	//Returns a valid 2D uModel for vertex shaders
	inline mat4 createumodel(
		const vec2 pos,
		const f32 rotDeg,
		const vec2 size)
	{
		f32 r = radians(rotDeg);
		f32 c = cos(r);
		f32 s = sin(r);

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
		f32 xx = rot.x * rot.x;
		f32 yy = rot.y * rot.y;
		f32 zz = rot.z * rot.z;
		f32 xy = rot.x * rot.y;
		f32 xz = rot.x * rot.z;
		f32 yz = rot.y * rot.z;
		f32 wx = rot.w * rot.x;
		f32 wy = rot.w * rot.y;
		f32 wz = rot.w * rot.z;

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

	//Turns position/orientation into a view transform
	inline mat4 lookat(
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

	//Spherical linear interpolation between two non-normalized floats by t
	inline f32 slerp(
		f32 a,
		f32 b,
		f32 t)
	{
		if (a == b) return a;

		f32 dotAB = clamp(a * b, -1.0f, 1.0f);
		f32 theta = acos(dotAB) * t;

		f32 sinTheta = sin(theta);
		f32 w1 = sin((1.0f - t) * theta);
		f32 w2 = sin(t * theta);

		return (a * w1 + b * w2) / sinTheta;
	}
	//Spherical linear interpolation between two non-normalized vec2s by t
	inline vec2 slerp(
		const vec2 a,
		const vec2 b,
		f32 t)
	{
		vec2 na = normalize(a);
		vec2 nb = normalize(b);
		f32 dotAB = clamp(dot(na, nb), -1.0f, 1.0f);

		f32 theta = acos(dotAB) * t;
		vec2 rel = normalize(nb - na * dotAB);
		return na * cos(theta) + rel * sin(theta);
	}
	//Spherical linear interpolation between two non-normalized vec3s by t
	inline vec3 slerp(
		const vec3& a,
		const vec3& b,
		f32 t)
	{
		vec3 na = normalize(a);
		vec3 nb = normalize(b);
		f32 dotAB = clamp(dot(na, nb), -1.0f, 1.0f);

		f32 theta = acos(dotAB) * t;
		vec3 rel = normalize(nb - na * dotAB);
		return na * cos(theta) + rel * sin(theta);
	}
	//Spherical linear interpolation between two non-normalized vec4s by t
	inline vec4 slerp(
		const vec4& a,
		const vec4& b,
		f32 t)
	{
		vec4 na = normalize(a);
		vec4 nb = normalize(b);
		f32 dotAB = clamp(dot(na, nb), -1.0f, 1.0f);

		//ensure shortest path
		if (dotAB < 0.0f)
		{
			nb = nb * -1.0f;
			dotAB = -dotAB;
		}

		f32 theta = acos(dotAB);
		if (theta < epsilon) return na;

		f32 sinTheta = sin(theta);
		f32 w1 = sin((1.0f - t) * theta) / sinTheta;
		f32 w2 = sin(t * theta) / sinTheta;

		return normalize(na * w1 + nb * w2);
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

	//Uses std::sqrt and returns unit-accurate distance between two vec2s
	inline f32 distancesqrt(
		const vec2 a, 
		const vec2 b)
	{
		f32 dx = a.x - b.x;
		f32 dy = a.y - b.y;

		return sqrt(dx * dx + dy * dy);
	}
	//Uses std::sqrt and returns unit-accurate distance between two vec3s
	inline f32 distancesqrt(
		const vec3& a, 
		const vec3& b)
	{
		f32 dx = a.x - b.x;
		f32 dy = a.y - b.y;
		f32 dz = a.z - b.z;

		return sqrt(dx * dx + dy * dy + dz * dz);
	}
	//Uses std::sqrt and returns unit-accurate distance between two vec4s
	inline f32 distancesqrt(
		const vec4& a, 
		const vec4& b)
	{
		f32 dx = a.x - b.x;
		f32 dy = a.y - b.y;
		f32 dz = a.z - b.z;
		f32 dw = a.w - b.w;

		return sqrt(dx * dx + dy * dy + dz * dz + dw * dw);
	}

	//Does not use std::sqrt and returns squared distance between two vec2s
	inline f32 distancefast(
		const vec2 a, 
		const vec2 b)
	{
		f32 dx = a.x - b.x;
		f32 dy = a.y - b.y;

		return dx * dx + dy * dy;
	}
	//Does not use std::sqrt and returns squared distance between two vec3s
	inline f32 distancefast(
		const vec3& a, 
		const vec3& b)
	{
		f32 dx = a.x - b.x;
		f32 dy = a.y - b.y;
		f32 dz = a.z - b.z;

		return dx * dx + dy * dy + dz * dz;
	}
	//Does not use std::sqrt and returns squared distance between two vec4s
	inline f32 distancefast(
		const vec4& a, 
		const vec4& b)
	{
		f32 dx = a.x - b.x;
		f32 dy = a.y - b.y;
		f32 dz = a.z - b.z;
		f32 dw = a.w - b.w;

		return dx * dx + dy * dy + dz * dz + dw * dw;
	}

	inline vec3 reflect(
		const vec3& I, 
		const vec3& N)
	{
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
		f32 s = sin(half);

		return normalize(quat
			{
				na.x * s,
				na.y * s,
				na.z * s,
				cos(half)
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

	//Returns neutral matrix of a mat2 (no transform)
	constexpr mat2 identity2()
	{
		return
		{
			1.0f, 0.0f,
			0.0f, 1.0f
		};
	}
	//Returns neutral matrix of a mat3 (no transform)
	constexpr mat3 identity3()
	{
		return
		{
			1.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 1.0f
		};
	}
	//Returns neutral matrix of a mat4 (no transform)
	constexpr mat4 identity4()
	{
		return
		{
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};
	}

	constexpr quat identity_quat() { return { 0, 0, 0, 1 }; }
}