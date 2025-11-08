//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include "KalaHeaders/log_utils.hpp"

#include "core/core.hpp"
#include "core/input.hpp"
#include "ui/widget.hpp"
#include "ui/text.hpp"
#include "graphics/window.hpp"
#include "graphics/opengl/opengl.hpp"
#include "graphics/opengl/opengl_functions_core.hpp"
#include "ui/image.hpp"

using KalaHeaders::Log;
using KalaHeaders::LogType;

using KalaWindow::Core::KalaWindowCore;
using KalaWindow::Core::Input;
using KalaWindow::Graphics::Window;
using KalaWindow::Graphics::TargetType;
using KalaWindow::Graphics::OpenGL::OpenGL_Context;
using namespace KalaWindow::Graphics::OpenGLFunctions;

using std::to_string;
using std::back_inserter;
using std::min;
using std::max;

namespace KalaWindow::UI
{
	vector<Widget*> Widget::HitWidgets(u32 windowID)
	{
		Window* window = Window::registry.GetContent(windowID);

		if (!window
			|| !window->IsInitialized())
		{
			Log::Print(
				"Cannot get hit widgets because the window is invalid!",
				"WIDGET",
				LogType::LOG_ERROR,
				2);

			return{};
		}

		if (window->IsIdle()) return{};

		vector<OpenGL_Context*> contexts = OpenGL_Context::registry.GetAllWindowContent(windowID);
		OpenGL_Context* context = contexts.empty() ? nullptr : contexts.front();

		if (!context
			|| !context->IsInitialized()
			|| !context->IsContextValid())
		{
			Log::Print(
				"Cannot get hit widgets because the window '" + window->GetTitle() + "' OpenGL context is invalid!",
				"WIDGET",
				LogType::LOG_ERROR,
				2);

			return{};
		}

		vector<Input*> inputs = Input::registry.GetAllWindowContent(windowID);
		Input* input = inputs.empty() ? nullptr : inputs.front();

		if (!input
			|| !input->IsInitialized())
		{
			Log::Print(
				"Cannot get hit widgets because the window '" + window->GetTitle() + "' input context is invalid!",
				"WIDGET",
				LogType::LOG_ERROR,
				2);

			return{};
		}

		vec2 mousePos = input->GetMousePosition();

		//
		// 2D HIT TEST
		//

		vector<Widget*> hitWidgets{};

		vector<Widget*> existingWidgets{};
		vector<Image*> images = Image::registry.GetAllWindowContent(windowID);
		vector<Text*> text = Text::registry.GetAllWindowContent(windowID);

		existingWidgets.reserve(images.size() + text.size());
		std::transform(images.begin(), images.end(), back_inserter(existingWidgets),
			[](Image* i) { return static_cast<Widget*>(i); });
		std::transform(text.begin(), text.end(), back_inserter(existingWidgets),
			[](Text* t) { return static_cast<Widget*>(t); });

		for (auto& w : existingWidgets)
		{
			if (!w->isInteractable) continue;

			if (mousePos.x >= w->render.aabb[0].x
				&& mousePos.x <= w->render.aabb[1].x
				&& mousePos.y >= w->render.aabb[0].y
				&& mousePos.y <= w->render.aabb[1].y)
			{
				hitWidgets.push_back(w);
			}
		}

		sort(hitWidgets.begin(), hitWidgets.end(),
			[](Widget* a, Widget* b)
			{
				return a->zOrder < b->zOrder;
			});

		return hitWidgets;
	}

	bool Widget::IsHovered() const
	{
		if (!isInteractable)
		{
			Log::Print(
				"Cannot check widget '" + name + "' hover state because it is not interactable!",
				"WIDGET",
				LogType::LOG_DEBUG);

			return false;
		}

		if (windowID == 0)
		{
			Log::Print(
				"Cannot check widget '" + name + "' hover state because its window ID is 0!",
				"WIDGET",
				LogType::LOG_ERROR,
				2);

			return false;
		}

		Window* window = Window::registry.GetContent(windowID);

		if (!window
			|| !window->IsInitialized())
		{
			Log::Print(
				"Cannot check widget '" + name + "' hover state because its window is invalid!",
				"WIDGET",
				LogType::LOG_ERROR,
				2);

			return false;
		}

		if (window->IsIdle()) return false;

		vector<OpenGL_Context*> contexts = OpenGL_Context::registry.GetAllWindowContent(windowID);
		OpenGL_Context* context = contexts.empty() ? nullptr : contexts.front();

		if (!context
			|| !context->IsInitialized()
			|| !context->IsContextValid())
		{
			Log::Print(
				"Cannot check widget '" + name + "' hover state because the window '" + window->GetTitle() + "' OpenGL context is invalid!",
				"WIDGET",
				LogType::LOG_ERROR,
				2);

			return false;
		}

		vector<Input*> inputs = Input::registry.GetAllWindowContent(windowID);
		Input* input = inputs.empty() ? nullptr : inputs.front();

		if (!input
			|| !input->IsInitialized())
		{
			Log::Print(
				"Cannot check widget '" + name + "' hover state because the window '" + window->GetTitle() + "' input context is invalid!",
				"WIDGET",
				LogType::LOG_ERROR,
				2);

			return false;
		}

		const vector<Widget*>& hitWidgets = HitWidgets(windowID);

		if (hitWidgets.empty()) return false;

		return this == hitWidgets[0];
	}

	void Widget::PollEvents(Input* input)
	{
		if (!input
			&& !isInteractable)
		{
			return;
		}

		if (event.function_button_pressed
			&& ((event.keyPressed != Key::Unknown
			&& input->IsKeyPressed(event.keyPressed))
			|| (event.mousePressed != MouseButton::Unknown
			&& input->IsMouseButtonPressed(event.mousePressed))))
		{
			event.function_button_pressed();
		}
		if (event.function_button_released
			&& ((event.keyReleased != Key::Unknown
			&& input->IsKeyReleased(event.keyReleased))
			|| (event.mouseReleased != MouseButton::Unknown
			&& input->IsMouseButtonReleased(event.mouseReleased))))
		{
			event.function_button_released();
		}
		if (event.function_button_held
			&& ((event.keyHeld != Key::Unknown
			&& input->IsKeyHeld(event.keyHeld))
			|| (event.mouseHeld != MouseButton::Unknown
			&& input->IsMouseButtonHeld(event.mouseHeld))))
		{
			event.function_button_held();
		}
		if (event.function_mouse_dragged
			&& event.mouseDragged != MouseButton::Unknown
			&& input->IsMouseButtonDragging(event.mouseDragged))
		{
			event.function_mouse_dragged();
		}
		if (event.function_mouse_hovered
			&& IsHovered())
		{
			event.function_mouse_hovered();
		}
		if (event.function_mouse_scrolled)
		{
			float swDelta = input->GetScrollwheelDelta();
			if (swDelta != 0) event.function_mouse_scrolled();
		}
	}

	void Widget::CreateWidgetGeometry(
		const vector<vec2>& vertices,
		const vector<u32>& indices,
		const vector<u32>& uvs,
		u32& vaoOut,
		u32& vboOut,
		u32& eboOut)
	{
		if (vertices.empty()
			|| indices.empty())
		{
			KalaWindowCore::ForceClose(
				"Widget error",
				"Failed to create widget geometry because vertices or indices were unassigned!");	
		}
		
		struct Vertex
		{
			vec2 pos;
			vec2 uv;
		};
		vector<Vertex> verts{};
		
		if (vertices.size() < 4
			|| uvs.size() < 8)
		{
			KalaWindowCore::ForceClose(
				"Widget error",
				"Did not get enough vertices or uvs!");	
		}
	
		verts.resize(vertices.size());
		
		verts[0].pos = vertices[0];
		verts[1].pos = vertices[1];
		verts[2].pos = vertices[2];
		verts[3].pos = vertices[3];
	
		verts[0].uv = vec2(uvs[0], uvs[1]);
		verts[1].uv = vec2(uvs[2], uvs[3]);
		verts[2].uv = vec2(uvs[4], uvs[5]);
		verts[3].uv = vec2(uvs[6], uvs[7]);

		glGenVertexArrays(1, &vaoOut);
		glGenBuffers(1, &vboOut);
		glGenBuffers(1, &eboOut);

		glBindVertexArray(vaoOut);

		//VBO
		glBindBuffer(GL_ARRAY_BUFFER, vboOut);
		glBufferData(
			GL_ARRAY_BUFFER,
			verts.size() * sizeof(Vertex),
			verts.data(),
			GL_STATIC_DRAW);

		//EBO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboOut);
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
	}

	Widget::~Widget() {}
}