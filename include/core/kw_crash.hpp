//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>

#include "core_utils.hpp"

namespace KalaWindow::Graphics
{
	class Window_Global;
}

namespace KalaWindow::Core
{
	using std::string;

	//Max allowed length of crash force close popup reason
	static constexpr size_t MAX_REASON_LENGTH = 256;

	class LIB_API CrashHandler
	{
	friend class KalaWindowCore;
	friend class KalaWindow::Graphics::Window_Global;
	public:
	private:
		static void Initialize();
		KNODISCARD
		static bool IsInitialized();

		static void SetForceCloseContent(
			string&& title, 
			string&& reason);
	};
}