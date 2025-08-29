//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <functional>

#include "KalaHeaders/core_types.hpp"
#include "glm/glm.hpp"

namespace KalaWindow::UI
{
	using glm::vec2;
	using std::function;

	class DebugUI
	{
	public:
		static bool Initialize();
		static bool IsInitialized() { return isInitialized; }

		static void SetTopBarFunction(const function<void()>& newFunction)
		{
			topBarFunction = newFunction;
		}
		static const function<void()>& GetTopBarFunction()
		{
			return topBarFunction;
		}

		static void SetMainRenderFunction(const function<void()>& newFunction)
		{
			mainRenderFunction = newFunction;
		}
		static const function<void()>& GetMainRenderFunction()
		{
			return mainRenderFunction;
		}

		static vec2 CenterWindow(
			vec2 size,
			u32 windowID);

		static void Render(u32 windowID);

		static void Shutdown();
	private:
		static inline bool isInitialized{};

		static inline function<void()> topBarFunction{};
		static inline function<void()> mainRenderFunction{};
	};
}