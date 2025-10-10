//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <vector>

#include "KalaHeaders/core_utils.hpp"

#include "core/glm_global.hpp"

namespace KalaWindow::UI
{
	using std::vector;

	class Widget;
	class OpenGL_Shader;

	class LIB_API WidgetManager
	{
	public:
		//Initializes the global widget system reused across all windows.
		//Also initializes the quad and text shaders
		static WidgetManager* Initialize(u32 windowID);

		//Returns all hit widgets in 2D or 3D space sorted by highest Z first.
		//For 2D click tests you need to pass both as equal values with Z as 0
		static vector<Widget*> HitWidgets(
			const vec3& origin, 
			const vec3& target);

		inline OpenGL_Shader* GetQuadShader()
		{
			return shader_quad;
		}
		inline OpenGL_Shader* GetTextShader()
		{
			return shader_text;
		}
	private:
		OpenGL_Shader* shader_quad{};
		OpenGL_Shader* shader_text{};
	};
}