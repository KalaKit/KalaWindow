//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <iostream>
#include <chrono>
#include <ctime>
#include <sstream>

#include "core/log.hpp"

using std::cout;
using std::cerr;
using std::clog;
using std::chrono::system_clock;
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::setfill;
using std::setw;
using std::put_time;
using std::localtime;
using std::stringstream;

namespace KalaWindow::Core
{
	const string& Logger::GetTime(TimeFormat timeFormat)
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

		stringstream ss{};
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

	const string& Logger::GetDate(DateFormat dateFormat)
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

		stringstream ss{};

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

	void Logger::Print(
		const string& message,
		const string& target,
		LogType type,
		unsigned int indentation,
		TimeFormat timeFormat,
		DateFormat dateFormat)
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
			fullMessage += timeStamp + " ] [ ";
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

	void Logger::Print(const string& message)
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
}