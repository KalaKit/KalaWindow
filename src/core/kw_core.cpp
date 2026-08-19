//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include "core/kw_core.hpp"

#if defined(KWIN_ANY)
#include <windows.h>
#include <mmsystem.h>
#include <intrin.h>
#include <powerbase.h>
#include <winternl.h>
#include <psapi.h>
	#if defined(KWIN_GNU)
	typedef struct _PROCESSOR_POWER_INFORMATION
	{
		ULONG number{};
		ULONG MaxMhz{};
		ULONG CurrentMhz{};
		ULONG MhzLimit{};
		ULONG MaxIdleState{};
		ULONG CurrentIdleState{};
	} PROCESSOR_POWER_INFORMATION, *PPROCESSOR_POWER_INFORMATION;
	#endif
#elif defined(KLIN_ANY)
#include <csignal>
#include <X11/Xlib.h>
#include <linux/limits.h>
#include <cpuid.h>
#include <sys/sysinfo.h>
#include <sys/utsname.h>
#endif

#include <fstream>
#include <set>

#include "vulkan/vulkan_core.h"

#include "log_utils.hpp"
#include "string_utils.hpp"

#include "core/kw_crash.hpp"
#include "core/kw_input.hpp"
#include "graphics/kw_vulkan.hpp"
#include "graphics/kw_window_global.hpp"

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;
using KalaHeaders::KalaLog::TimeFormat;
using KalaHeaders::KalaLog::DateFormat;

using KalaHeaders::KalaString::TrimString;

using KalaWindow::Graphics::VulkanContext;
using KalaWindow::Graphics::Window_Global;

#if defined(KLIN_ANY)
using std::raise;
#endif

using std::to_string;
using std::string_view;
using std::ifstream;
using std::set;
using std::pair;

#if defined(KWIN_GNU)
__attribute__((target("xsave")))
#endif

static u64 ReadXRC0()
{
#if defined(KWIN_ANY)
	return _xgetbv(0);
#else
	u32 xcrLow{}, xcrHigh{};
	__asm__ __volatile__("xgetbv" : "=a"(xcrLow), "=d"(xcrHigh) : "c"(0));
	return ((u64)xcrHigh << 32) | xcrLow;
#endif
}

namespace KalaWindow::Core
{
	//The ID that is bumped by every object in KalaWindow when it needs a new ID
	static u32 globalID{};

	u32 KalaWindowCore::GetGlobalID() { return globalID; }
	void KalaWindowCore::SetGlobalID(u32 newID) { globalID = newID; }

	path KalaWindowCore::GetExePath()
	{
		path exePath{};

#if defined(KWIN_ANY)
		wchar_t buffer[MAX_PATH]{};
		DWORD length = GetModuleFileNameW(
			nullptr,
			buffer,
			MAX_PATH);

		if (length > 0
			&& length < MAX_PATH)
		{	
			exePath = path(buffer);
		}
		else
		{
			ForceClose(
				"KalaWindow core error",
				"Failed to get path to executable!");
		}
#elif defined(KLIN_ANY)
		char buffer[PATH_MAX]{};
		ssize_t length = readlink(
			"/proc/self/exe",
			buffer,
			sizeof(buffer) - 1);

		if (length > 0)
		{
			buffer[length] = '\0';
			exePath = path(buffer);
		}
		else
		{
			ForceClose(
				"KalaWindow core error",
				"Failed to get path to executable!");
		}
#endif
		return exePath;
	}

	CPUInfo KalaWindowCore::GetCPUInfo()
	{
		static bool hasInfo{};
		static CPUInfo cpuInfo{};

		auto get_cpu_brand = []() -> string
			{
				int regs[4]{};
				char brand[49]{};

#if defined(KWIN_ANY)
				__cpuid(
					regs,
					0x80000002);
				memcpy(
					brand,
					regs,
					sizeof(regs));

				__cpuid(
					regs,
					0x80000003);
				memcpy(
					brand + 16,
					regs,
					sizeof(regs));

				__cpuid(
					regs,
					0x80000004);
				memcpy(
					brand + 32,
					regs,
					sizeof(regs));			
#elif defined(KLIN_ANY)
				__get_cpuid(
					0x80000002, 
					(u32*)&regs[0],
					(u32*)&regs[1],
					(u32*)&regs[2],
					(u32*)&regs[3]);
				memcpy(
					brand,
					regs,
					sizeof(regs));

				__get_cpuid(
					0x80000003, 
					(u32*)&regs[0],
					(u32*)&regs[1],
					(u32*)&regs[2],
					(u32*)&regs[3]);
				memcpy(
					brand + 16,
					regs,
					sizeof(regs));

				__get_cpuid(
					0x80000004, 
					(u32*)&regs[0],
					(u32*)&regs[1],
					(u32*)&regs[2],
					(u32*)&regs[3]);
				memcpy(
					brand + 32,
					regs,
					sizeof(regs));
#endif

				return TrimString(string(brand));
			};

		auto get_cpu_vendor = []() -> string
			{
				int regs[4]{};
				char vendor[13]{};

#if defined(KWIN_ANY)
				__cpuid(regs, 0);
#elif defined(KLIN_ANY)
				__get_cpuid(
					0,
					(u32*)&regs[0],
					(u32*)&regs[1],
					(u32*)&regs[2],
					(u32*)&regs[3]);
#endif

				memcpy(
					vendor, 
					&regs[1], 
					4);
				memcpy(
					vendor + 4, 
					&regs[3], 
					4);
				memcpy(
					vendor + 8, 
					&regs[2], 
					4);

				return string(vendor);
			};

		auto get_physical_core_count = []() -> u16
			{
#if defined(KWIN_ANY)
				DWORD bufferSize{};
				GetLogicalProcessorInformationEx(
					RelationProcessorCore,
					nullptr,
					&bufferSize);

				vector<u8> buffer(bufferSize);
				auto* info = rcast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(buffer.data());

				if (!GetLogicalProcessorInformationEx(
					RelationProcessorCore,
					info,
					&bufferSize))
				{
					Log::Print(
						"Failed to get physical core count because GetLogicalProcessorInformationEx failed!",
						"KW_CORE",
						LogType::LOG_ERROR,
						2);

					return 0;
				}

				u16 physicalCores{};

				DWORD offset{};
				while (offset < bufferSize)
				{
					auto* current = rcast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(buffer.data() + offset);
				
					if (current->Relationship == RelationProcessorCore) physicalCores++;

					offset += current->Size;
				}

				return physicalCores;
#elif defined(KLIN_ANY)
				ifstream file("/proc/cpuinfo");
				if (!file.is_open())
				{
					Log::Print(
						"Failed to get physical core count because '/ptoc/cpuinfo' couldn't be opened!",
						"KW_CORE",
						LogType::LOG_ERROR,
						2);

					return 0;
				}

				string line{};

				set<pair<u32, u32>> physicalCoreIDs{};

				u32 currentPhysicalID{};

				while (getline(file, line))
				{
					if (line.rfind("physical id", 0) == 0)
					{
						currentPhysicalID = stoul(line.substr(line.find(':') + 1));
					}
					else if (line.rfind("core id", 0) == 0)
					{
						u32 currentCoreID = stoul(line.substr(line.find(':') + 1));
						physicalCoreIDs.insert({ currentPhysicalID, currentCoreID });
					}
				}

			return (u16)physicalCoreIDs.size();
#endif
			};

		auto get_logical_thread_count = []() -> u16
			{
#if defined(KWIN_ANY)
				return scast<u16>(GetActiveProcessorCount(ALL_PROCESSOR_GROUPS));
#elif defined(KLIN_ANY)
				return scast<u16>(sysconf(_SC_NPROCESSORS_ONLN));
#endif
			};

		auto get_base_clock_speed = []() -> u32
			{
#if defined(KWIN_ANY)
				HKEY key{};
				RegOpenKeyExA(
					HKEY_LOCAL_MACHINE,
					"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
					0,
					KEY_READ,
					&key);

				DWORD mhz{};
				DWORD size = sizeof(mhz);
				RegQueryValueExA(
					key,
					"~MHz",
					nullptr,
					nullptr,
					(LPBYTE)&mhz,
					&size);
				RegCloseKey(key);

				return scast<u32>(mhz);
#elif defined(KLIN_ANY)
				ifstream file("/sys/devices/system/cpu/cpu0/cpufreq/base_frequency");
				if (!file.is_open())
				{
					Log::Print(
						"Failed to get base clock speed because '/sys/devices/system/cpu/cpu0/cpufreq/base_frequency' couldn't be opened!",
						"KW_CORE",
						LogType::LOG_ERROR,
						2);

					return 0;
				}

				u32 khz{};
				file >> khz;

				return khz / 1000;
#endif
			};

		auto get_cpu_cache_sizes = []() -> array<u64, 3>
			{
				array<u64, 3> cacheSizes{};

				bool isAMD = cpuInfo.vendor == "AuthenticAMD";

				if (!isAMD)
				{
					for (u32 i = 0; i < 4; i++)
					{
						int regs[4]{};
#if defined(KWIN_ANY)
						__cpuidex(regs, 4, i);
#elif defined(KLIN_ANY)
						__get_cpuid_count(
							4,
							i,
							(u32*)&regs[0],
							(u32*)&regs[1],
							(u32*)&regs[2],
							(u32*)&regs[3]);
#endif

						u32 cacheType = regs[0] & 0x1F; //EAX bits 0-4
						if (cacheType == 0) break; //no more cache levels

						u32 cacheLevel = (regs[0] >> 5) & 0x7; //EAX bits 5-7

						//skip instruction caches
						if (cacheType == 2) continue;

						//EBX bits 22-31
						u32 ways = ((regs[1] >> 22) & 0x3FF) + 1;
						//EBX bits 12-21   
						u32 partitions = ((regs[1] >> 12) & 0x3FF) + 1;
						//EBX bits 0-11
						u32 lineSize = (regs[1] & 0xFFF) + 1;
						//ECX
						u32 sets = regs[2] + 1;

						u64 sizeBytes = (u64)ways * partitions * lineSize * sets;

						if (cacheLevel == 1) cacheSizes[0] = sizeBytes;
						else if (cacheLevel == 2) cacheSizes[1] = sizeBytes;
						else if (cacheLevel == 3) cacheSizes[2] = sizeBytes; 
					}
				}
				else
				{
					int regsL1[4]{};
					int regsL23[4]{};

#if defined(KWIN_ANY)
					__cpuid(
						regsL1, 
						0x80000005);
					__cpuid(
						regsL23, 
						0x80000006);
#elif defined(KLIN_ANY)
					__get_cpuid(
						0x80000005,
						(u32*)&regsL1[0],
						(u32*)&regsL1[1],
						(u32*)&regsL1[2],
						(u32*)&regsL1[3]);
					__get_cpuid(
						0x80000006,
						(u32*)&regsL23[0],
						(u32*)&regsL23[1],
						(u32*)&regsL23[2],
						(u32*)&regsL23[3]);
#endif

					//L1 data cache size - ECX bits 24-31 in KB
					u32 l1DataKB = (regsL1[2] >> 24) & 0xFF;
					cacheSizes[0] = (u64)l1DataKB * 1024;

					//L2 cache size - ECX bits 16-31 in KB
					u32 l2KB = (regsL23[2] >> 16) & 0xFFFF;
					cacheSizes[1] = (u64)l2KB * 1024;

					//L3 cache size - EDX bits 18-31 in 512KB units
					u32 l3Units = (regsL23[3] >> 18) & 0x3FFF;
					cacheSizes[2] = (u64)l3Units * 512 * 1024;
				}

				return cacheSizes;
			};

		auto get_cpu_feature_flags = []() -> u32
			{
				u32 flags = scast<u32>(CPUFeatureFlag::CPU_FEATURE_NONE);

				int regs0[4]{}; //leaf 0 - max supported leaf
				int regs1[4]{}; //leaf 1

#if defined(KWIN_ANY)
				__cpuid(
					regs0, 
					0);
				__cpuid(
					regs1, 
					1);
#elif defined(KLIN_ANY)
				__get_cpuid(
					0,
					(u32*)&regs0[0],
					(u32*)&regs0[1],
					(u32*)&regs0[2],
					(u32*)&regs0[3]);
				__get_cpuid(
					1,
					(u32*)&regs1[0],
					(u32*)&regs1[1],
					(u32*)&regs1[2],
					(u32*)&regs1[3]);
#endif
				u32 maxLeaf = (u32)regs0[0];

				//leaf 1, EDX
				if (regs1[3] & (1 << 25)) flags |= scast<u32>(CPUFeatureFlag::CPU_FEATURE_SSE);
				if (regs1[3] & (1 << 26)) flags |= scast<u32>(CPUFeatureFlag::CPU_FEATURE_SSE2);

				//leaf 1, ECX
				bool osxSaveSupported = (regs1[2] & (1 << 27)) != 0; //OSXSAVE bit
				bool avxSupportedByCPU = (regs1[2] & (1 << 28)) != 0; //AVX bit

				if (regs1[2] & (1 << 0))  flags |= scast<u32>(CPUFeatureFlag::CPU_FEATURE_SSE3);
				if (regs1[2] & (1 << 9))  flags |= scast<u32>(CPUFeatureFlag::CPU_FEATURE_SSSE3);
				if (regs1[2] & (1 << 19)) flags |= scast<u32>(CPUFeatureFlag::CPU_FEATURE_SSE4_1);
				if (regs1[2] & (1 << 20)) flags |= scast<u32>(CPUFeatureFlag::CPU_FEATURE_SSE4_2);
				if (regs1[2] & (1 << 12)) flags |= scast<u32>(CPUFeatureFlag::CPU_FEATURE_FMA3);

				//confirm OS has actually enabled AVX state saving via XGETBV before trusting AVX/AVX2/AVX512
				bool osSupportsAVX{};
				if (osxSaveSupported) osSupportsAVX = (ReadXRC0() & 0x6) == 0x6;

				if (avxSupportedByCPU
					&& osSupportsAVX)
				{
					flags |= scast<u32>(CPUFeatureFlag::CPU_FEATURE_AVX);
				}

				//leaf 7 subleaf 0
				if (maxLeaf >= 7)
				{
					int regs7[4]{};

#if defined(KWIN_ANY)
					__cpuidex(
						regs7,
						7,
						0);
#elif defined(KLIN_ANY)
					__get_cpuid_count(
						7,
						0,
						(u32*)&regs7[0],
						(u32*)&regs7[1],
						(u32*)&regs7[2],
						(u32*)&regs7[3]);
#endif

					//AVX2/AVX512F dewpend on the same os-enabled vector state as AVX
					if (avxSupportedByCPU
						&& osSupportsAVX)
					{
						if (regs7[1] & (1 << 5))  flags |= scast<u32>(CPUFeatureFlag::CPU_FEATURE_AVX2);
						if (regs7[1] & (1 << 16)) flags |= scast<u32>(CPUFeatureFlag::CPU_FEATURE_AVX512F);
					}

					//BMI1/BMI2 dont depend on XGETBV
					if (regs7[1] & (1 << 3))  flags |= scast<u32>(CPUFeatureFlag::CPU_FEATURE_BMI1);
					if (regs7[1] & (1 << 8))  flags |= scast<u32>(CPUFeatureFlag::CPU_FEATURE_BMI2);
				}

				return flags;
			};

		if (!hasInfo)
		{
			cpuInfo.brand = get_cpu_brand();
			cpuInfo.vendor = get_cpu_vendor();
			cpuInfo.physicalCores = get_physical_core_count();
			cpuInfo.logicalThreads = get_logical_thread_count();
			cpuInfo.baseClockSpeedMHz = get_base_clock_speed();
			cpuInfo.cacheSizes = get_cpu_cache_sizes();
			cpuInfo.featureFlags = get_cpu_feature_flags();

			hasInfo = true;
		}

		return cpuInfo;
	}
	string KalaWindowCore::GetCPUInfoString()
	{
		CPUInfo cpu = GetCPUInfo();

		string cacheStr = 
			"    L1: " + to_string(cpu.cacheSizes[0] / 1024) + " KB\n"
			"    L2: " + to_string(cpu.cacheSizes[1] / 1024) + " KB\n"
			"    L3: " + to_string(cpu.cacheSizes[2] / 1024 / 1024) + " MB";

		string featuresStr{};
		if (cpu.featureFlags & scast<u32>(CPUFeatureFlag::CPU_FEATURE_SSE))     featuresStr += "    SSE";
		if (cpu.featureFlags & scast<u32>(CPUFeatureFlag::CPU_FEATURE_SSE2))    featuresStr += "\n    SSE2";
		if (cpu.featureFlags & scast<u32>(CPUFeatureFlag::CPU_FEATURE_SSE3))    featuresStr += "\n    SSE3";
		if (cpu.featureFlags & scast<u32>(CPUFeatureFlag::CPU_FEATURE_SSSE3))   featuresStr += "\n    SSSE3";
		if (cpu.featureFlags & scast<u32>(CPUFeatureFlag::CPU_FEATURE_SSE4_1))  featuresStr += "\n    SSE4_1";
		if (cpu.featureFlags & scast<u32>(CPUFeatureFlag::CPU_FEATURE_SSE4_2))  featuresStr += "\n    SSE4_2";

		if (cpu.featureFlags & scast<u32>(CPUFeatureFlag::CPU_FEATURE_AVX))     featuresStr += "\n    AVX";
		if (cpu.featureFlags & scast<u32>(CPUFeatureFlag::CPU_FEATURE_AVX2))    featuresStr += "\n    AVX2";
		if (cpu.featureFlags & scast<u32>(CPUFeatureFlag::CPU_FEATURE_AVX512F)) featuresStr += "\n    AVX512";

		if (cpu.featureFlags & scast<u32>(CPUFeatureFlag::CPU_FEATURE_FMA3))    featuresStr += "\n    FMA3";

		if (cpu.featureFlags & scast<u32>(CPUFeatureFlag::CPU_FEATURE_BMI1))    featuresStr += "\n    BMI1";
		if (cpu.featureFlags & scast<u32>(CPUFeatureFlag::CPU_FEATURE_BMI2))    featuresStr += "\n    BMI2";

		if (featuresStr.empty()) featuresStr = "\n    None";

		return 
			"\nCPU data:\n"
			"  Brand: " + cpu.brand + "\n"
			+ "  Vendor: " + cpu.vendor + "\n"
			+ "  Physical cores: " + to_string(cpu.physicalCores) + "\n"
			+ "  Logical threads: " + to_string(cpu.logicalThreads) + "\n"
			+ "  Base clock: " + to_string(cpu.baseClockSpeedMHz) + " MHz\n"
			+ "  Cache sizes:\n" + cacheStr + "\n"
			+ "  Features:\n" + featuresStr;
	}

	vector<GPUInfo> KalaWindowCore::GetGPUInfo()
	{
		static bool hasInfo{};
		static vector<GPUInfo> gpuInfo{};

		if (!hasInfo)
		{
			VkInstance vkInstance = VulkanContext::GetInstance();
			if (vkInstance == VK_NULL_HANDLE)
			{
				Log::Print(
					"Failed to get GPU info because global Vulkan has not been initialized!",
					"KW_CORE",
					LogType::LOG_ERROR,
					2);

				return {};
			}

			u32 deviceCount{};
			vkEnumeratePhysicalDevices(
				vkInstance,
				&deviceCount,
				nullptr);

			vector<VkPhysicalDevice> devices(deviceCount);
			vkEnumeratePhysicalDevices(
				vkInstance,
				&deviceCount,
				devices.data());

			for (VkPhysicalDevice device : devices)
			{
				VkPhysicalDeviceDriverProperties driverProps{};
				driverProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES;

				VkPhysicalDeviceProperties2 props2{};
				props2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
				props2.pNext = &driverProps;

				vkGetPhysicalDeviceProperties2(
					device,
					&props2);

				VkPhysicalDeviceProperties& props = props2.properties;

				VkPhysicalDeviceMemoryProperties memProps{};
				vkGetPhysicalDeviceMemoryProperties(
					device,
					&memProps);

				u64 vram{};
				for (u32 i = 0; i < memProps.memoryHeapCount; i++)
				{
					if (memProps.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
					{
						vram += memProps.memoryHeaps[i].size;
					}
				}

				GPUInfo info{};
				info.brand = string(props.deviceName);
				info.vendorID = props.vendorID;
				info.deviceID = props.deviceID;
				info.isDiscrete = props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
				info.vramBytes = vram;
				info.driverVersion = 
					string(driverProps.driverName) 
					+ " " + string(driverProps.driverInfo);

				gpuInfo.push_back(info);
			}

			hasInfo = true;
		}

		return gpuInfo;
	}
	string KalaWindowCore::GetGPUInfoString()
	{
		vector<GPUInfo> gpus = GetGPUInfo();

		if (gpus.empty())
		{
			Log::Print(
				"Failed to get GPU info as string because global Vulkan has not been initialized!",
				"KW_CORE",
				LogType::LOG_ERROR,
				2);

			return {};
		}

		string result = "GPU data:\n";

		for (size_t i = 0; i < gpus.size(); i++)
		{
			const GPUInfo& gpu = gpus[i];

			result +=
				"  GPU " + to_string(i) + ":\n"
				+ "    Brand: " + gpu.brand + "\n"
				+ "    VRAM: " + to_string(gpu.vramBytes / 1024 / 1024) + " MB\n"
				+ "    Driver: " + gpu.driverVersion + "\n"
				+ "    Type: " + string(gpu.isDiscrete ? "Discrete" : "Integrated") + "\n"
				+ "    Vendor ID: " + to_string(gpu.vendorID) + "\n"
				+ "    Device ID: " + to_string(gpu.deviceID);
				
			if (i + 1 < gpus.size()) result += "\n\n";
		}

		return result;
	}

	RAMInfo KalaWindowCore::GetRAMInfo(bool recheck)
	{
		static bool hasInfo{};
		static RAMInfo ramInfo{};

		auto get_total_memory = []() -> u64
			{
#if defined(KWIN_ANY)
				MEMORYSTATUSEX memStatus{};
				memStatus.dwLength = sizeof(memStatus);

				if (!GlobalMemoryStatusEx(&memStatus))
				{
					Log::Print(
						"Failed to get total memory because GetProcessMemoryInfo failed!",
						"KW_CORE",
						LogType::LOG_ERROR,
						2);

					return 0;
				}

				return memStatus.ullTotalPhys;
#elif defined(KLIN_ANY)
				struct sysinfo info{};
				if (sysinfo(&info) != 0)
				{
					Log::Print(
						"Failed to get total memory because sysinfo() failed!",
						"KW_CORE",
						LogType::LOG_ERROR,
						2);

					return 0;
				}

				return (u64)info.totalram * info.mem_unit;
#endif
			};

		auto get_available_memory = []() -> u64
			{
#if defined(KWIN_ANY)
				MEMORYSTATUSEX memStatus{};
				memStatus.dwLength = sizeof(memStatus);

				if (!GlobalMemoryStatusEx(&memStatus))
				{
					Log::Print(
						"Failed to get available memory because GetProcessMemoryInfo failed!",
						"KW_CORE",
						LogType::LOG_ERROR,
						2);

					return 0;
				}

				return memStatus.ullAvailPhys;
#elif defined(KLIN_ANY)
				ifstream file("/proc/meminfo");
				if (!file.is_open())
				{
					Log::Print(
						"Failed to get available memory because '/proc/meminfo' couldn't be opened!",
						"KW_CORE",
						LogType::LOG_ERROR,
						2);

					return 0;
				}

				string line{};
				while (getline(file, line))
				{
					if (line.rfind("MemAvailable:", 0) == 0)
					{
						size_t start = line.find_first_of("0123456789");
						if (start != string::npos)
						{
							u64 kb = stoull(line.substr(start));
							return kb * 1024;
						}
					}
				}

				Log::Print(
					"Failed to get available memory because 'MemAvailable' was not found in '/proc/meminfo'!",
					"KW_CORE",
					LogType::LOG_ERROR,
					2);

				return 0;
#endif
			};

		auto get_used_memory = []() -> u64
			{
#if defined(KWIN_ANY)
				PROCESS_MEMORY_COUNTERS pmc{};
				if (!GetProcessMemoryInfo(
					GetCurrentProcess(), 
					&pmc, 
					sizeof(pmc)))
				{
					Log::Print(
						"Failed to get used memory because GetProcessMemoryInfo failed!",
						"KW_CORE",
						LogType::LOG_ERROR,
						2);

					return 0;
				}

				return pmc.WorkingSetSize;
#elif defined(KLIN_ANY)
				ifstream file("/proc/self/status");
				if (!file.is_open())
				{
					Log::Print(
						"Failed to get used memory because '/proc/self/status' couldn't be opened!",
						"KW_CORE",
						LogType::LOG_ERROR,
						2);

					return 0;
				}

				string line{};
				while (getline(file, line))
				{
					if (line.rfind("VmRSS:", 0) == 0)
					{
						size_t start = line.find_first_of("0123456789");
						if (start != string::npos)
						{
							u64 kb = stoull(line.substr(start));
							return kb * 1024;
						}
					}
				}

				Log::Print(
					"Failed to get available memory because 'VmRSS' was not found in '/proc/self/status'!",
					"KW_CORE",
					LogType::LOG_ERROR,
					2);

				return 0;
#endif
			};

		if (!hasInfo
			|| recheck)
		{
			ramInfo.totalBytes = get_total_memory();
			ramInfo.availableBytes = get_available_memory();
			ramInfo.usedBytes = get_used_memory();

			hasInfo = true;
		}

		return ramInfo;
	}
	string KalaWindowCore::GetRAMInfoString(bool recheck)
	{
		RAMInfo ram = GetRAMInfo(recheck);

		return
			"RAM data:\n"
			"  Total: " + to_string(ram.totalBytes / 1024 / 1024) + " MB\n"
			+ "  Available: " + to_string(ram.availableBytes / 1024 / 1024) + " MB\n"
			+ "  Used by this process: " + to_string(ram.usedBytes / 1024 / 1024) + " MB";
	}

	OSInfo KalaWindowCore::GetOSInfo()
	{
		static bool hasInfo{};
		static OSInfo osInfo{};

		if (!hasInfo)
		{
			auto get_os_name_and_version = [](
				bool onWine, 
				bool onVM) -> pair<string, string>
				{
#if defined(KWIN_ANY)
					u32 buildNumber = Window_Global::GetBuildNumber();

					//Windows 11 still reports major version 10 internally,
					//build number is the reliable way to distinguish 10 vs 11
					string name = (buildNumber >= 22000) ? "Windows 11" : "Windows 10";

					name = onWine ? name + " (Wine)" : name;
					name = onVM ? name + " (Virtual Machine)" : name;

					string buildNumberStr = to_string(buildNumber);
					u32 buildRevision = Window_Global::GetBuildRevision();

					string finalBuildStr = buildRevision != 0 
						? buildNumberStr + "." + to_string(buildRevision)
						: buildNumberStr;
					
					return { name, finalBuildStr };
#elif defined(KLIN_ANY)
					string name = "Linux";
					string prettyName{};

					ifstream file("/etc/os-release");
					if (!file.is_open())
					{
						Log::Print(
							"Failed to get OS name because '/etc/os-release' couldn't be opened!",
							"KW_CORE",
							LogType::LOG_ERROR,
							2);
					}
					else
					{
						string line{};
						while (getline(file, line))
						{
							if (line.rfind("PRETTY_NAME=", 0) == 0)
							{
								prettyName = line.substr(12);
								if (!prettyName.empty()
									&& prettyName.front() == '"')
								{
									prettyName.erase(0, 1);
								}
								if (!prettyName.empty()
									&& prettyName.back() == '"')
								{
									prettyName.pop_back();
								}
							}
						}

						if (!prettyName.empty()) name = "Linux (" + prettyName + ")";
					}

					struct utsname uts{};
					string kernelVersion{};

					if (uname(&uts) != 0)
					{
						Log::Print(
							"Failed to get kernel version because uname() failed!",
							"KW_CORE",
							LogType::LOG_ERROR,
							2);
					}
					else kernelVersion = string(uts.release);

					name = onVM ? name + " (Virtual Machine)" : name;

					return { name, kernelVersion };
#endif
				};

			auto get_architecture = []() -> string
				{
#if defined(KWIN_ANY)
	#if defined(_M_ARM64)
					return "ARM64";
	#elif defined(_M_X64)
					return "x64";
	#elif defined(_M_IX86)
					return "x86";
	#else
					return "Unknown";
	#endif
#elif defined(KLIN_ANY)
					struct utsname uts{};
					if (uname(&uts) != 0)
					{
						Log::Print(
							"Failed to get architecture because uname() failed!",
							"KW_CORE",
							LogType::LOG_ERROR,
							2);

						return "Unknown";
					}

					return string(uts.machine);
#endif
				};

			auto get_wine_status = []() -> bool
				{
#if defined(KWIN_ANY)
					return GetProcAddress(
						GetModuleHandleW(L"ntdll.dll"), 
						"wine_get_version");
#elif defined(KLIN_ANY)
					return false;
#endif
				};

			auto get_vm_status = []() -> bool
				{
					u32 regs[4]{};
#if defined(KWIN_ANY)
					__cpuid((int*)regs, 1);
#elif defined(KLIN_ANY)
					__get_cpuid(
						1,
						&regs[0],
						&regs[1],
						&regs[2],
						&regs[3]);
#endif
					//ECX bit 31
					return (regs[2] & (1u << 31)) != 0;
				};

			bool onWine = get_wine_status();
			bool onVM = get_vm_status();

			pair<string, string> nv = get_os_name_and_version(
				onWine,
				onVM);

			osInfo.name = nv.first;
			osInfo.version = nv.second;
			osInfo.architecture = get_architecture();
			osInfo.isOnWine = onWine;
			osInfo.isOnVirtualMachine = onVM;

			hasInfo = true;
		}

		return osInfo;
	}
	string KalaWindowCore::GetOSInfoString()
	{
		OSInfo os = GetOSInfo();

		return
			"OS data:\n"
			"  Name: " + os.name + "\n"
			+ "  Version: " + os.version + "\n"
			+ "  Architecture: " + os.architecture;
	}

	void KalaWindowCore::ForceClose(
		string&& target,
		string&& reason)
	{
		Log::Print(
			"\n================"
			"\nFORCE CLOSE"
			"\n================\n",
			true);

		Log::Print(
			reason,
			target,
			LogType::LOG_ERROR,
			2,
			true,
			TimeFormat::TIME_NONE,
			DateFormat::DATE_NONE);

		CrashHandler::SetForceCloseContent(
			std::move(target),
			std::move(reason));

#if defined(KWIN_ANY)
		__debugbreak();
#elif defined(KLIN_ANY)
		raise(SIGTRAP);
#endif
	}
}