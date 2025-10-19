//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string_view>

namespace KalaWindow::Graphics::OpenGL::Shader
{
	using std::string_view;

	inline constexpr string_view shader_text_vertex =
	R"(
		#version 330 core

		layout (location = 0) in vec2 aPos;
		layout (location = 1) in vec2 aTexCoord;

		out vec2 TexCoord;

		uniform mat3 uModel;
		uniform mat3 uProjection;

		uniform float italic_skew;    //horizontal slant factor
		uniform float kerning_offset; //per-glyph x-adjustment
		
		void main()
		{
			vec2 pos = aPos;
			
			//apply kerning (horizontal shift)
			pos.x += kerning_offset;
			
			//apply italic skew (shear on x based on y)
			pos.x += pos.y * italic_skew;

			vec3 worldPos = uProjection * uModel * vec3(pos, 1.0);
			gl_Position = vec4(worldPos.xy, 0.0, 1.0);
			TexCoord = aTexCoord;
		}
	)";

	inline constexpr string_view shader_text_fragment =
	R"(
		#version 330 core

		in vec2 TexCoord;
		out vec4 FragColor;

		uniform sampler2D texture0;
		uniform bool useTexture; //mark as true if you want to pass a texture

		uniform vec3 color;    //blended with texture or non-texture base color
		uniform float opacity; //makes this transparent if below 1.0

		//underline
		uniform bool underline = false;
		uniform float underline_strength;
		uniform float underline_thickness;
		uniform float underline_offset;
		uniform vec3 underline_color;

		//stripe
		uniform bool striped;
		uniform float stripe_darkness;
		uniform float stripe_repeat;
		uniform vec2 stripe_size;

		//shadow
		uniform bool shadow;
		uniform vec2 shadow_offset;
		uniform vec3 shadow_color;
		uniform float shadow_opacity;

		//glow
		uniform bool glow;
		uniform vec3 glow_color;
		uniform float glow_strength;
		uniform float glow_radius;

		vec3 ApplyUnderline(vec3 baseColor);
		vec3 ApplyStripes(vec3 baseColor);
		vec3 ApplyShadow(vec3 baseColor, vec3 fallbackColor, inout float alpha);
		vec3 ApplyGlow(vec3 baseColor, vec3 fallbackColor, inout float alpha);
		
		void main()
		{
			float safeOpacity = clamp(opacity, 0.0, 1.0);
			vec3 safeColor = clamp(color, 0.0, 1.0);

			if (safeOpacity < 0.1) discard;

			vec4 texColor = vec4(1.0);
			if (useTexture) texColor = texture(texture0, TexCoord);

			vec3 base = texColor.rgb * safeColor;
			float alpha = texColor.a * safeOpacity;

			//disable shadow if glow is enabled
			if (shadow && glow) shadow = false;

			base = ApplyShadow(base, safeColor, alpha);
			base = ApplyGlow(base, safeColor, alpha);
			base = ApplyUnderline(base);
			base = ApplyStripes(base);

			FragColor = vec4(base, alpha);
		}

		vec3 ApplyUnderline(vec3 baseColor)
		{
			if (underline)
			{
				float safeStrength = clamp(underline_strength, 0.0, 1.0);
				float safeThickness = clamp(underline_thickness, 0.0, 1.0);
				float safeOffset = clamp(underline_offset, -1.0, 1.0);
				vec3 safeColor = clamp(underline_color, 0.0, 1.0);

				if (TexCoord.y < safeOffset + safeThickness
					&& TexCoord.y > safeOffset)
				{
					baseColor = mix(baseColor, safeColor, safeStrength);
				}
			}

			return baseColor;
		}

		vec3 ApplyStripes(vec3 baseColor)
		{
			if (striped)
			{
				float safeDarkness = clamp(stripe_darkness, 0.0, 1.0);
				float safeRepeat = clamp(stripe_repeat, 1.0, 8.0);
				vec2 safeSize = clamp(stripe_size, vec2(0.001), vec2(256.0));

				float stripe = mod(floor(
					TexCoord.x
					* safeSize.x + TexCoord.y
					* safeSize.y), safeRepeat);

				baseColor *= mix(1.0, safeDarkness, stripe);
			}

			return baseColor;
		}

		vec3 ApplyShadow(vec3 baseColor, vec3 fallbackColor, inout float alpha)
		{
			if (shadow)
			{
				float safeOpacity = clamp(shadow_opacity, 0.0, 1.0);
				vec3 safeColor = clamp(shadow_color, 0.0, 1.0);
				vec2 safeOffset = clamp(shadow_offset, vec2(-1.0), vec2(1.0));

				vec4 shadowSample = vec4(1.0);

				if (useTexture) shadowSample = texture(texture0, TexCoord + safeOffset);
				else            shadowSample.rgb = fallbackColor;

				baseColor = mix(safeColor, baseColor, shadowSample.a);
				alpha = max(alpha, shadowSample.a * safeOpacity);
			}

			return baseColor;
		}

		vec3 ApplyGlow(vec3 baseColor, vec3 fallbackColor, inout float alpha)
		{
			float safeStrength = clamp(glow_strength, 0.0, 1.0);
			float safeRadius = clamp(glow_radius, 0.0, 0.5);
			vec3 safeColor = clamp(glow_color, 0.0, 1.0);

			vec4 glowSample = vec4(0.0);
			if (useTexture)
			{
				glowSample += texture(texture0, TexCoord + vec2(-safeRadius, 0.0));
				glowSample += texture(texture0, TexCoord + vec2( safeRadius, 0.0));
				glowSample += texture(texture0, TexCoord + vec2(0.0, -safeRadius));
				glowSample += texture(texture0, TexCoord + vec2(0.0,  safeRadius));
				glowSample /= 4.0;
			}
			else glowSample = vec4(fallbackColor, 1.0);

			baseColor += safeColor * glowSample.a * safeStrength;

			return baseColor;
		}
	)";
}