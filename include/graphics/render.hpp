//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <vector>

#include "core/platform.hpp"
#include "core/enums.hpp"

namespace KalaWindow::Graphics
{
	using std::string;
	using std::vector;

	enum class ShutdownState
	{
		SHUTDOWN_CLEAN,   //Regular exit (exit)
		SHUTDOWN_FAILURE, //Problem detected, controlled shutdown (terminate)
		SHUTDOWN_CRITICAL //Catastrophic/forced shutdown, worst case scenario (abort)
	};

	class KALAWINDOW_API Render
	{
	public:
		static inline DebugType debugType;

		static bool Initialize();

		//The core shutdown function, call this
		//to trigger an immediate shutdown.
		//Turn emergencyShutdown true only if you have a known crash location.
		static void Shutdown(ShutdownState state);
	};
}