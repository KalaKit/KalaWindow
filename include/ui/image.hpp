//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include "ui/widget.hpp"
#include "core/registry.hpp"

namespace KalaWindow::UI
{
	using KalaWindow::Core::Registry;

	class LIB_API Image : public Widget
	{
	public:
		static inline Registry<Image> registry{};
		static inline vector<Image*> runtimeImages{};

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

		//Core render function for all widget systems, must be overridden per inherited widget
		virtual bool Render(const mat3& projection) override;

		//Do not destroy manually, erase from registry instead
		virtual ~Image() override;
	};
}