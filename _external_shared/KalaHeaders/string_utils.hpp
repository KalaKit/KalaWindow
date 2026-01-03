//------------------------------------------------------------------------------
// string_utils.hpp
//
// Copyright (C) 2026 Lost Empire Entertainment
//
// This is free source code, and you are welcome to redistribute it under certain conditions.
// Read LICENSE.md for more information.
//
// Provides:
//   - Various string conversions and functions to improve workflow with string operations
//------------------------------------------------------------------------------

#pragma once

#include <string>
#include <cstring>
#include <vector>
#include <algorithm>

//static_cast
#ifndef scast
	#define scast static_cast
#endif

namespace KalaHeaders::KalaString
{	
	using std::stoi;
	using std::stoll;
	using std::stoul;
	using std::stoull;
	using std::stod;
	using std::stod;
	using std::stold;

	using std::string;
	using std::string_view;
	using std::vector;
	using std::search;
	using std::transform;
	using std::toupper;
	using std::tolower;
	using std::isdigit;
	using std::isspace;
	using std::memcpy;
	using std::memset;

	//
	// CONVERSION FUNCTIONS
	//

	//Convert T to string
	template<typename T>
	inline string ToString(const T& value) { return std::to_string(value); }

	//Convert bool to 'true' or 'false'
	template<> inline string ToString<bool>(const bool& state) { return state ? "true" : "false"; }

	template<typename T> inline T FromString(const string& s); //Convert string to T

	template<> inline bool               FromString<bool>(const string& s) { return (s == "true" || s == "1"); } //Convert string to bool

	template<> inline int                FromString<int>(const string& s) { return stoi(s); }                  //Convert string to int
	template<> inline long               FromString<long>(const string& s) { return stol(s); }                 //Convert string to long
	template<> inline long long          FromString<long long>(const string& s) { return stoll(s); }           //Convert string to long long
	template<> inline unsigned int       FromString<unsigned int>(const string& s) { return stoul(s); }        //Convert string to unsigned int
	template<> inline unsigned long      FromString<unsigned long>(const string& s) { return stoul(s); }       //Convert string to unsigned long
	template<> inline unsigned long long FromString<unsigned long long>(const string& s) { return stoull(s); } //Convert string to unsigned long long
	template<> inline float              FromString<float>(const string& s) { return stof(s); }                //Convert string to float
	template<> inline double             FromString<double>(const string& s) { return stod(s); }               //Convert string to double
	template<> inline long double        FromString<long double>(const string& s) { return stold(s); }         //Convert string to long double

	//
	// GENERAL FUNCTIONS
	//
	
	//Copies the value of the origin string_view within the bounds of the target char array,
	//does not fill empty chars after \0,
	//does not mutate output on empty input
	template<size_t N>
	inline void StringToCharArray(string_view inValue, char (&outValue)[N])
	{
		//skip if there is no input data
		if (inValue.empty()) return;
		
		size_t len = inValue.size();
		if (len >= N) len = N - 1;
		
		memcpy(outValue, inValue.data(), len);
		outValue[len] = '\0';
	}
	//Copies the value of the origin string within the bounds of the target char array,
	//does not fill empty chars after \0,
	//does not mutate output on empty input
	template<size_t N>
	inline void StringToCharArray(const string& inValue, char (&outValue)[N])
	{
		//skip if there is no input data
		if (inValue.empty()) return;
		
		size_t len = inValue.size();
		if (len >= N) len = N - 1;
		
		memcpy(outValue, inValue.data(), len);
		outValue[len] = '\0';
	}
	
	//Fills in remaining space of char array after '\0',
	//does not mutate output if no null terminator exists
	template<size_t N>
	inline void ZeroPadCharArray(char(&outValue)[N])
	{
		//find the null terminator
		size_t i = 0;
		for (; i < N; i++) { if (outValue[i] == '\0') break; }
		
		//skip if no null terminator was found
		if (i == N) return;
		
		//zero-pad everything after the null-terminator
		memset(&outValue[i + 1], 0, N - (i + 1));
	}

	//Check if origin contains target, with optional case sensitivity flag
	inline bool ContainsString(
		const string& origin,
		const string& target,
		bool ignoreCase = true)
	{
		//return false if origin or target is empty
		if (origin.empty()
			|| target.empty())
		{
			return false;
		}

		//can't contain something longer than itself
		if (target.size() > origin.size()) return false;

		//case-sensitive search
		if (!ignoreCase) return origin.find(target) != string::npos;

		//case-insensitive search
		auto it = search(
			origin.begin(), 
			origin.end(),
			target.begin(), 
			target.end(),
			[](unsigned char char1, unsigned char char2)
			{
				return tolower(char1) == tolower(char2);
			});

		return it != origin.end();
	}

	//Check if origin is the same as target, with optional case sensitivity flag
	inline bool CompareStrings(
		const string& origin,
		const string& target,
		bool ignoreCase = true)
	{
		//return false if origin or target is empty
		if (origin.empty()
			|| target.empty())
		{
			return false;
		}

		//case-sensitive compare
		if (!ignoreCase) return origin == target;

		//case-insensitive compare
		if (origin.size() != target.size()) return false;

		for (size_t i = 0; i < origin.size(); ++i)
		{
			if (scast<char>(tolower(scast<unsigned char>(origin[i])))
				!= scast<char>(tolower(scast<unsigned char>(target[i]))))
			{
				return false;
			}
		}

		return true;
	}
	
	//Split origin into a vector of chunks between each splitter,
	//keep strings between two tokens as a single string with preserved tokens
	inline vector<string> TokenizeString(
		const string& origin,
		char token,
		const string& splitter)
	{
		//return nothing if origin is empty
		if (origin.empty()) return{};
		
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
		
		return result;
	}
	

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

	//Join all chunks in parts vector together into a single string
	//and add delimiter after each chunk except the last one
	inline string JoinString(
		const vector<string>& parts,
		const string& delimiter)
	{
		//return nothing if parts vector is empty
		if (parts.empty()) return "";

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

	//Remove all occurrences of target from origin
	inline string RemoveAllFromString(
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

	//Replace all occurrences of target from origin with replacement
	inline string ReplaceAllFromString(
		const string& origin,
		const string& target,
		const string& replacement)
	{
		//return origin if target is empty
		if (target.empty()) return origin;

		string result = origin;
		size_t pos{};

		while ((pos = result.find(target, pos)) != string::npos)
		{
			result.replace(pos, target.length(), replacement);
			pos += replacement.length();
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
				return scast<char>(toupper(c));
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
				return scast<char>(tolower(c));
			});

		return result;
	}
	
	//Returns true if the string has one or more numbers
	inline bool HasAnyNumber(const string& origin)
	{
		for (unsigned char c : origin) if (isdigit(c)) return true;
		
		return false;
	}
	//Returns true if the string has one or more non-number characters
	inline bool HasAnyNonNumber(const string& origin)
	{
		for (unsigned char c : origin) if (!isdigit(c)) return true;
		
		return false;
	}
	//Returns true if the string has one or more white-space characters
	//  - ' '
	//  - '\t'
	//  - '\n'
	//  - '\r'
	inline bool HasAnyWhiteSpace(const string& origin)
	{
		for (unsigned char c : origin) if (isspace(c)) return true;
		
		return false;
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