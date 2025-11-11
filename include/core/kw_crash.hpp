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

	class LIB_API CrashHandler
	{
	public:
		//Initialize the crash handler. Always creates a timestamped
		//crash log file at exe root if programm crashes.
		//Assign the program name that will be displayed in the crash log,
		//the optional function that will be called for your content that you wanna handle at crash
		//and an optional flag to choose whether or not you want a crash dump
		static void Initialize(
			const string& programName,
			const function<void()>& shutdownFunction = nullptr,
			bool createDump = false);
	};
}