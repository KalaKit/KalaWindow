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
	enum class TimeFormat
	{
		TIME_NONE,
		TIME_HMS,        //23:59:59
		TIME_HMS_MS,     //23:59:59:123
		TIME_12H,        //11:59:59 PM
		TIME_ISO_8601,   //23:59:59Z
		TIME_FILENAME,   //23-59-59
		TIME_FILENAME_MS //23-59-59-123
	};
	enum class DateFormat
	{
		DATE_NONE,
		DATE_DMY,          //31/12/2025
		DATE_MDY,          //12/31/2025
		DATE_ISO_8601,     //2025-12-31
		DATE_TEXT_DMY,     //31 December, 2025
		DATE_TEXT_MDY,     //December 31, 2025
		DATE_FILENAME_DMY, //31-12-2025
		DATE_FILENAME_MDY  //12-31-2025
	};

	class Logger
	{
	public:
		//Returns current time in chosen format.
		static string GetTime(TimeFormat time);
		//Returns current date in chosen format.
		static string GetDate(DateFormat date);

		//Prints a log message to the console using cout or cerr (for LOG_ERROR).
		//Target is the origin, good for tracking which class owns the message type.
		//LogType sets the tag type, LOG_INFO has no tag.
		//DateFormat adds current date.
		//TimeFormat adds current time.
		//Indentation adds extra spaces in front of the message.
		//A newline is added automatically so std::endline or \n is not needed.
		//The full format is this: [DATE | TIME] [ TYPE | TARGET ] MESSAGE
		static void Print(
			const string& message,
			const string& target,
			LogType type,
			unsigned int indentation = 0,
			TimeFormat timeFormat = TimeFormat::TIME_NONE,
			DateFormat dateFormat = DateFormat::DATE_NONE);
	};
}