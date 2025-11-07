//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <memory>

#include "KalaHeaders/log_utils.hpp"
#include "KalaHeaders/import_ktf.hpp"

#include "core/core.hpp"
#include "ui/text.hpp"
#include "ui/font.hpp"
#include "graphics/window.hpp"
#include "graphics/opengl/opengl.hpp"
#include "graphics/opengl/opengl_functions_core.hpp"
#include "graphics/opengl/opengl_texture.hpp"

using KalaHeaders::Log;
using KalaHeaders::LogType;
using KalaHeaders::GlyphHeader;
using KalaHeaders::GlyphTable;
using KalaHeaders::GlyphBlock;

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
	Text* Text::Initialize(
		const string& name,
		u32 windowID,
		u32 glyphIndex,
		u32 fontID,
		const vec2 pos,
		const float rot,
		const vec2 size,
		Widget* parentWidget,
		OpenGL_Texture* texture,
		OpenGL_Shader* shader)
	{
		Window* window = Window::registry.GetContent(windowID);

		if (!window)
		{
			Log::Print(
				"Cannot load text '" + name + "' because its window context is invalid!",
				"TEXT",
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
				"Cannot load text '" + name + "' because its OpenGL context is invalid!",
				"TEXT",
				LogType::LOG_ERROR);

			return nullptr;
		}

		u32 newID = ++globalID;
		unique_ptr<Text> newText = make_unique<Text>();
		Text* textPtr = newText.get();

		textPtr->hierarchy.thisObject = textPtr;

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
			textPtr->hierarchy.SetParent(parentWidget);
		}

		//font is required
		Font* font{};
		if (fontID == 0) 
		{
			Log::Print(
				"Failed to load Text widget '" + name + "' because its Font ID '" + to_string(fontID) + "' is unassigned!",
				"TEXT",
				LogType::LOG_ERROR);

			return nullptr;
		}
		
		font = Font::registry.GetContent(fontID);
		if (!font)
		{
			Log::Print(
				"Failed to load Text widget '" + name + "' because its Font ID '" + to_string(fontID) + "' is invalid!",
				"TEXT",
				LogType::LOG_ERROR);

			return nullptr;
		}
		
		textPtr->fontID = fontID;

		const GlyphHeader& header = font->GetGlyphHeader();
		vector<GlyphBlock> glyphs = font->GetGlyphBlocks();
		if (glyphIndex >= glyphs.size())
		{
			Log::Print(
				"Failed to load Text widget '" + name + "' because the Glyph index '" + to_string(glyphIndex) + "' is out of range!",
				"TEXT",
				LogType::LOG_ERROR);

			return nullptr;
		}
		
		const auto& glyph = glyphs[glyphIndex];
		
		glGenTextures(1, &textPtr->textureID);
		glBindTexture(GL_TEXTURE_2D, textPtr->textureID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			glyph.width,
			glyph.height,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			glyph.rawPixels.data());
		
		vector<vec2> verts{};
		verts.push_back(vec2(glyph.vertices[0][0], glyph.vertices[0][1]));
		verts.push_back(vec2(glyph.vertices[1][0], glyph.vertices[1][1]));
		verts.push_back(vec2(glyph.vertices[2][0], glyph.vertices[2][1]));
		verts.push_back(vec2(glyph.vertices[3][0], glyph.vertices[3][1]));
		
		vector<u32> inds{};
		inds.push_back(static_cast<u32>(header.indices[0]));
		inds.push_back(static_cast<u32>(header.indices[1]));
		inds.push_back(static_cast<u32>(header.indices[2]));
		inds.push_back(static_cast<u32>(header.indices[3]));
		inds.push_back(static_cast<u32>(header.indices[4]));
		inds.push_back(static_cast<u32>(header.indices[5]));
		
		vector<u32> uvs{};
		uvs.push_back(static_cast<u32>(header.uvs[0][0]));
		uvs.push_back(static_cast<u32>(header.uvs[0][1]));
		uvs.push_back(static_cast<u32>(header.uvs[1][0]));
		uvs.push_back(static_cast<u32>(header.uvs[1][1]));
		uvs.push_back(static_cast<u32>(header.uvs[2][0]));
		uvs.push_back(static_cast<u32>(header.uvs[2][1]));
		uvs.push_back(static_cast<u32>(header.uvs[3][0]));
		uvs.push_back(static_cast<u32>(header.uvs[3][1]));
		
		textPtr->render.vertices = verts;
		textPtr->render.indices = inds;

		Widget::CreateWidgetGeometry(
			textPtr->render.vertices,
			textPtr->render.indices,
			uvs,
			textPtr->render.VAO,
			textPtr->render.VBO,
			textPtr->render.EBO);

		textPtr->transform = Transform2D::Initialize();

		textPtr->ID = newID;
		textPtr->windowID = windowID;
		textPtr->SetName(name);
		textPtr->render.canUpdate = true;
		textPtr->transform->SetPos(pos, PosTarget::POS_WORLD);
		textPtr->transform->SetRot(rot, RotTarget::ROT_WORLD);
		textPtr->transform->SetSize(size, SizeTarget::SIZE_WORLD);

		textPtr->isInitialized = true;

		registry.AddContent(newID, move(newText));
		runtimeText.push_back(textPtr);
		window->AddValue(TargetType::TYPE_WIDGET, newID);

		Log::Print(
			"Loaded text '" + name + "' with ID '" + to_string(newID) + "'!",
			"TEXT",
			LogType::LOG_SUCCESS);

		return textPtr;
	}

	void Text::SetFontID(u32 newValue)
	{
		if (newValue == 0)
		{
			Log::Print(
				"Cannot set Text widget '" + name + "' font to Font ID because it is empty!",
				"TEXT",
				LogType::LOG_ERROR);

			return;
		}

		Font* font = Font::registry.GetContent(fontID);
		if (font) fontID = newValue;
	}

	bool Text::Render(const mat4& projection)
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

		if (!render.shader->Bind())
		{
			Log::Print(
				"Failed to render Text widget '" + name + "' because its shader '" + render.shader->GetName() + "' failed to bind!",
				"IMAGE",
				LogType::LOG_ERROR);

			return false;
		}

		u32 programID = render.shader->GetProgramID();

		vec2 pos = transform->GetPos(PosTarget::POS_COMBINED);
		float rot = transform->GetRot(RotTarget::ROT_COMBINED);
		vec2 size = transform->GetSize(SizeTarget::SIZE_COMBINED);

		mat4 model = createumodel(pos, rot, size);

		render.shader->SetMat4(programID, "uModel", model);
		render.shader->SetMat4(programID, "uProjection", projection);

		bool isOpaque = render.opacity = 1.0f;
		bool isTransparentTexture = 
			render.texture
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
		else if (textureID != 0)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textureID);
			render.shader->SetInt(programID, "uTexture", 0);
			render.shader->SetBool(programID, "uUseTexture", true);
		}
		else render.shader->SetBool(programID, "uUseTexture", false);

		glBindVertexArray(render.VAO);
		glDrawElements(
			GL_TRIANGLES,
			render.indices.size(),
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

	Text::~Text()
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
		hierarchy.RemoveParent();

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