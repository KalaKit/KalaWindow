//------------------------------------------------------------------------------
// log_utils.hpp
//
// Copyright (C) 2025 Lost Empire Entertainment
//
// This is free source code, and you are welcome to redistribute it under certain conditions.
// Read LICENSE.md for more information.
//
// Provides:
//   - Detailed logger - time, date, log type, origin tag
//   - Simple logger - just a cout to the console with a single string parameter
//   - Log types - info (no log type stamp), debug (skipped in release), success, warning, error
//   - Time stamp, date stamp accurate to system clock
//------------------------------------------------------------------------------

#pragma once

#include <string>
#include <sstream>
#include <iostream>
#include <chrono>

namespace KalaHeaders
{
	using std::string;
	using std::ostringstream;
	using std::cout;
	using std::clog;
	using std::cerr;
	using std::chrono::system_clock;
	using std::chrono::duration_cast;
	using std::chrono::milliseconds;
	using std::setfill;
	using std::setw;
	using std::put_time;
	using std::localtime;

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

	class Log
	{
	public:
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
		static TimeFormat GetDefaultTimeFormat()
		{
			return defaultTimeFormat;
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
		static DateFormat GetDefaultDateFormat()
		{
			return defaultDateFormat;
		}

		//Returns current time in chosen or default format.
		static const string& GetTime(TimeFormat timeFormat = TimeFormat::TIME_DEFAULT)
		{
			static string result{};

			if (timeFormat == TimeFormat::TIME_NONE
				|| (timeFormat == TimeFormat::TIME_DEFAULT
					&& defaultTimeFormat == TimeFormat::TIME_NONE))
			{
				return result;
			}
			if (timeFormat == TimeFormat::TIME_DEFAULT)
			{
				return GetTime(defaultTimeFormat);
			}

			auto now = system_clock::now();
			auto in_time_t = system_clock::to_time_t(now);
			auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

			ostringstream ss{};
			switch (timeFormat)
			{
			default: break;
			case TimeFormat::TIME_HMS:
				ss << put_time(localtime(&in_time_t), "%H:%M:%S");
				break;
			case TimeFormat::TIME_HMS_MS:
				ss << put_time(localtime(&in_time_t), "%H:%M:%S")
					<< ':'
					<< setw(3)
					<< setfill('0')
					<< ms.count();
				break;
			case TimeFormat::TIME_12H:
				ss << put_time(localtime(&in_time_t), "%I:%M:%S %p");
				break;
			case TimeFormat::TIME_ISO_8601:
				ss << put_time(localtime(&in_time_t), "%H:%M:%S") << "Z";
				break;
			case TimeFormat::TIME_FILENAME:
				ss << put_time(localtime(&in_time_t), "%H-%M-%S");
				break;
			case TimeFormat::TIME_FILENAME_MS:
				ss << put_time(localtime(&in_time_t), "%H-%M-%S")
					<< '-'
					<< setw(3)
					<< setfill('0')
					<< ms.count();
				break;
			}

			result = ss.str();
			return result;
		}
		//Returns current date in chosen or default format.
		static const string& GetDate(DateFormat dateFormat = DateFormat::DATE_DEFAULT)
		{
			static string result{};

			if (dateFormat == DateFormat::DATE_NONE
				|| (dateFormat == DateFormat::DATE_DEFAULT
					&& defaultDateFormat == DateFormat::DATE_NONE))
			{
				return result;
			}
			if (dateFormat == DateFormat::DATE_DEFAULT)
			{
				return GetDate(defaultDateFormat);
			}

			auto now = system_clock::now();
			auto in_time_t = system_clock::to_time_t(now);

			ostringstream ss{};

			switch (dateFormat)
			{
			default: break;
			case DateFormat::DATE_DMY:
				ss << put_time(localtime(&in_time_t), "%d/%m/%Y");
				break;
			case DateFormat::DATE_MDY:
				ss << put_time(localtime(&in_time_t), "%m/%d/%Y");
				break;
			case DateFormat::DATE_ISO_8601:
				ss << put_time(localtime(&in_time_t), "%Y-%m-%d");
				break;
			case DateFormat::DATE_TEXT_DMY:
				ss << put_time(localtime(&in_time_t), "%d %B, %Y");
				break;
			case DateFormat::DATE_TEXT_MDY:
				ss << put_time(localtime(&in_time_t), "%B %d, %Y");
				break;
			case DateFormat::DATE_FILENAME_DMY:
				ss << put_time(localtime(&in_time_t), "%d-%B-%Y");
				break;
			case DateFormat::DATE_FILENAME_MDY:
				ss << put_time(localtime(&in_time_t), "%B-%d-%Y");
				break;
			}
			result = ss.str();

			return result;
		}

		//Prints a log message to the console using
		//cout, clog or cerr depending on the log type.
		//A newline is added automatically so std::endline or \n is not needed.
		//Full result: [DATE | TIME] [ TYPE | TARGET ] MESSAGE
		//  - message: the actual message of this log
		//  - target: name of the namespace, class, function or variable of this log
		//  - type: sets the tag type, LOG_INFO has no tag
		//  - indentation: optional leading space count in after time and date stamp
		//  - timeFormat: optional time stamp
		//  - dateFormat: optional date stamp
		static void Print(
			const string& message,
			const string& target,
			LogType type,
			unsigned int indentation = 0,
			TimeFormat timeFormat = TimeFormat::TIME_DEFAULT,
			DateFormat dateFormat = DateFormat::DATE_DEFAULT)
		{
#ifndef _DEBUG
			if (type == LogType::LOG_DEBUG) return;
#endif

			if (message.empty())
			{
				Print(
					"Cannot write a log message with no message!",
					"LOG",
					LogType::LOG_ERROR,
					2);
				return;
			}
			if (target.empty())
			{
				Print(
					"Cannot write a log message with no target!",
					"LOG",
					LogType::LOG_ERROR,
					2);
				return;
			}

			string safeMessage = message;
			string safeTarget = target;
			if (message.length() > 1000)
			{
				Print(
					"Log message length is too long! Message was cut off after 1000 characters.",
					"LOG",
					LogType::LOG_WARNING);
				safeMessage = safeMessage.substr(0, 997) + "...";
			}
			if (target.length() > 20)
			{
				Print(
					"Log target length is too long! Target was cut off after 20 characters.",
					"LOG",
					LogType::LOG_WARNING);
				safeTarget = safeTarget.substr(0, 17) + "...";
			}

			string fullMessage = "[ ";

			string indent(indentation, ' ');

			if (dateFormat != DateFormat::DATE_NONE
				&& defaultDateFormat != DateFormat::DATE_NONE)
			{
				string dateStamp = GetDate(dateFormat);
				fullMessage += dateStamp + " | ";
			}

			if (timeFormat != TimeFormat::TIME_NONE
				&& defaultTimeFormat != TimeFormat::TIME_NONE)
			{
				string timeStamp = GetTime(timeFormat);
				if (!indent.empty()) fullMessage += timeStamp + " ]" + indent + "[ ";
				else fullMessage += timeStamp + " ] [ ";
			}
			else
			{
				if (!indent.empty()) fullMessage += indent + "[ ";
				else fullMessage += " [";
			}

			string logType{};
			switch (type)
			{
			case LogType::LOG_DEBUG:
				fullMessage += "DEBUG | ";
				break;
			case LogType::LOG_SUCCESS:
				fullMessage += "SUCCESS | ";
				break;
			case LogType::LOG_WARNING:
				fullMessage += "WARNING | ";
				break;
			case LogType::LOG_ERROR:
				fullMessage += "ERROR | ";
				break;
			}

			fullMessage += target + " ] " + message + "\n";

			switch (type)
			{
			case LogType::LOG_ERROR:
				cerr << fullMessage;
				break;
			case LogType::LOG_WARNING:
			case LogType::LOG_DEBUG:
				clog << fullMessage;
				break;
			case LogType::LOG_INFO:
			case LogType::LOG_SUCCESS:
			default:
				cout << fullMessage;
				break;
			}
		}

		//Overload for the core print function.
		//This one simply prints the message to cout with a newline at the end.
		static void Print(const string& message)
		{
			if (message.empty())
			{
				Print(
					"Cannot write a log message with no message!",
					"LOG",
					LogType::LOG_ERROR,
					2);
				return;
			}

			string safeMessage = message;
			if (message.length() > 1000)
			{
				Print(
					"Log message length is too long! Message was cut off after 1000 characters.",
					"LOG",
					LogType::LOG_DEBUG);
				safeMessage = safeMessage.substr(0, 997) + "...";
			}


			cout << safeMessage << "\n";
		}
	private:
		static inline TimeFormat defaultTimeFormat = TimeFormat::TIME_HMS_MS;
		static inline DateFormat defaultDateFormat = DateFormat::DATE_NONE;
	};
}