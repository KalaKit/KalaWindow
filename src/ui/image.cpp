//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <memory>

#include "KalaHeaders/log_utils.hpp"

#include "core/containers.hpp"
#include "ui/image.hpp"
#include "graphics/window.hpp"
#include "graphics/opengl/opengl.hpp"

using KalaHeaders::Log;
using KalaHeaders::LogType;

using KalaWindow::Core::globalID;
using KalaWindow::Core::GetValueByID;
using KalaWindow::Core::windowContent;
using KalaWindow::Core::WindowContent;
using KalaWindow::Graphics::Window;
using KalaWindow::Graphics::OpenGL::OpenGL_Context;

using std::unique_ptr;
using std::make_unique;
using std::to_string;

namespace KalaWindow::UI
{
	Image* Image::Initialize(
		u32 windowID,
		u32 textureID,
		Widget* parentWidget,
		const string& name)
	{
		Window* window = GetValueByID<Window>(windowID);

		if (!window)
		{
			Log::Print(
				"Cannot load image '" + name + "' because its window context is invalid!",
				"TEXT",
				LogType::LOG_ERROR);

			return nullptr;
		}

		WindowContent* content = windowContent[window].get();
		OpenGL_Context* context = content->glContext.get();

		if (!context)
		{
			Log::Print(
				"Cannot load image '" + name + "' because its OpenGL context is invalid!",
				"TEXT",
				LogType::LOG_ERROR);

			return nullptr;
		}

		u32 newID = ++globalID;
		unique_ptr<Image> newImage = make_unique<Image>();
		Image* imagePtr = newImage.get();

		Log::Print(
			"Loading image '" + name + "' with ID '" + to_string(newID) + "'.",
			"TEXT",
			LogType::LOG_DEBUG);

		//<<< load image here

		imagePtr->ID = newID;
		imagePtr->SetName(name);

		content->widgets[newID] = move(newImage);
		content->runtimeWidgets.push_back(imagePtr);
		content->runtimeImages.push_back(imagePtr);

		Log::Print(
			"Loaded image '" + name + "' with ID '" + to_string(newID) + "'!",
			"TEXT",
			LogType::LOG_SUCCESS);

		return imagePtr;
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