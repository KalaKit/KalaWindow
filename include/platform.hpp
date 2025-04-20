//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <iostream>
#include <cstdint>

//
// LOG PREPROCESSORS
//

#ifndef WRITE_LOG
	#define WRITE_LOG(type, msg) std::cout << "[KALAKIT_" << KALAKIT_MODULE << " | " << type << "] " << msg << "\n"
#endif

//log types
#ifdef KALAWINDOW_DEBUG
	#define LOG_DEBUG(msg) WRITE_LOG("DEBUG", msg)
#else
	#define LOG_DEBUG(msg)
#endif
#define LOG_SUCCESS(msg) WRITE_LOG("SUCCESS", msg)
#define LOG_ERROR(msg) WRITE_LOG("ERROR", msg)

//
// DLL EXPORT/IMPORT MACRO
//

#ifdef KALAKIT_WINDOWS
	#ifdef KALAWINDOW_DLL_EXPORT
		#define KALAWINDOW_API __declspec(dllexport)
	#else
		#define KALAWINDOW_API __declspec(dllimport)
	#endif
#elif KALAKIT_X11
	#define KALAWINDOW_API __attribute__((visibility("default")))
#else
	#define KALAWINDOW_API
#endif

//
// TEMPLATE FOR ALL OS VARIABLES
//

template <typename Tag>
struct StrongHandle
{
	uintptr_t value;
	
	//default constructor
	StrongHandle() : value(0) {} 

	explicit StrongHandle(uintptr_t v) : value(v) {}

	bool operator==(const StrongHandle& other) const { return value == other.value; }
	bool operator!=(const StrongHandle& other) const { return value != other.value; }

	uintptr_t get() const { return value; }
};

//
// DECLARE PLATFORM VARIABLES
//

struct kvec2 { float x, y; };
struct kvec3 { float x, y, z; };
struct kvec4 { float x, y, z, w; };

struct kmat2 { kvec2 columns[2]; };
struct kmat3 { kvec3 columns[3]; };
struct kmat4 { kvec4 columns[4]; };
