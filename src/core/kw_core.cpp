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
#include <linux/limits.h>
#endif

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

namespace KalaWindow::Core
{
	//The ID that is bumped by every object in KalaWindow when it needs a new ID
	static u32 globalID{};

	u32 KalaWindowCore::GetGlobalID() { return globalID; }
	void KalaWindowCore::SetGlobalID(u32 newID) { globalID = newID; }

	path KalaWindowCore::GetExePath()
	{
		path exePath{};

#ifdef _WIN32
		wchar_t buffer[MAX_PATH]{};
		DWORD length = GetModuleFileNameW(
			nullptr,
			buffer,
			MAX_PATH);

		if (length > 0
			&& length < MAX_PATH)
		{	
			exePath = path(buffer);
		}
		else
		{
			ForceClose(
				"KalaWindow core error",
				"Failed to get path to executable!");
		}
#else
		char buffer[PATH_MAX]{};
		ssize_t length = readlink(
			"/proc/self/exe",
			buffer,
			sizeof(buffer) - 1);

		if (length > 0)
		{
			buffer[length] = '\0';
			exePath = path(buffer);
		}
		else
		{
			ForceClose(
				"KalaWindow core error",
				"Failed to get path to executable!");
		}
#endif
		return exePath;
	}

	void KalaWindowCore::ForceClose(
		string&& target,
		string&& reason)
	{
		Log::Print(
			"\n================"
			"\nFORCE CLOSE"
			"\n================\n",
			true);

		Log::Print(
			reason,
			target,
			LogType::LOG_ERROR,
			2,
			true,
			TimeFormat::TIME_NONE,
			DateFormat::DATE_NONE);

		CrashHandler::SetForceCloseContent(
			std::move(target),
			std::move(reason));

#ifdef _WIN32
		__debugbreak();
#else
		raise(SIGTRAP);
#endif
	}
}