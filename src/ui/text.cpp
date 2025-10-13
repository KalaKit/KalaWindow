//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <memory>

#include "KalaHeaders/log_utils.hpp"

#include "core/containers.hpp"
#include "ui/text.hpp"
#include "graphics/window.hpp"
#include "graphics/opengl/opengl.hpp"

using KalaHeaders::Log;
using KalaHeaders::LogType;

using KalaWindow::Core::globalID;
using KalaWindow::Core::GetValueByID;
using KalaWindow::Core::windowContent;
using KalaWindow::Core::createdFonts;
using KalaWindow::Core::WindowContent;
using KalaWindow::Graphics::Window;
using KalaWindow::Graphics::OpenGL::OpenGL_Context;

using std::unique_ptr;
using std::make_unique;
using std::to_string;

namespace KalaWindow::UI
{
	Text* Text::Initialize(
		const string& name,
		u32 windowID,
		u32 fontID,
		Widget* parentWidget,
		OpenGL_Texture* texture,
		OpenGL_Shader* shader)
	{
		Window* window = GetValueByID<Window>(windowID);

		if (!window)
		{
			Log::Print(
				"Cannot load text '" + name + "' because its window context is invalid!",
				"TEXT",
				LogType::LOG_ERROR);

			return nullptr;
		}

		WindowContent* content{};
		if (windowContent.contains(window))
		{
			content = windowContent[window].get();
		}

		if (!content)
		{
			Log::Print(
				"Cannot load text texture '" + name + "' because its window '" + window->GetTitle() + "' is missing from window content!",
				"TEXT",
				LogType::LOG_ERROR);

			return nullptr;
		}

		OpenGL_Context* context{};
		if (content->glContext) context = content->glContext.get();

		if (!context
			|| !context->IsInitialized()
			|| !context->IsContextValid())
		{
			Log::Print(
				"Cannot load text '" + name + "' because its OpenGL context is invalid!",
				"TEXT",
				LogType::LOG_ERROR);

			return nullptr;
		}

		u32 newID = ++globalID;
		unique_ptr<Text> newText = make_unique<Text>();
		Text* textPtr = newText.get();

		Log::Print(
			"Loading text '" + name + "' with ID '" + to_string(newID) + "'.",
			"TEXT",
			LogType::LOG_DEBUG);

		//texture is optional
		if (texture
			&& texture->IsInitialized())
		{
			textPtr->render.texture = texture;
		}

		//shader is required
		if (!shader
			|| !shader->IsInitialized())
		{
			Log::Print(
				"Failed to load Text widget '" + name + "' because its shader context is invalid!",
				"TEXT",
				LogType::LOG_ERROR);

			return nullptr;
		}
		textPtr->render.shader = shader;

		//parent is optional
		if (parentWidget
			&& parentWidget->IsInitialized())
		{
			textPtr->SetParent(parentWidget);
		}

		//font is required
		if (fontID != 0)
		{
			Font* font{};
			if (createdFonts.contains(fontID))
			{
				font = createdFonts[fontID].get();
			}

			if (font) textPtr->fontID = fontID;
		}
		if (textPtr->fontID == 0)
		{
			Log::Print(
				"Failed to load Text widget '" + name + "' because its Font ID '" + to_string(fontID) + "' is invalid!",
				"TEXT",
				LogType::LOG_ERROR);

			return nullptr;
		}

		//<<< load text here

		textPtr->ID = newID;
		textPtr->SetName(name);

		content->widgets[newID] = move(newText);
		content->runtimeWidgets.push_back(textPtr);
		content->runtimeText.push_back(textPtr);

		Log::Print(
			"Loaded text '" + name + "' with ID '" + to_string(newID) + "'!",
			"TEXT",
			LogType::LOG_SUCCESS);

		return textPtr;
	}

	bool Text::Render(
		const mat4& view,
		const mat4& projection)
	{
		return false;
	}

	Text::~Text()
	{

	}
}