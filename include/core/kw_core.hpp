//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <functional>
#include <filesystem>
#include <array>
#include <vector>
#include <source_location>

#include "macros.hpp"

namespace KalaWindow::Core
{
	using std::string;
	using std::string_view;
	using std::function;
	using std::filesystem::path;
	using std::array;
	using std::vector;
	using std::source_location;

	static constexpr u8 MIN_NAME_LENGTH = 4;
	static constexpr u8 MAX_NAME_LENGTH = 64;

	enum class CPUFeatureFlag : u32
	{
		NONE = 0,

		SSE    = 1u << 0,
		SSE2   = 1u << 1,
		SSE3   = 1u << 2,
		SSSE3  = 1u << 3,
		SSE4_1 = 1u << 4,
		SSE4_2 = 1u << 5,

		AVX     = 1u << 6,
		AVX2    = 1u << 7,
		AVX512F = 1u << 8,

		FMA3 = 1u << 9,

		BMI1 = 1u << 10,
		BMI2 = 1u << 11
	};

	struct LIB_API CPUInfo
	{
		string brand{};
		string vendor{};
		u16 physicalCores{};
		u16 logicalThreads{};
		u32 baseClockSpeedMHz{};
		array<u64, 3> cacheSizes{}; //level 1-3 cpu cache sizes
		u32 featureFlags{};
	};
	struct LIB_API GPUInfo
	{
		string brand{};
		u64 vramBytes{};
		string driverVersion{};
		bool isDiscrete{};
		u32 vendorID{};
		u32 deviceID{};
	};
	struct LIB_API RAMInfo
	{
		u64 totalBytes{};     //total amount of system memory
		u64 availableBytes{}; //currently available amount of system memory
		u64 usedBytes{};      //memory used currently by this process
	};
	struct LIB_API OSInfo
	{
		string name{};             //OS name - Windows 10/11 or Linux
		string version{};          //build number for Windows, kernel version for Linux
		string architecture{};     //x64, x86, ARM64 etc
		bool isOnWine{};           //true if using Wine or Proton
		bool isOnVirtualMachine{}; //true if using any virtual machine
	};

	class LIB_API KalaWindowCore
	{
	public:
		KNODISCARD
		static u32 GetGlobalID();
		static void SetGlobalID(u32 newID);

		KNODISCARD
		static path GetExePath();

		//Get CPU info as individual fields in a struct
		KNODISCARD
		static CPUInfo GetCPUInfo();
		//Get all CPU info as a single string
		KNODISCARD
		static string GetCPUInfoString();

		//Get GPU info per GPU as individual fields in a struct
		KNODISCARD
		static vector<GPUInfo> GetGPUInfo();
		//Get all GPU info as a single string
		KNODISCARD
		static string GetGPUInfoString();

		//Get RAM info as individual fields in a struct,
		//set recheck to true if you want to get new current data
		KNODISCARD
		static RAMInfo GetRAMInfo(bool recheck = false);
		//Get all RAM info as a single string,
		//set recheck to true if you want to get new current data
		KNODISCARD
		static string GetRAMInfoString(bool recheck = false);

		//Get OS info as individual fields in a struct
		KNODISCARD
		static OSInfo GetOSInfo();
		//Get all OS info as a single string
		KNODISCARD
		static string GetOSInfoString();

		//Use this when you absolutely need a hard crash at this very moment
		KNORETURN
		static void ForceClose(
			string_view title,
			string_view reason,
			const source_location& location = source_location::current());
	};
}
