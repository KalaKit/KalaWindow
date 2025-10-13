//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>

#include "ui/widget.hpp"

namespace KalaWindow::UI
{
	using std::string;

	class Text : public Widget
	{
	public:
		//Creates a new text widget.
		//Parent widget and texture ID are optional
		static Text* Initialize(
			u32 windowID,
			u32 fontID,
			u32 textureID,
			Widget* parentWidget,
			const string& name);

		//Core render function for all widget systems, must be overridden per inherited widget.
		//Pass mat4(1.0f) to view and pass 2D projection as ortho(0.0f, windowWidth, windowHeight, 0.0f)
		//if you want 2D UI, otherwise this widget element is drawn in 3D space
		virtual bool Render(
			const mat4& view,
			const mat4& projection) override;

		//Do not destroy manually, erase from containers.hpp instead
		virtual ~Text();
	private:
		u32 fontID{};
	};
}