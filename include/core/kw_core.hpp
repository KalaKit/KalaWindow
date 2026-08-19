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

#include "core_utils.hpp"

namespace KalaWindow::Core
{
	using std::string;
	using std::string_view;
	using std::function;
	using std::filesystem::path;
	using std::array;
	using std::vector;

	enum class CPUFeatureFlag : u32
	{
		CPU_FEATURE_NONE    = 0,

		CPU_FEATURE_SSE     = 1 << 0,
		CPU_FEATURE_SSE2    = 1 << 1,
		CPU_FEATURE_SSE3    = 1 << 2,
		CPU_FEATURE_SSSE3   = 1 << 3,
		CPU_FEATURE_SSE4_1  = 1 << 4,
		CPU_FEATURE_SSE4_2  = 1 << 5,

		CPU_FEATURE_AVX     = 1 << 6,
		CPU_FEATURE_AVX2    = 1 << 7,
		CPU_FEATURE_AVX512F = 1 << 8,

		CPU_FEATURE_FMA3    = 1 << 9,

		CPU_FEATURE_BMI1    = 1 << 10,
		CPU_FEATURE_BMI2    = 1 << 11
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

	static constexpr u8 MAX_NAME_LENGTH = 64;

	class LIB_API KalaWindowCore
	{
	public:
		static u32 GetGlobalID();
		static void SetGlobalID(u32 newID);

		static path GetExePath();

		//Get CPU info as individual fields in a struct
		static CPUInfo GetCPUInfo();
		//Get all CPU info as a single string
		static string GetCPUInfoString();

		//Get GPU info per GPU as individual fields in a struct
		static vector<GPUInfo> GetGPUInfo();
		//Get all GPU info as a single string
		static string GetGPUInfoString();

		//Get RAM info as individual fields in a struct,
		//set recheck to true if you want to get new current data
		static RAMInfo GetRAMInfo(bool recheck = false);
		//Get all RAM info as a single string,
		//set recheck to true if you want to get new current data
		static string GetRAMInfoString(bool recheck = false);

		//Get OS info as individual fields in a struct
		static OSInfo GetOSInfo();
		//Get all OS info as a single string
		static string GetOSInfoString();

		//Use this when you absolutely need a hard crash at this very moment
		static void ForceClose(
			string&& title,
			string&& reason);
	};
}
