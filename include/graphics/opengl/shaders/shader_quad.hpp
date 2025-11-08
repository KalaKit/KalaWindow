//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string_view>

namespace KalaWindow::Graphics::OpenGL::Shader
{
	using std::string_view;

	inline constexpr string_view shader_quad_vertex = 
	R"(
		#version 330 core

		layout (location = 0) in vec2 aPos;
		layout (location = 1) in vec2 aTexCoord;

		out vec2 TexCoord;

		uniform mat4 uModel;
		uniform mat4 uProjection;
		
		void main()
		{
			//view matrix is identity and unused

			vec4 worldPos = uProjection * uModel * vec4(aPos, 0.0, 1.0);
			gl_Position = vec4(worldPos);

			TexCoord = aTexCoord;
		}
	)";

	inline constexpr string_view shader_quad_fragment =
	R"(
		#version 330 core

		in vec2 TexCoord;
		out vec4 FragColor;

		uniform sampler2D uTexture;
		uniform bool uUseTexture = false; //mark as true if you want to pass a texture

		uniform vec3 uColor;    //blended with texture or non-texture base color
		uniform float uOpacity; //makes this transparent if below 1.0
		
		void main()
		{
			float safeOpacity = clamp(uOpacity, 0.0, 1.0);
			vec3 safeColor = clamp(uColor, 0.0, 1.0);

			if (safeOpacity < 0.1) discard;

			vec4 texColor = vec4(1.0);
			if (uUseTexture) texColor = texture(uTexture, TexCoord);

			FragColor = vec4(texColor.rgb * safeColor, texColor.a * safeOpacity);
		}
	)";
}