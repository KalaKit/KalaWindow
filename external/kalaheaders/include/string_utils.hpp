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

#include <vector>
#include <string>
#include <cstring>
#include <algorithm>
#include <ios>
#include <charconv>

#include "macros.hpp"

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

	//Returns true if string is valid UTF8
	KNODISCARD
	inline bool IsValidUTF8(string_view value)
	{
		size_t bytes{};

		while (bytes < value.size())
		{
			const u8 c = scast<u8>(value[bytes]);

			size_t charLen{};

			if (c <= 0x7F) charLen = 1;
			else if (c >= 0xC2 && c <= 0xDF) charLen = 2;
			else if (c >= 0xE0 && c <= 0xEF) charLen = 3;
			else if (c >= 0xF0 && c <= 0xF4) charLen = 4;
			else return false;
				
			//incomplete char
			if (charLen > value.size() - bytes) return false;

			//validate continuation bytes
			for (size_t i = 1; i < charLen; ++i)
			{
				const u8 next = scast<u8>(value[bytes + i]);

				if ((next & 0xC0) != 0x80) return false;
			}

			//reject overlong encodings, surrogates and values above U+10FFFF
			if (charLen >= 3)
			{
				const u8 next = scast<u8>(value[bytes + 1]);

				if ((c == 0xE0 && next < 0xA0)
					|| (c == 0xED && next > 0x9F)
					|| (c == 0xF0 && next < 0x90)
					|| (c == 0xF4 && next > 0x8F))
				{
					return false;
				}
			}
			
			bytes += charLen;
		}

		return true;
	}

	//Get the UTF8-safe size of any string, returns error on failure
	KNODISCARD
	inline string GetLength(string_view value, size_t& size)
	{
		if (!IsValidUTF8(value))
		{
			return "Failed to get string length because it is not valid UTF8!";
		}

		size_t length{};

		for (const char c : value)
		{
			const u8 byte = scast<u8>(c);

			//continuation bytes start with binary 10xxxxxx
			if ((byte & 0xC0) != 0x80)
			{
				++length;
			}
		}

		size = length;
		return {};
	}

	//Check if the given string is within min and max range,
	//returns error on failure
	KNODISCARD
	inline string IsStringInRange(
		string_view value,
		size_t minLength,
		size_t maxLength,
		bool& outResult)
	{
		size_t length{};
		string err = GetLength(value, length);

		if (!err.empty()) return err;

		outResult = length >= minLength && length <= maxLength;

		return {};
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
				? "Failed to convert hex to dec because inString is not a valid hex string!"
				: "Failed to convert hex to dec because inString is too large for u64!";
		}

		outValue = result;
		return "";
	}

	//Casts a base-10 unsigned decimal number to a base-16 hexadecimal string,
	//something a human can read, can never realistically fail so there is no error string,
	//set padLeadingZero to true to pad odd-length hexadecimal strings
	//to an even number of digits which is useful for byte-oriented output
	inline void DecToHex(
		u64 inNumber,
		string& outValue,
		bool padLeadingZero = false)
	{
		char buf[16]{};

		to_chars_result error = to_chars(
			buf,
			buf + sizeof(buf),
			inNumber,
			16);

		outValue = string(buf, error.ptr);

		if (padLeadingZero
			&& outValue.size() % 2 != 0)
		{
			outValue.insert(outValue.begin(), '0');
		}
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
		if (inValue.empty()) return "Failed to convert string to char array because input was empty!";
		
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
		if (i == N) return "Failed to zero pad char array because there was no null terminator!";
		
		//zero-pad everything after the null-terminator
		memset(&outValue[i + 1], 0, N - (i + 1));

		return "";
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
		outResult = false;

		if (origin.empty()
			|| target.empty())
		{
			return "Failed to compare strings because origin or target was empty!";
		}

		if (!IsValidUTF8(origin)
			|| !IsValidUTF8(target))
		{
			return "Failed to compare strings because origin or target was not valid UTF8!";
		}

		//case-sensitive compare
		if (!ignoreCase)
		{
			outResult = origin == target;
			return "";
		}

		for (const char c : origin)
		{
			if (scast<unsigned char>(c) > 0x7F)
			{
				return "Failed to compare strings because non-ascii case-insensitive comparison is not supported!";
			}
		}

		for (const char c : target)
		{
			if (scast<unsigned char>(c) > 0x7F)
			{
				return "Failed to compare strings because non-ascii case-insensitive comparison is not supported!";
			}
		}

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
		if (origin.empty()) return "Failed to tokenize string because origin was empty!";
		
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
		if (origin.empty()) return "Failed to split string because origin was empty!";

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
		if (parts.empty()) return "Failed to join string because there were no parts to join!";

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
		if (origin.empty()) return "Failed to trim string because origin was empty!";

		const char* whitespace = " \t\n\r\f\v";
		size_t start = origin.find_first_not_of(whitespace);

		if (start == string::npos) return "Failed to trim string because origin was all whitespace";

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
		if (target.empty()) return "Failed to remove from string because target was empty!";

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
		if (target.empty()) return "Failed to replace from string because target was empty!";

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
		if (origin.empty()) return "Failed to replace before string because origin was empty!";
		if (target.empty()) return "Failed to replace before string because target was empty!";

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
		if (origin.empty()) return "Failed to replace after string because origin was empty!";
		if (target.empty()) return "Failed to replace after string because target was empty!";

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
		if (origin.empty()) return "Failed to get before string because origin was empty!";
		if (target.empty()) return "Failed to get before string because target was empty!";

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
		if (origin.empty()) return "Failed to get after string because origin was empty!";
		if (target.empty()) return "Failed to get after string because target was empty!";

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
		if (origin.empty()) return "Failed to uppercase string because origin was empty!";

		if (!IsValidUTF8(origin))
		{
			return "Failed to uppercase string because origin was not valid UTF8!";
		}

		for (const char c : origin)
		{
			if (scast<unsigned char>(c) > 0x7F)
			{
				return "Failed to uppercase string because non-ascii uppercasing is not supported!";
			}
		}

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
		if (origin.empty()) return "Failed to lowercase string because origin was empty!";

		if (!IsValidUTF8(origin))
		{
			return "Failed to lowercase string because origin was not valid UTF8!";
		}

		for (const char c : origin)
		{
			if (scast<unsigned char>(c) > 0x7F)
			{
				return "Failed to lowercase string because non-ascii lowercasing is not supported!";
			}
		}

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
