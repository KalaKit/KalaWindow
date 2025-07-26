//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>

#include "core/platform.hpp"

namespace KalaWindow::Core
{
	using std::string;

	enum class LogType
	{
		LOG_INFO,    //General-purpose log message, sent to std::cout
		LOG_DEBUG,   //Debugging message, only appears in debug builds, sent to std::clog
		LOG_SUCCESS, //Confirmation that an operation succeeded, sent to std::cout
		LOG_WARNING, //Non-critical issue that should be looked into, sent to std::clog
		LOG_ERROR    //Serious issue or failure, sent to std::cerr
	};
	enum class TimeFormat
	{
		TIME_NONE,
		TIME_DEFAULT,    //Globally defined default time format
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
		DATE_DEFAULT,      //Globally defined default date format
		DATE_DMY,          //31/12/2025
		DATE_MDY,          //12/31/2025
		DATE_ISO_8601,     //2025-12-31
		DATE_TEXT_DMY,     //31 December, 2025
		DATE_TEXT_MDY,     //December 31, 2025
		DATE_FILENAME_DMY, //31-12-2025
		DATE_FILENAME_MDY  //12-31-2025
	};

	class KALAWINDOW_API Logger
	{
	public:
		static TimeFormat GetDefaultTimeFormat()
		{
			return defaultTimeFormat;
		}
		static void SetDefaultTimeFormat(TimeFormat format)
		{
			if (format == TimeFormat::TIME_DEFAULT)
			{
				Print(
					"Cannot set default time format as TIME_DEFAULT!",
					"LOG",
					LogType::LOG_ERROR,
					2);
				return;
			}
			if (format == TimeFormat::TIME_NONE)
			{
				Print(
					"Default time format was set to TIME_NONE!"
					"No timestamps will be printed for logs that use TIME_DEFAULT.",
					"LOG",
					LogType::LOG_WARNING);
			}

			defaultTimeFormat = format;
		}

		static DateFormat GetDefaultDateFormat()
		{
			return defaultDateFormat;
		}
		static void SetDefaultDateFormat(DateFormat format)
		{
			if (format == DateFormat::DATE_DEFAULT)
			{
				Print(
					"Cannot set default date format as DATE_DEFAULT!",
					"LOG",
					LogType::LOG_ERROR,
					2);
				return;
			}
			if (format == DateFormat::DATE_NONE)
			{
				Print(
					"Default DATE format was set to DATE_NONE!"
					"No date stamps will be printed for logs that use DATE_DEFAULT.",
					"LOG",
					LogType::LOG_WARNING);
			}

			defaultDateFormat = format;
		}

		//Returns current time in chosen or default format.
		static const string& GetTime(TimeFormat time = TimeFormat::TIME_DEFAULT);
		//Returns current date in chosen or default format.
		static const string& GetDate(DateFormat date = DateFormat::DATE_DEFAULT);

		//Prints a log message to the console using
		//cout, clog or cerr depending on the log type.
		//A newline is added automatically so std::endline or \n is not needed.
		//Full result: [DATE | TIME] [ TYPE | TARGET ] MESSAGE
		//  - message: the actual message of this log
		//  - target: name of the namespace, class, function or variable of this log
		//  - type: sets the tag type, LOG_INFO has no tag
		//  - indentation: optional leading space count in front of the message
		//  - timeFormat: optional time stamp
		//  - dateFormat: optional date stamp
		static void Print(
			const string& message,
			const string& target,
			LogType type,
			unsigned int indentation = 0,
			TimeFormat timeFormat = TimeFormat::TIME_DEFAULT,
			DateFormat dateFormat = DateFormat::DATE_DEFAULT);

		//Overload for the core print function.
		//This one simply prints the message to cout with a newline at the end.
		static void Print(const string& message);
	private:
		static inline TimeFormat defaultTimeFormat = TimeFormat::TIME_HMS_MS;
		static inline DateFormat defaultDateFormat = DateFormat::DATE_NONE;
	};
}