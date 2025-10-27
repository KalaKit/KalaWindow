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
//   - GLM-like containers as kvec2, kvec3, kvec4, kmat2, kmat3, kmat4, kquat (kvec4)
//   - operators and helpers for kvec, kmat and kquat types
//   - kmat containers as column-major and scalar form
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
	inline f32 safediv_a(
		f32 origin,
		f32 divisor)
	{
		const f32 safeDivisor = (fabsf(divisor) > epsilon) ? divisor : 1.0f;
		return origin / safeDivisor;
	}
	//Used for compound division and prevents division by 0, mutates origin instead of returning result
	inline constexpr void safediv_c(
		f32& origin,
		f32 divisor)
	{
		const f32 safeDivisor = (fabsf(divisor) > epsilon) ? divisor : 1.0f;
		origin /= safeDivisor;
	}

	//
	// KVEC
	//

	template<size_t N>
	struct kvec_storage;

	template<>
	struct kvec_storage<2> { f32 x{}, y{}; };
	template<>
	struct kvec_storage<3> { f32 x{}, y{}, z{}; };
	template<>
	struct kvec_storage<4> { f32 x{}, y{}, z{}, w{}; };

	template <size_t N>
	struct kvec : public kvec_storage<N>
	{
		static_assert(N >= 2 && N <= 4, "kvec can only have 2, 3, or 4 components.");

		kvec() = default;

		kvec(f32 _x)
			requires (N == 2)
			: kvec_storage<N>{ _x, _x } {}
		kvec(f32 _x, f32 _y)
			requires (N == 2)
			: kvec_storage<N>{ _x, _y } {}

		kvec(f32 _x)
			requires (N == 3)
			: kvec_storage<N>{ _x, _x, _x } {}
		kvec(f32 _x, f32 _y, f32 _z)
			requires (N == 3)
			: kvec_storage<N>{ _x, _y, _z } {}

		kvec(f32 _x)
			requires (N == 4)
			: kvec_storage<N>{ _x, _x, _x, _x } {}
		kvec(f32 _x, f32 _y, f32 _z, f32 _w)
			requires (N == 4)
			: kvec_storage<N>{ _x, _y, _z, _w } {}

		using storage = kvec_storage<N>;

		//
		// ARITHMETIC OPERATORS
		//

		kvec operator+(const kvec& v) const 
		{ 
			if constexpr (N == 2) return { this->x + v.x, this->y + v.y };
			if constexpr (N == 3) return { this->x + v.x, this->y + v.y, this->z + v.z };
			if constexpr (N == 4) return { this->x + v.x, this->y + v.y, this->z + v.z, this->w + v.w };
		}
		kvec operator+(f32 s) const
		{
			if constexpr (N == 2) return { this->x + s, this->y + s };
			if constexpr (N == 3) return { this->x + s, this->y + s, this->z + s };
			if constexpr (N == 4) return { this->x + s, this->y + s, this->z + s, this->w + s };
		}

		kvec operator-(const kvec& v) const 
		{ 
			if constexpr (N == 2) return { this->x - v.x, this->y - v.y };
			if constexpr (N == 3) return { this->x - v.x, this->y - v.y, this->z - v.z };
			if constexpr (N == 4) return { this->x - v.x, this->y - v.y, this->z - v.z, this->w - v.w };
		}
		kvec operator-(f32 s) const
		{
			if constexpr (N == 2) return { this->x - s, this->y - s };
			if constexpr (N == 3) return { this->x - s, this->y - s, this->z - s };
			if constexpr (N == 4) return { this->x - s, this->y - s, this->z - s, this->w - s };
		}

		kvec operator*(const kvec& v) const 
		{ 
			if constexpr (N == 2) return { this->x * v.x, this->y * v.y };
			if constexpr (N == 3) return { this->x * v.x, this->y * v.y, this->z * v.z };
			if constexpr (N == 4) return { this->x * v.x, this->y * v.y, this->z * v.z, this->w * v.w };
		}
		kvec operator*(f32 s) const
		{
			if constexpr (N == 2) return { this->x * s, this->y * s };
			if constexpr (N == 3) return { this->x * s, this->y * s, this->z * s };
			if constexpr (N == 4) return { this->x * s, this->y * s, this->z * s, this->w * s };
		}

		kvec operator/(const kvec& v) const
		{
			if constexpr (N == 2) return { safediv_a(this->x, v.x), safediv_a(this->y, v.y) };
			if constexpr (N == 3) return { safediv_a(this->x, v.x), safediv_a(this->y, v.y), safediv_a(this->z, v.z) };
			if constexpr (N == 4) return { safediv_a(this->x, v.x), safediv_a(this->y, v.y), safediv_a(this->z, v.z), safediv_a(this->w, v.w) };
		}
		kvec operator/(f32 s) const 
		{ 
			if constexpr (N == 2) return { safediv_a(this->x, s), safediv_a(this->y, s) };
			if constexpr (N == 3) return { safediv_a(this->x, s), safediv_a(this->y, s), safediv_a(this->z, s) };
			if constexpr (N == 4) return { safediv_a(this->x, s), safediv_a(this->y, s), safediv_a(this->z, s), safediv_a(this->w, s) };
		}

		kvec operator-() const 
		{ 
			if constexpr (N == 2) return { -this->x, -this->y };
			if constexpr (N == 3) return { -this->x, -this->y, -this->z };
			if constexpr (N == 4) return { -this->x, -this->y, -this->z, -this->w };
		}

		bool operator==(const kvec& v) const
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
		bool operator!=(const kvec& v) const { return !(*this == v); }

		//
		// COMPOUND OPERATORS
		//

		kvec& operator+=(const kvec& v) 
		{ 
			if constexpr (N == 2) { this->x += v.x; this->y += v.y; return *this; }
			if constexpr (N == 3) { this->x += v.x; this->y += v.y; this->z += v.z; return *this; }
			if constexpr (N == 4) { this->x += v.x; this->y += v.y; this->z += v.z; this->w += v.w; return *this; }
		}
		kvec& operator+=(f32 s)
		{
			if constexpr (N == 2) { this->x += s; this->y += s; return *this; }
			if constexpr (N == 3) { this->x += s; this->y += s; this->z += s; return *this; }
			if constexpr (N == 4) { this->x += s; this->y += s; this->z += s; this->w += s; return *this; }
		}

		kvec& operator-=(const kvec& v) 
		{ 
			if constexpr (N == 2) { this->x -= v.x; this->y -= v.y; return *this; }
			if constexpr (N == 3) { this->x -= v.x; this->y -= v.y; this->z -= v.z; return *this; }
			if constexpr (N == 4) { this->x -= v.x; this->y -= v.y; this->z -= v.z; this->w -= v.w; return *this; }
		}
		kvec& operator-=(f32 s)
		{
			if constexpr (N == 2) { this->x -= s; this->y -= s; return *this; }
			if constexpr (N == 3) { this->x -= s; this->y -= s; this->z -= s; return *this; }
			if constexpr (N == 4) { this->x -= s; this->y -= s; this->z -= s; this->w -= s; return *this; }
		}

		kvec& operator*=(const kvec& v)
		{
			if constexpr (N == 2) { this->x *= v.x; this->y *= v.y; return *this; }
			if constexpr (N == 3) { this->x *= v.x; this->y *= v.y; this->z *= v.z; return *this; }
			if constexpr (N == 4) { this->x *= v.x; this->y *= v.y; this->z *= v.z; this->w *= v.w; return *this; }
		}
		kvec& operator*=(f32 s)
		{
			if constexpr (N == 2) { this->x *= s; this->y *= s; return *this; }
			if constexpr (N == 3) { this->x *= s; this->y *= s; this->z *= s; return *this; }
			if constexpr (N == 4) { this->x *= s; this->y *= s; this->z *= s; this->w *= s; return *this; }
		}

		kvec& operator/=(const kvec& v) 
		{ 
			if constexpr (N == 2) { safediv_c(this->x, v.x); safediv_c(this->y, v.y); return *this; }
			if constexpr (N == 3) { safediv_c(this->x, v.x); safediv_c(this->y, v.y); safediv_c(this->z, v.z); return *this; }
			if constexpr (N == 4) { safediv_c(this->x, v.x); safediv_c(this->y, v.y); safediv_c(this->z, v.z); safediv_c(this->w, v.w); return *this; }
		}
		kvec& operator/=(f32 s) 
		{ 
			if constexpr (N == 2) { safediv_c(this->x, s); safediv_c(this->y, s); return *this; }
			if constexpr (N == 3) { safediv_c(this->x, s); safediv_c(this->y, s); safediv_c(this->z, s); return *this; }
			if constexpr (N == 4) { safediv_c(this->x, s); safediv_c(this->y, s); safediv_c(this->z, s); safediv_c(this->w, s); return *this; }
		}
	};

	template<typename F, size_t N>
	constexpr kvec<N> apply_scalar(const kvec<N>& v, f32 s, F func)
	{
		if constexpr (N == 2) return { func(v.x, s), func(v.y, s) };
		if constexpr (N == 3) return { func(v.x, s), func(v.y, s), func(v.z, s) };
		if constexpr (N == 4) return { func(v.x, s), func(v.y, s), func(v.z, s), func(v.w, s) };
	}

	template<typename F, size_t N>
	constexpr kvec<N> apply_scalar(const kvec<N>& v1, const kvec<N>& v2, F func)
	{
		if constexpr (N == 2) return { func(v1.x, v2.x), func(v1.y, v2.y) };
		if constexpr (N == 3) return { func(v1.x, v2.x), func(v1.y, v2.y), func(v1.z, v2.z) };
		if constexpr (N == 4) return { func(v1.x, v2.x), func(v1.y, v2.y), func(v1.z, v2.z), func(v1.w, v2.w) };
	}

	//add

	template<size_t N> 
		requires (N >= 2 && N <= 4)
	inline kvec<N> operator+(const kvec<N>& v, f32 s)
	{
		return apply_scalar(v, s, [](f32 a, f32 b) { return a + b; });
	}
	template<size_t N>
		requires (N >= 2 && N <= 4)
	inline kvec<N> operator+(f32 s, const kvec<N>& v)
	{
		return apply_scalar(v, s, [](f32 a, f32 b) { return a + b; });
	}
	template<size_t N>
		requires (N >= 2 && N <= 4)
	inline kvec<N> operator+(const kvec<N> v1, const kvec<N>& v2)
	{
		return apply_scalar(v1, v2, [](f32 a, f32 b) { return a + b; });
	}
	template<size_t N1, size_t N2>
		requires (N1 > N2 && N1 <= 4 && N2 >= 2)
	inline kvec<N1> operator+(const kvec<N1>& a, const kvec<N2>& b)
	{
		kvec<N1> r = a;

		r.x += b.x;
		r.y += b.y;
		if constexpr (N2 >= 3 && N1 >= 3) r.z += b.z;
		if constexpr (N2 == 4 && N1 == 4) r.w += b.w;

		return r;
	}

	template<size_t N>
		requires (N >= 2 && N <= 4)
	inline kvec<N>& operator+=(kvec<N>& v, f32 s)
	{
		v = apply_scalar(v, s, [](f32 a, f32 b) { return a + b; });
		return v;
	}
	template<size_t N>
		requires (N >= 2 && N <= 4)
	inline kvec<N>& operator+=(kvec<N>& v1, const kvec<N>& v2)
	{
		v1 = apply_scalar(v1, v2, [](f32 a, f32 b) { return a + b; });
		return v1;
	}
	template<size_t N1, size_t N2>
		requires (N1 > N2 && N1 <= 4 && N2 >= 2)
	inline kvec<N1>& operator+=(kvec<N1>& a, const kvec<N2>& b)
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
	inline kvec<N> operator-(const kvec<N>& v, f32 s)
	{
		return apply_scalar(v, s, [](f32 a, f32 b) { return a - b; });
	}
	template<size_t N>
		requires (N >= 2 && N <= 4)
	inline kvec<N> operator-(f32 s, const kvec<N>& v)
	{
		return apply_scalar(v, s, [](f32 a, f32 b) { return a - b; });
	}
	template<size_t N>
		requires (N >= 2 && N <= 4)
	inline kvec<N> operator-(const kvec<N> v1, const kvec<N>& v2)
	{
		return apply_scalar(v1, v2, [](f32 a, f32 b) { return a - b; });
	}
	template<size_t N1, size_t N2>
		requires (N1 > N2 && N1 <= 4 && N2 >= 2)
	inline kvec<N1> operator-(const kvec<N1>& a, const kvec<N2>& b)
	{
		kvec<N1> r = a;

		r.x -= b.x;
		r.y -= b.y;
		if constexpr (N2 >= 3 && N1 >= 3) r.z -= b.z;
		if constexpr (N2 == 4 && N1 == 4) r.w -= b.w;

		return r;
	}

	template<size_t N>
		requires (N >= 2 && N <= 4)
	inline kvec<N>& operator-=(kvec<N>& v, f32 s)
	{
		v = apply_scalar(v, s, [](f32 a, f32 b) { return a - b; });
		return v;
	}
	template<size_t N>
		requires (N >= 2 && N <= 4)
	inline kvec<N>& operator-=(kvec<N>& v1, const kvec<N>& v2)
	{
		v1 = apply_scalar(v1, v2, [](f32 a, f32 b) { return a - b; });
		return v1;
	}
	template<size_t N1, size_t N2>
		requires (N1 > N2 && N1 <= 4 && N2 >= 2)
	inline kvec<N1>& operator-=(kvec<N1>& a, const kvec<N2>& b)
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
	inline kvec<N> operator*(const kvec<N>& v, f32 s)
	{
		return apply_scalar(v, s, [](f32 a, f32 b) { return a * b; });
	}
	template<size_t N>
		requires (N >= 2 && N <= 4)
	inline kvec<N> operator*(f32 s, const kvec<N>& v)
	{
		return apply_scalar(v, s, [](f32 a, f32 b) { return a * b; });
	}
	template<size_t N>
		requires (N >= 2 && N <= 4)
	inline kvec<N> operator*(const kvec<N> v1, const kvec<N>& v2)
	{
		return apply_scalar(v1, v2, [](f32 a, f32 b) { return a * b; });
	}
	template<size_t N1, size_t N2>
		requires (N1 > N2 && N1 <= 4 && N2 >= 2)
	inline kvec<N1> operator*(const kvec<N1>& a, const kvec<N2>& b)
	{
		kvec<N1> r = a;

		r.x *= b.x;
		r.y *= b.y;
		if constexpr (N2 >= 3 && N1 >= 3) r.z *= b.z;
		if constexpr (N2 == 4 && N1 == 4) r.w *= b.w;

		return r;
	}

	template<size_t N>
		requires (N >= 2 && N <= 4)
	inline kvec<N>& operator*=(kvec<N>& v, f32 s)
	{
		v = apply_scalar(v, s, [](f32 a, f32 b) { return a * b; });
		return v;
	}
	template<size_t N>
		requires (N >= 2 && N <= 4)
	inline kvec<N>& operator*=(kvec<N>& v1, const kvec<N>& v2)
	{
		v1 = apply_scalar(v1, v2, [](f32 a, f32 b) { return a * b; });
		return v1;
	}
	template<size_t N1, size_t N2>
		requires (N1 > N2 && N1 <= 4 && N2 >= 2)
	inline kvec<N1>& operator*=(kvec<N1>& a, const kvec<N2>& b)
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
	inline kvec<N> operator/(const kvec<N>& v, f32 s)
	{
		return apply_scalar(v, s, [](f32 a, f32 b) { return safediv_a(a, b); });
	}
	template<size_t N>
		requires (N >= 2 && N <= 4)
	inline kvec<N> operator/(f32 s, const kvec<N>& v)
	{
		return apply_scalar(v, s, [](f32 a, f32 b) { return safediv_a(a, b); });
	}
	template<size_t N>
		requires (N >= 2 && N <= 4)
	inline kvec<N> operator/(const kvec<N> v1, const kvec<N>& v2)
	{
		return apply_scalar(v1, v2, [](f32 a, f32 b) { return safediv_a(a, b); });
	}
	template<size_t N1, size_t N2>
		requires (N1 > N2 && N1 <= 4 && N2 >= 2)
	inline kvec<N1> operator/(const kvec<N1>& a, const kvec<N2>& b)
	{
		kvec<N1> r = a;

		r.x = safediv_a(r.x, b.x);
		r.y = safediv_a(r.y, b.y);
		if constexpr (N2 >= 3 && N1 >= 3) r.z = safediv_a(r.z, b.z);
		if constexpr (N2 == 4 && N1 == 4) r.w = safediv_a(r.w, b.w);

		return r;
	}

	template<size_t N>
		requires (N >= 2 && N <= 4)
	inline kvec<N>& operator/=(kvec<N>& v, f32 s)
	{
		v = apply_scalar(v, s, [](f32 a, f32 b) { return safediv_c(a, b); });
		return v;
	}
	template<size_t N>
		requires (N >= 2 && N <= 4)
	inline kvec<N>& operator/=(kvec<N>& v1, const kvec<N>& v2)
	{
		v1 = apply_scalar(v1, v2, [](f32 a, f32 b) { return safediv_c(a, b); });
		return v1;
	}
	template<size_t N1, size_t N2>
		requires (N1 > N2 && N1 <= 4 && N2 >= 2)
	inline kvec<N1>& operator/=(kvec<N1>& a, const kvec<N2>& b)
	{
		safediv_c(a.x, b.x);
		safediv_c(a.y, b.y);
		if constexpr (N2 >= 3 && N1 >= 3) safediv_c(a.z, b.z);
		if constexpr (N2 == 4 && N1 == 4) safediv_c(a.w, b.w);

		return a;
	}

	//
	// KMAT
	//

	template<size_t N>
	struct kmat_storage;

	template<>
	struct kmat_storage<2>
	{
		f32 m00{}, m01{};
		f32 m10{}, m11{};
	};
	template<>
	struct kmat_storage<3>
	{
		f32 m00{}, m01{}, m02{};
		f32 m10{}, m11{}, m12{};
		f32 m20{}, m21{}, m22{};
	};
	template<>
	struct kmat_storage<4>
	{
		f32 m00{}, m01{}, m02{}, m03{};
		f32 m10{}, m11{}, m12{}, m13{};
		f32 m20{}, m21{}, m22{}, m23{};
		f32 m30{}, m31{}, m32{}, m33{};
	};

	template <size_t N>
	struct kmat : public kmat_storage<N>
	{
		static_assert(N >= 2 && N <= 4, "kmat can only have 2, 3, or 4 components.");

		kmat() = default;

		kmat(
			f32 _m)
			requires (N == 2)
			: kmat_storage<N>{ _m, _m, _m, _m } {}
		kmat(
			f32 _m00, f32 _m01,
			f32 _m10, f32 _m11)
			requires (N == 2)
			: kmat_storage<N>{ _m00, _m01, _m10, _m11 } {}

		kmat(
			f32 _m)
			requires (N == 3)
			: kmat_storage<N>
			{
				_m, _m, _m,
				_m, _m, _m,
				_m, _m, _m
			} {}
		kmat(
			f32 _m00, f32 _m01, f32 _m02,
			f32 _m10, f32 _m11, f32 _m12,
			f32 _m20, f32 _m21, f32 _m22)
			requires (N == 3)
			: kmat_storage<N>
			{
				_m00, _m01, _m02,
				_m10, _m11, _m12,
				_m20, _m21, _m22 
			} {}

		kmat(
			f32 _m)
			requires (N == 4)
			: kmat_storage<N>
			{
				_m, _m, _m, _m,
				_m, _m, _m, _m,
				_m, _m, _m, _m,
				_m, _m, _m, _m
			} {}
		kmat(
			f32 _m00, f32 _m01, f32 _m02, f32 _m03,
			f32 _m10, f32 _m11, f32 _m12, f32 _m13,
			f32 _m20, f32 _m21, f32 _m22, f32 _m23,
			f32 _m30, f32 _m31, f32 _m32, f32 _m33)
			requires (N == 4)
			: kmat_storage<N>
			{
				_m00, _m01, _m02, _m03,
				_m10, _m11, _m12, _m13,
				_m20, _m21, _m22, _m23,
				_m30, _m31, _m32, _m33
			} {}

		using storage = kmat_storage<N>;

		//
		// ARITHMETIC OPERATORS
		//

		kmat operator+(const kmat& m) const
		{
			kmat r = *this;
			r += m;

			return r;
		}
		kmat operator+(f32 s) const
		{
			kmat r = *this;
			r += s;

			return r;
		}

		kmat operator-(const kmat& m) const
		{
			kmat r = *this;
			r -= m;

			return r;
		}
		kmat operator-(f32 s) const
		{
			kmat r = *this;
			r -= s;

			return r;
		}

		kmat operator*(const kmat& m) const
		{
			kmat r = *this;
			r *= m;

			return r;
		}
		kmat operator*(f32 s) const
		{
			kmat r = *this;
			r *= s;

			return r;
		}

		kmat operator/(const kmat& m) const { return *this * inverse(m); }
		kmat operator/(f32 s) const 
		{ 
			kmat r = *this;
			r /= s;

			return r;
		}

		kmat operator-() const 
		{ 
			kmat r = *this; 
			r *= -1.0f; 

			return r; 
		}

		bool operator==(const kmat& m) const
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
		bool operator!=(const kmat& m) const { return !(*this == m); }

		//
		// COMPOUND OPERATORS
		//

		kmat& operator+=(const kmat& m)
		{
			if constexpr (N == 2)
			{
				this->m00 += m.m00; this->m01 += m.m01;
				this->m10 += m.m10; this->m11 += m.m11;
			}
			if constexpr (N == 3)
			{
				this->m00 += m.m00; this->m01 += m.m01; this->m02 += m.m02;
				this->m10 += m.m10; this->m11 += m.m11; this->m12+= m.m12;
				this->m20 += m.m20; this->m21 += m.m21; this->m22 += m.m22;
			}
			if constexpr (N == 4)
			{
				this->m00 += m.m00; this->m01 += m.m01; this->m02 += m.m02; this->m03 += m.m03;
				this->m10 += m.m10; this->m11 += m.m11; this->m12 += m.m12; this->m13 += m.m13;
				this->m20 += m.m20; this->m21 += m.m21; this->m22 += m.m22; this->m23 += m.m23;
				this->m30 += m.m30; this->m31 += m.m31; this->m32 += m.m32; this->m33 += m.m33;
			}

			return *this;
		}
		kmat& operator+=(f32 s)
		{
			if constexpr (N == 2)
			{
				this->m00 += s; this->m01 += s;
				this->m10 += s; this->m11 += s;
			}
			if constexpr (N == 3)
			{
				this->m00 += s; this->m01 += s; this->m02 += s;
				this->m10 += s; this->m11 += s; this->m12 += s;
				this->m20 += s; this->m21 += s; this->m22 += s;
			}
			if constexpr (N == 4)
			{
				this->m00 += s; this->m01 += s; this->m02 += s; this->m03 += s;
				this->m10 += s; this->m11 += s; this->m12 += s; this->m13 += s;
				this->m20 += s; this->m21 += s; this->m22 += s; this->m23 += s;
				this->m30 += s; this->m31 += s; this->m32 += s; this->m33 += s;
			}

			return *this;
		}

		kmat& operator-=(const kmat& m)
		{
			if constexpr (N == 2)
			{
				this->m00 -= m.m00; this->m01 -= m.m01;
				this->m10 -= m.m10; this->m11 -= m.m11;
			}
			if constexpr (N == 3)
			{
				this->m00 -= m.m00; this->m01 -= m.m01; this->m02 -= m.m02;
				this->m10 -= m.m10; this->m11 -= m.m11; this->m12 -= m.m12;
				this->m20 -= m.m20; this->m21 -= m.m21; this->m22 -= m.m22;
			}
			if constexpr (N == 4)
			{
				this->m00 -= m.m00; this->m01 -= m.m01; this->m02 -= m.m02; this->m03 -= m.m03;
				this->m10 -= m.m10; this->m11 -= m.m11; this->m12 -= m.m12; this->m13 -= m.m13;
				this->m20 -= m.m20; this->m21 -= m.m21; this->m22 -= m.m22; this->m23 -= m.m23;
				this->m30 -= m.m30; this->m31 -= m.m31; this->m32 -= m.m32; this->m33 -= m.m33;
			}

			return *this;
		}
		kmat& operator-=(f32 s)
		{
			if constexpr (N == 2)
			{
				this->m00 -= s; this->m01 -= s;
				this->m10 -= s; this->m11 -= s;
			}
			if constexpr (N == 3)
			{
				this->m00 -= s; this->m01 -= s; this->m02 -= s;
				this->m10 -= s; this->m11 -= s; this->m12 -= s;
				this->m20 -= s; this->m21 -= s; this->m22 -= s;
			}
			if constexpr (N == 4)
			{
				this->m00 -= s; this->m01 -= s; this->m02 -= s; this->m03 -= s;
				this->m10 -= s; this->m11 -= s; this->m12 -= s; this->m13 -= s;
				this->m20 -= s; this->m21 -= s; this->m22 -= s; this->m23 -= s;
				this->m30 -= s; this->m31 -= s; this->m32 -= s; this->m33 -= s;
			}

			return *this;
		}

		kmat& operator*=(const kmat& m)
		{
			if constexpr (N == 2)
			{
				const f32 a00 = this->m00, a01 = this->m01;
				const f32 a10 = this->m10, a11 = this->m11;

				this->m00 = a00 * m.m00 + a10 * m.m01;
				this->m01 = a01 * m.m00 + a11 * m.m01;

				this->m10 = a00 * m.m10 + a10 * m.m11;
				this->m11 = a01 * m.m10 + a11 * m.m11;
			}
			if constexpr (N == 3)
			{
				const f32 a00 = this->m00, a01 = this->m01, a02 = this->m02;
				const f32 a10 = this->m10, a11 = this->m11, a12 = this->m12;
				const f32 a20 = this->m20, a21 = this->m21, a22 = this->m22;

				this->m00 = a00 * m.m00 + a01 * m.m10 + a02 * m.m20;
				this->m01 = a00 * m.m01 + a01 * m.m11 + a02 * m.m21;
				this->m02 = a00 * m.m02 + a01 * m.m12 + a02 * m.m22;

				this->m10 = a10 * m.m00 + a11 * m.m10 + a12 * m.m20;
				this->m11 = a10 * m.m01 + a11 * m.m11 + a12 * m.m21;
				this->m12 = a10 * m.m02 + a11 * m.m12 + a12 * m.m22;

				this->m20 = a20 * m.m00 + a21 * m.m10 + a22 * m.m20;
				this->m21 = a20 * m.m01 + a21 * m.m11 + a22 * m.m21;
				this->m22 = a20 * m.m02 + a21 * m.m12 + a22 * m.m22;
			}
			if constexpr (N == 4)
			{
				const f32 a00 = this->m00, a01 = this->m01, a02 = this->m02, a03 = this->m03;
				const f32 a10 = this->m10, a11 = this->m11, a12 = this->m12, a13 = this->m13;
				const f32 a20 = this->m20, a21 = this->m21, a22 = this->m22, a23 = this->m23;
				const f32 a30 = this->m30, a31 = this->m31, a32 = this->m32, a33 = this->m33;

				this->m00 = a00 * m.m00 + a01 * m.m10 + a02 * m.m20 + a03 * m.m30;
				this->m01 = a00 * m.m01 + a01 * m.m11 + a02 * m.m21 + a03 * m.m31;
				this->m02 = a00 * m.m02 + a01 * m.m12 + a02 * m.m22 + a03 * m.m32;
				this->m03 = a00 * m.m03 + a01 * m.m13 + a02 * m.m23 + a03 * m.m33;

				this->m10 = a10 * m.m00 + a11 * m.m10 + a12 * m.m20 + a13 * m.m30;
				this->m11 = a10 * m.m01 + a11 * m.m11 + a12 * m.m21 + a13 * m.m31;
				this->m12 = a10 * m.m02 + a11 * m.m12 + a12 * m.m22 + a13 * m.m32;
				this->m13 = a10 * m.m03 + a11 * m.m13 + a12 * m.m23 + a13 * m.m33;

				this->m20 = a20 * m.m00 + a21 * m.m10 + a22 * m.m20 + a23 * m.m30;
				this->m21 = a20 * m.m01 + a21 * m.m11 + a22 * m.m21 + a23 * m.m31;
				this->m22 = a20 * m.m02 + a21 * m.m12 + a22 * m.m22 + a23 * m.m32;
				this->m23 = a20 * m.m03 + a21 * m.m13 + a22 * m.m23 + a23 * m.m33;

				this->m30 = a30 * m.m00 + a31 * m.m10 + a32 * m.m20 + a33 * m.m30;
				this->m31 = a30 * m.m01 + a31 * m.m11 + a32 * m.m21 + a33 * m.m31;
				this->m32 = a30 * m.m02 + a31 * m.m12 + a32 * m.m22 + a33 * m.m32;
				this->m33 = a30 * m.m03 + a31 * m.m13 + a32 * m.m23 + a33 * m.m33;
			}

			return *this;
		}
		kmat& operator*=(f32 s)
		{
			if constexpr (N == 2)
			{
				this->m00 *= s; this->m01 *= s;
				this->m10 *= s; this->m11 *= s;
			}
			if constexpr (N == 3)
			{
				this->m00 *= s; this->m01 *= s; this->m02 *= s;
				this->m10 *= s; this->m11 *= s; this->m12 *= s;
				this->m20 *= s; this->m21 *= s; this->m22 *= s;
			}
			if constexpr (N == 4)
			{
				this->m00 *= s; this->m01 *= s; this->m02 *= s; this->m03 *= s;
				this->m10 *= s; this->m11 *= s; this->m12 *= s; this->m13 *= s;
				this->m20 *= s; this->m21 *= s; this->m22 *= s; this->m23 *= s;
				this->m30 *= s; this->m31 *= s; this->m32 *= s; this->m33 *= s;
			}

			return *this;
		}

		kmat& operator/=(const kmat& m) 
		{ 
			*this = *this * inverse(m);
			return *this;
		}
		kmat& operator/=(f32 s)
		{
			if constexpr (N == 2)
			{
				this->m00 = safediv_c(this->m00, s); this->m01 = safediv_c(this->m01, s);
				this->m10 = safediv_c(this->m10, s); this->m11 = safediv_c(this->m11, s);
			}
			if constexpr (N == 3)
			{
				this->m00 = safediv_c(this->m00, s); this->m01 = safediv_c(this->m01, s); this->m02 = safediv_c(this->m02, s);
				this->m10 = safediv_c(this->m10, s); this->m11 = safediv_c(this->m11, s); this->m12 = safediv_c(this->m12, s);
				this->m20 = safediv_c(this->m20, s); this->m21 = safediv_c(this->m21, s); this->m22 = safediv_c(this->m22, s);
			}
			if constexpr (N == 4)
			{
				this->m00 = safediv_c(this->m00, s); this->m01 = safediv_c(this->m01, s); this->m02 = safediv_c(this->m02, s); this->m03 = safediv_c(this->m03, s);
				this->m10 = safediv_c(this->m10, s); this->m11 = safediv_c(this->m11, s); this->m12 = safediv_c(this->m12, s); this->m13 = safediv_c(this->m13, s);
				this->m20 = safediv_c(this->m20, s); this->m21 = safediv_c(this->m21, s); this->m22 = safediv_c(this->m22, s); this->m23 = safediv_c(this->m23, s);
				this->m30 = safediv_c(this->m30, s); this->m31 = safediv_c(this->m31, s); this->m32 = safediv_c(this->m32, s); this->m33 = safediv_c(this->m33, s);
			}

			return *this;
		}
	};

	//multiply kmat by same kvec

	template<size_t N>
		requires(N >= 2 && N <= 4)
	constexpr kvec<N> operator*(const kmat<N>& m, const kvec<N>& v)
	{
		if constexpr (N == 2)
			return 
			{
				m.m00 * v.x + m.m01 * v.y,
				m.m10 * v.x + m.m11 * v.y
			};
		if constexpr (N == 3)
			return 
			{
				m.m00 * v.x + m.m01 * v.y + m.m02 * v.z,
				m.m10 * v.x + m.m11 * v.y + m.m12 * v.z,
				m.m20 * v.x + m.m21 * v.y + m.m22 * v.z
			};
		if constexpr (N == 4)
			return 
			{
				m.m00 * v.x + m.m01 * v.y + m.m02 * v.z + m.m03 * v.w,
				m.m10 * v.x + m.m11 * v.y + m.m12 * v.z + m.m13 * v.w,
				m.m20 * v.x + m.m21 * v.y + m.m22 * v.z + m.m23 * v.w,
				m.m30 * v.x + m.m31 * v.y + m.m32 * v.z + m.m33 * v.w
			};
	}

	using kvec2 = kvec<2>; //Vector: x, y
	using kvec3 = kvec<3>; //Vector: x, y, z
	using kvec4 = kvec<4>; //Vector: x, y, z, w

	using kquat = kvec<4>; //Quaternion: x, y, z, w

	using kmat2 = kmat<2>; //Matrix: m00 - m11
	using kmat3 = kmat<3>; //Matrix: m00 - m22
	using kmat4 = kmat<4>; //Matrix: m00 - m33

	const kvec3 GRAVITY = { 0.0f, -9.81f, 0.0f };

	const kvec3 DIR_RIGHT = { 1, 0,  0 };
	const kvec3 DIR_UP    = { 0, 1,  0 };
	const kvec3 DIR_FRONT = { 0, 0, -1 };

	const kvec3 ROT_PITCH = { 1, 0, 0 };
	const kvec3 ROT_YAW   = { 0, 1, 0 };
	const kvec3 ROT_ROLL  = { 0, 0, 1 };

	//
	// HELPER FUNCTIONS USING DEFINED KMAT AND KVEC CONTAINERS
	//

	//Convert degrees to radians
	inline f32 radians(f32 deg) { return deg * 0.017453f; }
	//Convert degrees to radians
	inline kvec2 radians(const kvec2 v)
	{
		return { radians(v.x), radians(v.y) };
	}
	//Convert degrees to radians
	inline kvec3 radians(const kvec3& v)
	{
		return { radians(v.x), radians(v.y), radians(v.z) };
	}
	//Convert degrees to radians
	inline kvec4 radians(const kvec4& v)
	{
		return { radians(v.x), radians(v.y), radians(v.z), radians(v.w) };
	}

	//Convert radians to degrees
	inline f32 degrees(f32 rad) { return rad * 57.295780f; }
	//Convert radians to degrees
	inline kvec2 degrees(const kvec2 v)
	{
		return { degrees(v.x), degrees(v.y) };
	}
	//Convert radians to degrees
	inline kvec3 degrees(const kvec3& v)
	{
		return { degrees(v.x), degrees(v.y), degrees(v.z) };
	}
	//Convert radians to degrees
	inline kvec4 degrees(const kvec4& v)
	{
		return { degrees(v.x), degrees(v.y), degrees(v.z), degrees(v.w) };
	}

	//Converts 2D euler (degrees) to quaternion
	inline kquat toquat(const kvec2 euler)
	{
		f32 half = radians(euler.y * 0.5f);
		f32 cz = cos(half);
		f32 sz = sin(half);

		//rotation around Z axis
		return { 0.0f, 0.0f, sz, cz };
	}
	//Converts 3D euler (degrees) to quaternion
	inline kquat toquat(const kvec3& euler)
	{
		kvec3 r = radians(euler) * 0.5f;

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

	//Converts kquat to 2D euler (degrees)
	inline kvec2 toeuler2(const kquat& q)
	{
		f32 yaw = atan2(2.0f * (q.w * q.z), 1.0f - 2.0f * (q.z * q.z));
		return kvec2(0.0f, degrees(yaw));
	}
	//Converts kquat to 3D euler (degrees)
	inline kvec3 toeuler3(const kquat& q)
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

		return degrees(kvec3{ pitch, yaw, roll });
	}

	//Converts kmat3 to quat
	inline kquat toquat(const kmat3& m)
	{
		f32 trace = m.m00 + m.m11 + m.m22;
		if (trace > 0.0f)
		{
			f32 s = 0.5f / sqrt(trace + 1.0f);
			return 
			{
				(m.m21 - m.m12) * s,
				(m.m02 - m.m20) * s,
				(m.m10 - m.m01) * s,
				0.25f / s
			};
		}
		else if (m.m00 > m.m11 && m.m00 > m.m22)
		{
			f32 s = 2.0f * sqrt(1.0f + m.m00 - m.m11 - m.m22);
			return 
			{
				0.25f * s,
				(m.m01 + m.m10) / s,
				(m.m02 + m.m20) / s,
				(m.m21 - m.m12) / s
			};
		}
		else if (m.m11 > m.m22)
		{
			f32 s = 2.0f * sqrt(1.0f + m.m11 - m.m00 - m.m22);
			return 
			{
				(m.m01 + m.m10) / s,
				0.25f * s,
				(m.m12 + m.m21) / s,
				(m.m02 - m.m20) / s
			};
		}
		else
		{
			f32 s = 2.0f * sqrt(1.0f + m.m22 - m.m00 - m.m11);
			return 
			{
				(m.m02 + m.m20) / s,
				(m.m12 + m.m21) / s,
				0.25f * s,
				(m.m10 - m.m01) / s
			};
		}
	}
	//Converts kmat4 to quat
	inline kquat toquat(const kmat4& m)
	{
		f32 trace = m.m00 + m.m11 + m.m22;
		if (trace > 0.0f)
		{
			f32 s = 0.5f / sqrt(trace + 1.0f);
			return 
			{
				(m.m21 - m.m12) * s,
				(m.m02 - m.m20) * s,
				(m.m10 - m.m01) * s,
				0.25f / s
			};
		}
		else if (m.m00 > m.m11 && m.m00 > m.m22)
		{
			f32 s = 2.0f * sqrt(1.0f + m.m00 - m.m11 - m.m22);
			return 
			{
				0.25f * s,
				(m.m01 + m.m10) / s,
				(m.m02 + m.m20) / s,
				(m.m21 - m.m12) / s
			};
		}
		else if (m.m11 > m.m22)
		{
			f32 s = 2.0f * sqrt(1.0f + m.m11 - m.m00 - m.m22);
			return 
			{
				(m.m01 + m.m10) / s,
				0.25f * s,
				(m.m12 + m.m21) / s,
				(m.m02 - m.m20) / s
			};
		}
		else
		{
			f32 s = 2.0f * sqrt(1.0f + m.m22 - m.m00 - m.m11);
			return 
			{
				(m.m02 + m.m20) / s,
				(m.m12 + m.m21) / s,
				0.25f * s,
				(m.m10 - m.m01) / s
			};
		}
	}

	//Converts kquat to kmat3
	inline kmat3 tomat3(const kquat& q)
	{
		f32 x2 = q.x + q.x;
		f32 y2 = q.y + q.y;
		f32 z2 = q.z + q.z;

		f32 xx = q.x * x2;
		f32 yy = q.y * y2;
		f32 zz = q.z * z2;
		f32 xy = q.x * y2;
		f32 xz = q.x * z2;
		f32 yz = q.y * z2;
		f32 wx = q.w * x2;
		f32 wy = q.w * y2;
		f32 wz = q.w * z2;

		return
		{
			1.0f - (yy + zz), xy + wz,           xz - wy,
			xy - wz,          1.0f - (xx + zz),  yz + wx,
			xz + wy,          yz - wx,           1.0f - (xx + yy)
		};
	}
	//Converts kquat to kmat4
	inline kmat4 tomat4(const kquat& q)
	{
		kmat3 r = tomat3(q);
		return {
			r.m00, r.m01, r.m02, 0.0f,
			r.m10, r.m11, r.m12, 0.0f,
			r.m20, r.m21, r.m22, 0.0f,
			0.0f,  0.0f,  0.0f,  1.0f
		};
	}

	//Wraps a rotation axis between 0 to 360 degrees
	inline f32 wrap(f32 deg)
	{
		deg = fmod(deg, 360.0f);
		if (deg < 0.0f) deg += 360.0f;

		return deg;
	}

	//ortographic projection, top-left origin, Y-down projection
	inline kmat4 ortho(const kvec2 viewport)
	{
		f32 left = 0.0f;
		f32 right = viewport.x;
		f32 bottom = viewport.y;
		f32 top = 0.0f;

		f32 zNear = 0.1f;
		f32 zFar = 100.0f;

		f32 rl = right - left;
		f32 tb = top - bottom;

		return
		{
			2.0f / rl,  0.0f,         0.0f,                  -(right + left) / rl,
			0.0f,      -(2.0f / tb),  0.0f,                  -(top + bottom) / tb,
			0.0f,       0.0f,        -2.0f / (zFar - zNear), -(zFar + zNear) / (zFar - zNear),
			0.0f,       0.0f,         0.0f,                   1.0f
		};
	}

	//perpective projection, bottom-left origin, Y-up projection
	inline kmat4 perspective(
		const kvec2 viewport,
		f32 fovDeg,
		f32 zNear,
		f32 zFar)
	{
		f32 aspect = viewport.x / viewport.y;
		f32 f = 1.0f / tan(radians(fovDeg) * 0.5f);
		f32 fn = zFar - zNear;

		return
		{
			f / aspect, 0.0f,  0.0f,                 0.0f,
			0.0f,       f,     0.0f,                 0.0f,
			0.0f,       0.0f, -(zFar + zNear) / fn, -(2.0f * zFar * zNear) / fn,
			0.0f,       0.0f, -1.0f,                 0.0f
		};
	}

	inline kmat3 translate(
		const kmat3& m,
		const kvec2 v)
	{
		kmat3 t = m;
		t.m20 += v.x;
		t.m21 += v.y;

		return t;
	}
	inline kmat4 translate(
		const kmat4& m,
		const kvec3& v)
	{
		kmat4 t = m;
		t.m30 += v.x;
		t.m31 += v.y;
		t.m32 += v.z;

		return t;
	}

	inline kmat2 rotate(f32 deg)
	{
		f32 r = radians(deg);
		f32 c = cos(r);
		f32 s = sin(r);

		return { c, -s, s, c };
	}

	inline kmat3 rotate(
		const kmat3& m,
		f32 deg,
		const kvec3& axis)
	{
		f32 rad = radians(deg);
		const f32 c = cos(rad);
		const f32 s = sin(rad);
		const f32 ic = 1.0f - c;

		const f32
			x = axis.x,
			y = axis.y,
			z = axis.z;

		kmat3 r =
		{
			c + x * x * ic,  x * y * ic - z * s,  x * z * ic + y * s,
			y * x * ic + z * s,   c + y * y * ic, y * z * ic - x * s,
			z * x * ic - y * s,   z * y * ic + x * s,  c + z * z * ic
		};

		return m * r;
	}
	inline kmat3 rotate(
		const kmat3& m,
		f32 deg)
	{
		f32 rad = radians(deg);
		const f32 c = cos(rad);
		const f32 s = sin(rad);

		kmat3 r = {
			c, -s, 0.0f,
			s,  c, 0.0f,
			0.0f, 0.0f, 1.0f
		};

		return m * r;
	}

	inline kmat4 rotate(
		const kmat4& m,
		f32 deg,
		const kvec3& axis)
	{
		f32 rad = radians(deg);
		const f32 c = cos(rad);
		const f32 s = sin(rad);
		const f32 ic = 1.0f - c;

		const f32
			x = axis.x,
			y = axis.y,
			z = axis.z;

		kmat4 r =
		{
			c + x * x * ic,  x * y * ic - z * s,  x * z * ic + y * s,  0.0f,
			y * x * ic + z * s,   c + y * y * ic, y * z * ic - x * s,  0.0f,
			z * x * ic - y * s,   z * y * ic + x * s,  c + z * z * ic, 0.0f,
			0.0f,                 0.0f,                0.0f,                1.0f
		};

		return m * r;
	}

	inline kmat3 scale(
		const kmat3& m,
		const kvec2 v)
	{
		kmat3 s = m;
		s.m00 *= v.x;
		s.m11 *= v.y;

		return s;
	}
	inline kmat4 scale(
		const kmat4& m,
		const kvec3& v)
	{
		kmat4 s = m;
		s.m00 *= v.x;
		s.m11 *= v.y;
		s.m22 *= v.z;

		return s;
	}

	//Computes kvec2 magnitude (distance from a)
	inline f32 length(const kvec2 v)
	{
		return sqrt(
			v.x * v.x
			+ v.y * v.y);
	}
	//Computes kvec3 magnitude (distance from a)
	inline f32 length(const kvec3& v)
	{
		return sqrt(
			v.x * v.x
			+ v.y * v.y
			+ v.z * v.z);
	}
	//Computes kvec4 magnitude (distance from a)
	inline f32 length(const kvec4& v)
	{
		return sqrt(
			v.x * v.x
			+ v.y * v.y
			+ v.z * v.z
			+ v.w * v.w);
	}

	//Returns unit-length version of kvec2
	inline kvec2 normalize(const kvec2 v)
	{
		f32 len = length(v);
		return (len != 0.0f) ? v / len : kvec2{};
	}
	//Returns unit-length version of kvec3
	inline kvec3 normalize(const kvec3& v)
	{
		f32 len = length(v);
		return (len != 0.0f) ? v / len : kvec3{};
	}
	//Returns unit-length version of kvec4
	inline kvec4 normalize(const kvec4& v)
	{
		f32 len = length(v);
		return (len != 0.0f) ? v / len : kvec4{};
	}
	//Normalizes a quaternion
	inline kquat normalize_quat(const kquat& q)
	{
		f32 len = sqrt(
			q.x * q.x
			+ q.y * q.y
			+ q.z * q.z
			+ q.w * q.w);

		return q / len;
	}

	//Vector pointing from one position to another
	inline kvec2 direction(
		const kvec2 a, 
		const kvec2 b)
	{
		return normalize(b - a);
	}
	//Vector pointing from one position to another
	inline kvec3 direction(
		const kvec3& a, 
		const kvec3& b)
	{
		return normalize(b - a);
	}

	//Measures alignment between two kvec2s
	inline f32 dot(
		const kvec2 a,
		const kvec2 b)
	{
		return
			a.x * b.x
			+ a.y * b.y;
	}
	//Measures alignment between two kvec3s
	inline f32 dot(
		const kvec3& a,
		const kvec3& b)
	{
		return
			a.x * b.x
			+ a.y * b.y
			+ a.z * b.z;
	}
	//Measures alignment between two kvec4s
	inline f32 dot(
		const kvec4& a,
		const kvec4& b)
	{
		return
			a.x * b.x
			+ a.y * b.y
			+ a.z * b.z
			+ a.w * b.w;
	}

	//Returns the perpendicular magnitude in 2D
	inline f32 cross(
		const kvec2 a,
		const kvec2 b)
	{
		return
			a.x * b.y
			- a.y * b.x;
	}
	//Returns the normal kvec3 in 3D
	inline kvec3 cross(
		const kvec3& a,
		const kvec3& b)
	{
		return kvec3
		{
			a.y * b.z - a.z * b.y,
			a.z * b.x - a.x * b.z,
			a.x * b.y - a.y * b.x
		};
	}

	//Turns position/orientation into a view transform
	inline kmat4 lookat(
		const kvec3& origin, 
		const kvec3& target, 
		const kvec3& up)
	{
		kvec3 f = normalize(target - origin);
		kvec3 s = normalize(cross(f, up));
		kvec3 u = cross(s, f);

		kmat4 m(1.0f);
		m.m00 = s.x; m.m01 = u.x; m.m02 = -f.x;
		m.m10 = s.y; m.m11 = u.y; m.m12 = -f.y;
		m.m20 = s.z; m.m21 = u.z; m.m22 = -f.z;

		m.m30 = -dot(s, origin);
		m.m31 = -dot(u, origin);
		m.m32 =  dot(f, origin);

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
	//Linear interpolation between two kvec2s by t
	inline kvec2 lerp(
		const kvec2 a,
		const kvec2 b,
		f32 t)
	{
		return
		{
			lerp(a.x, b.x, t),
			lerp(a.y, b.y, t)
		};
	}
	//Linear interpolation between two kvec3s by t
	inline kvec3 lerp(
		const kvec3& a,
		const kvec3& b,
		f32 t)
	{
		return
		{
			lerp(a.x, b.x, t),
			lerp(a.y, b.y, t),
			lerp(a.z, b.z, t)
		};
	}
	//Linear interpolation between two kvec4s by t
	inline kvec4 lerp(
		const kvec4& a,
		const kvec4& b,
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
	//Spherical linear interpolation between two non-normalized kvec2s by t
	inline kvec2 slerp(
		const kvec2 a,
		const kvec2 b,
		f32 t)
	{
		kvec2 na = normalize(a);
		kvec2 nb = normalize(b);
		f32 dotAB = clamp(dot(na, nb), -1.0f, 1.0f);

		f32 theta = acos(dotAB) * t;
		kvec2 rel = normalize(nb - na * dotAB);
		return na * cos(theta) + rel * sin(theta);
	}
	//Spherical linear interpolation between two non-normalized kvec3s by t
	inline kvec3 slerp(
		const kvec3& a,
		const kvec3& b,
		f32 t)
	{
		kvec3 na = normalize(a);
		kvec3 nb = normalize(b);
		f32 dotAB = clamp(dot(na, nb), -1.0f, 1.0f);

		f32 theta = acos(dotAB) * t;
		kvec3 rel = normalize(nb - na * dotAB);
		return na * cos(theta) + rel * sin(theta);
	}
	//Spherical linear interpolation between two non-normalized kvec4s by t
	inline kvec4 slerp(
		const kvec4& a,
		const kvec4& b,
		f32 t)
	{
		kvec4 na = normalize(a);
		kvec4 nb = normalize(b);
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

	inline kvec2 smoothstep(
		const kvec2 edge0,
		const kvec2 edge1,
		const kvec2 x)
	{
		return
		{
			smoothstep(edge0.x, edge1.x, x.x),
			smoothstep(edge0.y, edge1.y, x.y)
		};
	}
	inline kvec3 smoothstep(
		const kvec3& edge0,
		const kvec3& edge1,
		const kvec3& x)
	{
		return
		{
			smoothstep(edge0.x, edge1.x, x.x),
			smoothstep(edge0.y, edge1.y, x.y),
			smoothstep(edge0.z, edge1.z, x.z)
		};
	}
	inline kvec4 smoothstep(
		const kvec4& edge0,
		const kvec4& edge1,
		const kvec4& x)
	{
		return
		{
			smoothstep(edge0.x, edge1.x, x.x),
			smoothstep(edge0.y, edge1.y, x.y),
			smoothstep(edge0.z, edge1.z, x.z),
			smoothstep(edge0.w, edge1.w, x.w)
		};
	}

	//Restricts a kvec2 to given ranges
	inline kvec2 kclamp(
		const kvec2 v,
		const kvec2 min,
		const kvec2 max)
	{
		return
		{
			clamp(v.x, min.x, max.x),
			clamp(v.y, min.y, max.y)
		};
	}
	//Restricts a kvec3 to given ranges
	inline kvec3 kclamp(
		const kvec3& v,
		const kvec3& min,
		const kvec3& max)
	{
		return
		{
			clamp(v.x, min.x, max.x),
			clamp(v.y, min.y, max.y),
			clamp(v.z, min.z, max.z)
		};
	}
	//Restricts a kvec4 to given ranges
	inline kvec4 kclamp(
		const kvec4& v,
		const kvec4& min,
		const kvec4& max)
	{
		return
		{
			clamp(v.x, min.x, max.x),
			clamp(v.y, min.y, max.y),
			clamp(v.z, min.z, max.z),
			clamp(v.w, min.w, max.w)
		};
	}

	//Uses std::sqrt and returns unit-accurate distance between two kvec2s
	inline f32 distancesqrt(
		const kvec2 a, 
		const kvec2 b)
	{
		f32 dx = a.x - b.x;
		f32 dy = a.y - b.y;

		return sqrt(dx * dx + dy * dy);
	}
	//Uses std::sqrt and returns unit-accurate distance between two kvec3s
	inline f32 distancesqrt(
		const kvec3& a, 
		const kvec3& b)
	{
		f32 dx = a.x - b.x;
		f32 dy = a.y - b.y;
		f32 dz = a.z - b.z;

		return sqrt(dx * dx + dy * dy + dz * dz);
	}
	//Uses std::sqrt and returns unit-accurate distance between two kvec4s
	inline f32 distancesqrt(
		const kvec4& a, 
		const kvec4& b)
	{
		f32 dx = a.x - b.x;
		f32 dy = a.y - b.y;
		f32 dz = a.z - b.z;
		f32 dw = a.w - b.w;

		return sqrt(dx * dx + dy * dy + dz * dz + dw * dw);
	}

	//Does not use std::sqrt and returns squared distance between two kvec2s
	inline f32 distancefast(
		const kvec2 a, 
		const kvec2 b)
	{
		f32 dx = a.x - b.x;
		f32 dy = a.y - b.y;

		return dx * dx + dy * dy;
	}
	//Does not use std::sqrt and returns squared distance between two kvec3s
	inline f32 distancefast(
		const kvec3& a, 
		const kvec3& b)
	{
		f32 dx = a.x - b.x;
		f32 dy = a.y - b.y;
		f32 dz = a.z - b.z;

		return dx * dx + dy * dy + dz * dz;
	}
	//Does not use std::sqrt and returns squared distance between two kvec4s
	inline f32 distancefast(
		const kvec4& a, 
		const kvec4& b)
	{
		f32 dx = a.x - b.x;
		f32 dy = a.y - b.y;
		f32 dz = a.z - b.z;
		f32 dw = a.w - b.w;

		return dx * dx + dy * dy + dz * dz + dw * dw;
	}

	inline kvec3 reflect(
		const kvec3& I, 
		const kvec3& N)
	{
		return I - 2.0f * dot(I, N) * N;
	}

	//Takes in non-normalized positions and returns angle in degrees (0..180),
	//uses kvec2 a as the reference direction
	inline f32 angle(
		const kvec2 a,
		const kvec2 b)
	{
		kvec2 na = normalize(a);
		kvec2 nb = normalize(b);

		f32 d = clamp(dot(na, nb), -1.0f, 1.0f);
		return degrees(acos(d));
	}
	//Takes in non-normalized positions and returns angle in degrees (0..180),
	//uses kvec3 a as the reference direction
	inline f32 angle(
		const kvec3& a,
		const kvec3& b,
		const kvec3& axis)
	{
		kvec3 na = normalize(a);
		kvec3 nb = normalize(b);
		kvec3 ax = normalize(axis);

		//project onto plane perpendicular to the axis
		na -= ax * dot(na, ax);
		nb -= ax * dot(nb, ax);

		na = normalize(na);
		nb = normalize(nb);

		f32 d = clamp(dot(na, nb), -1.0f, 1.0f);
		return degrees(acos(d));
	}

	//Takes in non-normalized positions and returns signed angle in degrees (-180..180),
	//uses kvec2 a as the reference direction
	inline f32 angle_s(
		const kvec2 a,
		const kvec2 b)
	{
		kvec2 na = normalize(a);
		kvec2 nb = normalize(b);

		f32 rad = atan2(nb.y, nb.x) - atan2(na.y, na.x);
		f32 deg = degrees(rad);

		if (deg > 180.0f) deg -= 360.0f;
		if (deg < -180.0f) deg += 360.0f;

		return deg;
	}
	//Takes in non-normalized positions and returns signed angle in degrees (-180..180),
	//uses kvec3 a as the reference direction
	inline f32 angle_s(
		const kvec3& a,
		const kvec3& b,
		const kvec3& axis)
	{
		kvec3 na = normalize(a);
		kvec3 nb = normalize(b);
		kvec3 ax = normalize(axis);

		//project onto plane perpendicular to the axis
		na -= ax * dot(na, ax);
		nb -= ax * dot(nb, ax);

		na = normalize(na);
		nb = normalize(nb);

		f32 rad = atan2(dot(cross(na, nb), ax), dot(na, nb));
		return degrees(rad);
	}

	//Takes in non-normalized positions and returns full angle in degrees (0..360),
	//uses kvec2 a as the reference direction
	inline f32 angle_f(
		const kvec2 a,
		const kvec2 b)
	{
		kvec2 na = normalize(a);
		kvec2 nb = normalize(b);

		f32 rad = atan2(nb.y, nb.x) - atan2(na.y, na.x);
		f32 deg = degrees(rad);

		if (deg < 0.0f) deg += 360.0f;

		return deg;
	}
	//Takes in non-normalized positions and returns full angle in degrees (0..360),
	//uses kvec3 a as the reference direction
	inline f32 angle_f(
		const kvec3& a,
		const kvec3& b,
		const kvec3& axis)
	{
		f32 deg = angle_s(a, b, axis);
		if (deg < 0.0f) deg += 360.0f;
		return deg;
	}

	//Takes in a non-normalized axis and returns a quaternion that rotates around angle and axis
	inline kquat angleaxis(
		f32 angle, 
		const kvec3& axis)
	{
		kvec3 na = normalize(axis);
		f32 half = angle * 0.5f;
		f32 s = sin(half);

		return normalize(kquat
			{
				na.x * s,
				na.y * s,
				na.z * s,
				cos(half)
			});
	}

	//Projects kvec2 a onto kvec2 b
	inline kvec2 project(
		const kvec2 a, 
		const kvec2 b)
	{
		return (dot(a, b) / dot(b, b)) * b;
	}
	//Projects kvec3 a onto kvec3 b
	inline kvec3 project(
		const kvec3& a, 
		const kvec3& b)
	{
		return (dot(a, b) / dot(b, b)) * b;
	}

	//Returns neutral matrix of a kmat2 (no transform)
	inline kmat2 identity2()
	{
		return
		{
			1.0f, 0.0f,
			0.0f, 1.0f
		};
	}
	//Returns neutral matrix of a kmat3 (no transform)
	inline kmat3 identity3()
	{
		return
		{
			1.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 1.0f
		};
	}
	//Returns neutral matrix of a kmat4 (no transform)
	inline kmat4 identity4()
	{
		return
		{
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};
	}

	inline kquat identity_quat() { return { 0, 0, 0, 1 }; }
}