//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include "KalaHeaders/math_utils.hpp"

#include "ui/widget.hpp"

namespace KalaWindow::UI
{
	using KalaHeaders::vec2;
	using KalaHeaders::mat3;

	class LIB_API Image : public Widget
	{
	public:
		//Initialize a new Image widget.
		//Parent widget is optional
		static Image* Initialize(
			const string& name,
			u32 windowID,
			const vec2 pos,
			const float rot,
			const vec2 size,
			Widget* parentWidget,
			OpenGL_Texture* texture,
			OpenGL_Shader* shader);

		//Render the image
		virtual bool Render(const mat4& projection) override;

		//Do not destroy manually, erase from registry instead
		virtual ~Image() override;
	};
}