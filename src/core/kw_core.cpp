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

#include <chrono>
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
#include "vulkan/kw_vulkan.hpp"

using KalaHeaders::KalaCore::ToVar;

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
using KalaWindow::Vulkan::Vulkan_Global;

#ifdef __linux__
using std::raise;
#endif

using std::string_view;
using std::chrono::steady_clock;
using std::chrono::time_point;
using std::chrono::duration;
using std::clamp;
using std::exception;

//TODO: define somehow in log_utils.hpp
//CrashHandler::AppendToCrashLog(msg);

namespace KalaWindow::Core
{
	//The ID that is bumped by every object in KalaWindow when it needs a new ID
	static u32 globalID{};

	static f64 deltaTime{};
	static f64 frameTime{};

	static function<void()> shutdownCallback{};

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

	void KalaWindowCore::SetUserShutdownCallback(const function<void()>& shutdown)
	{
		if (shutdown) shutdownCallback = shutdown;
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

	void KalaWindowCore::Shutdown()
	{
		Log::Print(
			"\n======================================================================"
			"\nSHUTTING DOWN"
			"\n======================================================================\n",
			true);
		
#ifdef _WIN32
		timeEndPeriod(1);
#endif //_WIN32

		if (shutdownCallback)
		{
			try
			{
				Log::Print(
					"Running user-provided shutdown callback.\n",
					"KW_CORE",
					LogType::LOG_INFO);

				shutdownCallback();

				Log::Print("\n====================\n");
			}
			catch (const exception& e)
			{
				Log::Print(
					"User-provided shutdown callback failed! Reason: " + string(e.what()),
					"KW_CORE",
					LogType::LOG_ERROR,
					2);
			}
		}
		
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
			if (display) XCloseDisplay(display);
		}
#endif

		Log::Print(
			"KalaWindow has been fully shut down!",
			"KW_CORE",
			LogType::LOG_SUCCESS);

		exit(0);
	}
}