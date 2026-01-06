//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <cstring>
#include <string>
#include <array>
#include <functional>
#include <atomic>

#include "KalaHeaders/core_utils.hpp"

namespace KalaWindow::Core
{
	using std::string;
	using std::string_view;
	using std::array;
	using std::function;
	using std::atomic;
	using std::memory_order_relaxed;
	using std::memcpy;

	using u16 = uint16_t;
	using u32 = uint32_t;

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
		static inline void AppendToCrashLog(string_view message)
		{
			static_assert(
				MAX_MESSAGE_LENGTH > 1,
				"Max message length is too small!");

			auto trim = [](string_view s) -> string
				{
					size_t bytes = 0;
					size_t chars = 0;

					while (bytes < s.size()
						&& chars < MAX_MESSAGE_LENGTH)
					{
						unsigned char c = scast<unsigned char>(s[bytes]);

						size_t charLen =
							(c < 0x80) ? 1
							: (c < 0xE0) ? 2
							: (c < 0xF0) ? 3
							: 4;

						if (bytes + charLen > s.size()) break; //incomplete char

						bytes += charLen;
						++chars;
					}

					string result(s.data(), bytes);

					if (bytes < s.size()) result.append("\n[TRIMMED LONG MESSAGE]");

					return result;
				};

			const string trimmed = trim(message);

			const u32 index = crashLogIndex.fetch_add(1, memory_order_relaxed) % 10;

			char* slot = crashLogBuffer[index];

			//copy up to max allowed chars chars (reserve 1 for null terminator)
			const size_t copyLength = trimmed.size() < MAX_MESSAGE_LENGTH - 1 ? trimmed.size() : MAX_MESSAGE_LENGTH - 1;

			memcpy(slot, trimmed.data(), copyLength);

			//explicit null-termination
			slot[copyLength] = '\0';
		}

		//Returns crash log content so that oldest is at the top and newest at the bottom
		static inline array<string_view, 10> GetCrashLogContent()
		{
			array<string_view, 10> content{};

			const u32 head = crashLogIndex.load(memory_order_relaxed);

			for (u32 i = 0; i < 10; ++i)
			{
				const u32 index = (head + i) % 10;
				const char* entry = crashLogBuffer[index];

				if (entry[0] != '\0') content[i] = string_view(entry);
			}

			return content;
		}
	private:
		//reserve (10 * MAX_MESSAGE_LENGTH) bytes for the last 10 log messages
		static inline char crashLogBuffer[10][MAX_MESSAGE_LENGTH];
		//Which slot are we currently on
		static inline atomic<u32> crashLogIndex{};
	};
}