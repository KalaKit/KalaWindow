//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef _WIN32
#include <windows.h>
#include <mmsystem.h>
#else
#include <csignal>
#endif
#include <functional>
#include <chrono>
#include <string>
#include <algorithm>

#include "KalaHeaders/log_utils.hpp"

#include "core/kw_core.hpp"
#include "core/kw_crash.hpp"
#include "core/kw_input.hpp"
#include "graphics/kw_window.hpp"
#include "graphics/kw_menubar_windows.hpp"
#include "opengl/kw_opengl.hpp"
#include "opengl/kw_opengl_shader.hpp"

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;
using KalaHeaders::KalaLog::TimeFormat;
using KalaHeaders::KalaLog::DateFormat;

using KalaWindow::Core::CrashHandler;
using KalaWindow::Graphics::Window;
using KalaWindow::Graphics::MenuBar;
using KalaWindow::OpenGL::OpenGL_Context;
using KalaWindow::OpenGL::OpenGL_Shader;

#ifdef __linux__
using std::raise;
#endif

using std::string_view;
using std::function;
using std::exception;
using std::to_string;
using std::chrono::steady_clock;
using std::chrono::time_point;
using std::chrono::duration;
using std::clamp;

static function<void()> userRegularShutdown;

#ifdef ENABLE_EMIT_LOG
void KalaHeaders::KalaLog::EmitLog(string_view msg)
{
	CrashHandler::AppendToCrashLog(msg);
}
#endif

namespace KalaWindow::Core
{
	//The ID that is bumped by every object in KalaWindow when it needs a new ID
	static inline u32 globalID{};

	static inline f64 deltaTime{};
	static inline f64 frameTime{};

	u32 KalaWindowCore::GetGlobalID() { return globalID; }
	void KalaWindowCore::SetGlobalID(u32 newID) { globalID = newID; }

	f64 KalaWindowCore::GetDeltaTime() { return deltaTime; }
	f64 KalaWindowCore::GetFrameTime() { return frameTime; }

	void KalaWindowCore::UpdateDeltaTime()
	{
		auto now = steady_clock::now();
		static time_point<steady_clock> lastFrameTime = now;

		duration<f64> delta = now - lastFrameTime;
		lastFrameTime = now;

		//unscaled, unclamped
		frameTime = delta.count();

		//regular deltatime
		deltaTime = clamp(delta.count(), 0.0, 0.1);
	}

	void KalaWindowCore::ForceClose(
		const string& target,
		const string& reason)
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

#ifdef _WIN32
		__debugbreak();
#else
		raise(SIGTRAP);
#endif
	}

	void KalaWindowCore::SetUserShutdownFunction(const function<void()>& regularShutdown)
	{
		userRegularShutdown = regularShutdown;
	}

	void KalaWindowCore::Shutdown(
		ShutdownState state,
		bool useWindowShutdown,
		const function<void()>& userShutdown)
	{
		Log::Print(
			"\n============="
			"\nSHUTTING DOWN"
			"\n=============\n",
			true);
		
#ifdef _WIN32
		timeEndPeriod(1);
#endif //_WIN32

		if (state == ShutdownState::SHUTDOWN_CRITICAL)
		{
#ifdef _DEBUG
			//skip all cleanup if a critical error was detected, we have no time to waste with cleanup here
			__debugbreak();
#else
			//clean, user-friendly exit in release
			quick_exit(EXIT_FAILURE);  
#endif
		}

		if (userRegularShutdown)
		{
			try
			{
				Log::Print(
					"Attempting to run user provided regular shutdown function...\n",
					"WINDOW",
					LogType::LOG_INFO);

				userRegularShutdown();

				Log::Print("\n====================\n");
			}
			catch (const exception& e)
			{
				Log::Print(
					"User-provided regular shutdown condition failed! Reason: " + string(e.what()),
					"WINDOW",
					LogType::LOG_ERROR,
					2);
			}
		}
		
		OpenGL_Shader::GetRegistry().RemoveAllContent();
		OpenGL_Context::GetRegistry().RemoveAllContent();

		Input::GetRegistry().RemoveAllContent();
		MenuBar::GetRegistry().RemoveAllContent();
		Window::GetRegistry().RemoveAllContent();

		if (!useWindowShutdown
			&& userShutdown)
		{
			try
			{
				Log::Print("\n====================\n");

				Log::Print(
					"Attempting to run user provided post-render shutdown function...\n",
					"WINDOW",
					LogType::LOG_INFO);

				userShutdown();

				Log::Print("\n====================\n");
			}
			catch (const exception& e)
			{
				Log::Print(
					"User-provided post-render shutdown condition failed! Reason: " + string(e.what()),
					"WINDOW",
					LogType::LOG_ERROR,
					2);
			}
		}

		Log::Print(
			"KalaWindow shutting down with state = " + to_string(static_cast<int>(state)),
			"WINDOW",
			LogType::LOG_SUCCESS);

		if (useWindowShutdown) exit(0);
	}
}