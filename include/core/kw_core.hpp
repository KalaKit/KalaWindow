//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <functional>

#include "KalaHeaders/core_utils.hpp"

namespace KalaWindow::Core
{
	using std::string;
	using std::function;

	using u32 = uint32_t;
	using f64 = double;

	enum class ShutdownState
	{
		SHUTDOWN_CLEAN,   //Regular exit (exit)
		SHUTDOWN_CRITICAL //Catastrophic/forced shutdown, worst case scenario (abort)
	};

	class LIB_API KalaWindowCore
	{
	public:
		static u32 GetGlobalID();
		static void SetGlobalID(u32 newID);

		static f64 GetDeltaTime();
		static f64 GetFrameTime();

		//Update deltatime and frametime
		static void UpdateDeltaTime();

		//Use this when you absolutely need a hard crash at this very moment.
		//Aborts and doesn't clean up data.
		[[noreturn]] static void ForceClose(
			const string& title,
			const string& reason);

		//Intended to be used for regular shutdown conditions, if program exited
		//with no errors and so on. Called at shutdown stage before any
		//windows or the render pipeline are destroyed.
		static void SetUserShutdownFunction(const function<void()>& regularShutdown);

		/// <summary>
		/// Handles the shutdown conditions of KalaWindow.
		/// </summary>
		/// <param name="state">
		///		Targets either regular exit, terminate or abort
		///		based on ShutdownState enum.
		/// </param>
		/// <param name="useWindowShutdown">
		///		If false, then KalaWindow ShutdownState and its actions are ignored
		///		and user must provide their own setup that is called after all windows and the render pipeline are destroyed.
		/// </param>
		/// <param name="userShutdown">
		///		The function user can optionally pass to KalaWindow shutdown procedure,
		///     called dynamically either before window and render pipeline shutdown
		///     if useWindowShutdown is true, otherwise it is called after.
		/// </param>
		[[noreturn]] static void Shutdown(
			ShutdownState state,
			bool useWindowShutdown = true,
			const function<void()>& userShutdown = nullptr);
	};
}