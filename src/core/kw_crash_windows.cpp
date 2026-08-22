//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include "core/kw_crash.hpp"

#if defined(KWIN_ANY)

#include <windows.h>
#include <dbghelp.h>
#include <libloaderapi.h>
#include <minwindef.h>
#include <processthreadsapi.h>

#include <cstring>
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>

#include "log_utils.hpp"
#include "file_utils.hpp"
#include "string_utils.hpp"

#include "core/kw_core.hpp"
#include "graphics/kw_window_global.hpp"

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;
using KalaHeaders::KalaLog::TimeFormat;

using KalaHeaders::KalaFile::WriteTextToFile;

using KalaHeaders::KalaString::DecToHex;

using KalaWindow::Core::KalaWindowCore;
using KalaWindow::Core::MAX_NAME_LENGTH;
using KalaWindow::Graphics::Window_Global;
using KalaWindow::Graphics::PopupAction;
using KalaWindow::Graphics::PopupResult;
using KalaWindow::Graphics::PopupType;

using std::wstring;

using std::string;
using std::string_view;
using std::ofstream;
using std::ostringstream;
using std::filesystem::path;

static bool isInitialized{};

static path exeDir{};

static string forceCloseTitle{};
static string forceCloseReason{};

//Whether or not to create a dump file at crash
static bool canCreateDump = true;

//Windows crash handler that calls the minidump creator function
//and sends error info to error popup
static LONG WINAPI HandleCrash(EXCEPTION_POINTERS* info);

//Creates a windows crash .dmp file to exe location.
static void WriteMiniDump(
	EXCEPTION_POINTERS* info,
	string_view exePath,
	string_view timeStamp);

//Appends up to to last 10 frames of the call stack upon crash
static void AppendCallStackToStream(
	ostringstream& oss, 
	CONTEXT* context);

//Write crash log to exe path
static void WriteLog(
	string_view message,
	string_view timeStamp);

using u32 = uint32_t;

namespace KalaWindow::Core
{
	void CrashHandler::Initialize()
	{
		exeDir = KalaWindowCore::GetExePath().parent_path();

		//reserve emergency stack space (for stack overflow handling)

		ULONG stackSize = 32768; //32KB
		SetThreadStackGuarantee(&stackSize);

		SetUnhandledExceptionFilter(HandleCrash);

#ifdef KW_NO_DUMP
		canCreateDump = false;
#endif

		isInitialized = true;

		Log::Print(
			"Initialized crash handler!",
			"KW_CRASH",
			LogType::LOG_SUCCESS);
	}

	bool CrashHandler::IsInitialized() { return isInitialized; }

	void CrashHandler::SetForceCloseContent(
        string&& title,
        string&& reason)
    {
		forceCloseTitle  = title.substr(0, MAX_NAME_LENGTH);
		forceCloseReason = reason.substr(0, MAX_REASON_LENGTH);
    }
}

LONG WINAPI HandleCrash(EXCEPTION_POINTERS* info)
{
	DWORD code = info->ExceptionRecord->ExceptionCode;

	if (code == EXCEPTION_BREAKPOINT)
	{
		if (Window_Global::CreatePopup(
			std::move(forceCloseTitle),
			std::move(forceCloseReason),
			PopupAction::POPUP_ACTION_OK,
			PopupType::POPUP_TYPE_ERROR) ==
			PopupResult::POPUP_RESULT_OK)
		{
			return EXCEPTION_EXECUTE_HANDLER;
		}
	}

    //What the user sees
	ostringstream userStream{};

    //Whats written to the log file
    ostringstream logStream{};

	logStream << "[CRASH DETECTED]\n\n";

    string hexCode{};
    DecToHex(code, hexCode);

	string hexAddr{};
	DecToHex((uintptr_t)info->ExceptionRecord->ExceptionAddress, hexAddr);

	logStream << "Exception code: " << hexCode << "\n";
	logStream << "Address: 0x" << hexAddr << "\n\n";

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

		string hexAccType{};
		DecToHex(accessType[1], hexAccType);

		logStream << "Reason: Access violation - attempted to " << accessStr
			<< " invalid memory at address 0x" << hexAccType;

		if (accessType[0] == 8)
		{
			logStream << "(possible code execution or exploit attempt)";
		}
		logStream << "\n";

		userStream << "Attempted to " << accessStr << " invalid memory";

		break;
	}
	case EXCEPTION_STACK_OVERFLOW:
		logStream << "Reason: Stack overflow (likely due to infinite recursion)\n";
		userStream << "A stack overflow was hit";

		break;
	case EXCEPTION_INT_DIVIDE_BY_ZERO:
		logStream << "Reason: Integer divide by zero\n";
		userStream << "An integer divide by zero error was reached";

		break;

		//
		// RARE BUT USEFUL CRASHES
		//

	case EXCEPTION_ILLEGAL_INSTRUCTION:
		logStream << "Reason: Illegal CPU instruction executed\n";
		userStream << "An illegal CPU instruction was executed";

		break;
	case EXCEPTION_GUARD_PAGE:
		logStream << "Reason: Guard page accessed (likely stack guard or memory protection violation)\n";
		userStream << "The guard page was accessed";

		break;
	case EXCEPTION_PRIV_INSTRUCTION:
		logStream << "Reason: Privileged instruction executed in user mode\n";
		userStream << "A privileged instruction was executed in user mode";

		break;
	case EXCEPTION_NONCONTINUABLE_EXCEPTION:
		logStream << "Reason: Attempted to continue after a non-continuable exception (fatal logic error)\n";
		userStream << "An attempt to continue after a non-continuable exception was reached";

		break;
	case EXCEPTION_IN_PAGE_ERROR:
		logStream << "Reason: Memory access failed (I/O or paging failure)\n";
		userStream << "Memory access failed";

		break;

	//ignore single step
	case EXCEPTION_SINGLE_STEP: return EXCEPTION_CONTINUE_SEARCH;

	default:
		string outValue{};
		DecToHex(scast<u64>(code), outValue);

		logStream << "Unknown exception! Code: " + outValue + "\n";
		userStream << "An unknown exception was reached";
		break;
	}

	userStream << "!\n\n"
        << "The application must close and cannot continue running.\n"
        << "A log file has been created in the folder of this application.";

    logStream << "\n========================================\n\n";

    logStream << "System info:\n"
		<< KalaWindowCore::GetCPUInfoString()
        << "\n\n"
        << KalaWindowCore::GetGPUInfoString()
        << "\n\n"
        << KalaWindowCore::GetRAMInfoString(true)
        << "\n\n"
        << KalaWindowCore::GetOSInfoString()
        << "\n\n";

    logStream << "========================================\n\n";

	AppendCallStackToStream(logStream, info->ContextRecord);

	logStream << "\n========================================\n\n";

	string timeStamp = Log::GetTime(TimeFormat::TIME_FILENAME);

	if (canCreateDump)
	{
		WriteMiniDump(
			info, 
			exeDir.string().c_str(),
			timeStamp);

		logStream << "A dump file '" << timeStamp << ".dmp" << "' was created at exe root folder.";
	}
	else
	{
		Log::Print(
			"Dump file creation disabled by user.",
			"KW_CRASH",
			LogType::LOG_INFO,
			0,
			true);
	}

	Log::Print(userStream.str());

	WriteLog(
		logStream.str(),
		timeStamp);

	if (Window_Global::CreatePopup(
		string(Window_Global::GetAppName()),
		userStream.str(),
		PopupAction::POPUP_ACTION_OK,
		PopupType::POPUP_TYPE_ERROR) == PopupResult::POPUP_RESULT_OK)
	{
		TerminateProcess(
			GetCurrentProcess(),
			scast<UINT>(code));
	}

	return EXCEPTION_EXECUTE_HANDLER;
}

void WriteMiniDump(
	EXCEPTION_POINTERS* info,
	string_view exePath,
	string_view timeStamp)
{
	string filePath = string(timeStamp) + ".dmp";

	int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, string(exePath).c_str(), -1, nullptr, 0);
	wstring widePath(sizeNeeded - 1, 0); // -1 to exclude null terminator
	MultiByteToWideChar(CP_UTF8, 0, string(exePath).c_str(), -1, &widePath[0], sizeNeeded);

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
			"KW_CRASH",
			LogType::LOG_INFO);

		dumpInfo.ExceptionPointers = info;
		dumpInfo.ClientPointers = FALSE;

		static constexpr MINIDUMP_TYPE dumpType = scast<MINIDUMP_TYPE>(
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
	ostringstream& logStream, 
	CONTEXT* context)
{
	HANDLE process = GetCurrentProcess();

	HANDLE thread = GetCurrentThread();
	DWORD mdThreadID = GetThreadId(thread);

	ostringstream debugMsg{};
	debugMsg << "Stackwalk thread : " << mdThreadID;

	Log::Print(
		debugMsg.str(),
		"KW_CRASH",
		LogType::LOG_INFO);

	SymSetOptions(
		SYMOPT_LOAD_LINES         //file/line info
		| SYMOPT_UNDNAME          //demangle c++ symbols
		| SYMOPT_DEFERRED_LOADS); //don't load all symbols immediately (faster)
	SymInitialize(
		process, 
		exeDir.string().c_str(), 
		TRUE);

	STACKFRAME64 stack = {};

	DWORD machineType = IMAGE_FILE_MACHINE_AMD64;
	stack.AddrPC.Offset = context->Rip;
	stack.AddrFrame.Offset = context->Rbp;
	stack.AddrStack.Offset = context->Rsp;

	stack.AddrPC.Mode = AddrModeFlat;
	stack.AddrFrame.Mode = AddrModeFlat;
	stack.AddrStack.Mode = AddrModeFlat;

	logStream << "Call stack:\n\n";

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
		SYMBOL_INFO* symbol = rcast<SYMBOL_INFO*>(symbolBuffer);
		symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
		symbol->MaxNameLen = MAX_SYM_NAME;

		logStream << "  " << i << ": ";
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
				logStream << demangled;
			}
			else logStream << symbol->Name;
		}
		else logStream << "(symbol not found)\n";

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

			logStream << "\n        script: " << shortPath;
			logStream << "\n        line: " << lineInfo.LineNumber;
		}

		string hexAddr{};
		DecToHex(addr, hexAddr);

		logStream << " [0x" << hexAddr << "]\n";
	}

	SymCleanup(process);
}

void WriteLog(
	string_view message,
	string_view timeStamp)
{
	string fileName = string(timeStamp) + ".txt";
	path fullPath = exeDir / fileName;

	string err = WriteTextToFile(
		fullPath,
		message);

	if (!err.empty())
	{
		Log::Print(
			"Failed to write to log file '" + fullPath.string() + "'! Reason: " + err,
			"KW_CRASH",
			LogType::LOG_ERROR,
			2);
	}
}

#endif //KWIN_ANY