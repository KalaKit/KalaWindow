//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <functional>

#include "KalaHeaders/core_types.hpp"
#include "KalaHeaders/api.hpp"
#include "glm/glm.hpp"

//forward declare imgui context struct
//so we dont have to include the whole imgui header
struct ImGuiContext;

namespace KalaWindow::UI
{
	using glm::vec2;
	using std::string;
	using std::function;

	struct WindowSettings
	{
		bool isMovable = true;
		bool isResizable = true;
		bool isCollapsible = true;
		bool hasToolbar = true;
		bool saveSettings = true;
	};

	class LIB_API DebugUI
	{
	public:
		//Set up ImGui
		static bool Initialize(bool enableDocking = true);
		static bool IsInitialized() { return isInitialized; }
		static bool IsDockingEnabled() { return isDockingEnabled; }

		//Assign the top bar function that should hold all your top bar draw functions,
		//does not need to be ran every frame.
		static void SetTopBarFunction(const function<void()>& newFunction)
		{
			topBarFunction = newFunction;
		}
		static const function<void()>& GetTopBarFunction()
		{
			return topBarFunction;
		}

		//Assign the main function that should hold all your main draw functions,
		//does not need to be ran every frame.
		static void SetMainRenderFunction(const function<void()>& newFunction)
		{
			mainRenderFunction = newFunction;
		}
		static const function<void()>& GetMainRenderFunction()
		{
			return mainRenderFunction;
		}

		//Place ImGui window to the center
		static vec2 CenterWindow(
			vec2 size,
			u32 windowID);

		//Renders a regular freeform window that can be 
		//rendered for the whole executable lifetime.
		//Modify the WindowSettings struct bool states to control limitations of this window.
		//Assign a function to control what content is rendered inside this window.
		//Leaving position at 0 moves this ImGui window to the center.
		//Leaving min and max size at 0 adds no size constraints to this ImGui window.
		static void RenderWindow(
			u32 ID,
			WindowSettings settings,
			function<void()> func,
			const string& title,
			vec2 size,
			vec2 pos = vec2(0),
			vec2 minSize = vec2(0),
			vec2 maxSize = vec2(0));

		//Renders a non-movable and non-resizable modal ImGui at the center of the
		//main window that should only appear when conditions are met.
		//Window ID needs to be a valid executable window ID that you created.
		//Assign a function to control what content is rendered inside this window.
		//Leaving size at 0 makes the size default to 300x200.
		//This ImGui window data will not be stored by ImGui.
		static void RenderModalWindow(
			u32 ID,
			u32 windowID,
			function<void()> func,
			const string& title,
			vec2 size = vec2(0));

		//ImGui main draw loop
		static void Render(u32 windowID);

		//Shut down ImGui
		static void Shutdown();
	private:
		static inline bool isInitialized{};
		static inline bool isDockingEnabled{};

		static inline function<void()> topBarFunction{};
		static inline function<void()> mainRenderFunction{};
	};
}