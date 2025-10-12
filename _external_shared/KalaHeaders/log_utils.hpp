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
//   - Simple logger - just a fwrite to the console with a single string parameter
//   - Log types - info (no log type stamp), debug (skipped in release), success, warning, error
//   - Time stamp, date stamp accurate to system clock
//------------------------------------------------------------------------------

#pragma once

#include <string>
#include <chrono>
#include <array>

namespace KalaHeaders
{
	using std::string;
	using std::string_view;
	using std::chrono::system_clock;
	using std::chrono::duration_cast;
	using std::chrono::microseconds;
	using std::chrono::milliseconds;
	using std::localtime;
	using std::array;
	using std::fwrite;
	using std::fflush;

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

	struct CachedPrefix
	{
		LogType type{};
		string target{};
		string prefix{};
	};

	class Log
	{
	public:
		static inline void SetDefaultTimeFormat(TimeFormat format)
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
		static inline TimeFormat GetDefaultTimeFormat()
		{
			return defaultTimeFormat;
		}

		static inline void SetDefaultDateFormat(DateFormat format)
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

		//Returns current time in chosen or default format
		static inline const string& GetTime(TimeFormat timeFormat = TimeFormat::TIME_DEFAULT)
		{
			static string cached[static_cast<int>(TimeFormat::TIME_FILENAME_MS) + 1];
			static long long last_ms = -1;

			static const string empty{};

			if (timeFormat == TimeFormat::TIME_NONE
				|| (timeFormat == TimeFormat::TIME_DEFAULT
				&& defaultTimeFormat == TimeFormat::TIME_NONE))
			{
				return empty;
			}
			if (timeFormat == TimeFormat::TIME_DEFAULT)
			{
				return GetTime(defaultTimeFormat);
			}

			const auto now = system_clock::now();
			const auto us_since_epoch = duration_cast<microseconds>(now.time_since_epoch()).count();
			const auto ms_since_epoch = duration_cast<milliseconds>(now.time_since_epoch()).count();
	
			if (ms_since_epoch == last_ms
				&& !cached[static_cast<int>(timeFormat)].empty())
			{
				return cached[static_cast<int>(timeFormat)];
			}

			last_ms = ms_since_epoch;

			const auto in_time_t = system_clock::to_time_t(now);
			const int ms = (us_since_epoch / 1000) % 1000;
			const int us = us_since_epoch % 1000000;

			tm timeInfo{};
			localtime_s(&timeInfo, &in_time_t);

			char buffer[32]{};
			switch (timeFormat)
			{
			case TimeFormat::TIME_HMS:
				strftime(buffer, sizeof(buffer), "%H:%M:%S", &timeInfo);
				break;
			case TimeFormat::TIME_HMS_MS:
				{
					char tmp[16]{};
					strftime(tmp, sizeof(tmp), "%H:%M:%S", &timeInfo);
					snprintf(buffer, sizeof(buffer), "%s:%03d", tmp, ms);
				}
				break;
			case TimeFormat::TIME_12H:
				strftime(buffer, sizeof(buffer), "%I:%M:%S %p", &timeInfo);
				break;
			case TimeFormat::TIME_ISO_8601:
				strftime(buffer, sizeof(buffer), "%H:%M:%SZ", &timeInfo);
				break;
			case TimeFormat::TIME_FILENAME:
				strftime(buffer, sizeof(buffer), "%H-%M-%S", &timeInfo);
				break;
			case TimeFormat::TIME_FILENAME_MS:
				{
					char tmp[16]{};
					strftime(tmp, sizeof(tmp), "%H-%M-%S", &timeInfo);
					snprintf(buffer, sizeof(buffer), "%s:%03d", tmp, ms);
				}
				break;
			default:
				buffer[0] = '\0';
				break;
			}

			cached[static_cast<int>(timeFormat)] = buffer;
			return cached[static_cast<int>(timeFormat)];
		}
		//Returns current date in chosen or default format.
		static inline const string& GetDate(DateFormat dateFormat = DateFormat::DATE_DEFAULT)
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

			const auto now = system_clock::now();
			const auto in_time_t = system_clock::to_time_t(now);

			tm timeInfo{};
			localtime_s(&timeInfo, &in_time_t);

			char buffer[64]{};
			switch (dateFormat)
			{
			case DateFormat::DATE_DMY:
				strftime(buffer, sizeof(buffer), "%d/%m/%Y", &timeInfo);
				break;
			case DateFormat::DATE_MDY:
				strftime(buffer, sizeof(buffer), "%m/%d/%Y", &timeInfo);
				break;
			case DateFormat::DATE_ISO_8601:
				strftime(buffer, sizeof(buffer), "%Y-%m-%d", &timeInfo);
				break;
			case DateFormat::DATE_TEXT_DMY:
				strftime(buffer, sizeof(buffer), "%d %B, %Y", &timeInfo);
				break;
			case DateFormat::DATE_TEXT_MDY:
				strftime(buffer, sizeof(buffer), "%B %d, %Y", &timeInfo);
				break;
			case DateFormat::DATE_FILENAME_DMY:
				strftime(buffer, sizeof(buffer), "%d-%B-%Y", &timeInfo);
				break;
			case DateFormat::DATE_FILENAME_MDY:
				strftime(buffer, sizeof(buffer), "%B-%d-%Y", &timeInfo);
				break;
			default:
				buffer[0] = '\0';
			}

			result.assign(buffer);;
			return result;
		}

		//Prints a log message to the console using fwrite.
		//A newline is added automatically so std::endline or \n is not needed.
		//  - message: the actual message of this log
		//  - target: name of the namespace, class, function or variable of this log
		//  - type: sets the tag type, LOG_INFO has no tag
		//  - indentation: optional leading space count in after time and date stamp
		//  - flush: set to true for crash logs, diagnostics, assertion failures
		//  - timeFormat: optional time stamp
		//  - dateFormat: optional date stamp
		static inline void Print(
			const string& message,
			const string& target,
			LogType type,
			unsigned int indentation = 0,
			bool flush = false,
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

			string_view safeMessage{ message };
			string_view safeTarget{ target };
			if (message.size() > 2046) safeMessage = safeMessage.substr(0, 2046);
			if (target.size() > 20) safeTarget = safeTarget.substr(0, 19);

			const string& timeStamp = (
				timeFormat != TimeFormat::TIME_NONE
				&& defaultTimeFormat != TimeFormat::TIME_NONE)
				? GetTime(timeFormat)
				: "";

			const string& dateStamp = (
				dateFormat != DateFormat::DATE_NONE
				&& defaultDateFormat != DateFormat::DATE_NONE)
				? GetDate(dateFormat)
				: "";

			const string& prefix = GetCachedPrefix(type, safeTarget);

			thread_local array<char, 2048> buf{};
			char* p = buf.data();

			//append [ date ] [ time ]
			if (!dateStamp.empty())
			{
				*p++ = '[';
				*p++ = ' ';
				memcpy(p, dateStamp.data(), dateStamp.size());
				p += dateStamp.size();
				*p++ = ' ';
				*p++ = ']';
				*p++ = ' ';
			}
			if (!timeStamp.empty())
			{
				*p++ = '[';
				*p++ = ' ';
				memcpy(p, timeStamp.data(), timeStamp.size());
				p += timeStamp.size();
				*p++ = ' ';
				*p++ = ']';
				*p++ = ' ';
			}

			//indentation
			if (indentation > 0)
			{
				memset(p, ' ', indentation);
				p += indentation;
			}

			//cached prefix
			memcpy(p, prefix.data(), prefix.size());
			p += prefix.size();

			//message
			memcpy(p, safeMessage.data(), safeMessage.size());
			p += safeMessage.size();

			//newline
			*p++ = '\n';

			FILE* out = (type == LogType::LOG_ERROR)
				? stderr
				: stdout;

			const size_t length = static_cast<size_t>(p - buf.data());
			fwrite(buf.data(), 1, length, out);

			if (flush) fflush(out);
		}

		//Prints a log message to the console using fwrite.
		//A newline is added automatically so std::endline or \n is not needed.
		//  - message: the actual message of this log
		//  - flush: set to true for crash logs, diagnostics, assertion failures
		static inline void Print(
			const string& message,
			bool flush = false)
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

			string_view safeView{ message };
			if (safeView.size() > 2046) safeView = safeView.substr(0, 2046);

			thread_local array<char, 2048> buf{};
			const size_t length = safeView.size();
			const size_t totalLength = length + 1; //+1 for '\n'

			memcpy(buf.data(), safeView.data(), length);
			buf[length] = '\n';

			fwrite(buf.data(), 1, totalLength, stdout);
			if (flush) fflush(stdout);
		}
	private:
		static inline TimeFormat defaultTimeFormat = TimeFormat::TIME_HMS_MS;
		static inline DateFormat defaultDateFormat = DateFormat::DATE_NONE;

		static inline thread_local array<CachedPrefix, 64> prefixCache;
		static inline thread_local size_t prefixCount;

		static constexpr const char* LogTypeTag[] =
		{
			"",           //LOG_INFO
			"DEBUG | ",
			"SUCCESS | ",
			"WARNING | ",
			"ERROR | "
		};

		static inline const string& GetCachedPrefix(
			LogType type,
			string_view target)
		{
			//search existing entries

			for (size_t i = 0; i < prefixCount; ++i)
			{
				const auto& e = prefixCache[i];
				if (e.type == type
					&& e.target == target)
				{
					return e.prefix;
				}
			}

			//not found, make new

			const char* tag = LogTypeTag[static_cast<size_t>(type)];
			const size_t tagLength = strlen(tag);
			const size_t targetLength = target.size();

			//"[ " + tag + target + "] " = 2 + tagLength + targetLength + 2

			string built{};
			built.resize(2 + tagLength + targetLength + 2);

			char* p = built.data();
			p[0] = '[';
			p[1] = ' ';
			memcpy(p + 2, tag, tagLength);
			memcpy(p + 2 + tagLength, target.data(), targetLength);
			p[2 + tagLength + targetLength] = ']';
			p[3 + tagLength + targetLength] = ' ';

			size_t index = (prefixCount < prefixCache.size())
				? prefixCount++
				: (prefixCount++ % prefixCache.size());

			prefixCache[index] = { type, string(target), move(built) };
			return prefixCache[index].prefix;
		}
	};
}