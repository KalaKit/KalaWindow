//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include "KalaHeaders/math_utils.hpp"

#include "ui/widget.hpp"
#include "core/registry.hpp"

namespace KalaWindow::UI
{
	using KalaHeaders::kvec2;
	using KalaHeaders::kmat3;

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
			const kvec2 pos,
			const float rot,
			const kvec2 size,
			Widget* parentWidget,
			OpenGL_Texture* texture,
			OpenGL_Shader* shader);

		//Render the image. Pass viewport size so that the image can be
		//positioned to the window center and offset from that with world pos
		virtual bool Render(
			const kmat4& projection,
			const kvec2 viewportSize) override;

		//Do not destroy manually, erase from registry instead
		virtual ~Image() override;
	};
}