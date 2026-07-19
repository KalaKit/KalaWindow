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

#include "log_utils.hpp"

#include "core/kw_core.hpp"
#include "core/kw_crash.hpp"
#include "core/kw_input.hpp"

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;
using KalaHeaders::KalaLog::TimeFormat;
using KalaHeaders::KalaLog::DateFormat;

#ifdef __linux__
using std::raise;
#endif

using std::string_view;
using std::chrono::steady_clock;
using std::chrono::time_point;
using std::chrono::duration;
using std::clamp;

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
}