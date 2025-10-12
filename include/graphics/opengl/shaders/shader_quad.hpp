//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string_view>

#include "KalaHeaders/core_utils.hpp"

namespace KalaWindow::Graphics::OpenGL::Shader
{
	using std::string_view;

	LIB_API inline constexpr string_view shader_quad_vertex = 
	R"(
		#version 330 core

		layout (location = 0) in vec3 aPos;
		layout (location = 1) in vec2 aTexCoord;

		out vec2 TexCoord;

		uniform mat4 uModel;            //pos, rot and scale as mat4
		uniform mat4 uView = mat4(1.0); //unused for 2D
		uniform mat4 uProjection;       //required for 2D and 3D
		
		void main()
		{
			gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0);
			TexCoord = aTexCoord;
		}
	)";

	LIB_API inline constexpr string_view shader_quad_fragment =
	R"(
		#version 330 core

		in vec2 TexCoord;
		out vec4 FragColor;

		uniform sampler2D texture0;
		uniform bool useTexture = false;          //mark as true if you want to pass a texture

		uniform vec3 color = vec3(1.0, 1.0, 1.0); //blended with texture or non-texture base color
		uniform float opacity = 1.0;              //makes this transparent if below 1.0
		
		void main()
		{
			float safeOpacity = clamp(opacity, 0.0, 1.0);
			vec3 safeColor = clamp(color, 0.0, 1.0);

			if (safeOpacity < 0.1) discard;

			vec4 texColor = vec4(1.0);
			if (useTexture) texColor = texture(texture0, TexCoord);

			FragColor = vec4(texColor.rgb * safeColor, texColor.a * safeOpacity);
		}
	)";
}