//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include "ui/widget.hpp"

namespace KalaWindow::UI
{
	class Image : public Widget
	{
	public:
		static Image* Initialize();

		virtual bool Render(
			const mat4& view,
			const mat4& projection) override;

		//Do not destroy manually, erase from containers.hpp instead
		virtual ~Image();
	};
}