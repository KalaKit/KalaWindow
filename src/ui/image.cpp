//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <memory>

#include "KalaHeaders/log_utils.hpp"

#include "ui/image.hpp"
#include "core/core.hpp"
#include "graphics/window.hpp"
#include "graphics/opengl/opengl.hpp"
#include "graphics/opengl/opengl_functions_core.hpp"
#include "graphics/opengl/opengl_texture.hpp"

using KalaHeaders::Log;
using KalaHeaders::LogType;

using KalaWindow::Core::KalaWindowCore;
using KalaWindow::Core::globalID;
using KalaWindow::Graphics::Window;
using KalaWindow::Graphics::TargetType;
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
		const vec2 pos,
		const float rot,
		const vec2 size,
		Widget* parentWidget,
		OpenGL_Texture* texture,
		OpenGL_Shader* shader)
	{
		Window* window = Window::registry.GetContent(windowID);

		if (!window
			|| !window->IsInitialized())
		{
			Log::Print(
				"Cannot load image '" + name + "' because its window context is invalid!",
				"IMAGE",
				LogType::LOG_ERROR);

			return nullptr;
		}

		vector<OpenGL_Context*> contexts = OpenGL_Context::registry.GetAllWindowContent(windowID);
		OpenGL_Context* context = contexts.empty() ? nullptr : contexts.front();

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
			imagePtr->hierarchy.SetParent(imagePtr, parentWidget);
		}

		Widget::Create2DQuad(
			imagePtr->render.VAO,
			imagePtr->render.VBO,
			imagePtr->render.EBO);

		imagePtr->ID = newID;
		imagePtr->windowID = windowID;
		imagePtr->SetName(name);
		imagePtr->render.canUpdate = true;
		imagePtr->SetPos(pos, PosTarget::POS_WORLD);
		imagePtr->SetRot(rot, RotTarget::ROT_WORLD);
		imagePtr->SetSize(size, SizeTarget::SIZE_WORLD);

		imagePtr->isInitialized = true;

		registry.AddContent(newID, move(newImage));
		runtimeImages.push_back(imagePtr);
		window->AddValue(TargetType::TYPE_WIDGET, newID);

		Log::Print(
			"Loaded image '" + name + "' with ID '" + to_string(newID) + "'!",
			"IMAGE",
			LogType::LOG_SUCCESS);

		return imagePtr;
	}

	bool Image::Render(
		const mat3& projection,
		const vec2 viewportSize)
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

		vec2 clampedVP = clamp(viewportSize, vec2(1), vec2(10000));
		transform.viewportSize = clampedVP;

		UpdateOriginalPosition();

		u32 programID = render.shader->GetProgramID();

		mat3 model = mat3(1.0f);
		model = Translate2D(model, transform.combinedPos);
		model = Rotate2D(model, transform.combinedRot);
		model = Scale2D(model, transform.combinedSize);

		render.shader->SetMat3(programID, "uModel", model);
		render.shader->SetMat3(programID, "uProjection", projection);

		bool isOpaque = render.opacity = 1.0f;
		bool isTransparentTexture = render.texture
			&& (render.texture->GetFormat() == TextureFormat::Format_RGBA8
			|| render.texture->GetFormat() == TextureFormat::Format_RGBA16F
			|| render.texture->GetFormat() == TextureFormat::Format_RGBA32F
			|| render.texture->GetFormat() == TextureFormat::Format_SRGB8A8);

		bool isAlpha =
			isOpaque
			|| isTransparentTexture;

		if (isAlpha)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDepthMask(GL_FALSE);
		}

		render.shader->SetVec3(programID, "uColor", render.color);
		render.shader->SetFloat(programID, "uOpacity", render.opacity);

		if (render.texture)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, render.texture->GetOpenGLID());
			render.shader->SetInt(programID, "uTexture", 0);
			render.shader->SetBool(programID, "uUseTexture", true);
		}
		else render.shader->SetBool(programID, "uUseTexture", false);

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
		if (!isInitialized)
		{
			Log::Print(
				"Cannot destroy widget '" + name + "' with ID '" + to_string(ID) + "' because it is not initialized!",
				"WIDGET",
				LogType::LOG_ERROR,
				2);

			return;
		}

		Window* window = Window::registry.GetContent(windowID);

		if (!window
			|| !window->IsInitialized())
		{
			Log::Print(
				"Cannot destroy widget '" + name + "' with ID '" + to_string(ID) + "' because its window was not found!",
				"WIDGET",
				LogType::LOG_ERROR,
				2);

			return;
		}

		Log::Print(
			"Destroying widget '" + name + "' with ID '" + to_string(ID) + "' for window '" + window->GetTitle() + "'.",
			"WIDGET",
			LogType::LOG_INFO);

		hierarchy.RemoveAllChildren();
		hierarchy.RemoveParent(this);

		u32 vao = GetVAO();
		u32 vbo = GetVBO();
		u32 ebo = GetEBO();

		if (vao != 0)
		{
			glDeleteVertexArrays(1, &vao);
			vao = 0;
		}
		if (vbo != 0)
		{
			glDeleteBuffers(1, &vbo);
			vbo = 0;
		}
		if (ebo != 0)
		{
			glDeleteBuffers(1, &ebo);
			ebo = 0;
		}
	}
}