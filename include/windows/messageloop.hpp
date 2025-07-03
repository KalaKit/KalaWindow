//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef _WIN32

#pragma once

#include "core/platform.hpp"

namespace KalaWindow::Core
{
	class KALAWINDOW_API MessageLoop
	{
	public:
		static void* WindowProcCallback();
	};
}

#endif //_WIN32