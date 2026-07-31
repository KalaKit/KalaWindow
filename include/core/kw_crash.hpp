//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>

#include "core_utils.hpp"

namespace KalaWindow::Core
{
	//Max allowed length of crash force close popup reason
	constexpr size_t MAX_REASON_LENGTH = 256;

	using std::string;

	class LIB_API CrashHandler
	{
	friend class KalaWindowCore;
	public:
		//Initialize the crash handler. Always creates a timestamped
		//crash log file at exe root if program crashes.
		//Assign the program name that will be displayed in the crash log,
		//and define __NODUMP__ if you want to disable .dmp file creation
		static void Initialize(string&& programName);
		static bool IsInitialized();
	private:
		static void SetForceCloseContent(
			string&& title, 
			string&& reason);
	};
}