//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include "KalaHeaders/log_utils.hpp"

#include "core/input.hpp"
#include "ui/widget.hpp"
#include "ui/text.hpp"
#include "graphics/window.hpp"
#include "graphics/opengl/opengl.hpp"
#include "graphics/opengl/opengl_functions_core.hpp"
#include "ui/image.hpp"
#include "ui/text.hpp"

using KalaHeaders::Log;
using KalaHeaders::LogType;

using KalaWindow::Core::Input;
using KalaWindow::Graphics::Window;
using KalaWindow::Graphics::TargetType;
using KalaWindow::Graphics::OpenGL::OpenGL_Context;
using namespace KalaWindow::Graphics::OpenGLFunctions;

using std::to_string;
using std::back_inserter;

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

			if (mousePos.x >= w->transform.aabb[0].x
				&& mousePos.x <= w->transform.aabb[1].x
				&& mousePos.y >= w->transform.aabb[0].y
				&& mousePos.y <= w->transform.aabb[1].y)
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

	Widget::~Widget() {}
}