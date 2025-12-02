//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef _WIN32

#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <functional>
#include <Windows.h>
#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")

#include "KalaHeaders/log_utils.hpp"

#include "core/kw_crash.hpp"
#include "core/kw_core.hpp"
#include "graphics/kw_window_global.hpp"

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;
using KalaHeaders::KalaLog::TimeFormat;
using KalaHeaders::KalaLog::DateFormat;

using KalaWindow::Graphics::Window_Global;
using KalaWindow::Graphics::PopupAction;
using KalaWindow::Graphics::PopupResult;
using KalaWindow::Graphics::PopupType;

using std::string;
using std::ofstream;
using std::ostringstream;
using std::filesystem::path;
using std::filesystem::current_path;
using std::function;
using std::wstring;
using std::hex;
using std::dec;

//The name of this program that is displayed in the title of the error popup
static string assignedProgramName;
//The user-defined function that is called when a crash occurs
static function<void()> assignedShutdownFunction{};
//Whether or not to create a dump file at crash
static bool canCreateDump;

//Windows crash handler that calls the minidump creator function
//and sends error info to error popup
static LONG WINAPI HandleCrash(EXCEPTION_POINTERS* info);

//Creates a windows crash .dmp file to exe location.
static void WriteMiniDump(
	EXCEPTION_POINTERS* info,
	const string& exePath,
	const string& timeStamp);

//Appends up to to last 10 frames of the call stack upon crash
static void AppendCallStackToStream(
	ostringstream& oss, 
	CONTEXT* context);

//Write crash log to exe path
static void WriteLog(
	const string& message,
	const string& timeStamp);

namespace KalaWindow::Core
{
	void CrashHandler::Initialize(
		const string& programName,
		const function<void()>& shutdownFunction,
		bool createDump)
	{
		//reserve emergency stack space (for stack overflow handling)

		ULONG stackSize = 32768; //32KB
		SetThreadStackGuarantee(&stackSize);

		SetUnhandledExceptionFilter(HandleCrash);

		assignedProgramName = programName;
		assignedShutdownFunction = shutdownFunction;
		canCreateDump = createDump;

		Log::Print(
			"Initialized crash handler!",
			"CRASH_HANDLER",
			LogType::LOG_SUCCESS);
	}
}

LONG WINAPI HandleCrash(EXCEPTION_POINTERS* info)
{
	DWORD code = info->ExceptionRecord->ExceptionCode;

	ostringstream oss;
	oss << "Crash detected!\n\n";

	oss << "Exception code: " << hex << code << "\n";
	oss << "Address: 0x" << hex << (uintptr_t)info->ExceptionRecord->ExceptionAddress << "\n\n";

	switch (code)
	{
		//
		// COMMON AND HIGH PRIORITY CRASH TYPES
		//

	case EXCEPTION_ACCESS_VIOLATION:
	{
		const ULONG_PTR* accessType = info->ExceptionRecord->ExceptionInformation;

		const char* accessStr = "unknown";
		switch (accessType[0])
		{
		case 0: accessStr = "read from"; break;
		case 1: accessStr = "write to"; break;
		case 8: accessStr = "execute"; break;
		}

		oss << "Reason: Access violation - attempted to " << accessStr
			<< " invalid memory at address 0x" << hex << accessType[1];

		if (accessType[0] == 8)
		{
			oss << "(possible code execution or exploit attempt)";
		}
		oss << "\n";
		break;
	}
	case EXCEPTION_STACK_OVERFLOW:
		oss << "Reason: Stack overflow (likely due to infinite recursion)\n";
		break;
	case EXCEPTION_INT_DIVIDE_BY_ZERO:
		oss << "Reason: Integer divide by zero\n";
		break;

		//
		// RARE BUT USEFUL CRASHES
		//

	case EXCEPTION_ILLEGAL_INSTRUCTION:
		oss << "Reason: Illegal CPU instruction executed\n";
		break;
	case EXCEPTION_BREAKPOINT:
		oss << "Reason: Breakpoint hit (INT 3 instruction executed)\n";
		break;
	case EXCEPTION_GUARD_PAGE:
		oss << "Reason: Guard page accessed (likely stack guard or memory protection violation)\n";
		break;
	case EXCEPTION_PRIV_INSTRUCTION:
		oss << "Reason: Privileged instruction executed in user mode\n";
		break;
	case EXCEPTION_NONCONTINUABLE_EXCEPTION:
		oss << "Reason: Attempted to continue after a non-continuable exception (fatal logic error)\n";
		break;
	case EXCEPTION_IN_PAGE_ERROR:
		oss << "Reason: Memory access failed (I/O or paging failure)\n";
		break;

	default:
		oss << "Reason: Unknown exception (code: 0x" << hex << code << ")\n";
		break;
	}

	AppendCallStackToStream(oss, info->ContextRecord);

	string timeStamp = Log::GetTime(TimeFormat::TIME_FILENAME);

	if (canCreateDump)
	{
		WriteMiniDump(
			info, 
			current_path().string().c_str(),
			timeStamp);

		oss << "A dump file '" << timeStamp << ".dmp" << "' was created at exe root folder.";
	}
	else
	{
		Log::Print(
			"Dump file creation disabled by user.",
			"CRASH_HANDLER",
			LogType::LOG_DEBUG,
			0,
			true);
	}

	if (Window_Global::CreatePopup(
		assignedProgramName,
		oss.str(),
		PopupAction::POPUP_ACTION_OK,
		PopupType::POPUP_TYPE_ERROR) ==
		PopupResult::POPUP_RESULT_OK)
	{
		WriteLog(
			oss.str(),
			timeStamp);

		Log::Print(
			oss.str(),
			"FORCE CLOSE",
			LogType::LOG_ERROR,
			2,
			true,
			TimeFormat::TIME_NONE,
			DateFormat::DATE_NONE);

		if (assignedShutdownFunction) assignedShutdownFunction();
	}

	return EXCEPTION_EXECUTE_HANDLER;
}

void WriteMiniDump(
	EXCEPTION_POINTERS* info,
	const string& exePath,
	const string& timeStamp)
{
	string filePath = timeStamp + ".dmp";

	int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, exePath.c_str(), -1, nullptr, 0);
	wstring widePath(sizeNeeded - 1, 0); // -1 to exclude null terminator
	MultiByteToWideChar(CP_UTF8, 0, exePath.c_str(), -1, &widePath[0], sizeNeeded);

	//build full path to dump file
	widePath += L"\\" + wstring(filePath.begin(), filePath.end());

	HANDLE hFile = CreateFileW(
		widePath.c_str(),
		GENERIC_WRITE,
		0,
		nullptr,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		nullptr);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		MINIDUMP_EXCEPTION_INFORMATION dumpInfo{};

		DWORD mdThreadID = GetThreadId(GetCurrentThread());
		dumpInfo.ThreadId = mdThreadID;

		ostringstream debugMsg{};
		debugMsg << "Minidump thread: " << mdThreadID;

		Log::Print(
			debugMsg.str(),
			"CRASH_HANDLER",
			LogType::LOG_DEBUG);

		dumpInfo.ExceptionPointers = info;
		dumpInfo.ClientPointers = FALSE;

		constexpr MINIDUMP_TYPE dumpType = static_cast<MINIDUMP_TYPE>(
			MiniDumpWithIndirectlyReferencedMemory  //includes memory referenced by the stack
			| MiniDumpScanMemory                    //helps resolve pointers for better stack analysis
			| MiniDumpWithThreadInfo                //thread names and IDs
			| MiniDumpWithUnloadedModules);         //helps with crashes during shutdown/unloads

		MiniDumpWriteDump(
			GetCurrentProcess(),
			GetCurrentProcessId(),
			hFile,
			dumpType,
			&dumpInfo,
			nullptr,
			nullptr);

		CloseHandle(hFile);
	}
}

void AppendCallStackToStream(
	ostringstream& oss, 
	CONTEXT* context)
{
	HANDLE process = GetCurrentProcess();

	HANDLE thread = GetCurrentThread();
	DWORD mdThreadID = GetThreadId(thread);

	ostringstream debugMsg{};
	debugMsg << "Stackwalk thread : " << mdThreadID;

	Log::Print(
		debugMsg.str(),
		"CRASH_HANDLER",
		LogType::LOG_DEBUG);

	SymSetOptions(
		SYMOPT_LOAD_LINES         //file/line info
		| SYMOPT_UNDNAME          //demangle c++ symbols
		| SYMOPT_DEFERRED_LOADS); //don't load all symbols immediately (faster)
	SymInitialize(
		process, 
		nullptr, 
		TRUE);

	STACKFRAME64 stack = {};

	DWORD machineType = IMAGE_FILE_MACHINE_AMD64;
	stack.AddrPC.Offset = context->Rip;
	stack.AddrFrame.Offset = context->Rbp;
	stack.AddrStack.Offset = context->Rsp;

	stack.AddrPC.Mode = AddrModeFlat;
	stack.AddrFrame.Mode = AddrModeFlat;
	stack.AddrStack.Mode = AddrModeFlat;

	oss << "\n========================================\n\n";
	oss << "Call stack:\n";

	for (int i = 0; i < 10; ++i)
	{
		if (!StackWalk64(
			machineType,
			process,
			thread,
			&stack,
			context,
			nullptr,
			SymFunctionTableAccess64,
			SymGetModuleBase64,
			nullptr))
		{
			break;
		}

		DWORD64 addr = stack.AddrPC.Offset;
		if (addr == 0) break;

		char symbolBuffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME]{};
		SYMBOL_INFO* symbol = reinterpret_cast<SYMBOL_INFO*>(symbolBuffer);
		symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
		symbol->MaxNameLen = MAX_SYM_NAME;

		oss << "  " << i << ": ";
		DWORD64 displacement = 0;
		if (SymFromAddr(
			process,
			addr,
			&displacement,
			symbol))
		{
			char demangled[1024];
			if (UnDecorateSymbolName(
				symbol->Name,
				demangled,
				sizeof(demangled),
				UNDNAME_COMPLETE))
			{
				oss << demangled;
			}
			else oss << symbol->Name;
		}
		else oss << "(symbol not found)\n";

		//file and line info
		IMAGEHLP_LINE64 lineInfo;
		DWORD lineDisplacement = 0;
		ZeroMemory(&lineInfo, sizeof(lineInfo));
		lineInfo.SizeOfStruct = sizeof(lineInfo);

		if (SymGetLineFromAddr64(
			process,
			addr,
			&lineDisplacement,
			&lineInfo))
		{
			string fullPath = lineInfo.FileName;
			size_t lastSlash = fullPath.find_last_of("\\/");
			size_t secondLastSlash = fullPath.find_last_of("\\/", lastSlash - 1);

			string shortPath = (secondLastSlash != string::npos)
				? fullPath.substr(secondLastSlash + 1)
				: fullPath;

			oss << "\n        script: " << shortPath;
			oss << "\n        line: " << dec << lineInfo.LineNumber;
		}

		oss << " [0x" << hex << addr << "]\n";
	}

	SymCleanup(process);

	oss << "\n========================================\n\n";
}

void WriteLog(
	const string& message,
	const string& timeStamp)
{
	string fileName = timeStamp + ".txt";
	string fullPath = (current_path() / fileName).string();

	ofstream logFile(fullPath);

	if (!logFile.is_open())
	{
		Log::Print(
			"Failed to open log file to write into it!",
			"CRASH_HANDLER",
			LogType::LOG_ERROR);

		return;
	}

	logFile << message;

	logFile.close();
}

#endif //_WIN32