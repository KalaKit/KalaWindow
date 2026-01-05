//------------------------------------------------------------------------------
// color_utils.hpp
//
// Copyright (C) 2026 Lost Empire Entertainment
//
// This is free source code, and you are welcome to redistribute it under certain conditions.
// Read LICENSE.md for more information.
//
// Provides:
//   - custom color container as a variable for linear RGBA operations
//   - color conversion, color operators
//------------------------------------------------------------------------------

#include <algorithm>

using std::clamp;
using std::min;
using std::max;

//8-bit unsigned int
//Min: 0
//Max: 255
using u8 = uint8_t;

//32-bit unsigned int
//Min: 0
//Max: 4,294,967,295
using u32 = uint32_t;

//32-bit float
//6 decimal precision
using f32 = float;

//static_cast
#ifndef scast
	#define scast static_cast
#endif

namespace KalaHeaders::KalaColor
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
	// COLOR
	//
	//============================================================================

	//Linear RGBA color variable, alpha channel defaults to 1.0f,
	//accepts overflow below 0.0f and above 1.0 on all channels
	struct color
	{
		f32 r{}, g{}, b{}, a = 1.0f;

		constexpr color() = default;

		//single float that fills RGB and leaves A untouched
		constexpr color(f32 _r)
			: r(_r), g(_r), b(_r) {}

		//three floats that fill RGB and leave A untouched
		constexpr color(f32 _r, f32 _g, f32 _b)
			: r(_r), g(_g), b(_b) {}
		//four floats that fill RGBA
		constexpr color(f32 _r, f32 _g, f32 _b, f32 _a)
			: r(_r), g(_g), b(_b), a(_a) {}

		//three-float array that only fills RGB and leaves A untouched
		constexpr color(const f32(&_f)[3])
			: r(_f[0]), g(_f[1]), b(_f[2]) {}
		//four-float array that fills RGBA
		constexpr color(const f32(&_f)[4])
			: r(_f[0]), g(_f[1]), b(_f[2]), a(_f[3]) {}
		//three-float array that fills RGB + float that fills A
		constexpr color(const f32(&_fa)[3], f32 _fb)
			: r(_fa[0]), g(_fa[1]), b(_fa[2]), a(_fb) {}

		//============================================================================
		//
		// COLOR ARITHMETIC OPERATORS
		//
		//============================================================================

		constexpr color operator+(f32 s) const
		{
			return { r + s, g + s, b + s };
		}
		constexpr color operator+(const color& c) const
		{
			return { r + c.r, g + c.g, b + c.b, a + c.a };
		}

		constexpr color operator-(f32 s) const
		{
			return { r - s, g - s, b - s };
		}
		constexpr color operator-(const color& c) const
		{
			return { r - c.r, g - c.g, b - c.b, a - c.a };
		}

		constexpr color operator*(f32 s) const
		{
			return { r * s, g * s, b * s };
		}
		constexpr color operator*(const color& c) const
		{
			return { r * c.r, g * c.g, b * c.b, a * c.a };
		}

		constexpr color operator/(f32 s) const
		{
			return
			{
				safediv_a(r, s),
				safediv_a(g, s),
				safediv_a(b, s)
			};
		}
		constexpr color operator/(const color& c) const
		{
			return
			{
				safediv_a(r, c.r),
				safediv_a(g, c.g),
				safediv_a(b, c.b),
				safediv_a(a, c.a)
			};
		}

		constexpr bool operator<(f32 s) const
		{
			return
				r < s
				&& g < s
				&& b < s;
		}
		constexpr bool operator<(const color& c) const
		{
			return
				r < c.r
				&& g < c.g
				&& b < c.b
				&& a < c.a;
		}

		constexpr bool operator>(f32 s) const
		{
			return
				r > s
				&& g > s
				&& b > s;
		}
		constexpr bool operator>(const color& c) const
		{
			return
				r > c.r
				&& g > c.g
				&& b > c.b
				&& a > c.a;
		}

		bool operator==(const color& c) const
		{
			return
				fabsf(r - c.r) <= epsilon
				&& fabsf(g - c.g) <= epsilon
				&& fabsf(b - c.b) <= epsilon
				&& fabsf(a - c.a) <= epsilon;
		}

		bool operator!=(const color& c) const { return !(*this == c); }

		constexpr color operator-() const { return { -r, -g, -b, a }; }

		//============================================================================
		//
		// COLOR COMPOUND OPERATORS
		//
		//============================================================================

		constexpr color& operator+=(f32 s)
		{
			r += s, g += s, b += s;
			return *this;
		}
		constexpr color& operator+=(const color& c)
		{
			r += c.r, g += c.g, b += c.b, a += c.a;
			return *this;
		}

		constexpr color& operator-=(f32 s)
		{
			r -= s, g -= s, b -= s;
			return *this;
		}
		constexpr color& operator-=(const color& c)
		{
			r -= c.r, g -= c.g, b -= c.b, a -= c.a;
			return *this;
		}

		constexpr color& operator*=(f32 s)
		{
			r *= s, g *= s, b *= s;
			return *this;
		}
		constexpr color& operator*=(const color& c)
		{
			r *= c.r, g *= c.g, b *= c.b, a *= c.a;
			return *this;
		}

		constexpr color& operator/=(f32 s)
		{
			safediv_c(r, s), safediv_c(g, s), safediv_c(b, s);
			return *this;
		}
		constexpr color& operator/=(const color& c)
		{
			safediv_c(r, c.r), safediv_c(g, c.g), safediv_c(b, c.b), safediv_c(a, c.a);
			return *this;
		}

		bool operator<=(f32 s) const { return { r <= s && g <= s && b <= s && a <= s }; }
		bool operator<=(const color& c) const { return { r <= c.r && g <= c.g && b <= c.b && a <= c.a }; }

		bool operator>=(f32 s) const { return { r >= s && g >= s && b >= s && a >= s }; }
		bool operator>=(const color& c) const { return { r >= c.r && g >= c.g && b >= c.b && a >= c.a }; }
	};

	template<typename F>
	constexpr color apply_scalar(const color& c, f32 s, F func)
	{
		return { func(c.r, s), func(c.g, s), func(c.b, s) };
	}

	//add

	constexpr color operator+(f32 s, const color& c)
	{
		return apply_scalar(c, s, [](f32 a, f32 b) { return a + b; });
	}

	//subtract

	constexpr color operator-(f32 s, const color& c)
	{
		return apply_scalar(c, s, [](f32 a, f32 b) { return a - b; });
	}

	//multiply

	constexpr color operator*(f32 s, const color& c)
	{
		return apply_scalar(c, s, [](f32 a, f32 b) { return a * b; });
	}

	//divide

	constexpr color operator/(f32 s, const color& c)
	{
		return apply_scalar(c, s, [](f32 a, f32 b) { return safediv_a(a, b); });
	}

	//Returns range-normalized float
	inline f32 normalize_r(f32 f)
	{
		bool isNormalized =
			f >= -epsilon &&
			f <= 1.0f + epsilon;

		return isNormalized
			? f
			: clamp(f, 0.0f, 1.0f);
	}

	//Returns range-normalized color
	inline color normalize_r(const color& c)
	{
		return color(
			normalize_r(c.r),
			normalize_r(c.g),
			normalize_r(c.b),
			normalize_r(c.a));
	}

	//Returns true if float a is close to float b within epsilon range
	inline bool isnear(f32 a, f32 b = {})
	{
		return fabsf(a - b) <= epsilon;
	}

	//Returns true if color a is close to color b within epsilon range
	inline bool isnear(const color& a, const color& b = {})
	{
		return fabsf(a.r - b.r) <= epsilon
			&& fabsf(a.g - b.g) <= epsilon
			&& fabsf(a.b - b.b) <= epsilon
			&& fabsf(a.a - b.a) <= epsilon;
	}

	//============================================================================
	//
	// CONVERT COLOR
	//
	//============================================================================
	
	enum class ColorConvertType : u8
	{
		COLOR_NONE = 0, //default, wont do any conversion
		
		COLOR_SRGB_TO_LINEAR = 1, //converts directly from srgb to linear
		COLOR_LINEAR_TO_SRGB = 2, //converts directly from linear to srgb
		
		//srgb conversion
		
		COLOR_HSL_TO_HSV = 3, //converts directly from hsl to hsv
		COLOR_HSV_TO_HSL = 4, //converts directly from hsv to hsl
		
		COLOR_SRGB_TO_HSV       = 5,
		COLOR_SRGB_TO_HSL       = 6,
		COLOR_SRGB_TO_RGB8      = 7,
		COLOR_SRGB_TO_CMYK      = 8,
		
		COLOR_HSV_TO_SRGB       = 10,
		COLOR_HSL_TO_SRGB       = 11,
		COLOR_RGB8_TO_SRGB      = 12,
		COLOR_CMYK_TO_SRGB      = 13,
		
		COLOR_SRGB_TO_PREMULTIPLIED   = 15,
		COLOR_SRGB_FROM_PREMULTIPLIED = 16,
		
		//linear conversion
		
		COLOR_XYZ_TO_LAB = 17, //converts directly from xyz to lab
		COLOR_LAB_TO_XYZ = 18, //converts directly from lab to xyz
		
		COLOR_OKLAB_TO_OKLCH = 19, //converts directly from oklab to oklch
		COLOR_OKLCH_TO_OKLAB = 20, //converts directly from oklch to oklab
		
		COLOR_LINEAR_TO_XYZ   = 21,
		COLOR_LINEAR_TO_LAB   = 22,
		COLOR_LINEAR_TO_OKLAB = 23,
		COLOR_LINEAR_TO_OKLCH = 24,
		
		COLOR_XYZ_TO_LINEAR   = 25,
		COLOR_LAB_TO_LINEAR   = 26,
		COLOR_OKLAB_TO_LINEAR = 27,
		COLOR_OKLCH_TO_LINEAR = 28
	};
	
	//Converts input color to returned color with chosen ColorConvertType
	//  x = R,
	//  y = G,
	//  z = B,
	//  w = A
	inline color convert_color(
		ColorConvertType type,
		const color& c)
	{		
		bool canNormalize = 
			type == ColorConvertType::COLOR_SRGB_TO_LINEAR
			|| type == ColorConvertType::COLOR_SRGB_TO_HSV
			|| type == ColorConvertType::COLOR_SRGB_TO_HSL
			|| type == ColorConvertType::COLOR_SRGB_TO_RGB8
			|| type == ColorConvertType::COLOR_SRGB_TO_CMYK;
			
		//always range-normalize if non-linear
		color nc = canNormalize
			? normalize_r(c)
			: c;
		
		auto to_linear = [](f32 c) -> f32
			{
				if (c <= 0.04045f) return c / 12.92f;
				return powf((c + 0.055f) / 1.055f, 2.4f);
			};
			
		auto to_srgb = [](f32 c) -> f32
			{
				if (c <= 0.0031308f) return c * 12.92f;
				return 1.055f * powf(c, 1.0f / 2.4f) - 0.055f;
			};
		
		auto SRGB_TO_LINEAR = [&]() -> color
			{
				return color(
					to_linear(nc.r),
					to_linear(nc.g),
					to_linear(nc.b),
					nc.a);
			};
		auto LINEAR_TO_SRGB = [&]() -> color
			{
				return color(
					to_srgb(nc.r),
					to_srgb(nc.g),
					to_srgb(nc.b),
					nc.a);
			};
			
		auto HSL_TO_HSV = [&]() -> color
			{
				f32 H = nc.r;
				f32 S = nc.g;
				f32 L = nc.b;
				
				//convert HSL to HSV
				
				f32 V = L + S * min(L, 1.0f - L);
				
				//saturation
				
				f32 S_v{};
				if (V > epsilon) S_v = (2.0f * (1.0f - L / V));
				
				return color(H, S_v, V, nc.a);
			};
		auto HSV_TO_HSL = [&]() -> color
			{
				f32 H = nc.r;
				f32 S = nc.g;
				f32 V = nc.b;
				
				//lightness
				
				f32 L = V * (1.0f - S * 0.5f);
				
				//saturation
				
				f32 S_l{};
				if (L > epsilon
					&& L < 1.0f - epsilon)
				{
					S_l = (V - L) / min(L, 1.0f - L);	
				}
				
				return color(H, S_l, L, nc.a);
			};
	
		auto SRGB_TO_HSV = [&]() -> color
			{
				f32 maxc = max(nc.r, max(nc.g, nc.b));
				f32 minc = min(nc.r, min(nc.g, nc.b));
				f32 delta = maxc - minc;
				
				f32 h{};
				f32 s{};
				f32 v = maxc;
				
				if (delta > epsilon)
				{
					//saturation
					
					if (maxc > 0.0f) s = delta / maxc;
					
					//hue
					
					if (isnear(maxc, nc.r)) h = (nc.g - nc.b) / delta;
					else if (isnear(maxc, nc.g)) h = 2.0f + (nc.b - nc.r) / delta;
					else h = 4.0f + (nc.r - nc.g) / delta;
					
					h /= 6.0f;
					if (h < 0.0f) h += 1.0f;
				}
				
				return color(h, s, v, nc.a);
			};
		auto SRGB_TO_HSL = [&]() -> color
			{
				f32 maxc = max(nc.r, max(nc.g, nc.b));
				f32 minc = min(nc.r, min(nc.g, nc.b));
				f32 delta = maxc - minc;
				
				f32 h{};
				f32 s{};
				f32 l = 0.5f * (maxc + minc);
				
				if (delta <= epsilon) return color(0, 0, l, nc.a);
				else
				{
					//saturation
					
					if (l < 0.5f) s = delta / (maxc + minc);
					else s = delta / (2.0f - maxc - minc);
					
					//hue
					
					if (isnear(maxc, nc.r)) h = (nc.g - nc.b) / delta;
					else if (isnear(maxc, nc.g)) h = 2.0f + (nc.b - nc.r) / delta;
					else h = 4.0f + (nc.r - nc.g) / delta;
					
					h /= 6.0f;
					if (h < 0.0f) h += 1.0f;
				}
				
				return color(h, s, l, nc.a);
			};
		auto SRGB_TO_RGB8 = [&]() -> color
			{
				auto to_rgb8 = [](f32 c) -> f32
					{
						return clamp(c * 255.0f, 0.0f, 255.0f);
					};
					
				return color(
					to_rgb8(nc.r),
					to_rgb8(nc.g),
					to_rgb8(nc.b),
					to_rgb8(nc.a));
			};
		auto SRGB_TO_CMYK = [&]() -> color
			{
				f32 k = 1.0f - max(nc.r, max(nc.g, nc.b));
				
				//pure black shortcut
				if (k >= 1.0f - epsilon) return color{};
				
				f32 c = (1.0f - nc.r - k) / (1.0f - k);
				f32 m = (1.0f - nc.g - k) / (1.0f - k);
				f32 y = (1.0f - nc.b - k) / (1.0f - k);
				
				//r = C, g = M, b = Y, a = K
				return color(c, m, y, k);
			};
			
		auto HSV_TO_SRGB = [&]() -> color
			{
				f32 h = nc.r;
				f32 s = nc.g;
				f32 v = nc.b;
				
				if (s <= epsilon) return color(v, v, v, nc.a);
				
				h = fmodf(h, 1.0f) * 6.0f;
				f32 i = floorf(h);
				f32 f = h - i;
				
				f32 p = v * (1.0f - s);
				f32 q = v * (1.0f - s * f);
				f32 t = v * (1.0f - s * (1.0f - f));
				
				f32 R{};
				f32 G{};
				f32 B{};
				
				switch (scast<int>(i))
				{
					default:
					case 0: R = v; G = t; B = p; break;
					case 1: R = q; G = v; B = p; break;
					case 2: R = p; G = v; B = t; break;
					case 3: R = p; G = q; B = v; break;
					case 4: R = t; G = p; B = v; break;
					case 5: R = v; G = p; B = q; break;
				}
				
				return color(R, G, B, nc.a);
			};
		auto HSL_TO_SRGB = [&]() -> color
			{
				f32 h = nc.r;
				f32 s = nc.g;
				f32 l = nc.b;
				
				f32 R{};
				f32 G{};
				f32 B{};
				
				if (s <= epsilon) R = G = B = l;
				else
				{
					auto hue_to_srgb = [](f32 p, f32 q, f32 t) -> f32
						{
							if (t < 0.0f) t += 1.0f;
							if (t > 1.0f) t -= 1.0f;
							
							if (t < 1.0f / 6.0f) return p + (q - p) * 6.0f * t;
							if (t < 1.0f / 2.0f) return q;
							if (t < 2.0f / 3.0f) return p + (q - p) * (2.0f / 3.0f - t) * 6.0f;
							
							return p;
						};
						
					f32 q = (l < 0.5f)
						? (l * (1.0f + s))
						: (l + s - l * s);
						
					f32 p = 2.0f * l - q;
					
					R = hue_to_srgb(p, q, h + 1.0f / 3.0f);
					G = hue_to_srgb(p, q, h);
					B = hue_to_srgb(p, q, h - 1.0f / 3.0f);
				}
				
				return color(R, G, B, nc.a);
			};
		auto RGB8_TO_SRGB = [&]() -> color
		{ 
			return color(
				nc.r / 255.0f,
				nc.g / 255.0f,
				nc.b / 255.0f,
				nc.a);
		};
		auto CMYK_TO_SRGB = [&]() -> color
			{
				f32 C = nc.r;
				f32 M = nc.g;
				f32 Y = nc.b;
				f32 K = nc.a;
				
				//true black shortcut
				if (K >= 1.0f - epsilon) return color{};
				
				f32 R = (1.0f - C) * (1.0f - K);
				f32 G = (1.0f - M) * (1.0f - K);
				f32 B = (1.0f - Y) * (1.0f - K);
				
				//alpha as 1.0f because CMYK has no alpha channel
				return color(R, G, B, 1.0f);
			};
			
		auto SRGB_TO_PREMULTIPLIED = [&]() -> color
			{
				return color(nc.r * nc.a, nc.g * nc.a, nc.b * nc.a, nc.a);
			};
		auto SRGB_FROM_PREMULTIPLIED = [&]() -> color
			{
				//completely transparent, no color information
				if (nc.a <= epsilon) return color(0.0f);
				
				return color(nc.r / nc.a, nc.g / nc.a, nc.b / nc.a, nc.a);
			};
			
		//linear conversion
			
		auto XYZ_TO_LAB = [&](
			const color& inColor = color(),
			bool useOriginal = true) -> color
			{
				//reference white (D65)
				
				constexpr f32 Xn = 0.95047f;
				constexpr f32 Yn = 1.00000f;
				constexpr f32 Zn = 1.08883f;
				
				auto f = [](f32 t) -> f32
					{
						constexpr f32 eps = 0.008856f;
						constexpr f32 k = 7.787f;    //116 / (3 * eps^(2/3))
						constexpr f32 c = 0.137931f; //16.0 / 116.0
						
						return (t > eps)
							? cbrtf(t)
							: (k * t) + c;
					};
					
				f32 X = nc.r;
				f32 Y = nc.g;
				f32 Z = nc.b;
				f32 W = nc.a;
				
				if (!useOriginal)
				{
					X = inColor.r;
					Y = inColor.g;
					Z = inColor.b;
					W = inColor.a;
				}
					
				f32 xr = X / Xn;
				f32 yr = Y / Yn;
				f32 zr = Z / Zn;
				
				f32 fx = f(xr);
				f32 fy = f(yr);
				f32 fz = f(zr);
				
				f32 L = (116.0f * fy) - 16.0f;
				f32 A = 500.0f * (fx - fy);
				f32 B = 200.0f * (fy - fz);
				
				return color(L, A, B, W);
			};
		auto LAB_TO_XYZ = [&]() -> color
			{
				//reference white (D65)
				
				constexpr f32 Xn = 0.95047f;
				constexpr f32 Yn = 1.00000f;
				constexpr f32 Zn = 1.08883f;
				
				f32 L = nc.r;
				f32 A = nc.g;
				f32 B = nc.b;
				
				auto finv = [](f32 t) -> f32
					{
						constexpr f32 eps = 0.008856f;
						constexpr f32 k = 7.787f;    //116 / (3 * eps^(2/3))
						constexpr f32 c = 0.137931f; //16.0 / 116.0
						
						f32 t3 = t * t * t;
						return (t3 > eps)
							? t3
							: (t - c) / k;
					};
					
				f32 fy = (L + 16.0f) / 116.0f;
				f32 fx = fy + (A / 500.0f);
				f32 fz = fy - (B / 200.0f);
				
				f32 xr = finv(fx);
				f32 yr = finv(fy);
				f32 zr = finv(fz);
				
				return color(
					xr * Xn,
					yr * Yn,
					zr * Zn,
					nc.a);
			};
			
		auto OKLAB_TO_OKLCH = [&]() -> color
			{
				f32 L = nc.r;
				f32 A = nc.g;
				f32 Bc = nc.b;
				
				//chroma
				
				f32 C = sqrtf(A * A + Bc * Bc);
				
				//hue angle in radians
				
				f32 h = atan2f(Bc, A);
				
				//normalize hue to 0-1
				
				f32 h_norm = h / (2.0f * PI);
				if (h_norm < 0.0f) h_norm += 1.0f;
				
				return color(L, C, h_norm, nc.a);
			};
		auto OKLCH_TO_OKLAB = [&]() -> color
			{
				f32 L = nc.r;
				f32 C = nc.g;
				f32 h = nc.b; //normalized 0-1 hue
				
				//convert hue back to radians
				
				f32 angle = h * (2.0f * PI);
				
				//convert polar to cartesian
				
				f32 A = C * cosf(angle);
				f32 Bc = C * sinf(angle);
				
				return color(L, A, Bc, nc.a);
			};
			
		auto LINEAR_TO_XYZ = [&]() -> color
			{
				f32 X =
					0.4124564f * nc.r
					+ 0.3575761f * nc.g
					+ 0.1804375f * nc.b;

				f32 Y =
					0.2126729f * nc.r
					+ 0.7151522f * nc.g
					+ 0.0721750f * nc.b;

				f32 Z =
					0.0193339f * nc.r
					+ 0.1191920f * nc.g
					+ 0.9503041f * nc.b;

				return color(X, Y, Z, nc.a);
			};
		auto LINEAR_TO_LAB = [&]() -> color
			{
				return XYZ_TO_LAB(LINEAR_TO_XYZ(), false);
			};
		auto LINEAR_TO_OKLAB = [&](
			const color& inColor = color(),
			bool useOriginal = true) -> color
			{
				f32 R = useOriginal ? nc.r : inColor.r;
				f32 G = useOriginal ? nc.g : inColor.g;
				f32 B = useOriginal ? nc.b : inColor.b;
				f32 A_ = useOriginal ? nc.a : inColor.a;
				
				//convert linear to LMS
				
				f32 l = 0.4122214708f * R + 0.5363325363f * G + 0.0514459929f * B;
				f32 m = 0.2119034982f * R + 0.6806995451f * G + 0.1073969566f * B;
				f32 s = 0.0883024619f * R + 0.2817188376f * G + 0.6299787005f * B;
				
				//non-linear transform
				
				f32 l_ = cbrtf(l);
				f32 m_ = cbrtf(m);
				f32 s_ = cbrtf(s);
				
				//convert LMS to oklab
				
				f32 L = 0.2104542553f * l_ + 0.7936177850f * m_ - 0.0040720468f * s_;
				f32 A = 1.9779984951f * l_ - 2.4285922050f * m_ + 0.4505937099f * s_;
				f32 Bc = 0.0259040371f * l_ + 0.7827717662f * m_ - 0.8086757660f * s_;
				
				return color(L, A, Bc, A_);
			};
		auto LINEAR_TO_OKLCH = [&]() -> color
			{
				color oklab = LINEAR_TO_OKLAB(color(nc.r, nc.g, nc.b, nc.a), false);
				
				f32 L = oklab.r;
				f32 A = oklab.g;
				f32 Bc = oklab.b;
				f32 A_ = oklab.a;
				
				float C = sqrtf(A * A + Bc * Bc);
				float h = atan2f(Bc, A) / (2.0f * PI);
				if (h < 0.0f) h += 1.0f;
				
				return color(L, C, h, A_);
			};
			
		auto XYZ_TO_LINEAR = [&](
			const color& inColor = color(),
			bool useOriginal = true) -> color
			{ 
				f32 X = useOriginal ? nc.r : inColor.r;
				f32 Y = useOriginal ? nc.g : inColor.g;
				f32 Z = useOriginal ? nc.b : inColor.b;
				f32 W = useOriginal ? nc.a : inColor.a;

				f32 r =
					3.2404542f * X
					+ -1.5371385f * Y
					+ -0.4985314f * Z;

				f32 g =
					-0.9692660f * X
					+ 1.8760108f * Y
					+ 0.0415560f * Z;

				f32 b =
					0.0556434f * X
					+ -0.2040259f * Y
					+ 1.0572252f * Z;
			
				return color(r, g, b, W);
			};
		auto LAB_TO_LINEAR = [&]() -> color
			{
				return XYZ_TO_LINEAR(LAB_TO_XYZ(), false);
			};
		auto OKLAB_TO_LINEAR = [&](
			const color& inColor = color(),
			bool useOriginal = true) -> color
			{
				f32 L  = useOriginal ? nc.r : inColor.r;
				f32 A  = useOriginal ? nc.g : inColor.g;
				f32 Bc = useOriginal ? nc.b : inColor.b;
				f32 A_ = useOriginal ? nc.a : inColor.a;
				
				//convert oklab to LMS
				
				f32 l_ = L + 0.3963377774f * A + 0.2158037573f * Bc;
				f32 m_ = L - 0.1055613458f * A - 0.0638541728f * Bc;
				f32 s_ = L - 0.0894841775f * A - 1.2914855480f * Bc;
				
				//reverse non-linear
				
				f32 l = l_ * l_ * l_;
				f32 m = m_ * m_ * m_;
				f32 s = s_ * s_ * s_;
				
				//convert LMS to linear rgb
				
				f32 R_lin =  4.0767416621f * l - 3.3077115913f * m + 0.2309699292f * s;
				f32 G_lin = -1.2684380046f * l + 2.6097574011f * m - 0.3413193965f * s;
				f32 B_lin = -0.0041960863f * l - 0.7034186147f * m + 1.7076147010f * s;
				
				return color(
					R_lin,
					G_lin,
					B_lin,
					A_);
			};
		auto OKLCH_TO_LINEAR = [&]() -> color
			{
				f32 L = nc.r;
				f32 C = nc.g;
				f32 h = nc.b;
				f32 A_ = nc.a;
				
				//convert hue to radians
				
				f32 angle = h * (2.0f * PI);
				
				//convert polar to cartesian
				
				f32 A = C * cosf(angle);
				f32 Bc = C * sinf(angle);
				
				return OKLAB_TO_LINEAR(color(L, A, Bc, A_), false);
			};
			
		switch (type)
		{
		default:
		case ColorConvertType::COLOR_NONE: return c;
			
		case ColorConvertType::COLOR_SRGB_TO_LINEAR: return SRGB_TO_LINEAR();
		case ColorConvertType::COLOR_LINEAR_TO_SRGB: return LINEAR_TO_SRGB();
		
		//srgb conversion
		
		case ColorConvertType::COLOR_HSL_TO_HSV: return HSL_TO_HSV();
		case ColorConvertType::COLOR_HSV_TO_HSL: return HSV_TO_HSL();
		
		case ColorConvertType::COLOR_SRGB_TO_HSV:  return SRGB_TO_HSV();
		case ColorConvertType::COLOR_SRGB_TO_HSL:  return SRGB_TO_HSL();
		case ColorConvertType::COLOR_SRGB_TO_RGB8: return SRGB_TO_RGB8();
		case ColorConvertType::COLOR_SRGB_TO_CMYK: return SRGB_TO_CMYK();
		
		case ColorConvertType::COLOR_HSV_TO_SRGB:  return HSV_TO_SRGB();
		case ColorConvertType::COLOR_HSL_TO_SRGB:  return HSL_TO_SRGB();
		case ColorConvertType::COLOR_RGB8_TO_SRGB: return RGB8_TO_SRGB();
		case ColorConvertType::COLOR_CMYK_TO_SRGB: return CMYK_TO_SRGB();
		
		case ColorConvertType::COLOR_SRGB_TO_PREMULTIPLIED:   return SRGB_TO_PREMULTIPLIED();
		case ColorConvertType::COLOR_SRGB_FROM_PREMULTIPLIED: return SRGB_FROM_PREMULTIPLIED();
		
		//linear conversion
		
		case ColorConvertType::COLOR_XYZ_TO_LAB: return XYZ_TO_LAB();
		case ColorConvertType::COLOR_LAB_TO_XYZ: return LAB_TO_XYZ();
		
		case ColorConvertType::COLOR_OKLAB_TO_OKLCH: return OKLAB_TO_OKLCH();
		case ColorConvertType::COLOR_OKLCH_TO_OKLAB: return OKLCH_TO_OKLAB();
		
		case ColorConvertType::COLOR_XYZ_TO_LINEAR:   return XYZ_TO_LINEAR();
		case ColorConvertType::COLOR_LAB_TO_LINEAR:   return LAB_TO_LINEAR();
		case ColorConvertType::COLOR_OKLAB_TO_LINEAR: return OKLAB_TO_LINEAR();
		case ColorConvertType::COLOR_OKLCH_TO_LINEAR: return OKLCH_TO_LINEAR();
		
		case ColorConvertType::COLOR_LINEAR_TO_XYZ:   return LINEAR_TO_XYZ();
		case ColorConvertType::COLOR_LINEAR_TO_LAB:   return LINEAR_TO_LAB();
		case ColorConvertType::COLOR_LINEAR_TO_OKLAB: return LINEAR_TO_OKLAB();
		case ColorConvertType::COLOR_LINEAR_TO_OKLCH: return LINEAR_TO_OKLCH();
		}
	}
	
	//============================================================================
	//
	// COLOR OPERATORS
	//
	//============================================================================
	
	constexpr color XYZ_D65 =
	{
		0.95047f,
		1.0f,
		1.08883f
	};

	enum class ColorEncodeType : u8
	{
		//Nonlinear, gamma-encoded sRGB transfer function.
		//Values must be range-normalized before use.
		COLORENCODE_SRGB = 0,
		
		//Linear-light RGB (no gamma curve).
		//Scene-referred or linear SDR.
		//Values must not be range-normalized or gamma-decoded.
		COLORENCODE_LINEAR = 1
	};

	//Returns neutral color
	constexpr color identity_color() { return color{}; }
	//Returns true if color is true identity
	inline bool isidentity(const color& v) { return isnear(v); }

	//Brightens shadows and compresses highlights.
	//Works the same way on sRGB and linear.
	//Gamma value is clamped from 0.01 to 10.
	inline color gamma(
		const color& c,
		f32 gammaValue)
	{
		f32 clamped = clamp(gammaValue, 0.01f, 10.0f);
		
		f32 inv = 1.0f / clamped;

		return color(
			powf(c.r, inv),
			powf(c.g, inv),
			powf(c.b, inv),
			c.a);
	}
	//Darkens shadows and expands highlights.
	//Works the same way on sRGB and linear.
	//Gamma value is clamped from 0.01 to 10.
	inline color degamma(
		const color& c,
		f32 gammaValue)
	{
		f32 clamped = clamp(gammaValue, 0.01f, 10.0f);
		
		return color(
			powf(c.r, clamped),
			powf(c.g, clamped),
			powf(c.b, clamped),
			c.a);
	}
	
	//Adds or removes a constant value uniformly across RGB channels.
	//Works the same way on sRGB and linear.
	//  scale = 0 - unchanged
	//  scale < 0 - darker
	//  scale > 0 - brighter
	inline color brightness(
		const color& c,
		f32 scale)
	{
		return color(
			c.r + scale,
			c.g + scale,
			c.b + scale,
			c.a);
	}

	//
	// SRGB ONLY
	//

	//Makes the colors of a color channel blocky by forcing them into big chunky steps.
	//sRGB only - posterize is a display-quantization effect and has no meaningful linear equivalent.
	//Levels is clamped from 1 to 256, output is clamped from 0.0 to 1.0.
	inline f32 posterize(
		f32 colorChannel,
		u32 levels)
	{
		f32 k = clamp(levels, 1u, 256u);

		//always range-normalize up front
		f32 nc = normalize_r(colorChannel);

		return normalize_r(floorf(nc * k) / k);
	}

	//Increases saturation while protecting already-saturated colors.
	//sRGB only - vibrance is a perceptual look operator, not scene-referred color math.
	//Scale is clamped from 0 to 10.
	//  scale = 0 - unchanged
	//  scale > 0 - more vibrant
	inline color vibrance(
		const color& c,
		f32 scale)
	{
		f32 k = clamp(scale, 0.0f, 10.0f);
		
		//always range-normalize up front
		color nc = normalize_r(c);

		//convert to hsv

		color hsv = convert_color(ColorConvertType::COLOR_SRGB_TO_HSV, nc);

		//vibrance protects already-saturated colors:
		//higher effect for lower-saturation pixels

		f32 vibranceFactor = k * (1.0f - hsv.g);

		hsv.g = clamp(hsv.g + vibranceFactor, 0.0f, 1.0f);

		//back to rgb

		return normalize_r(convert_color(ColorConvertType::COLOR_HSV_TO_SRGB, hsv));
	}

	//Rotates hue in normalized units.
	//sRGB only - hue is a perceptual attribute and is undefined in linear light.
	//  shift = 0-1, where 1 wraps back to 0
	inline color hue_shift(
		const color& c,
		f32 shift)
	{
		//always range-normalize up front
		color nc = normalize_r(c);

		f32 h = shift - floorf(shift);

		//convert to hsv

		color hsv = convert_color(ColorConvertType::COLOR_SRGB_TO_HSV, nc);

		hsv.r = hsv.r + h;
		if (hsv.r >= 1.0f) hsv.r -= 1.0f;

		//back to rgb

		return normalize_r(convert_color(ColorConvertType::COLOR_HSV_TO_SRGB, hsv));
	}
	
	//Rotates hue in degrees instead of normalized units.
	//sRGB only - wrapper around hue_shift using degree-based input.
	//  degrees wraps automatically, 0-360 is one full rotation
	inline color hue_rotate(
		const color& c,
		f32 degrees)
	{
		f32 shift = degrees / 360.0f;
		return hue_shift(
			c,
			shift);
	}

	//
	// LINEAR ONLY
	//

	//Returns relative luminance from a linear RGB color.
	//Linear only - sRGB is display-referred and scene math should never be done in display space.
	inline f32 luminance(const color& c)
	{
		return
			c.r * 0.2126f
			+ c.g * 0.7152f
			+ c.b * 0.0722f;
	}

	//Controls perceived colorfulness using OKLCH chroma.
	//Linear only - sRGB is display-referred and scene math should never be done in display space.
	//Scale is clamped from 0.0 to 10.
	//  scale = 0 - grayscale,
	//  scale = 1 - unchanged,
	//  scale > 1 - increased saturation
	inline color saturation(
		const color& c,
		f32 scale)
	{
		f32 k = clamp(scale, 0.0f, 10.0f);

		color oklch = convert_color(ColorConvertType::COLOR_LINEAR_TO_OKLCH, c);

		//perceptual chroma scaling
		oklch.g *= k;

		return convert_color(
			ColorConvertType::COLOR_OKLCH_TO_LINEAR, oklch);
	}

	//Adjusts perceptual contrast by scaling OKLCH lightness.
	//Linear only - sRGB is display-referred and scene math should never be done in display space.
	//Scale is clamped from 0.0 to 10, pivot is clamped from 0.0 to 1.0.
	//  scale = 0 - collapses lightness to pivot
	//  scale = 1 - unchanged
	//  scale > 1 - increased contrast
	inline color contrast(
		const color& c,
		f32 scale,
		f32 pivot = 0.5f)
	{
		f32 k = clamp(scale, 0.0f, 10.0f);
		f32 p = clamp(pivot, 0.0f, 1.0f);

		color oklch = convert_color(ColorConvertType::COLOR_LINEAR_TO_OKLCH, c);

		//perceptual contract around target pivot
		oklch.r = p + (oklch.r - p) * k;

		return convert_color(
			ColorConvertType::COLOR_OKLCH_TO_LINEAR, oklch);
	}

	//Applies a linear exposure adjustment in EV scaling.
	//Linear only - sRGB is display-referred and scene math should never be done in display space.
	//EV is clamped from -10 to 10.
	inline color exposure(
		const color& c,
		f32 ev)
	{
		f32 clamped = clamp(ev, -10.0f, 10.0f);

		f32 s = powf(2.0f, clamped);

		return color(c.r * s, c.g * s, c.b * s, c.a);
	}
	
	//Applies a chromatic tint by scaling OKLab chroma, preserving perceptual brightness.
	//Linear only - sRGB is display-referred and scene math should never be done in display space.
	inline color tint(
		const color& c,
		const color& tint)
	{
		color lab = convert_color(
			ColorConvertType::COLOR_LINEAR_TO_OKLAB, c);

		//apply tint to chroma only
		lab.g *= tint.r;
		lab.b *= tint.g;

		return convert_color(
			ColorConvertType::COLOR_OKLAB_TO_LINEAR, lab);
	}

	enum class WhiteBalanceModel : u8
	{  
		WB_BRADFORD = 0, //standard photographic adaptation
		WB_CAT02    = 1, //CIECAM02 adaptation
		WB_CAT16    = 2  //modern CIECAM16 adaptation
	};
	
	//Shifts the color toward a target white point.
	//Linear only - sRGB is display-referred and scene math should never be done in display space.
	//If whitepoint is (1, 1, 1) then output is returned as input color.
	inline color white_balance(
		const color& c,
		const color& whitePoint,
		WhiteBalanceModel model)
	{
		//skip if identity RGB is near 1
		if (isnear(whitePoint.r, 1.0f)
			&& isnear(whitePoint.g, 1.0f)
			&& isnear(whitePoint.b, 1.0f))
		{
			return c;
		}

		color xyz = convert_color(ColorConvertType::COLOR_LINEAR_TO_XYZ, c);
		color wp_xyz = convert_color(ColorConvertType::COLOR_LINEAR_TO_XYZ, whitePoint);

		wp_xyz.r = max(wp_xyz.r, epsilon);
		wp_xyz.g = max(wp_xyz.g, epsilon);
		wp_xyz.b = max(wp_xyz.b, epsilon);

		switch (model)
		{
		case WhiteBalanceModel::WB_BRADFORD:
		{
			//xyz -> lms (color)

			f32 L =  0.8951f * xyz.r + 0.2664f * xyz.g - 0.1614f * xyz.b;
			f32 M = -0.7502f * xyz.r + 1.7135f * xyz.g + 0.0367f * xyz.b;
			f32 S =  0.0389f * xyz.r - 0.0685f * xyz.g + 1.0296f * xyz.b;

			//xyz -> lms (target white)

			f32 Lw =  0.8951f * wp_xyz.r + 0.2664f * wp_xyz.g - 0.1614f * wp_xyz.b;
			f32 Mw = -0.7502f * wp_xyz.r + 1.7135f * wp_xyz.g + 0.0367f * wp_xyz.b;
			f32 Sw =  0.0389f * wp_xyz.r - 0.0685f * wp_xyz.g + 1.0296f * wp_xyz.b;

			//xyz -> lms (source white, D65)

			f32 Ls =  0.8951f * XYZ_D65.r + 0.2664f * XYZ_D65.g - 0.1614f * XYZ_D65.b;
			f32 Ms = -0.7502f * XYZ_D65.r + 1.7135f * XYZ_D65.g + 0.0367f * XYZ_D65.b;
			f32 Ss =  0.0389f * XYZ_D65.r - 0.0685f * XYZ_D65.g + 1.0296f * XYZ_D65.b;

			//chromatic adaptation

			L *= (Lw / Ls);
			M *= (Mw / Ms);
			S *= (Sw / Ss);

			//lms -> xyz

			xyz.r =  0.9869929f * L - 0.1470543f * M + 0.1599627f * S;
			xyz.g =  0.4323053f * L + 0.5183603f * M + 0.0492912f * S;
			xyz.b = -0.0085287f * L + 0.0400428f * M + 0.9684867f * S;

			break;
		}
		case WhiteBalanceModel::WB_CAT02:
		{
			//xyz -> lms

			f32 L  =  0.7328f * xyz.r + 0.4296f * xyz.g - 0.1624f * xyz.b;
			f32 M  = -0.7036f * xyz.r + 1.6975f * xyz.g + 0.0061f * xyz.b;
			f32 S  =  0.0030f * xyz.r + 0.0136f * xyz.g + 0.9834f * xyz.b;

			//xyz -> lms (target white)

			f32 Lw =  0.7328f * wp_xyz.r + 0.4296f * wp_xyz.g - 0.1624f * wp_xyz.b;
			f32 Mw = -0.7036f * wp_xyz.r + 1.6975f * wp_xyz.g + 0.0061f * wp_xyz.b;
			f32 Sw =  0.0030f * wp_xyz.r + 0.0136f * wp_xyz.g + 0.9834f * wp_xyz.b;

			//xyz -> lms (source white, D65)

			f32 Ls =  0.7328f * XYZ_D65.r + 0.4296f * XYZ_D65.g - 0.1624f * XYZ_D65.b;
			f32 Ms = -0.7036f * XYZ_D65.r + 1.6975f * XYZ_D65.g + 0.0061f * XYZ_D65.b;
			f32 Ss =  0.0030f * XYZ_D65.r + 0.0136f * XYZ_D65.g + 0.9834f * XYZ_D65.b;

			//chromatic adaptation

			L *= (Lw / Ls);
			M *= (Mw / Ms);
			S *= (Sw / Ss);

			//lms -> xyz

			xyz.r =  1.096124f * L - 0.278869f * M + 0.182745f * S;
			xyz.g =  0.454369f * L + 0.473533f * M + 0.072098f * S;
			xyz.b = -0.009628f * L - 0.005698f * M + 1.015326f * S;

			break;
		}
		case WhiteBalanceModel::WB_CAT16:
		{
			//xyz -> lms

			f32 L  =  0.401288f * xyz.r + 0.650173f * xyz.g - 0.051461f * xyz.b;
			f32 M  = -0.250268f * xyz.r + 1.204414f * xyz.g + 0.045854f * xyz.b;
			f32 S  = -0.002079f * xyz.r + 0.048952f * xyz.g + 0.953127f * xyz.b;

			//xyz -> lms (target white)

			f32 Lw =  0.401288f * wp_xyz.r + 0.650173f * wp_xyz.g - 0.051461f * wp_xyz.b;
			f32 Mw = -0.250268f * wp_xyz.r + 1.204414f * wp_xyz.g + 0.045854f * wp_xyz.b;
			f32 Sw = -0.002079f * wp_xyz.r + 0.048952f * wp_xyz.g + 0.953127f * wp_xyz.b;

			//xyz -> lms (source white, D65)

			f32 Ls =  0.401288f * XYZ_D65.r + 0.650173f * XYZ_D65.g - 0.051461f * XYZ_D65.b;
			f32 Ms = -0.250268f * XYZ_D65.r + 1.204414f * XYZ_D65.g + 0.045854f * XYZ_D65.b;
			f32 Ss = -0.002079f * XYZ_D65.r + 0.048952f * XYZ_D65.g + 0.953127f * XYZ_D65.b;

			//chromatic adaptation

			L *= (Lw / Ls);
			M *= (Mw / Ms);
			S *= (Sw / Ss);

			//lms -> xyz

			xyz.r =  1.862067f * L - 1.011255f * M + 0.149187f * S;
			xyz.g =  0.387526f * L + 0.621447f * M - 0.008973f * S;
			xyz.b = -0.015841f * L - 0.034123f * M + 1.049964f * S;

			break;
		}
		}

		return convert_color(
			ColorConvertType::COLOR_XYZ_TO_LINEAR, xyz);
	}

	//Applies a physically-based color temperature in Kelvin.
	//Linear only - sRGB is display-referred and scene math should never be done in display space.
	inline color temperature(
		const color& c,
		f32 kelvin,
		WhiteBalanceModel model = WhiteBalanceModel::WB_BRADFORD)
	{
		//returns linear RGB white for a given Kelvin temperature
		auto kelvin_to_linear_srgb = [](f32 kelvin)
			{
				//valid range for CIE daylight locus
				kelvin = clamp(kelvin, 4000.0f, 25000.0f);

				const f32 T = kelvin;

				f32 x{};

				if (T <= 7000.0f)
				{
					x = -4.6070e9f / (T * T * T)
						+ 2.9678e6f / (T * T)
						+ 0.09911e3f / T
						+ 0.244063f;
				}
				else
				{
					x = -2.0064e9f / (T * T * T)
						+ 1.9018e6f / (T * T)
						+ 0.24748e3f / T
						+ 0.237040f;
				}

				f32 y = -3.000f * x * x + 2.870f * x - 0.275f;

				f32 X = x / y;
				f32 Y = 1.0f;
				f32 Z = (1.0f - x - y) / y;

				f32 r =
					3.2404542f * X
					- 1.5371385f * Y
					- 0.4985314f * Z;

				f32 g =
					-0.9692660f * X
					+ 1.8760108f * Y
					+ 0.0415560f * Z;

				f32 b =
					0.0556434f * X
					- 0.2040259f * Y
					+ 1.0572252f * Z;

				return color(r, g, b);
			};

		color targetWhite = kelvin_to_linear_srgb(kelvin);
		
		return white_balance(
			c,
			targetWhite,
			model);
	}

	//Optional parameters for shadows_midtones_highlights
	struct smh_params
	{
		f32 shadow_end = 0.25f;
		f32 mid_center = 0.5f;
		f32 highlight_start = 0.75f;
	};

	//Adjusts shadows, midtones and highlights independently.
	//Linear only - sRGB is display-referred and scene math should never be done in display space.
	//All three values are clamped from -1.0 to 1.0
	//	- shadows affect dark areas,
	//	- midtones affect mid-range luminance,
	//  - highlights affect bright areas
	inline color shadows_midtones_highlights(
		const color& c,
		f32 shadows,
		f32 midtones,
		f32 highlights,
		const smh_params& sp = {})
	{
		auto smoothstep = [](
			const f32 edge0,
			const f32 edge1,
			const f32 x) -> f32
			{
				//helps avoid division by 0
				const f32 denom = max(edge1 - edge0, epsilon);

				f32 t = clamp((x - edge0) / denom, 0.0f, 1.0f);

				return t * t * (3.0f - 2.0f * t);
			};

		f32 s = clamp(shadows,    -1.0f, 1.0f);
		f32 m = clamp(midtones,   -1.0f, 1.0f);
		f32 h = clamp(highlights, -1.0f, 1.0f);
		
		f32 L = luminance(c);
		
		//weight curves (smoothstep transitions)

		f32 s_end = sp.shadow_end;
		f32 m_cen = sp.mid_center;
		f32 h_sta = sp.highlight_start;

		s_end = max(s_end, 0.0f);
		h_sta = max(h_sta, s_end + epsilon);
		m_cen = clamp(
			m_cen,
			s_end + epsilon,
			h_sta - epsilon);
		
		f32 wShadow =    1.0f - smoothstep(s_end, m_cen, L);
		f32 wHighlight = smoothstep(m_cen, h_sta, L);
		f32 wMid =       max(1.0f - wShadow - wHighlight, 0.0f); //prevent slipping below 0

		//enforce full coverage
		f32 wSum = wShadow + wMid + wHighlight;
		if (wSum < 1.0f) wMid += (1.0f - wSum);
		
		f32 scale = max(
			1.0f 
			+ s * wShadow 
			+ m * wMid
			+ h * wHighlight,
			0.0f);

		return color
		(
			c.r * scale,
			c.g * scale,
			c.b * scale,
			c.a
		);
	}
}