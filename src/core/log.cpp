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
	void Logger::Print(
		const string& message,
		const string& target,
		LogType type,
		bool addTimeStamp,
		unsigned int indentation)
	{
		if (message.empty())
		{
			Print(
				"Cannot write a log message with no message!",
				"LOG",
				LogType::LOG_ERROR);
			return;
		}
		if (target.empty())
		{
			Print(
				"Cannot write a log message with no target!",
				"LOG",
				LogType::LOG_ERROR);
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
		if (target.length() > 50)
		{
			Print(
				"Log target length is too long! Target was cut off after 50 characters.",
				"LOG",
				LogType::LOG_WARNING);
			safeTarget = safeTarget.substr(0, 47) + "...";
		}

		string fullMessage = "[ ";

		string timeStamp{};
		if (addTimeStamp)
		{
			auto now = system_clock::now();
			auto in_time_t = system_clock::to_time_t(now);
			auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

			stringstream ss{};
			ss << put_time(localtime(&in_time_t), "%H:%M:%S")
				<< ':'
				<< setw(3)
				<< setfill('0')
				<< ms.count();

			timeStamp = ss.str();
			fullMessage += timeStamp;
		}

		if (addTimeStamp) fullMessage += " ] [ ";

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

		fullMessage += target + " ]" + message + "\n";

		if (type == LogType::LOG_ERROR) cerr << fullMessage;
		else cout << fullMessage;
	}
}