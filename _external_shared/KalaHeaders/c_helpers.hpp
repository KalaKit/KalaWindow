//------------------------------------------------------------------------------
// c_helpers.hpp
//
// Copyright (C) 2025 Lost Empire Entertainment
//
// This is free source code, and you are welcome to redistribute it under certain conditions.
// Read LICENSE.md for more information.
//
// Provides:
//   - Checks
//     - kh_empty, kh_len, kh_starts, kh_ends
//   - Comparisons
//     - kh_equals, kh_iequals, kh_nequals, kh_niequals
//   - Allocation and ownership
//     - kh_set, kh_dup, kh_free
//   - Copy & append
//     - kh_copy, kh_cat
//   - Search & cleanup
//     - kh_fchar, kh_lchar, kh_trim, kh_remove, kh_aremove
//   - Modification
//     - kh_tolower, kh_toupper, kh_replace, kh_areplace
//------------------------------------------------------------------------------

#pragma once

#include <cstring>
#include <cstdlib>
#include <cctype>

using std::free;
using std::strlen;
using std::strchr;
using std::strrchr;
using std::memcpy;
using std::memcmp;
using std::memchr;
using std::memmove;
using std::malloc;
using std::tolower;
using std::toupper;

//
// CHECKS
//

//Check if string is empty
inline bool kh_empty(const char* str)
{
	return !str || *str == '\0';
}

inline size_t kh_len(const char* str)
{
	return str ? strlen(str) : 0;
}

//Check if string starts with prefix
inline bool kh_starts(const char* str, const char* prefix)
{
	size_t ls = kh_len(str);
	size_t lp = kh_len(prefix);
	if (lp > ls) return false;
	return memcmp(str, prefix, lp) == 0;
}
//Check if string ends with suffix
inline bool kh_ends(const char* str, const char* suffix)
{
	size_t ls = kh_len(str);
	size_t le = kh_len(suffix);
	if (le > ls) return false;
	return memcmp(str + ls - le, suffix, le) == 0;
}

//
// COMPARISONS
//

//Case-sensitive full string compare
inline bool kh_equals(const char* a, const char* b)
{
	if (a == b) return true;

	size_t la = kh_len(a);
	size_t lb = kh_len(b);
	if (la != lb) return false;
	return memcmp(a, b, la) == 0;
}
//Case-insensitive full string compare
inline bool kh_iequals(const char* a, const char* b)
{
	if (a == b) return true;
	if (!a || !b) return false;

	while (*a && *b)
	{
		if (tolower(static_cast<unsigned char>(*a)) !=
			tolower(static_cast<unsigned char>(*b)))
			return false;
		++a; ++b;
	}
	return *a == *b;
}

//
// ALLOCATION AND OWNERSHIP
//

//Assign new string to destination and free old
inline void kh_set(char*& dst, const char* src)
{
	if (!src) return; //preserve old if new is unassigned

	if (dst)
	{
		free(dst);
		dst = nullptr;
	}

	size_t len = kh_len(src);
	dst = static_cast<char*>(malloc(len + 1));
	if (dst) memcpy(dst, src, len + 1);
}

//Allocate and return a duplicate of the source string
inline char* kh_dup(const char* src)
{
	size_t len = kh_len(src);
	char* dst = static_cast<char*>(malloc(len + 1));
	if (dst) memcpy(dst, src, len + 1);
	return dst;
}

//Free and reset in one step, prevents dangling pointers
template <typename T>
inline void kh_free(T*& ptr)
{
	if (ptr)
	{
		free(ptr);
		ptr = nullptr;
	}
}

//
// COPY AND APPEND
//

//Safe copy of fixed size string
inline void kh_copy(char* dst, const char* src, size_t max)
{
	if (max == 0) return;

	size_t n = kh_len(src);
	if (n >= max) n = max - 1;
	memcpy(dst, src, n);
	dst[max - 1] = '\0';
}

//Append one string to another safely in a buffer
inline void kh_cat(char* dst, const char* src, size_t max)
{
	if (max == 0) return;

	size_t dlen = kh_len(dst);
	size_t slen = kh_len(src);
	if (dlen >= max - 1) return; //already full

	size_t copy_len = (slen < max - dlen - 1)
		? slen
		: (max - dlen - 1);
	memcpy(dst + dlen, src, copy_len);
	dst[dlen + copy_len] = '\0';
}

// Length-limited case-sensitive string compare
inline bool kh_nequals(const char* a, const char* b, size_t max)
{
	if (a == b) return true;
	if (!a || !b) return false;

	for (size_t i = 0; i < max; ++i)
	{
		if (a[i] != b[i]) return false;
		if (a[i] == '\0') return true; //both ended before max
	}
	return true; //first max chars matched
}
//Length-limited case-insensitive string compare
inline bool kh_niequals(const char* a, const char* b, size_t max)
{
	if (a == b) return true;
	if (!a || !b) return false;

	for (size_t i = 0; i < max; ++i)
	{
		unsigned char ca = static_cast<unsigned char>(a[i]);
		unsigned char cb = static_cast<unsigned char>(b[i]);

		if (tolower(ca) != tolower(cb)) return false;
		if (a[i] == '\0' || b[i] == '\0') return true; //both ended before max
	}
	return true; //first max chars matched (ignoring case)
}

//
// SEARCH AND CLEANUP
//

//Quickly locate the first occurence of a character in a string
inline const char* kh_fchar(const char* str, char c)
{
	if (!str) return nullptr;
	return strchr(str, c);
}

//Quickly locate the last occurence of a character in a string
inline const char* kh_lchar(const char* str, char c)
{
	if (!str) return nullptr;
	return strrchr(str, c);
}

//Modify string and remove leading and trailing whitespaces
inline void kh_trim(char* str)
{
	if (!str) return;

	//trim leading

	char* start = str;
	while (
		*start
		&& isspace(static_cast<unsigned char>(*start)))
	{
		++start;
	}

	//trim trailing
	char* end = start + strlen(start);
	while (
		end > start
		&& isspace(static_cast<unsigned char>(end[-1])))
	{
		--end;
	}

	*end = '\0';
	if (start != str) memmove(str, start, (end - start) + 1);
}

//Remove the first occurence of a char
inline void kh_remove(char* str, char c)
{
	if (!str) return;

	char* read = str;
	char* write = str;

	bool removed = false;
	while (*read)
	{
		if (!removed
			&& *read == c)
		{
			removed = true;
			++read;
			continue;
		}
		*write++ = *read++;
	}
	*write = '\0';
}
//Remove all occurences of a char
inline void kh_aremove(char* str, char c)
{
	if (!str) return;

	char* read = str;
	char* write = str;

	while (*read)
	{
		if (*read != c) *write++ = *read;

		++read;
	}
	*write = '\0';
}

//
// MODIFICATION
//

//Set existing string to lowercase
inline void kh_tolower(char* str)
{
	if (!str) return;

	for (; *str; ++str)
	{
		*str = static_cast<char>(tolower(static_cast<unsigned char>(*str)));
	}
}
//Set existing string to uppercase
inline void kh_toupper(char* str)
{
	if (!str) return;

	for (; *str; ++str)
	{
		*str = static_cast<char>(toupper(static_cast<unsigned char>(*str)));
	}
}

//Replace first occurrence of a character in place
inline void kh_replace(char* str, char from, char to)
{
	if (!str) return;

	for (; *str; ++str)
	{
		if (*str == from)
		{
			*str = to;
			return; //stop after first replacement
		}
	}
}
//Replace all occurrences of a character in place
inline void kh_areplace(char* str, char from, char to)
{
	if (!str) return;

	for (; *str; ++str)
	{
		if (*str == from) *str = to;
	}
}