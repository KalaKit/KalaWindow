//------------------------------------------------------------------------------
// kalautils.hpp
//
// Copyright (C) 2025 Lost Empire Entertainment
//
// This is free source code, and you are welcome to redistribute it under certain conditions.
// Read LICENSE.md for more information.
//
// Shorthands and helpers for many common C++ 20 operations.
//
// Provides:
//   - MATH                   - shorthands for common math variables
//   - PLATFORM CONVERT       - helpers for common platform-agnostic conversion operations
//   - STRING                 - shorthands and helpers for string operations
//   - TYPE TRAITS            - helpers for checking if var is type
//   - FIXED TYPE CONTAINERS  - shorthands and helpers for common fixed-type containers
//   - MEMORY                 - shorthands and helpers for smart pointer operations
//   - TUPLE                  - helpers for tuple operations
//   - SPAN                   - shorthands and helpers for span operations
//   - DYNAMIC TYPE VARIABLES - shorthands and helpers for optional and variant operations
//   - FILESYSTEM             - shorthands and helpers for filesystem operations
//   - FILESTREAM             - shorthands and helpers for fstream operations
//   - STRINGSTREAM           - shorthands and helpers for sstream operations
//   - LOG                    - shorthands and helpers for logging operations
//   - TIME                   - shorthands and helpers for time operations
//   - SORT                   - helpers for sort operations
//   - THREADING              - shorthands and helpers for threading operations
//   - NETWORKING             - shorthands and helpers for networking operations
//------------------------------------------------------------------------------

#pragma once

#include <cstddef> //for size_t

#include <string>
#include <type_traits>

#include <array>
#include <utility> //for pair
#include <vector>
#include <unordered_map>
#include <map>

#include <memory>

#include <tuple>

#include <variant>
#include <optional>

#include <span>

#include <filesystem>

#include <fstream>
#include <sstream>

#include <iostream>

#include <algorithm> //for sort

using std::size_t;
using std::uintmax_t; //return type for kdrem
using std::ignore;    //ignore a tuple variable
using std::byte;      //for binary operations in filestream
using std::streambuf; //for log ostream reset
using std::sort;

//
// MATH
//

namespace KMath
{
	//TODO: add shorthands and helpers for pi, deg, rad, min, max, clamp, lerp etc
	//TODO: move math shorthands from core_types.hpp to here
}

//
// PLATFORM CONVERT
//

namespace KConvert
{
	//TODO: move platform-agnostic helpers from core_types.hpp to here
}

//
// STRING
//

namespace KString
{
	using k_str = std::string;

	//Convert T to string
	template<typename T>
	inline k_str k_tostr(const T& value) { return std::to_string(value); }

	//Convert bool to 'true' or 'false'
	template<> inline k_str k_tostr<bool>(const bool& state) { return state ? "true" : "false"; }

	template<typename T> inline T k_fromstr(const k_str& s); //Convert string to T

	template<> inline bool               k_fromstr<bool>(const k_str& s) { return (s == "true" || s == "1"); } //Convert string to bool

	template<> inline int                k_fromstr<int>(const k_str& s) { return std::stoi(s); }                  //Convert string to int
	template<> inline long               k_fromstr<long>(const k_str& s) { return std::stol(s); }                 //Convert string to long
	template<> inline long long          k_fromstr<long long>(const k_str& s) { return std::stoll(s); }           //Convert string to long long
	template<> inline unsigned int       k_fromstr<unsigned int>(const k_str& s) { return std::stoul(s); }        //Convert string to unsigned int
	template<> inline unsigned long      k_fromstr<unsigned long>(const k_str& s) { return std::stoul(s); }       //Convert string to unsigned long
	template<> inline unsigned long long k_fromstr<unsigned long long>(const k_str& s) { return std::stoull(s); } //Convert string to unsigned long long
	template<> inline float              k_fromstr<float>(const k_str& s) { return std::stof(s); }                //Convert string to float
	template<> inline double             k_fromstr<double>(const k_str& s) { return std::stod(s); }               //Convert string to double
	template<> inline long double        k_fromstr<long double>(const k_str& s) { return std::stold(s); }         //Convert string to long double
}

//
// TYPE TRAITS
//

namespace KType
{
	//Returns true if T is integral number (any non-floating-point)
	template<typename T>
	inline constexpr bool k_chkint() { return std::is_integral_v<T>; }

	//Returns true if T is floating point number (any non-integral)
	template<typename T>
	inline constexpr bool k_chkflt() { return std::is_floating_point_v<T>; }

	//Returns true if T is any number (integral or floating point)
	template<typename T>
	inline constexpr bool k_chknum() { return std::is_arithmetic_v<T>; }

	//Returns true if T is enum
	template<typename T>
	inline constexpr bool k_chkenum() { return std::is_enum_v<T>; }

	//Returns true if T is pointer
	template<typename T>
	inline constexpr bool k_chkptr() { return std::is_pointer_v<T>; }

	//Returns true if T is a class
	template<typename T>
	inline constexpr bool k_chkcls() { return std::is_class_v<T>; }

	//Returns true if A and B are same type
	template<typename A, typename B>
	inline constexpr bool k_chksame() { return std::is_same_v<A, B>; }
}

//
// FIXED TYPE CONTAINERS
//

namespace KContainer
{
	template<typename T, size_t N>
	using k_arr = std::array<T, N>;

	template<typename A, typename B>
	using k_pair = std::pair<A, B>;

	template<typename T>
	using k_vec = std::vector<T>;

	template<typename K, typename V>
	using k_umap = std::unordered_map<K, V>;

	template<typename K, typename V>
	using k_map = std::map<K, V>;
	
	//TODO: add get, set, push, emplace templates
}

//
// MEMORY
//

namespace KMemory
{
	template<typename T>
	using k_uptr = std::unique_ptr<T>;

	template<typename T>
	using k_sptr = std::shared_ptr<T>;

	//Convert T to unique ptr
	template<typename T, typename... Args>
	inline k_uptr<T> kuni(Args&&... args) { return std::make_unique<T>(std::forward<Args>(args)...); }

	//Convert T to shared ptr
	template<typename T, typename... Args>
	inline k_sptr<T> ksha(Args&&... args) { return std::make_shared<T>(std::forward<Args>(args)...); }
}

//
// TUPLE
//

namespace KTuple
{
	//Create an empty tuple
	template<typename... Ts>
	using k_tup = std::tuple<Ts...>;

	//Create a tuple from existing variables
	template<typename... Ts>
	inline auto k_mtup(Ts&&... args) { return std::make_tuple(std::forward<Ts>(args)...); }

	//Tie variables to a tuple (for unpacking or comparison)
	template<typename... Ts>
	inline auto k_tie(Ts&... args) { return std::tie(args...); }
}

//
// SPAN
//

namespace KSpan
{
	//Peek inside a dynamic size container without allocating or freeing memory
	template<typename T>
	using k_span = std::span<T>;

	//Peek inside a fixed size container without allocating or freeing memory
	template<typename T, size_t N>
	using k_fspan = std::span<T, N>;

	//Get variables from dynamic span - from offset up to chosen amount
	template<typename T>
	inline auto k_sspan(k_span<T> s, size_t offset, size_t count) { return s.subspan(offset, count); }

	//Get variables from dynamic span - from offset up to end
	template<typename T>
	inline auto k_sspan(k_span<T> s, size_t offset) { return s.subspan(offset); }

	//Get variables from fixed span - from offset up to chosen amount
	template<typename T, size_t N>
	inline auto k_sspan(k_fspan<T, N> fs, size_t offset, size_t count) { return fs.subspan(offset, count); }

	//Get variables from fixed span - from offset up to end
	template<typename T, size_t N>
	inline auto k_sspan(k_fspan<T, N> fs, size_t offset) { return fs.subspan(offset); }

}

//
// DYNAMIC TYPE VARIABLES
//

namespace KDynamic
{
	//Holds a single variable from known types
	template<typename... Ts>
	using k_var = std::variant<Ts...>;

	//Returns true if variant holds type T
	template<typename T, typename... Ts>
	inline constexpr bool k_vhas(const k_var<Ts...>& v) { return std::holds_alternative<T>(v); }

	//Safely get pointer to value in variant container if it is T, otherwise return nullptr
	template<typename T, typename... Ts>
	inline T* k_vgetif(k_var<Ts...>* v) { return std::get_if<T>(v); }
	//Safely get const pointer to value in variant container if it is T, otherwise return nullptr
	template<typename T, typename... Ts>
	inline const T* k_vgetif(const k_var<Ts...>* v) { return std::get_if<T>(v); }

	//Directly get reference to value in variant container of type T, throws if wrong type
	template<typename T, typename... Ts>
	inline T& k_vget(k_var<Ts...>& v) { return std::get<T>(v); }
	//Directly get const reference in variant container to value of type T, throws if wrong type
	template<typename T, typename... Ts>
	inline const T& k_vget(const k_var<Ts...>& v) { return std::get<T>(v); }

	//Holds a single variable, value might not exist
	template<typename T>
	using k_opt = std::optional<T>;

	//Returns true if optional holds a value
	template<typename T>
	inline constexpr bool k_ohas(const k_opt<T>& o) { return o.has_value(); }

	//Safely get pointer to value in optional, otherwise return nullptr
	template<typename T>
	inline T* k_ogetif(k_opt<T>* o) { return o ? &o->value() : nullptr; }
	//Safely get const pointer to value in optional, otherwise return nullptr
	template<typename T>
	inline const T* k_ogetif(const k_opt<T>* o) { return o ? &o->value() : nullptr; }

	//Directly get reference to value in optional of type T, undefined behavior if empty
	template<typename T>
	inline T& k_oget(k_opt<T>& o) { return o.value(); }
	//Directly get const reference to value in optional of type T, undefined behavior if empty
	template<typename T>
	inline const T& k_oget(const k_opt<T>& o) { return o.value(); }
}

//
// FILESYSTEM
//

namespace KFile
{
	using KString::k_str;

	using k_path = std::filesystem::path;
	using k_dirit = std::filesystem::directory_iterator;
	using k_rdirit = std::filesystem::recursive_directory_iterator;

	//Get working directory
	inline k_path k_cpath() { return std::filesystem::current_path(); }

	//Return absolute path of relative path to working directory
	inline k_path k_abs(const k_path& p) { return std::filesystem::absolute(p); }

	//Return canonical path of relative path to working directory
	inline k_path k_can(const k_path& p) { return std::filesystem::canonical(p); }

	//Return weakly canonical path of relative path to working directory
	inline k_path k_wcan(const k_path& p) { return std::filesystem::weakly_canonical(p); }

	//Returns true if path is valid
	inline bool k_exists(const k_path& p) { return std::filesystem::exists(p); }

	//Returns true if path is directory
	inline bool k_isdir(const k_path& p) { return std::filesystem::is_directory(p); }

	//Returns true if path is regular file
	inline bool k_isfile(const k_path& p) { return std::filesystem::is_regular_file(p); }

	//Returns true if path has extension
	inline bool k_hasext(const k_path& p) { return p.has_extension(); }

	//Get extension from path
	inline k_str k_getext(const k_path& p) { return p.extension().string(); }

	//Get path file name with extension
	inline k_str k_getfname(const k_path& p) { return p.filename().string(); }

	//Get path file name without extension
	inline k_str k_getfstem(const k_path& p) { return p.stem().string(); }

	//Get path parent
	inline k_path k_getfpar(const k_path& p) { return p.parent_path(); }

	//Merge two paths together and return result
	inline k_path k_join(const k_path& a, const k_path& b) { return a / b; }

	//Append path b into path a
	inline void k_ijoin(k_path& a, const k_path& b) { a /= b; }

	//Create directory from given path
	inline bool k_cdir(const k_path& p) { return std::filesystem::create_directory(p); }

	//Delete file at path
	inline bool k_rem(const k_path& p) { return std::filesystem::remove(p); }

	//Delete directory and all its contents at path
	inline uintmax_t k_drem(const k_path& p) { return std::filesystem::remove_all(p); }

	//Clears the contents of a directory without deleting the directory itself
	inline void k_clrdir(const k_path& p)
	{
		for (const auto& entry : k_dirit(p))
		{
			if (k_isdir(entry.path())) k_drem(entry.path());
			else k_rem(entry.path());
		}
	}

	//Copy file or directory from path a to path b
	inline void k_copy(const k_path& a, const k_path& b) { std::filesystem::copy(a, b); }

	//Rename or move file or directory from path a to path b
	inline void k_ren(const k_path& a, const k_path& b) { std::filesystem::rename(a, b); }
}

//
// FILESTREAM
//

namespace KFileStream
{
	using KString::k_str;
	using KFile::k_path;
	using KContainer::k_vec;

	using k_ifstr = std::ifstream; //input file stream
	using k_ofstr = std::ofstream; //output file stream
	using k_fstr = std::fstream; //input/output file stream

	//string operations

	//Read from file and return whole file as string
	inline k_str k_fsread(const k_path& p)
	{
		k_ifstr f(p, std::ios::ate);
		if (!f) return{}; //failed to open file to read

		const auto size = f.tellg();
		if (size <= 0) return{}; //empty file or error

		f.seekg(0);
		k_str result{};
		result.resize(static_cast<size_t>(size));
		f.read(result.data(), size);
		return result;
	}

	//Read from file and return all line as vector of strings
	inline k_vec<k_str> k_fslread(const k_path& p)
	{
		k_vec<k_str> lines{};
		k_ifstr f(p);
		if (!f) return lines; //failed to open file to read

		k_str line{};
		while (std::getline(f, line)) { lines.push_back(line); }
		return lines;
	}

	//Clear the contents of a string file without deleting the file itself
	inline void k_fsclear(const k_path& p)
	{
		k_ofstr f(p, std::ios::trunc);
	}

	//Write string to file (overwrite)
	inline void k_fswrite(const k_path& p, const k_str& data)
	{
		k_ofstr f(p, std::ios::trunc);
		if (!f) return; //failed to open file to write

		f << data;
	}

	//Write string to file (append after existing data)
	inline void k_fsawrite(const k_path& p, const k_str& data)
	{
		k_ofstr f(p, std::ios::app);
		if (!f) return; //failed to open file to write

		f << data;
	}

	//Write lines to file (overwrite)
	inline void k_fslwrite(const k_path& p, const k_vec<k_str>& lines)
	{
		k_ofstr f(p, std::ios::trunc);
		if (!f) return; //failed to open file to write

		for (const auto& line : lines) { f << line << '\n'; }
	}

	//Write lines to file (append after existing data)
	inline void k_fslawrite(const k_path& p, const k_vec<k_str>& lines)
	{
		k_ofstr f(p, std::ios::app);
		if (!f) return; //failed to open file to write

		for (const auto& line : lines) { f << line << '\n'; }
	}

	//binary operations
	
	//Read from file and return as byte buffer
	inline k_vec<byte> k_fbread(const k_path& p)
	{
		k_ifstr f(p, std::ios::binary | std::ios::ate);
		if (!f) return{}; //failed to open file to read

		const auto size = f.tellg();
		if (size <= 0) return{}; //empty file or error

		f.seekg(0);
		k_vec<byte> buffer(static_cast<size_t>(size));
		f.read(reinterpret_cast<char*>(buffer.data()), size);
		return buffer;
	}

	//Clear the contents of a binary file without deleting the file itself
	inline void k_fbclear(const k_path& p)
	{
		k_ofstr f(p, std::ios::binary | std::ios::trunc);
	}

	//Write binary buffer to file (overwrite)
	inline void k_fbwrite(const k_path& p, const k_vec<byte>& data)
	{
		k_ofstr f(p, std::ios::binary | std::ios::trunc);
		if (!f) return; //failed to open file to write

		f.write(
			reinterpret_cast<const char*>(data.data()),
			static_cast<std::streamsize>(data.size()));
	}

	//Write binary buffer to file (append after existing data)
	inline void k_fbawrite(const k_path& p, const k_vec<byte>& data)
	{
		k_ofstr f(p, std::ios::binary | std::ios::app);
		if (!f) return; //failed to open file to write

		f.write(
			reinterpret_cast<const char*>(data.data()),
			static_cast<std::streamsize>(data.size()));
	}
}

//
// STRINGSTREAM
//

namespace KStringStream
{
	using KString::k_str;
	using KContainer::k_vec;
	using KDynamic::k_opt;

	using k_isstr = std::istringstream; //read string input
	using k_osstr = std::ostringstream; //write string output
	using k_sstr = std::stringstream; //read/write string input/output

	//Split string into tokens by char type
	inline k_vec<k_str> k_ssplit(const k_str& s, char delim)
	{
		k_vec<k_str> tokens{};
		k_isstr ss(s);
		k_str item{};
		while (std::getline(ss, item, delim)) { tokens.push_back(item); }
		return tokens;
	}

	//Safely convert string to any T
	template<typename T>
	inline k_opt<T> k_sfromstr(const k_str& s)
	{
		k_isstr ss(s);
		T value{};
		if (ss >> value && ss.eof()) return value;
		return {};
	}

	//Safely convert any T to string
	template<typename T>
	inline k_str k_stostr(const T& value)
	{
		k_osstr ss{};
		ss << value;
		return ss.str();
	}
}

//
// LOG
//

namespace KLog
{
	using KFile::k_path;
	using KStringStream::k_osstr;
	using KFileStream::k_fsawrite;
	using KFileStream::k_ofstr;

	using k_ostr = std::ostream;
	using k_istr = std::istream;

	inline k_ostr& k_out = std::cout; //Buffered standard output
	inline k_ostr& k_log = std::clog; //Buffered log output
	inline k_ostr& k_err = std::cerr; //Unbuffered error output, flushed immediately
	inline k_istr& k_in = std::cin;   //Standard input

	//Redirect ostream to file until k_oreset is called for this ostream.
	//Use the returned streambuf pointer to reset that buffer later
	inline streambuf* k_oredir(const k_path& p, k_ostr& os) 
	{
		static k_ofstr f{};
		f.open(p, std::ios::app);
		if (!f.is_open()) return nullptr;

		return os.rdbuf(f.rdbuf()); //redirect stream buffer
	}

	//Restore ostream to original buffer, use the returned buffer from redirect
	inline void k_oreset(k_ostr& os, streambuf* originalBuf)
	{
		os.rdbuf(originalBuf);
	}

	//Write any stream type message to target string file
	template<typename... Ts>
	inline void k_wfile(const k_path& p, Ts&&... args)
	{
		k_osstr ss{};
		(ss << ... << args); //fold expression for all args
		k_fsawrite(p, ss.str());
	}
}

//
// TIME
//

namespace KTime
{
	//TODO: add content
}

//
// SORT
//

namespace KSort
{
	//TODO: add content
}

//
// THREADING
//

namespace KThread
{
	//TODO: add shorthands and helpers for thread, mutex, lock, launching, joining, detaching, closing threads
}

//
// NETWORKING
//

namespace KNetworking
{
	//TODO: add shorthands and helpers for sockets, ip parsing, send/recv
}