//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef __linux__

#include <string>
#include <functional>

#include "core/crash.hpp"

using std::string;
using std::function;

//Whether or not to create a dump file at crash
static bool createDump;

//The name of this program that is displayed in the title of the error popup
static string programName;

//The user-defined function that is called when a crash occurs
static function<void()> userShutdownFunction{};

namespace KalaWindow::Core
{
	void CrashHandler::Initialize(
		const string& programName,
		bool dumpState)
	{
		Log::Print(
			"Initialized crash handler!",
			"CRASH_HANDLER",
			LogType::LOG_SUCCESS);
	}

	void CrashHandler::SetUserCrashFunction(const function<void()>& crashShutdown)
	{
		userShutdownFunction = crashShutdown;
	}
}

#endif //__linux__