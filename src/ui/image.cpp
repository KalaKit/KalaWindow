//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <memory>

#include "core/containers.hpp"
#include "ui/image.hpp"

using KalaWindow::Core::globalID;

using std::unique_ptr;
using std::make_unique;

namespace KalaWindow::UI
{
	Image* Image::Initialize()
	{
		return nullptr;
	}

	bool Image::Render(
		const mat4& view,
		const mat4& projection)
	{
		return false;
	}

	Image::~Image()
	{

	}
}