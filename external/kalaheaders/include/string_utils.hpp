//---------------------------------------------------------------------------
// string_utils.hpp
//
// Copyright (C) 2026 Lost Empire Entertainment
//
// This is free source code, and you are welcome to redistribute it under certain conditions.
// Read LICENSE.md for more information.
//
// Provides:
//   - various string conversions and functions to improve workflow with string operations
//---------------------------------------------------------------------------

#pragma once

//
// SKIP UNSUPPORTED C++ VERSION
//

#if __cplusplus < 202002L
	#error "UNSUPPORTED C++ VERSION! SUPPORTED: C++20 AND ABOVE"
#endif

//
// SKIP UNSUPPORTED PLATFORMS AND ARCHITECTURES
//

#if !defined(K_REDEFINE_GUARD_PLAT_ARCH)
	#define K_REDEFINE_GUARD_PLAT_ARCH

	#if defined(__APPLE__) || \
		defined(__FreeBSD__) || \
		defined(__OpenBSD__) || \
		defined(__NetBSD__) || \
		defined(__DragonFly__) || \
		defined(__CYGWIN__) || \
		defined(__ANDROID__)
		#error "UNSUPPORTED TARGET! SUPPORTED: _WIN32, __linux__"
	#elif !defined(_WIN32) && \
		!defined(__linux__)
		#error "UNSUPPORTED TARGET! SUPPORTED: _WIN32, __linux__"
	#elif !defined(_M_X64) && \
		!defined(__x86_64__)
		#error "UNSUPPORTED ARCHITECTURE! SUPPORTED: x64"
	#endif
#endif

//
// WINDOWS/LINUX MACROS
//

#if !defined(K_REDEFINE_GUARD_WIN_LIN)
	#define K_REDEFINE_GUARD_WIN_LIN

	#if defined(_WIN32)
		//any targeting windows
		#define KWIN_ANY

		//any msvc targeting windows
		#if defined(_MSC_VER)
			#define KWIN_MSVC
		//any posix targeting Windows
		#elif defined(__GNUC__)
			#define KWIN_GNU
		#else
			#error "UNKNOWN COMPILER DETECTED"
		#endif
	#endif

	#if defined(__linux__)
		//any targeting linux
		#define KLIN_ANY

		//any libc targeting linux
		#if defined(__GLIBC__)
			#define KLIN_GNU
		//any musl targeting linux
		#else
			#define KLIN_MUSL
		#endif
	#endif
#endif

//
// DEBUG MACRO
//

#if !defined(K_REDEFINE_GUARD_REL_DEB)
	#define K_REDEFINE_GUARD_REL_DEB

	#if !defined(KDEBUG)
		#if (defined(_MSC_VER) || \
			defined(__MINGW64__)) && \
			defined(_DEBUG)
			#define KDEBUG
		#elif defined(__linux__) && \
			!defined(NDEBUG)
			#define KDEBUG
		#endif
	#endif
#endif

//
// CAST SHORTHANDS
//

#if !defined(K_REDEFINE_GUARD_CASTS)
	#define K_REDEFINE_GUARD_CASTS

	#define rcast reinterpret_cast
	#define scast static_cast
	#define ccast const_cast
#endif

//
// COMPILER MACROS
//

#if !defined(KNORETURN)
	#define KNORETURN [[noreturn]]
#endif

#if !defined(KNODISCARD)
	#define KNODISCARD [[nodiscard]]
#endif

#include <cstdint>

//
// NUMERIC TYPE SHORTHANDS
//

#if !defined(KNUM)
	#define KNUM
	//8-bit unsigned int
	//Min: 0
	//Max: 255
	using u8 = uint8_t;

	//16-bit unsigned int
	//Min: 0
	//Max: 65,535
	using u16 = uint16_t;

	//32-bit unsigned int
	//Min: 0
	//Max: 4,294,967,295
	using u32 = uint32_t;

	//64-bit unsigned int
	//Replaces handles and pointers (uintptr_t)
	//Min: 0
	//Max: 18 quintillion
	using u64 = uint64_t;

	//8-bit int
	//Min: -128
	//Max: 127
	using i8 = int8_t;

	//16-bit int
	//Min: -32,768
	//Max: 32,767
	using i16 = int16_t;

	//32-bit int
	//Min: -2,147,483,648
	//Max: 2,147,483,647
	using i32 = int32_t;

	//64-bit int
	//Min: -9 quintillion
	//Max: 9 quintillion
	using i64 = int64_t;

	//32-bit float
	//6 decimal precision
	using f32 = float;

	//64-bit float
	//15 decimal precision
	using f64 = double;
#endif

#include <vector>
#include <string>
#include <cstring>
#include <algorithm>
#include <ios>
#include <charconv>

namespace KalaHeaders::KalaString
{
	using std::vector;
	using std::string;
	using std::string_view;
	using std::to_string;
	using std::search;
	using std::transform;
	using std::any_of;
	using std::toupper;
	using std::tolower;
	using std::memcpy;
	using std::memset;
	using std::hex;
	using std::dec;
	using std::from_chars;
	using std::from_chars_result;
	using std::to_chars;
	using std::to_chars_result;
	using std::errc;
	using std::error_code;
	using std::make_error_code;

	KNODISCARD
	inline bool IsAlpha(const char c)
	{
		return 
			(c >= 'a' && c <= 'z') 
			|| (c >= 'A' && c <= 'Z');
	}
	KNODISCARD
	inline bool IsNumber(const char c)
	{
		return c >= '0' && c <= '9';
	}
	KNODISCARD
	inline bool IsSpace(const char c) { return c == ' '; }
	KNODISCARD
	inline bool IsWhiteSpace(const char c)
	{
		return
			c == '\t'
			|| c == '\n'
			|| c == '\v'
			|| c == '\f'
			|| c == '\r';
	}

	KNODISCARD
	inline bool ContainsAlpha(string_view value)
	{
		return any_of(
			value.begin(),
			value.end(),
			IsAlpha);
	}
	KNODISCARD
	inline bool ContainsNumber(string_view value)
	{
		return any_of(
			value.begin(),
			value.end(),
			IsNumber);
	}
	KNODISCARD
	inline bool ContainsSpace(string_view value)
	{
		return any_of(
			value.begin(),
			value.end(),
			IsSpace);
	}
	KNODISCARD
	inline bool ContainsWhiteSpace(string_view value)
	{
		return any_of(
			value.begin(),
			value.end(),
			IsWhiteSpace);
	}

	KNODISCARD
	inline bool ContainsSymbol(string_view value)
	{
		return any_of(
			value.begin(),
			value.end(), 
			[](char c)
			{
				return !IsAlpha(c)
					&& !IsNumber(c)
					&& !IsSpace(c);
			});
	}

	//Returns true if string contains any unsafe file characters,
	//Safe: 'A-Z', 'a-z', '0-9', '_', '-', '.'
	KNODISCARD
	inline bool ContainsUnsafeFileChar(string_view value)
	{
		return any_of(
			value.begin(),
			value.end(), 
			[](char c)
			{
				return 
					!(IsAlpha(c)
					|| IsNumber(c)
					|| c == '_'
					|| c == '-'
					|| c == '.');
			});
	}

	//Returns true if the given string is within min and max range
	KNODISCARD
	inline bool IsStringInRange(
		string_view value,
		size_t minLength,
		size_t maxLength)
	{
		size_t bytes{};
		size_t chars{};

		while (bytes < value.size())
		{
			unsigned char c = scast<unsigned char>(value[bytes]);

			size_t charLen = 
				(c < 0x80) ? 1 
				: (c < 0xE0) ? 2 
				: (c < 0xF0) ? 3 
				: 4;
				
			//incomplete char
			if (bytes + charLen > value.size()) return false;
			
			bytes += charLen;
			++chars;

			if (chars > maxLength) return false;
		}

		return chars >= minLength;
	}

	//Casts a base-16 hexadecimal string to a base-10 unsigned decimal number,
	//something a CPU can use, returns error string on failure
	KNODISCARD
	inline string HexToDec(
		string_view inString,
		u64& outValue)
	{
		if (inString.empty()) return "Cannot convert empty string!";

		u64 result{};

		from_chars_result error = from_chars(
			inString.data(), 
			inString.data() + inString.size(), 
			result, 
			16);

		if (error.ec != errc{})
		{
			return error.ec == errc::invalid_argument
				? "HexToDec failed because inString is not a valid hex string!"
				: "HexToDec failed because inString is too large for u64!";
		}

		outValue = result;
		return "";
	}

	//Casts a base-10 unsigned decimal number to a base-16 hexadecimal string,
	//something a human can read, can never realistically fail so there is no error string
	inline void DecToHex(
		u64 inNumber,
		string& outValue)
	{
		char buf[16]{};

		to_chars_result error = to_chars(
			buf,
			buf + sizeof(buf),
			inNumber,
			16);

		outValue = string(buf, error.ptr);
	}
	
	//Copies the value of the origin string_view within the bounds of the target char array,
	//does not fill empty chars after \0, does not mutate output on empty input,
	//returns error string on failure
	template<size_t N>
	KNODISCARD
	inline string StringToCharArray(
		string_view inValue, 
		char (&outValue)[N])
	{
		//skip if there is no input data
		if (inValue.empty()) return "StringToCharArray failed because inValue was empty!";
		
		size_t len = inValue.size();
		if (len >= N) len = N - 1;
		
		memcpy(outValue, inValue.data(), len);
		outValue[len] = '\0';

		return "";
	}
	
	//Fills in remaining space of char array after '\0',
	//does not mutate output if no null terminator exists,
	//returns error string on failure
	template<size_t N>
	KNODISCARD
	inline string ZeroPadCharArray(char(&outValue)[N])
	{
		//find the null terminator
		size_t i{};
		for (; i < N; i++)
		{ 
			if (outValue[i] == '\0') break;
		}
		
		//skip if no null terminator was found
		if (i == N) return "ZeroPadCharArray failed because there was no null terminator!";
		
		//zero-pad everything after the null-terminator
		memset(&outValue[i + 1], 0, N - (i + 1));
	}

	//Check if origin is the same as target, with optional case sensitivity flag,
	//returns error string on failure
	KNODISCARD
	inline string StringsMatch(
		string_view origin,
		string_view target,
		bool& outResult,
		bool ignoreCase = true)
	{
		//return false if origin or target is empty
		if (origin.empty()
			|| target.empty())
		{
			return "StringsMatch failed because origin or target was empty!";
		}

		//case-sensitive compare
		if (!ignoreCase) outResult = origin == target;

		//case-insensitive compare
		if (origin.size() != target.size()) return "";

		for (size_t i = 0; i < origin.size(); ++i)
		{
			if (scast<char>(tolower(scast<unsigned char>(origin[i])))
				!= scast<char>(tolower(scast<unsigned char>(target[i]))))
			{
				return "";
			}
		}

		outResult = true;
		return "";
	}
	
	//Split origin into a vector of chunks between each splitter,
	//keep strings between two tokens as a single string with preserved tokens,
	//returns error string on failure
	KNODISCARD
	inline string TokenizeString(
		string_view origin,
		char token,
		string_view splitter,
		vector<string>& outValue)
	{
		if (origin.empty()) return "TokenizeString failed because origin was empty!";
		
		vector<string> result{};
		string current{};
		bool inQuotes{};
		size_t i = 0;
		
		while (i < origin.size())
		{
			char c = origin[i];
			
			if (c == token)
			{
				inQuotes = !inQuotes;
				current += c; //keep the token itself
				++i;
				continue;
			}
			
			if (!inQuotes
				&& origin.compare(i, splitter.size(), splitter) == 0)
			{
				if (!current.empty())
				{
					result.emplace_back(current);
					current.clear();
				}
				
				i += splitter.size();
				continue;
			}
			
			current += c;
			++i;
		}
		
		if (!current.empty()) result.emplace_back(current);
		outValue = std::move(result);
		
		return "";
	}
	

	//Split origin into a vector of chunks between each splitter,
	//returns error string on failure
	KNODISCARD
	inline string SplitString(
		string_view origin,
		string_view splitter,
		vector<string>& outResult)
	{
		//return nothing if origin is empty
		if (origin.empty()) return "SplitString failed because origin was empty!";

		vector<string> result{};
		size_t start{};
		size_t pos{};

		while ((pos = origin.find(splitter, start)) != string::npos)
		{
			result.emplace_back(origin.substr(start, pos - start));
			start = pos + splitter.length();
		}

		//push the remainder (or whole string if no delimiter was found)
		result.emplace_back(origin.substr(start));

		outResult = std::move(result);
		return "";
	}

	//Join all chunks in parts vector together into a single string
	//and add delimiter after each chunk except the last one,
	//returns error string on failure
	KNODISCARD
	inline string JoinString(
		const vector<string_view>& parts,
		string_view delimiter,
		string& outResult)
	{
		//return nothing if parts vector is empty
		if (parts.empty()) return "JoinString failed because there were no parts to join!";

		size_t totalSize{};

		//reserve space to avoid many reallocations
		for (const auto& part : parts) totalSize += part.size();
		totalSize += delimiter.size() * (parts.size() - 1);

		string result{};
		result.reserve(totalSize);

		for (size_t i = 0; i < parts.size(); ++i)
		{
			result += parts[i];
			if (i + 1 < parts.size()) result += delimiter;
		}

		outResult = std::move(result);
		return "";
	}

	//Remove leading and trailing whitespace characters from origin,
	//returns error string on failure
	KNODISCARD
	inline string TrimString(
		string_view origin,
		string& outResult)
	{
		if (origin.empty()) return "TrimString failed because origin was empty!";

		const char* whitespace = " \t\n\r\f\v";
		size_t start = origin.find_first_not_of(whitespace);

		if (start == string::npos) return "TrimString failed because origin was all whitespace";

		size_t end = origin.find_last_not_of(whitespace);

		outResult = string(origin.substr(start, end - start + 1));
		return "";
	}

	//Remove occurrences of target from origin,
	//if removeAll is true then all found occurences will be removed,
	//returns error string on failure
	KNODISCARD
	inline string RemoveFromString(
		string_view origin,
		string_view target,
		string& outResult,
		bool removeAll = false)
	{
		//return origin if target is empty
		if (target.empty()) return "RemoveFromString failed because target was empty!";

		string result(origin);
		size_t pos = result.find(target);

		//nothing was found, skip further actions
		if (pos == string::npos)
		{
			outResult = std::move(result);
			return "";
		}

		//remove first occurence
		result.erase(pos, target.length());

		//remove all remaining occurences if requested
		if (removeAll)
		{
			while ((pos = result.find(target, pos)) != string::npos)
			{
				result.erase(pos, target.length());
			}
		}

		outResult = std::move(result);
		return "";
	}

	//Replace occurences of target from origin with replacement,
	//if replaceAll is true then all found occurences will be replaced,
	//returns error string on failure
	KNODISCARD
	inline string ReplaceFromString(
		string_view origin,
		string_view target,
		string_view replacement,
		string& outResult,
		bool replaceAll = false)
	{
		//return origin if target is empty
		if (target.empty()) return "ReplaceFromString failed because target was empty!";

		string result(origin);
		size_t pos = result.find(target);

		//nothing was found, skip further actions
		if (pos == string::npos)
		{
			outResult = std::move(result);
			return "";
		}

		//replace first occurence
		result.replace(pos, target.length(), replacement);

		//replace all remaining occurences if requested
		if (replaceAll)
		{
			pos += replacement.length();

			while ((pos = result.find(target, pos)) != string::npos)
			{
				result.replace(pos, target.length(), replacement);
				pos += replacement.length();
			}
		}

		outResult = std::move(result);
		return "";
	}

	//Replaces everything before the end of target with replacer and returns the result,
	//returns error string on failure
	KNODISCARD
	inline string ReplaceBefore(
		string_view origin, 
		string_view target,
		string& outResult,
		string_view replacer = {})
	{
		if (origin.empty()) return "ReplaceBefore failed because origin was empty!";
		if (target.empty()) return "ReplaceBefore failed because target was empty!";

		string result(origin);
		size_t pos = origin.find(target);
		if (pos == string_view::npos)
		{
			outResult = std::move(result);
			return "";
		}

		result.clear();
		result.reserve(replacer.size() + origin.size() - pos - target.size());

		result.append(replacer);
		result.append(origin.substr(pos + target.size()));

		outResult = std::move(result);
		return "";
	}


	//Replaces everything after the start of target with replacer and returns the result,
	//returns error string on failure
	KNODISCARD
	inline string ReplaceAfter(
		string_view origin, 
		string_view target,
		string& outResult,
		string_view replacer = {})
	{
		if (origin.empty()) return "ReplaceAfter failed because origin was empty!";
		if (target.empty()) return "ReplaceAfter failed because target was empty!";

		string result(origin);
		size_t pos = origin.find(target);
		if (pos == string_view::npos)
		{
			outResult = std::move(result);
			return "";
		}

		result.clear();
		result.reserve(pos + replacer.size());

		result.append(origin.substr(0, pos));
		result.append(replacer);

		outResult = std::move(result);
		return "";
	}

	//Returns everything before the end of target,
	//returns error string on failure
	KNODISCARD
	inline string GetBefore(
		string_view origin, 
		string_view target,
		string& outValue)
	{
		if (origin.empty()) return "GetBefore failed because origin was empty!";
		if (target.empty()) return "GetBefore failed because target was empty!";

		string result(origin);
		size_t pos = origin.find(target);
		if (pos == string_view::npos)
		{
			outValue = std::move(result);
			return "";
		}

		outValue = string(origin.substr(0, pos));
		return "";
	}

	//Returns everything after the start of target,
	//returns error string on failure
	KNODISCARD
	inline string GetAfter(
		string_view origin, 
		string_view target,
		string& outValue)
	{
		if (origin.empty()) return "GetAfter failed because origin was empty!";
		if (target.empty()) return "GetAfter failed because target was empty!";

		string result(origin);
		size_t pos = origin.find(target);
		if (pos == string_view::npos)
		{
			outValue = std::move(result);
			return "";
		}

		outValue = string(origin.substr(pos + target.size()));
		return "";
	}

	//Set all letters of this string to uppercase letters,
	//returns error string on failure
	KNODISCARD
	inline string ToUpperString(
		string_view origin,
		string& outValue)
	{
		if (origin.empty()) return "ToUpperString failed because origin was empty!";

		string result = string(origin);

		transform(result.begin(), result.end(), result.begin(),
			[](unsigned char c) 
			{
				return scast<char>(toupper(c));
			});

		outValue = std::move(result);
		return "";
	}

	//Set all letters of this string to lowercase letters,
	//returns error string on failure
	KNODISCARD
	inline string ToLowerString(
		string_view origin,
		string& outValue)
	{
		if (origin.empty()) return "ToUpperString failed because origin was empty!";

		string result = string(origin);

		transform(result.begin(), result.end(), result.begin(),
			[](unsigned char c)
			{
				return scast<char>(tolower(c));
			});

		outValue = std::move(result);
		return "";
	}
}
