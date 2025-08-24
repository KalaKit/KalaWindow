//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <functional>

#include "KalaHeaders/api.hpp"
#include "KalaHeaders/core_types.hpp"

namespace KalaWindow::Core
{
	//TODO: INTEGRATE KALACRASHHANDLER NATIVELY WITH KALAWINDOW

	using std::string;
	using std::function;

	//Buttons shown on the popup
	enum class PopupAction
	{
		POPUP_ACTION_OK,            // OK button only
		POPUP_ACTION_OK_CANCEL,     // OK and Cancel buttons
		POPUP_ACTION_YES_NO,        // Yes and No buttons
		POPUP_ACTION_YES_NO_CANCEL, // Yes, No, and Cancel buttons
		POPUP_ACTION_RETRY_CANCEL   // Retry and Cancel buttons
	};

	//Icon shown on the popup
	enum class PopupType
	{
		POPUP_TYPE_INFO,    // Info icon (blue 'i')
		POPUP_TYPE_WARNING, // Warning icon (yellow triangle)
		POPUP_TYPE_ERROR,   // Error icon (red X)
		POPUP_TYPE_QUESTION // Question icon (used for confirmations)
	};

	//User response from the popup
	enum class PopupResult
	{
		POPUP_RESULT_NONE,   //No response or unknown
		POPUP_RESULT_OK,     //User clicked OK
		POPUP_RESULT_CANCEL, //User clicked Cancel
		POPUP_RESULT_YES,    //User clicked Yes
		POPUP_RESULT_NO,     //User clicked No
		POPUP_RESULT_RETRY   //User clicked Retry
	};

	enum class ShutdownState
	{
		SHUTDOWN_CLEAN,   //Regular exit (exit)
		SHUTDOWN_CRITICAL //Catastrophic/forced shutdown, worst case scenario (abort)
	};

	class LIB_API KalaWindowCore
	{
	public:
		//Display any kind of a popup on screen for info that should be shown immediately.
		static PopupResult CreatePopup(
			const string& title,
			const string& message,
			PopupAction action,
			PopupType type);

		//Returns Windows version as xxyyyyyy format,
		//where XX is windows version and YYYYYY is build version.
		//Six digits are reserved for build numbers, so builds are 0yyyyy mostly
		static u32 GetVersion();

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