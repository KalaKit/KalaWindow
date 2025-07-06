//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <vector>
#include <functional>

#include "core/platform.hpp"
#include "core/enums.hpp"

namespace KalaWindow::Graphics
{
	using std::string;
	using std::vector;
	using std::function;

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

		/// <summary>
		/// Handles the shutdown conditions of KalaWindow.
		/// </summary>
		/// <param name="state">
		///		Targets either regular exit, terminate or abort
		///		based on ShutdownState enum.
		/// </param>
		/// <param name="useWindowShutdown">
		///		If false, then KalaWindow ShutdownState and its actions are ignored
		///		and user must provide their own setup.
		/// </param>
		/// <param name="userEarlyShutdown">
		///		If true, then user-provided shutdown function userShutdown
		///		is called before the windows and the renderer are destroyed,
		///		otherwise userShutdown is called after them.
		/// </param>
		/// <param name="userShutdown">
		///		The function user can optionally pass to KalaWindow shutdown procedure.
		/// </param>
		static void Shutdown(
			ShutdownState state,
			bool useWindowShutdown = true,
			bool userEarlyShutdown = false,
			function<void()> userShutdown = nullptr);
	};
}