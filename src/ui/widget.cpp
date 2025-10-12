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

using KalaHeaders::Log;
using KalaHeaders::LogType;

using KalaWindow::Core::GetValueByID;
using KalaWindow::Core::WindowContent;
using KalaWindow::Core::windowContent;
using KalaWindow::Core::Input;
using KalaWindow::Graphics::Window;
using KalaWindow::Graphics::OpenGL::OpenGL_Context;

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

		WindowContent* content = windowContent[window].get();

		if (content->runtimeWidgets.empty()) return{};

		OpenGL_Context* context = content->glContext.get();

		if (!context
			|| !context->IsInitialized())
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

	void Widget::SetWindowID(u32 newID)
	{
		//skip if ID is empty
		if (newID == 0) return;
		//skip if ID is the same as current
		if (newID == ID) return;

		Window* window = GetValueByID<Window>(newID);

		//skip if ID doesnt lead to a real window
		if (!window) return;

		RemoveAllChildren();
		RemoveParent();

		windowID = newID;
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

		WindowContent* content = windowContent[window].get();

		OpenGL_Context* context = content->glContext.get();
		if (!context
			|| !context->IsInitialized())
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

	Widget::~Widget()
	{
		RemoveAllChildren();
		RemoveParent();

		Log::Print(
			"Destroying widget '" + name + "' with ID '" + to_string(ID) + "' .",
			"WIDGET",
			LogType::LOG_INFO);
	}
}