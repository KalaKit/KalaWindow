//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include "KalaHeaders/log_utils.hpp"

#include "core/containers.hpp"
#include "ui/widget.hpp"
#include "graphics/window.hpp"

using KalaHeaders::Log;
using KalaHeaders::LogType;

using KalaWindow::Core::GetValueByID;
using KalaWindow::Graphics::Window;

using std::to_string;

namespace KalaWindow::UI
{
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