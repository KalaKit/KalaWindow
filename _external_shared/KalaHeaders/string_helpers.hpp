//------------------------------------------------------------------------------
// string_helpers.hpp
//
// Copyright (C) 2025 Lost Empire Entertainment
//
// This is free source code, and you are welcome to redistribute it under certain conditions.
// Read LICENSE.md for more information.
//
// Provides:
//   - Various string helpers to improve workflow with string operations
//------------------------------------------------------------------------------

#pragma once

#include <string>
#include <vector>
#include <algorithm>

namespace KalaHeaders
{
	using std::string;
	using std::vector;
	using std::transform;
	using std::toupper;
	using std::tolower;	

	//Split origin into a vector of chunks between each splitter
	inline vector<string> SplitString(
		const string& origin,
		const string& splitter)
	{
		//return nothing if origin is empty
		if (origin.empty()) return{};

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

		return result;
	}

	//Remove leading and trailing whitespace characters from origin
	inline string TrimString(const string& origin)
	{
		const char* whitespace = " \t\n\r\f\v";
		size_t start = origin.find_first_not_of(whitespace);
		if (start == string::npos) return ""; // all whitespace

		size_t end = origin.find_last_not_of(whitespace);
		return origin.substr(start, end - start + 1);
	}

	//Remove all occurences of target from origin
	inline string RemoveFromString(
		const string& origin,
		const string& target)
	{
		//return origin if target is empty
		if (target.empty()) return origin;

		string result = origin;
		size_t pos{};

		while ((pos = result.find(target, pos)) != string::npos)
		{
			result.erase(pos, target.length());
		}

		return result;
	}

	//Set all letters of this string to uppercase letters
	inline string ToUpperString(const string& origin)
	{
		//return origin if target is empty
		if (origin.empty()) return "";

		string result = origin;

		transform(result.begin(), result.end(), result.begin(),
			[](unsigned char c) 
			{
				return static_cast<char>(toupper(c));
			});

		return result;
	}

	//Set all letters of this string to lowercase letters
	inline string ToLowerString(const string& origin)
	{
		//return origin if target is empty
		if (origin.empty()) return "";

		string result = origin;

		transform(result.begin(), result.end(), result.begin(),
			[](unsigned char c)
			{
				return static_cast<char>(tolower(c));
			});

		return result;
	}

	//Check if origin starts with target
	inline bool StartsWith(
		const string& origin,
		const string& target)
	{
		//can't start with something longer than itself
		if (target.size() > origin.size()) return false;

		return origin.compare(
			0,
			target.size(),
			target) == 0;
	}

	//Check if origin ends with target
	inline bool EndsWith(
		const string& origin,
		const string& target)
	{
		//can't end with something longer than itself
		if (target.size() > origin.size()) return false;

		return origin.compare(
			origin.size() - target.size(),
			target.size(),
			target) == 0;
	}
}