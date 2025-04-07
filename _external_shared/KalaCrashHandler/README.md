# KalaCrashHandler

[![License](https://img.shields.io/badge/license-Zlib-blue)](LICENSE.md)
![Platform](https://img.shields.io/badge/platform-Windows-brightgreen)
![Development Stage](https://img.shields.io/badge/development-Alpha-yellow)

![Logo](logo.png)

KalaCrashHandler is a lightweight C++ 20 library for Windows that detects mostly common and some rarer but useful crashes caused by the program the crash handler is attached to. It will display crash messages in the error popup and it will generate a log file with extra info and a dmp file for debugging.

# Prerequisites (when compiling from source code)

- Visual Studio 2022 (with C++ CMake tools and Windows 10 or 11 SDK)
- Ninja and CMake 3.30.3 or newer (or extract Windows_prerequsites.7z and run setup.bat)

To compile from source code simply run 'build_windows_release.bat' or 'build_windows_debug.bat' depending on your preferences then copy and attach the dll, lib and header files with your preferred way to your program source directory.

# How to use

```cpp
#include <string>
#include <cstdlib> //for shutting down program

#include "crashHandler.hpp"

using KalaKit::KalaCrashHandler;

static void Shutdown()
{
    exit(1); //non-zero - error
}

int main()
{
    //call this function to initialize crash handler
    KalaCrashHandler::Initialize();

    //attach a string to this function to set the name
    //of the program that will be displayed when the program crashes
    std::string name = "MyProgramName";
    KalaCrashHandler::SetProgramName(name);
	
    //set this to true if you wish to get a .dmp file 
    //generated at the location of your executable
    //every time the application crashes on Windows
    KalaCrashHandler::SetDumpCreateState(true);

    //attach any function similarly to this that will be called
    //once the user presses Ok or closes the error popup
    KalaCrashHandler::SetShutdownCallback([] { Shutdown(); });

    return 0;
}
```

---

# These crash types are supported and will be displayed

The images in the images folder also show each crash type (except EXCEPTION_IN_PAGE_ERROR) and what their error messages look like.

---

### Common and high priority crash types

#### Access violation (nullptr or invalid memory access)

    Occurs when a program dereferences a null or invalid pointer.  
    Typically caused by reading from, writing to, or executing memory that hasn’t been allocated.  
    The type of access (read, write, execute) is reported.  
    Exception: `EXCEPTION_ACCESS_VIOLATION`

#### Stack overflow (likely due to infinite recursion)

    Triggers infinite recursion or excessive stack allocation until the call stack limit is exceeded.  
    Results in a stack overflow exception and immediate crash.  
    Exception: `EXCEPTION_STACK_OVERFLOW`

#### Integer divide by zero

    Dividing an integer by zero results in a structured exception.  
    This is a definite crash on Windows platforms.  
    Exception: `EXCEPTION_INT_DIVIDE_BY_ZERO`

---

### Rare but useful crashes

#### Illegal CPU instruction executed

    Manually executes invalid or undefined CPU instructions (e.g. 0xFF 0xFF 0xFF 0xFF).  
    Always causes an illegal instruction exception.  
    Exception: `EXCEPTION_ILLEGAL_INSTRUCTION`

#### Breakpoint hit (INT 3 instruction executed)

    A software breakpoint (typically inserted by a debugger) was triggered.  
    This is normal during debugging but causes a crash if unhandled at runtime.  
    Exception: `EXCEPTION_BREAKPOINT`

#### Guard page accessed (likely stack guard or memory protection violation)

    Accessing a guard page (used for stack expansion or protected memory regions)  
    triggers a controlled crash, often seen just before a stack overflow.  
    Exception: `EXCEPTION_GUARD_PAGE`

#### Privileged instruction executed in user mode

    Executing a privileged CPU instruction (e.g. `hlt`, `cli`) from user mode causes a crash.  
    These are restricted to kernel-mode code.  
    Exception: `EXCEPTION_PRIV_INSTRUCTION`

#### Attempted to continue after a non-continuable exception

    Code tried to resume execution after a fatal exception that cannot be recovered from.  
    Typically a logic or state machine error.  
    Exception: `EXCEPTION_NONCONTINUABLE_EXCEPTION`

#### Memory access failed (I/O or paging failure)

    Occurs when accessing a valid memory address whose backing storage (e.g. memory-mapped file or pagefile)  
    could not be loaded into memory — typically due to I/O errors, device removal, or disk failure.  
    Exception: `EXCEPTION_IN_PAGE_ERROR`
