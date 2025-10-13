//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include "KalaHeaders/log_utils.hpp"

#include "core/containers.hpp"
#include "ui/widget.hpp"
#include "ui/text.hpp"
#include "graphics/window.hpp"
#include "graphics/opengl/opengl.hpp"
#include "graphics/opengl/opengl_functions_core.hpp"

using KalaHeaders::Log;
using KalaHeaders::LogType;

using KalaWindow::Core::GetValueByID;
using KalaWindow::Core::WindowContent;
using KalaWindow::Core::windowContent;
using KalaWindow::Core::Input;
using KalaWindow::Graphics::Window;
using KalaWindow::Graphics::OpenGL::OpenGL_Context;
using namespace KalaWindow::Graphics::OpenGLFunctions;

using std::to_string;

namespace KalaWindow::UI
{
	vector<Widget*> Widget::HitWidgets(
		u32 windowID,
		const vec3& origin,
		const vec3& target,
		float distance)
	{
		if (windowID == 0) return{};

		Window* window = GetValueByID<Window>(windowID);

		if (!window
			|| !window->IsInitialized()
			|| window->IsIdle())
		{
			return{};
		}

		WindowContent* content{};
		if (windowContent.contains(window))
		{
			content = windowContent[window].get();
		}

		if (!content) return{};

		if (content->runtimeWidgets.empty()) return{};

		OpenGL_Context* context{};
		if (content->glContext) context = content->glContext.get();

		if (!context
			|| !context->IsInitialized()
			|| !context->IsContextValid())
		{
			return{};
		}

		Input* input = content->input.get();

		if (!input
			|| !input->IsInitialized())
		{
			return{};
		}

		vec2 mousePos = input->GetMousePosition();

		vector<Widget*> hitWidgets{};

		if (origin == vec3(0)
			&& target == vec3(0)
			&& distance == 0.0f)
		{
			//
			// 2D HIT TEST
			//
		}
		else
		{
			if (distance <= 0.1f
				|| origin == target)
			{
				return{};
			}

			//
			// 3D HIT TEST
			//
		}
	}

	bool Widget::IsHovered(
		const vec3& origin,
		const vec3& target,
		float distance) const
	{
		if (!isInitialized
			|| !isInteractable
			|| windowID == 0)
		{
			return false;
		}

		Window* window = GetValueByID<Window>(windowID);

		if (!window
			|| !window->IsInitialized()
			|| window->IsIdle())
		{
			return false;
		}

		WindowContent* content{};
		if (windowContent.contains(window))
		{
			content = windowContent[window].get();
		}

		if (!content) return false;

		OpenGL_Context* context{};
		if (content->glContext) context = content->glContext.get();

		if (!context
			|| !context->IsInitialized()
			|| !context->IsContextValid())
		{
			return false;
		}

		Input* input = content->input.get();

		if (!input
			|| !input->IsInitialized())
		{
			return false;
		}

		const vector<Widget*>& hitWidgets = HitWidgets(
			windowID,
			origin, 
			target, 
			distance);

		if (hitWidgets.empty()) return false;

		return this == hitWidgets[0];
	}

	void Widget::Create2DQuad(
		u32& vaoOut,
		u32& vboOut,
		u32& eboOut)
	{
		struct Vertex
		{
			vec2 pos;
			vec2 uv;
		};

		vector<Vertex> vertices =
		{
			{ vec2(-0.5f, -0.5f), vec2(0.0f, 0.0f) }, // bottom-left
			{ vec2(0.5f, -0.5f), vec2(1.0f, 0.0f) },  // bottom-right
			{ vec2(0.5f,  0.5f), vec2(1.0f, 1.0f) },  // top-right
			{ vec2(-0.5f,  0.5f), vec2(0.0f, 1.0f) }  // top-left
		};

		//two triangles forming a quad
		vector<u32> indices = { 0, 1, 2, 2, 3, 0 };

		u32 vao{};
		u32 vbo{};
		u32 ebo{};

		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);

		glBindVertexArray(vao);

		//VBO
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(
			GL_ARRAY_BUFFER,
			vertices.size() * sizeof(Vertex),
			vertices.data(),
			GL_STATIC_DRAW);

		//EBO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(
			GL_ELEMENT_ARRAY_BUFFER,
			indices.size() * sizeof(u32),
			indices.data(),
			GL_STATIC_DRAW);

		//position - layout 0
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(
			0,
			2,
			GL_FLOAT,
			GL_FALSE,
			sizeof(Vertex),
			(void*)offsetof(Vertex, pos));

		//uv - layout 1
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(
			1,
			2,
			GL_FLOAT,
			GL_FALSE,
			sizeof(Vertex),
			(void*)offsetof(Vertex, uv));

		glBindVertexArray(0);

		vaoOut = vao;
		vboOut = vbo;
		eboOut = ebo;
	}

	Widget::~Widget()
	{
		Log::Print(
			"Destroying widget '" + name + "' with ID '" + to_string(ID) + "' .",
			"WIDGET",
			LogType::LOG_INFO);

		RemoveAllChildren();
		RemoveParent();

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