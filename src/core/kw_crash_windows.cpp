//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef _WIN32

#include <windows.h>
#include <dbghelp.h>

#include <cstring>
#include <string>
#include <array>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <functional>

#include "KalaHeaders/log_utils.hpp"
#include "KalaHeaders/thread_utils.hpp"

#include "core/kw_crash.hpp"
#include "core/kw_core.hpp"
#include "graphics/kw_window_global.hpp"

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;
using KalaHeaders::KalaLog::TimeFormat;

using KalaHeaders::KalaThread::auptr;
using KalaHeaders::KalaThread::memory_order_relaxed;

using KalaWindow::Core::CrashHandler;
using KalaWindow::Core::MAX_MESSAGE_LENGTH;
using KalaWindow::Graphics::Window_Global;
using KalaWindow::Graphics::PopupAction;
using KalaWindow::Graphics::PopupResult;
using KalaWindow::Graphics::PopupType;

using std::wstring;

using std::string;
using std::string_view;
using std::array;
using std::ofstream;
using std::ostringstream;
using std::filesystem::current_path;
using std::function;
using std::hex;
using std::dec;
using std::atomic;
using std::memory_order_relaxed;
using std::memcpy;

//The name of this program that is displayed in the title of the error popup
static string assignedProgramName;
//The user-defined function that is called when a crash occurs
static function<void()> assignedShutdownFunction{};
//Whether or not to create a dump file at crash
static bool canCreateDump;

//reserve (10 * MAX_MESSAGE_LENGTH) bytes for the last 10 log messages
static inline char crashLogBuffer[10][MAX_MESSAGE_LENGTH];
//Which slot are we currently on
static inline auptr crashLogIndex{};

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

namespace KalaWindow::Core
{
	void CrashHandler::Initialize(
		string_view programName,
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

	void CrashHandler::AppendToCrashLog(string_view message)
	{
		static_assert(
			MAX_MESSAGE_LENGTH > 1,
			"Max message length is too small!");

		auto trim = [](string_view s) -> string
			{
				size_t bytes = 0;
				size_t chars = 0;

				while (bytes < s.size()
					&& chars < MAX_MESSAGE_LENGTH)
				{
					unsigned char c = scast<unsigned char>(s[bytes]);

					size_t charLen =
						(c < 0x80) ? 1
						: (c < 0xE0) ? 2
						: (c < 0xF0) ? 3
						: 4;

					if (bytes + charLen > s.size()) break; //incomplete char

					bytes += charLen;
					++chars;
				}

				string result(s.data(), bytes);

				if (bytes < s.size()) result.append("\n[TRIMMED LONG MESSAGE]");

				return result;
			};

		const string trimmed = trim(message);

		const u32 index = crashLogIndex.fetch_add(1, memory_order_relaxed) % 10;

		char* slot = crashLogBuffer[index];

		//copy up to max allowed chars chars (reserve 1 for null terminator)
		const size_t copyLength = trimmed.size() < MAX_MESSAGE_LENGTH - 1 ? trimmed.size() : MAX_MESSAGE_LENGTH - 1;

		memcpy(slot, trimmed.data(), copyLength);

		//explicit null-termination
		slot[copyLength] = '\0';
	}
}

LONG WINAPI HandleCrash(EXCEPTION_POINTERS* info)
{
	auto get_crash_log_content = []()
        {
            array<string_view, 10> content{};

            const u32 head = crashLogIndex.load(memory_order_relaxed);

            for (u32 i = 0; i < 10; ++i)
            {
                const u32 index = (head + i) % 10;
                const char* entry = crashLogBuffer[index];

                if (entry[0] != '\0') content[i] = string_view(entry);
            }

            return content;
        };

	DWORD code = info->ExceptionRecord->ExceptionCode;

    //What the user sees
	ostringstream userStream{};

    //Whats written to the log file
    ostringstream logStream{};

	logStream << "\n========================================\n";

	logStream << "\n[CRASH DETECTED]\n\n";

	logStream << "Exception code: " << hex << code << dec << "\n";
	logStream << "Address: 0x" << hex << (uintptr_t)info->ExceptionRecord->ExceptionAddress << dec << "\n\n";

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

		logStream << "Reason: Access violation - attempted to " << accessStr
			<< " invalid memory at address 0x" << hex << accessType[1] << dec;

		if (accessType[0] == 8)
		{
			logStream << "(possible code execution or exploit attempt)";
		}
		logStream << "\n";
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


	//don't throw a scary call stack error popup for breakpoint (or force close via __debugbreak())
	case EXCEPTION_BREAKPOINT:
	{
		isBreakpoint = true;

		logStream << "Reason: Breakpoint hit (INT 3 instruction executed)\n";
		userStream << "A breakpoint or force close state was reached";

		break;
	}

	default:
		logStream << "Reason: Unknown exception\n";
		userStream << "An unknown exception was reached";
		break;
	}

	userStream << "!\n\n"
        << "The application must close and cannot continue running.\n"
        << "A log file has been created in the folder of this application.";

	AppendCallStackToStream(oss, info->ContextRecord);

	//append crash log buffer

	logStream << "\nRecent log activity before crash:\n\n";

	array<string_view, 10> content = get_crash_log_content();
	for (const auto& c : content)
	{
		if (!c.empty())
		{
			logStream << c;
			if (c.back() != '\n') logStream << '\n';
		}
	}

	logStream << "\n========================================\n";

	string timeStamp = Log::GetTime(TimeFormat::TIME_FILENAME);

	if (canCreateDump)
	{
		WriteMiniDump(
			info, 
			current_path().string().c_str(),
			timeStamp);

		logStream << "A dump file '" << timeStamp << ".dmp" << "' was created at exe root folder.";
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

	Log::Print(oss.str(), true);

	WriteLog(
		oss.str(),
		timeStamp);

	if (Window_Global::CreatePopup(
		assignedProgramName,
		userStream.str(),
		PopupAction::POPUP_ACTION_OK,
		PopupType::POPUP_TYPE_ERROR) ==
		PopupResult::POPUP_RESULT_OK)
	{
		if (assignedShutdownFunction) assignedShutdownFunction();
	}

	return EXCEPTION_EXECUTE_HANDLER;
}

void WriteMiniDump(
	EXCEPTION_POINTERS* info,
	string_view exePath,
	string_view timeStamp)
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

	logStream << "\n========================================\n";
	logStream << "\nCall stack:\n\n";

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
			logStream << "\n        line: " << dec << lineInfo.LineNumber;
		}

		logStream << " [0x" << hex << addr << "]\n" << dec;
	}

	SymCleanup(process);

	logStream << "\n========================================\n";
}

void WriteLog(
	string_view message,
	string_view timeStamp)
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