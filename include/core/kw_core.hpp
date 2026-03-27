//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <functional>

#include "core_utils.hpp"

namespace KalaWindow::Core
{
	using std::string;
	using std::string_view;
	using std::function;

	using u32 = uint32_t;
	using f64 = double;

	class LIB_API KalaWindowCore
	{
	public:
		static u32 GetGlobalID();
		static void SetGlobalID(u32 newID);

		static f64 GetDeltaTime();
		static f64 GetFrameTime();

		//Update deltatime and frametime
		static void UpdateDeltaTime();

		static void SetUserShutdownCallback(const function<void()>& shutdown);

		//Use this when you absolutely need a hard crash at this very moment
		static void ForceClose(
			string_view title,
			string_view reason);

		//Close KalaWindow and clean up resources.
		static void Shutdown();
	};
}
