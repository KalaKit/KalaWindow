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
#include "graphics/opengl/opengl_functions_core.hpp"
#include "graphics/opengl/opengl_texture.hpp"

using KalaHeaders::Log;
using KalaHeaders::LogType;

using KalaWindow::Core::globalID;
using KalaWindow::Core::GetValueByID;
using KalaWindow::Core::windowContent;
using KalaWindow::Core::createdFonts;
using KalaWindow::Core::WindowContent;
using KalaWindow::Graphics::Window;
using KalaWindow::Graphics::OpenGL::OpenGL_Context;
using KalaWindow::Graphics::TextureFormat;
using namespace KalaWindow::Graphics::OpenGLFunctions;

using std::unique_ptr;
using std::make_unique;
using std::to_string;

namespace KalaWindow::UI
{
	Text* Text::Initialize(
		const string& name,
		u32 windowID,
		u32 fontID,
		const vec3& pos,
		const vec3& rot,
		const vec3& size,
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

		//TODO: load text here

		textPtr->ID = newID;
		textPtr->SetName(name);
		textPtr->render.canUpdate = true;
		textPtr->SetPos(pos, PosTarget::POS_WORLD);
		textPtr->SetRotVec(rot, RotTarget::ROT_WORLD);
		textPtr->SetSize(size, SizeTarget::SIZE_WORLD);

		content->widgets[newID] = move(newText);
		content->runtimeWidgets.push_back(textPtr);
		content->runtimeText.push_back(textPtr);

		Log::Print(
			"Loaded text '" + name + "' with ID '" + to_string(newID) + "'!",
			"TEXT",
			LogType::LOG_SUCCESS);

		return textPtr;
	}

	void Text::SetFontID(u32 newValue)
	{
		if (newValue != 0)
		{
			Font* font{};
			if (createdFonts.contains(newValue))
			{
				font = createdFonts[newValue].get();
			}

			if (font) fontID = newValue;
		}
		if (fontID == 0)
		{
			Log::Print(
				"Cannot set Text widget '" + name + "' font to Font ID '" + to_string(fontID) + "' because it is invalid!",
				"TEXT",
				LogType::LOG_ERROR);

			return;
		}
	}

	bool Text::Render(
		const mat4& view,
		const mat4& projection)
	{
		if (!render.canUpdate) return false;

		if (!render.shader)
		{
			Log::Print(
				"Failed to render Text widget '" + name + "' because its shader is nullptr!",
				"TEXT",
				LogType::LOG_ERROR);

			return false;
		}

		if (!render.texture)
		{
			Log::Print(
				"Failed to render Text widget '" + name + "' because its texture is nullptr!",
				"TEXT",
				LogType::LOG_ERROR);

			return false;
		}

		if (!render.shader->Bind())
		{
			Log::Print(
				"Failed to render Text widget '" + name + "' because its shader '" + render.shader->GetName() + "' failed to bind!",
				"IMAGE",
				LogType::LOG_ERROR);

			return false;
		}

		u32 programID = render.shader->GetProgramID();

		render.shader->SetMat3(programID, "uModel", GetWorldMatrix());
		render.shader->SetMat3(programID, "uProjection", projection);
		
		if (!render.is2D) render.shader->SetMat3(programID, "uView", view);
		else render.shader->SetMat3(programID, "uView", mat4(1.0f));

		if (render.texture)
		{
			bool isAlpha = render.opacity < 1.0f
				&& render.texture->GetFormat() == TextureFormat::Format_RGBA8
				|| render.texture->GetFormat() == TextureFormat::Format_RGBA16F
				|| render.texture->GetFormat() == TextureFormat::Format_RGBA32F
				|| render.texture->GetFormat() == TextureFormat::Format_SRGB8A8;

			if (isAlpha)
			{
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glDepthMask(GL_FALSE);
			}

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, render.texture->GetOpenGLID());
			render.shader->SetInt(programID, "uTexture", 0);

			glBindVertexArray(render.VAO);
			glDrawElements(
				GL_TRIANGLES,
				6,
				GL_UNSIGNED_INT,
				0);
			glBindVertexArray(0);

			if (isAlpha)
			{
				glDisable(GL_BLEND);
				glDepthMask(GL_TRUE);
			}
		}

		return true;
	}

	Text::~Text()
	{

	}
}