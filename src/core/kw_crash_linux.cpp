//Copyright(C) 2026 Lost Empire Entertainment
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
#include <sys/prctl.h>
#include <execinfo.h>
#include <dlfcn.h>
#include <cxxabi.h>

#include <sstream>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <string>
#include <fstream>
#include <filesystem>

#include "log_utils.hpp"

#include "core/kw_crash.hpp"
#include "core/kw_core.hpp"
#include "graphics/kw_window_global.hpp"

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;
using KalaHeaders::KalaLog::TimeFormat;

using KalaWindow::Core::KalaWindowCore;
using KalaWindow::Core::CrashHandler;
using KalaWindow::Graphics::Window_Global;
using KalaWindow::Graphics::PopupAction;
using KalaWindow::Graphics::PopupType;

using std::free;
using std::string;
using std::string_view;
using std::to_string;
using std::ostringstream;
using std::hex;
using std::dec;
using std::ofstream;
using std::filesystem::path;

static bool isInitialized{};

static path exeDir{};

static string forceCloseTitle{};
static string forceCloseReason{};

//The name of this program that is displayed in the title of the error popup
static string assignedProgramName;
//Whether or not to create a dump file at crash
static bool canCreateDump = true;

static volatile sig_atomic_t inCrashHandler{};

static stack_t altStack{};

static void SetUpAlternateStack()
{
    exeDir = KalaWindowCore::GetExePath().parent_path();

    const size_t stackSize = 256 * 1024; //256KB

    altStack.ss_sp = malloc(stackSize);

    if (!altStack.ss_sp)
    {
        KalaWindowCore::ForceClose(
            "KalaWindow crash error",
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
    void CrashHandler::Initialize(string&& programName)
    {
        if (isInitialized)
        {
            Log::Print(
			    "Failed to initialize crash handler because it has already been initialized!",
			    "KW_CRASH",
			    LogType::LOG_ERROR,
                2);

            return;
        }

        SetUpAlternateStack();

        struct sigaction sa{};
        sa.sa_flags = 
            SA_SIGINFO
            | SA_ONSTACK
            | SA_NODEFER;

        sa.sa_sigaction = HandleCrash;
        sigemptyset(&sa.sa_mask);

        sigaction(
            SIGBUS,
            &sa,
            nullptr);
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
        sigaction(
            SIGTRAP,
            &sa,
            nullptr);

		assignedProgramName = std::move(programName);

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

void HandleCrash(
    int signal,
    siginfo_t* info,
    void* ucontext)
{
    if (inCrashHandler) _exit(2);
    inCrashHandler = 1;

    //let any process attach a debugger
    prctl(
        PR_SET_PTRACER,
        PR_SET_PTRACER_ANY,
        0,
        0,
        0);

    pid_t pid = fork();
    if (pid == -1)
    {
        perror("KW_CRASH (failed to fork for Zenity)");
        _exit(1);
    }

    if (pid == 0)
    {
        if (signal == SIGTRAP)
        {
            string title = !forceCloseTitle.empty()
                ? forceCloseTitle
                : "KalaWindow force close error";
            string reason = !forceCloseTitle.empty()
                ? forceCloseReason
                : "An unknown force close occurred.";

            Window_Global::CreatePopup(
                std::move(title),
                std::move(reason),
                PopupAction::POPUP_ACTION_OK,
                PopupType::POPUP_TYPE_ERROR);

            _exit(1);
        }
        else GenerateFullCrashReport(signal, info, ucontext);

        _exit(1);
    }

    int status{};
    waitpid(pid, &status, 0);

    _exit(1);
}

void GenerateFullCrashReport(
    int signal,
    siginfo_t* info,
    void* ucontext)
{
    //What the user sees
	ostringstream userStream{};

    //Whats written to the log file
    ostringstream logStream{};

    logStream << "\n========================================\n";
    logStream << "\n[CRASH DETECTED]\n\n";

    logStream << "Exception code: " << hex << signal << dec << "\n";

    uintptr_t addr = (info && info->si_addr)
        ? (uintptr_t)info->si_addr
        : 0;
    logStream << "Address: 0x" << hex << addr << "\n\n";

    logStream << "Reason: ";

    switch (signal)
    {
        case SIGBUS:
        {
            logStream << "Bus error - invalid physical memory access at address 0x" << hex << addr << dec << "\n";
            userStream << "A bus error (invalid physical memory access) was reached";
            break;
        }
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
                    logStream << "Stack overflow (likely due to infinite recursion)\n";
                    userStream << "A stack overflow was hit";
                    break;
                }
            }

            //otherwise treat as normal access violation
            logStream << "Access violation - attempted to access invalid memory at address 0x" << hex << addr << dec << "\n";
            userStream << "An attempt to access invalid memory was reached";
            break;
        }
        case SIGFPE:
            if (info
                && info->si_code == FPE_INTDIV)
            {
                logStream << "Integer divide by zero";
                userStream << "An integer divide by zero error was reached";
            }
            else
            {
                logStream << "Arithmetic exception";
                userStream << "An arithmetic exception was reached";
            }

            logStream << "\n";
            break;
        case SIGILL:
            logStream << "Illegal CPU instruction executed\n";
            userStream << "An illegal CPU instruction was executed";
            break;
        case SIGABRT:
            logStream << "Abort signal received\n";
            userStream << "An abort signal was reached";
            break;
        
        default:
            logStream << "Unknown exception\n";
            userStream << "An unknown exception was reached";
            break;
    }

    userStream << "!\n\n"
        << "The application must close and cannot continue running.\n"
        << "A log file has been created in the folder of this application.";

    if (ucontext)
    {
        ucontext_t* ctx = (ucontext_t*)ucontext;
        uintptr_t rip = ctx->uc_mcontext.gregs[REG_RIP];

        logStream << "Instruction pointer: " << hex << rip << dec << "\n";
    }

    logStream << "\n========================================\n\n";
    logStream << "System info\n\n";

    logStream
        << KalaWindowCore::GetCPUInfoString()
        << "\n\n"
        << KalaWindowCore::GetGPUInfoString()
        << "\n\n"
        << KalaWindowCore::GetRAMInfoString(true)
        << "\n\n"
        << KalaWindowCore::GetOSInfoString()
        << "\n\n";

    logStream << "\n========================================\n";

    AppendCallStackToStream(logStream);

	logStream << "\n========================================\n";

	string timeStamp = Log::GetTime(TimeFormat::TIME_FILENAME);

    if (canCreateDump)
    {
        WriteMiniDump(
			exeDir.string(),
			timeStamp);

        logStream << "A dump file '" << timeStamp << ".dmp" << "' was created at exe root folder.";
    }
    else
    {
        Log::Print(
			"Dump file creation disabled by user.",
			"KW_CRASH",
			LogType::LOG_DEBUG,
			0,
			true);
    }

	Log::Print(logStream.str(), true);

	WriteLog(
		logStream.str(),
		timeStamp);

	Window_Global::CreatePopup(
		std::move(assignedProgramName),
		userStream.str(),
		PopupAction::POPUP_ACTION_OK,
		PopupType::POPUP_TYPE_ERROR);

    _exit(1);
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

void AppendCallStackToStream(ostringstream& logStream)
{
    static constexpr int MAX_FRAMES = 32;

    void* frames[MAX_FRAMES]{};
    int frameCount = backtrace(frames, MAX_FRAMES);

    logStream << "\n========================================\n";
    logStream << "\nCall stack:\n\n";

    for (int i = 0; i < frameCount && i < 10; ++i)
    {
        void* addr = frames[i];

        logStream << "  " << i << ": ";

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
                logStream << demangled;
                free(demangled);
            }
            else logStream << info.dli_sname;

            if (info.dli_fname) logStream << "\n        module: " << info.dli_fname;
        }
        else logStream << "(symbol not found)";

        logStream << "[0x" << hex << rcast<uintptr_t>(addr) << dec << "]\n";
    }
}

void WriteLog(
	string_view message,
	string_view timeStamp)
{
	string fileName = string(timeStamp) + ".txt";
	path fullPath = (exeDir / fileName).string();

	ofstream logFile(fullPath);

	if (!logFile.is_open())
	{
		Log::Print(
			"Failed to open log file to write into it!",
			"KW_CRASH",
			LogType::LOG_ERROR,
            2);

		return;
	}

	logFile << message;

	logFile.close();
}

#endif