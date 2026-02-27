//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef __linux__

#include <signal.h>
#include <csignal>
#include <unistd.h>
#include <ucontext.h>
#include <sys/ucontext.h>
#include <sys/wait.h>
#include <execinfo.h>
#include <dlfcn.h>
#include <cxxabi.h>

#include <sstream>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <string>
#include <functional>
#include <array>
#include <fstream>
#include <filesystem>

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

using KalaWindow::Core::KalaWindowCore;
using KalaWindow::Core::CrashHandler;
using KalaWindow::Core::MAX_MESSAGE_LENGTH;
using KalaWindow::Graphics::Window_Global;
using KalaWindow::Graphics::PopupAction;
using KalaWindow::Graphics::PopupType;
using KalaWindow::Graphics::PopupResult;

using u32 = uint32_t;
using std::memcpy;
using std::free;
using std::string;
using std::string_view;
using std::to_string;
using std::function;
using std::ostringstream;
using std::hex;
using std::dec;
using std::array;
using std::ofstream;
using std::filesystem::path;

static bool isInitialized{};

static volatile sig_atomic_t inCrashHandler{};

static stack_t altStack{};

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

static string GetExePath()
{
	char buffer[PATH_MAX]{};
	ssize_t len = readlink(
		"/proc/self/exe", 
		buffer,
		sizeof(buffer) -1);

	buffer[len] = '\0';

	string fullPath(buffer);
	size_t lastSlash = fullPath.find_last_of('/');
	return fullPath.substr(0, lastSlash);
}

static void SetUpAlternateStack()
{
    const size_t stackSize = 256 * 1024; //256KB

    altStack.ss_sp = malloc(stackSize);

    if (!altStack.ss_sp)
    {
        KalaWindowCore::ForceClose(
            "Crash handler error",
            "Failed to allocate memory for alternate stack!");
    }

    altStack.ss_size = stackSize;
    altStack.ss_flags = 0;

    if (sigaltstack(&altStack, nullptr) != 0)
    {
        perror("signalstack");
        _exit(1);
    }
} 

static void HandleCrash(
    int signal,
    siginfo_t* info,
    void* ucontext);

static void GenerateFullCrashReport(
    int signal,
    siginfo_t* info,
    void* ucontext);

void WriteMiniDump(
	string_view exePath,
	string_view timeStamp);

//Appends up to to last 10 frames of the call stack upon crash
static void AppendCallStackToStream(ostringstream& oss);

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
        if (isInitialized)
        {
            Log::Print(
			    "Failed to initialize crash handler because it has already been initialized!",
			    "CRASH_HANDLER",
			    LogType::LOG_ERROR,
                2);

            return;
        }

        SetUpAlternateStack();

        struct sigaction sa{};
        sa.sa_flags = 
            SA_SIGINFO
            | SA_ONSTACK;

        sa.sa_sigaction = HandleCrash;
        sigemptyset(&sa.sa_mask);

        sigaction(
            SIGSEGV,
            &sa,
            nullptr);
        sigaction(
            SIGFPE,
            &sa,
            nullptr);
        sigaction(
            SIGILL,
            &sa,
            nullptr);
        sigaction(
            SIGABRT,
            &sa,
            nullptr);

		assignedProgramName = programName;
		assignedShutdownFunction = shutdownFunction;
		canCreateDump = createDump;

        isInitialized = true;

		Log::Print(
			"Initialized crash handler!",
			"CRASH_HANDLER",
			LogType::LOG_SUCCESS);
    }

    bool CrashHandler::IsInitialized() { return isInitialized; }

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

void HandleCrash(
    int signal,
    siginfo_t* info,
    void* ucontext)
{
    if (inCrashHandler) _exit(2);
    inCrashHandler = 1;

    pid_t pid = fork();
    if (pid == 0) GenerateFullCrashReport(signal, info, ucontext);

    _exit(1);
}

void GenerateFullCrashReport(
    int signal,
    siginfo_t* info,
    void* ucontext)
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

	//special breakpoint-only stream
	ostringstream bposs{};
	bool isBreakpoint{};

    ostringstream oss{};
    oss << "\n========================================\n";
    oss << "\n[CRASH DETECTED]\n\n";

    oss << "Exception code: " << hex << signal << dec << "\n";

    uintptr_t addr = (info && info->si_addr)
        ? (uintptr_t)info->si_addr
        : 0;
    oss << "Address: 0x" << hex << addr << "\n\n";

    oss << "Reason: ";

    switch (signal)
    {
        case SIGSEGV:
        {
            uintptr_t fault = addr;
            uintptr_t rsp{};

            if (ucontext)
            {
                ucontext_t* ctx = (ucontext_t*)ucontext;
                rsp = ctx->uc_mcontext.gregs[REG_RSP];

                //fault very close to stack pointer
                const uintptr_t threshold = 65536;

                if (rsp
                    && fault <= rsp
                    && (rsp - fault) < threshold)
                {
                    oss << "Stack overflow (likely due to infinite recursion)\n";
                    break;
                }
            }

            //otherwise treat as normal access violation
            oss << "Access violation - attempted to ";

            if (info 
                && SEGV_ACCERR)
            {
                oss << "access protected memory";
            }
            else oss << "access invalid memory";

            oss << " at address 0x" << hex << addr << dec << "\n";
            break;
        }
        case SIGFPE:
            if (info
                && info->si_code == FPE_INTDIV)
            {
                oss << "Integer divide by zero";
            }
            else oss << "Arithmetic exception";

            oss << "\n";
            break;
        case SIGILL:
            if (info)
            {
                switch (info->si_code)
                {
                    case ILL_PRVOPC:
                    case ILL_PRVREG:
                        oss << "Privileged instruction executed in user mode";
                        break;
                    default:
                        oss << "Illegal CPU instruction executed";
                        break;
                }
            }
            else oss << "Illegal CPU instruction executed";

            oss << "\n";
            break;
        case SIGABRT:
            oss << "Abort signal received\n";
            break;

        //don't throw a scary call stack error popup for breakpoint (or force close via raise(SIGTRAP))
        case SIGTRAP:
        {
            isBreakpoint = true;

            oss << "Breakpoint hit (INT 3 instruction executed)\n";

            bposs << "A breakpoint or force close state was reached!\n\n"
                << "The application must close and cannot continue running.\n"
                << "A log file has been created in the folder of this application.";

            break;
        }
        
        default:
            oss << "Unknown exception\n";
            break;
    }

    if (ucontext)
    {
        ucontext_t* ctx = (ucontext_t*)ucontext;
        uintptr_t rip = ctx->uc_mcontext.gregs[REG_RIP];

        oss << "Instruction pointer: " << hex << rip << dec << "\n";
    }

    AppendCallStackToStream(oss);

	//append crash log buffer

	oss << "\nRecent log activity before crash:\n\n";

	array<string_view, 10> content = get_crash_log_content();
	for (const auto& c : content)
	{
		if (!c.empty())
		{
			oss << c;
			if (c.back() != '\n') oss << '\n';
		}
	}

	oss << "\n========================================\n";

	string timeStamp = Log::GetTime(TimeFormat::TIME_FILENAME);

    if (canCreateDump)
    {
        WriteMiniDump(
			GetExePath(),
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

	Log::Print(oss.str(), true);

	WriteLog(
		oss.str(),
		timeStamp);

	if (Window_Global::CreatePopup(
		assignedProgramName,
		isBreakpoint ? bposs.str() : oss.str(),
		PopupAction::POPUP_ACTION_OK,
		PopupType::POPUP_TYPE_ERROR) ==
		PopupResult::POPUP_RESULT_OK)
	{
		if (assignedShutdownFunction) assignedShutdownFunction();
	}
}

void WriteMiniDump(
	string_view exePath,
	string_view timeStamp)
{
    string output = 
        string(exePath) + "/"
        + string(timeStamp) + ".dmp";

    //we are inside the forked crash child,
    //the crashed process is the parent
    pid_t targetPID = getppid();

    pid_t pid = fork();

    if (pid == 0)
    {
        //child of crash child runs gcore
        execlp(
            "gcore",
            "gcore",
            "-o",
            output.c_str(),
            to_string(targetPID).c_str(),
            nullptr);

        _exit(1);
    }

    //wait for gcore to finish
    if (pid > 0) waitpid(pid, nullptr, 0);
}

void AppendCallStackToStream(ostringstream& oss)
{
    constexpr int MAX_FRAMES = 32;

    void* frames[MAX_FRAMES]{};
    int frameCount = backtrace(frames, MAX_FRAMES);

    oss << "\n========================================\n";
    oss << "\nCall stack:\n\n";

    for (int i = 0; i < frameCount && i < 10; ++i)
    {
        void* addr = frames[i];

        oss << "  " << i << ": ";

        Dl_info info{};
        if (dladdr(addr, &info)
            && info.dli_sname)
        {
            int status{};
            char* demangled = abi::__cxa_demangle(
                info.dli_sname,
                nullptr,
                nullptr,
                &status);

            if (status == 0
                && demangled)
            {
                oss << demangled;
                free(demangled);
            }
            else oss << info.dli_sname;

            if (info.dli_fname) oss << "\n        module: " << info.dli_fname;
        }
        else oss << "(symbol not found)";

        oss << "[0x" << hex << rcast<uintptr_t>(addr) << dec << "]\n";
    }

    oss << "\n========================================\n";
}

void WriteLog(
	string_view message,
	string_view timeStamp)
{
	string fileName = string(timeStamp) + ".txt";
	path fullPath = (path(GetExePath()) / fileName).string();

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

#endif