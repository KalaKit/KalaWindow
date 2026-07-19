//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string_view>
#include <array>
#include <functional>

#include "core_utils.hpp"
#include "core/kw_core.hpp"

namespace KalaWindow::Core
{
	using std::string_view;
	using std::array;
	using std::function;

	using u16 = uint16_t;

	//Max allowed crash log buffer message length
	constexpr u16 MAX_MESSAGE_LENGTH = 2000;

	class LIB_API CrashHandler
	{
	friend KalaWindowCore;
	public:
		//Initialize the crash handler. Always creates a timestamped
		//crash log file at exe root if program crashes.
		//Assign the program name that will be displayed in the crash log,
		//and define __NODUMP__ if you want to disable .dmp file creation
		static void Initialize(string_view programName);

		static bool IsInitialized();
	private:
		static void SetForceCloseContent(string_view title, string_view reason);
	};
}