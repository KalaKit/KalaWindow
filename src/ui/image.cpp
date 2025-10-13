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
#include "graphics/opengl/opengl_functions_core.hpp"
#include "graphics/opengl/opengl_texture.hpp"

using KalaHeaders::Log;
using KalaHeaders::LogType;

using KalaWindow::Core::globalID;
using KalaWindow::Core::GetValueByID;
using KalaWindow::Core::windowContent;
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
	Image* Image::Initialize(
		const string& name,
		u32 windowID,
		const vec3& pos,
		const vec3& rot,
		const vec3& size,
		Widget* parentWidget,
		OpenGL_Texture* texture,
		OpenGL_Shader* shader)
	{
		Window* window = GetValueByID<Window>(windowID);

		if (!window
			|| !window->IsInitialized())
		{
			Log::Print(
				"Cannot load image '" + name + "' because its window context is invalid!",
				"IMAGE",
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
				"Cannot load image texture '" + name + "' because its window '" + window->GetTitle() + "' is missing from window content!",
				"IMAGE",
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
				"Cannot load image '" + name + "' because its OpenGL context is invalid!",
				"IMAGE",
				LogType::LOG_ERROR);

			return nullptr;
		}

		u32 newID = ++globalID;
		unique_ptr<Image> newImage = make_unique<Image>();
		Image* imagePtr = newImage.get();

		Log::Print(
			"Loading image '" + name + "' with ID '" + to_string(newID) + "'.",
			"IMAGE",
			LogType::LOG_DEBUG);

		//texture is required
		if (!texture
			|| !texture->IsInitialized())
		{
			Log::Print(
				"Failed to load Image widget '" + name + "' because its texture context is invalid!",
				"TEXT",
				LogType::LOG_ERROR);

			return nullptr;
		}
		imagePtr->render.texture = texture;

		//shader is required
		if (!shader
			|| !shader->IsInitialized())
		{
			Log::Print(
				"Failed to load Image widget '" + name + "' because its shader context is invalid!",
				"TEXT",
				LogType::LOG_ERROR);

			return nullptr;
		}
		imagePtr->render.shader = shader;

		//parent is optional
		if (parentWidget
			&& parentWidget->IsInitialized())
		{
			imagePtr->SetParent(parentWidget);
		}

		Widget::Create2DQuad(
			imagePtr->render.VAO,
			imagePtr->render.VBO,
			imagePtr->render.EBO);

		imagePtr->ID = newID;
		imagePtr->SetName(name);
		imagePtr->render.canUpdate = true;
		imagePtr->SetPos(pos, PosTarget::POS_WORLD);
		imagePtr->SetRotVec(rot, RotTarget::ROT_WORLD);
		imagePtr->SetSize(size, SizeTarget::SIZE_WORLD);

		content->widgets[newID] = move(newImage);
		content->runtimeWidgets.push_back(imagePtr);
		content->runtimeImages.push_back(imagePtr);

		Log::Print(
			"Loaded image '" + name + "' with ID '" + to_string(newID) + "'!",
			"IMAGE",
			LogType::LOG_SUCCESS);

		return imagePtr;
	}

	bool Image::Render(
		const mat4& view,
		const mat4& projection)
	{
		if (!render.canUpdate) return false;

		if (!render.shader)
		{
			Log::Print(
				"Failed to render Image widget '" + name + "' because its shader is nullptr!",
				"IMAGE",
				LogType::LOG_ERROR);

			return false;
		}

		if (!render.shader->Bind())
		{
			Log::Print(
				"Failed to render Image widget '" + name + "' because its shader '" + render.shader->GetName() + "' failed to bind!",
				"IMAGE",
				LogType::LOG_ERROR);

			return false;
		}

		u32 programID = render.shader->GetProgramID();

		render.shader->SetMat3(programID, "uModel", GetWorldMatrix());
		render.shader->SetMat3(programID, "uProjection", projection);

		if (!render.is2D) render.shader->SetMat3(programID, "uView", view);
		else render.shader->SetMat3(programID, "uView", mat4(1.0f));

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

		return true;
	}

	Image::~Image()
	{

	}
}