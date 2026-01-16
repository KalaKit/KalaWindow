//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <array>
#include <functional>

#include "KalaHeaders/core_utils.hpp"

namespace KalaWindow::Core
{
	using std::string;
	using std::string_view;
	using std::array;
	using std::function;

	using u16 = uint16_t;

	//Max allowed crash log buffer message length
	constexpr u16 MAX_MESSAGE_LENGTH = 2000;

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

		//Pushes a string of up to max allowed characters characters to the crash log ring buffer.
		//Stores up to 10 messages and overwrites the oldest entries as new ones arrive.
		//Safe for multithreaded pushing.
		static void AppendToCrashLog(string_view message);

		//Returns crash log content so that oldest is at the top and newest at the bottom
		static array<string_view, 10> GetCrashLogContent();
	};
}