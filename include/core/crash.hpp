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
		//Initialize the crash handler.
		//Assign the program name that will be displayed in the crash log,
		//and an optional flag to choose whether or not you want a crash dump
		static void Initialize(
			const string& programName,
			bool dumpState = false);

		//Intended to be used for emergency shutdown conditions if program crashed
		//and you have critical actions you must always do even at crash
		static void SetUserCrashFunction(const function<void()>& crashShutdown);
	};
}