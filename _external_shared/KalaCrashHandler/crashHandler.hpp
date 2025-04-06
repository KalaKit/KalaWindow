//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#ifdef _WIN32
	#ifdef KALACRASH_DLL_EXPORT
		#define KALACRASH_API __declspec(dllexport)
	#else
		#define KALACRASH_API __declspec(dllimport)
	#endif
#else
	#define KALACRASH_API
#endif

#include <string>
#include <sstream>
#include <functional>
#ifdef _WIN32
#include <Windows.h>
#endif

namespace KalaKit
{
	using std::string;
	using std::ostringstream;
	using std::function;

	class KALACRASH_API KalaCrashHandler
	{
	public:
		/// <summary>
		/// Assign a name that will be displayed 
		/// before "has shut down" in the error popup title.
		/// </summary>
		static void SetProgramName(const string& newName) { name = newName; }
		/// <summary>
		/// If set to true, then a dmp file will be 
		/// created at crash in the location of the executable on Windows.
		/// </summary>
		static void SetDumpCreateState(bool newState) { createDump = newState; };

		static inline function<void()> ShutdownCallback;

		/// <summary>
		/// Assign a shutdown function with this function 
		/// that is called internally by KalaCrashHandler whenever 
		/// a crash is detected after the error popup has beem closed.
		/// </summary>
		static inline void SetShutdownCallback(function<void()> callback) 
		{
			ShutdownCallback = move(callback);
		}

		/// <summary>
		/// Internal shutdown function, not meant to be called by end user.
		/// </summary>
		static inline void Shutdown() { if (ShutdownCallback) ShutdownCallback(); }

		/// <summary>
		/// Initialize the crash handler
		/// </summary>
		static void Initialize();


	private:
		/// <summary>
		/// Toggles whether the dmp file is created on Windows or not.
		/// </summary>
		static inline bool createDump;

		/// <summary>
		/// The name of the program this crash handler is attached to
		/// </summary>
		static inline string name;

#ifdef _WIN32
		/// <summary>
		/// Windows crash handler that calls the minidump creator function and sends error info to error popup.
		/// </summary>
		static LONG WINAPI HandleCrash(EXCEPTION_POINTERS* info);
#endif

		static void WriteLog(
			const string& message, 
			const string& exePath, 
			const string& timeStamp);

#ifdef _WIN32
		/// <summary>
		/// Creates a windows crash .dmp file to exe location.
		/// </summary>
		static void WriteMiniDump(
			EXCEPTION_POINTERS* info, 
			const string& exePath, 
			const string& timeStamp);

		/// <summary>
		/// Appends up to to last 10 frames of the call stack upon crash.
		/// </summary>
		static void AppendCallStackToStream(ostringstream& oss, CONTEXT* context);
#endif

		/// <summary>
		/// Internal error popup function, not meant to be used by end user.
		/// </summary>
		static void CreateErrorPopup(const string& message);

		/// <summary>
		/// Return current day as number, current hour, minute and second.
		/// </summary>
		static string GetCurrentTimeStamp();

		/// <summary>
		/// Return current executable path.
		/// </summary>
		static string GetExePath();

	};
}