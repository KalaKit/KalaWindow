//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>

namespace KalaWindow::Core
{
	using std::string;

	enum class LogType
	{
		LOG_INFO,
		LOG_DEBUG,
		LOG_SUCCESS,
		LOG_WARNING,
		LOG_ERROR
	};

	class Logger
	{
		//Prints a log message to the console using cout or cerr (for LOG_ERROR).
		//Target is the origin, good for tracking which class owns the message type.
		//LogType sets the tag type, LOG_INFO has no tag.
		//AddTimeStamp adds an optional timestamp in HH:MM:SS:MS format.
		//Indentation adds extra spaces in front of the message.
		//A newline is added automatically so std::endline or \n is not needed.
		//The full format is this: [TIMESTAMP] [ TYPE | TARGET ] MESSAGE
		static void Print(
			const string& message,
			const string& target,
			LogType type,
			bool addTimeStamp = true,
			unsigned int indentation = 0);
	};
}