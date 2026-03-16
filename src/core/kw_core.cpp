//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef _WIN32
#include <windows.h>
#include <mmsystem.h>
#else
#include <csignal>
#include <X11/Xlib.h>
#endif

#include <functional>
#include <chrono>
#include <string>
#include <algorithm>

#include "core_utils.hpp"
#include "log_utils.hpp"

#ifdef __linux__
#include "graphics/kw_window_global.hpp"
#endif

#include "core/kw_core.hpp"
#include "core/kw_crash.hpp"
#include "core/kw_input.hpp"
#include "graphics/kw_window.hpp"
#include "graphics/kw_menubar_windows.hpp"
#include "opengl/kw_opengl.hpp"
#include "vulkan/kw_vulkan.hpp"

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;
using KalaHeaders::KalaLog::TimeFormat;
using KalaHeaders::KalaLog::DateFormat;

#ifdef _WIN32
using KalaWindow::Graphics::MenuBar;
#endif

#ifdef __linux__
using KalaWindow::Graphics::Window_Global;
using KalaWindow::Graphics::X11GlobalData;
#endif

using KalaWindow::Graphics::ProcessWindow;
using KalaWindow::OpenGL::OpenGL_Global;
using KalaWindow::Vulkan::Vulkan_Global;

#ifdef __linux__
using std::raise;
#endif

using std::string_view;
using std::function;
using std::exception;
using std::to_string;
using std::chrono::steady_clock;
using std::chrono::time_point;
using std::chrono::duration;
using std::clamp;

static function<void()> userRegularShutdown;

//TODO: define somehow in log_utils.hpp
//CrashHandler::AppendToCrashLog(msg);

namespace KalaWindow::Core
{
	//The ID that is bumped by every object in KalaWindow when it needs a new ID
	static inline u32 globalID{};

	static inline f64 deltaTime{};
	static inline f64 frameTime{};

	u32 KalaWindowCore::GetGlobalID() { return globalID; }
	void KalaWindowCore::SetGlobalID(u32 newID) { globalID = newID; }

	f64 KalaWindowCore::GetDeltaTime() { return deltaTime; }
	f64 KalaWindowCore::GetFrameTime() { return frameTime; }

	void KalaWindowCore::UpdateDeltaTime()
	{
		auto now = steady_clock::now();
		static time_point<steady_clock> lastFrameTime = now;

		duration<f64> delta = now - lastFrameTime;
		lastFrameTime = now;

		//unscaled, unclamped
		frameTime = delta.count();

		//regular deltatime
		deltaTime = clamp(delta.count(), 0.0, 0.1);
	}

	void KalaWindowCore::ForceClose(
		string_view target,
		string_view reason)
	{
		Log::Print(
			"\n================"
			"\nFORCE CLOSE"
			"\n================\n",
			true);

		Log::Print(
			string(reason),
			string(target),
			LogType::LOG_ERROR,
			2,
			true,
			TimeFormat::TIME_NONE,
			DateFormat::DATE_NONE);

		CrashHandler::SetForceCloseContent(
			string(target),
			string(reason));

#ifdef _WIN32
		__debugbreak();
#else
		raise(SIGTRAP);
#endif
	}

	void KalaWindowCore::SetUserShutdownFunction(const function<void()>& regularShutdown)
	{
		userRegularShutdown = regularShutdown;
	}

	void KalaWindowCore::Shutdown(
		ShutdownState state,
		bool useWindowShutdown,
		const function<void()>& userShutdown)
	{
		Log::Print(
			"\n============="
			"\nSHUTTING DOWN"
			"\n=============\n",
			true);
		
#ifdef _WIN32
		timeEndPeriod(1);
#endif //_WIN32

		if (state == ShutdownState::SHUTDOWN_CRITICAL)
		{
#ifdef _DEBUG
			//skip all cleanup if a critical error was detected, we have no time to waste with cleanup here
			__debugbreak();
#else
			//clean, user-friendly exit in release
			quick_exit(EXIT_FAILURE);  
#endif
		}

		if (userRegularShutdown)
		{
			try
			{
				Log::Print(
					"Attempting to run user provided regular shutdown function...\n",
					"WINDOW",
					LogType::LOG_INFO);

				userRegularShutdown();

				Log::Print("\n====================\n");
			}
			catch (const exception& e)
			{
				Log::Print(
					"User-provided regular shutdown condition failed! Reason: " + string(e.what()),
					"WINDOW",
					LogType::LOG_ERROR,
					2);
			}
		}
		
		if (OpenGL_Global::IsInitialized()) OpenGL_Global::Shutdown();
		if (Vulkan_Global::IsInitialized()) Vulkan_Global::Shutdown();

		Input::GetRegistry().RemoveAllContent();
#ifdef _WIN32
		MenuBar::GetRegistry().RemoveAllContent();
#endif
		ProcessWindow::GetRegistry().RemoveAllContent();

#ifdef __linux__
		const X11GlobalData& globalData = Window_Global::GetGlobalData();
		if (globalData.display)
		{
			XIM xim = ToVar<XIM>(globalData.xim);
			XCloseIM(xim);

			Display* display = ToVar<Display*>(globalData.display);
			XCloseDisplay(display);
		}
#endif

		if (!useWindowShutdown
			&& userShutdown)
		{
			try
			{
				Log::Print("\n====================\n");

				Log::Print(
					"Attempting to run user provided post-render shutdown function...\n",
					"WINDOW",
					LogType::LOG_INFO);

				userShutdown();

				Log::Print("\n====================\n");
			}
			catch (const exception& e)
			{
				Log::Print(
					"User-provided post-render shutdown condition failed! Reason: " + string(e.what()),
					"WINDOW",
					LogType::LOG_ERROR,
					2);
			}
		}

		Log::Print(
			"KalaWindow shutting down with state = " + to_string(scast<int>(state)),
			"WINDOW",
			LogType::LOG_SUCCESS);

		if (useWindowShutdown) exit(0);
	}
}