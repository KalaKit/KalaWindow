//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include "ui/widget.hpp"

namespace KalaWindow::UI
{
	class LIB_API Text : public Widget
	{
	public:
		//Initialize a new Text widget.
		//Parent widget and texture are optional
		static Text* Initialize(
			const string& name,
			u32 windowID,
			u32 fontID,
			Widget* parentWidget,
			OpenGL_Texture* texture,
			OpenGL_Shader* shader);

		//Core render function for all widget systems, must be overridden per inherited widget.
		//Pass mat4(1.0f) to view and pass 2D projection as ortho(0.0f, windowWidth, windowHeight, 0.0f)
		//if you want 2D UI, otherwise this widget element is drawn in 3D space
		virtual bool Render(
			const mat4& view,
			const mat4& projection) override;

		void SetFontID(u32 newValue);
		inline u32 GetFontID() const { return fontID; }

		//Do not destroy manually, erase from containers.hpp instead
		virtual ~Text();
	private:
		u32 fontID{};
	};
}