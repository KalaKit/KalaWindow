//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <functional>

#include "KalaHeaders/core_types.hpp"
#include "glm/glm.hpp"

//forward declare imgui context struct
//so we dont have to include the whole imgui header
struct ImGuiContext;

namespace KalaWindow::UI
{
	using glm::vec2;
	using std::function;

	class DebugUI
	{
	public:
		//Set up ImGui
		static bool Initialize();
		static bool IsInitialized() { return isInitialized; }

		//Get the ImGui context - this is required to be assigned to your ImGui
		//context to ensure everything draws relative to your executable
		static ImGuiContext* GetImGuiContext();

		//Assign the top bar function that should hold all your top bar draw functions
		static void SetTopBarFunction(const function<void()>& newFunction)
		{
			topBarFunction = newFunction;
		}
		static const function<void()>& GetTopBarFunction()
		{
			return topBarFunction;
		}

		//Assign the main function that should hold all your main draw functions
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

		//ImGui main draw loop
		static void Render(u32 windowID);

		//Shut down ImGui
		static void Shutdown();
	private:
		static inline bool isInitialized{};

		static inline function<void()> topBarFunction{};
		static inline function<void()> mainRenderFunction{};
	};
}