//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

//sane linux definitions
#if defined(_WIN32)
	#if defined(KW_USE_X11) || defined(KW_USE_WAYLAND)
	#error "Cannot use X11 and Wayland on Windows!"
	#endif
#elif defined(__linux__)
	#if defined(KW_USE_X11) && defined(KW_USE_WAYLAND)
		#error "Cannot use X11 and Wayland together!"
	#endif
	#if !defined(KW_USE_X11) && !defined(KW_USE_WAYLAND)
		#error "You must pick X11 or Wayland!"
	#endif
#endif

//sane graphics definitions
#if defined(KW_USE_GL) && defined(KW_USE_VK)
#error "Cannot use OpenGL and Vulkan together!"
#elif !defined(KW_USE_GL) && !defined(KW_USE_VK)
#error "You have to pick OpenGL or Vulkan!"
#endif

#if defined(_WIN32)
#include <windows.h>
#include <mmsystem.h>
#elif defined(__linux__)
#include <csignal>
#if defined(KW_USE_X11)
#include <X11/Xlib.h>
#endif
#endif

#include <functional>
#include <chrono>
#include <string>
#include <algorithm>

#include "KalaHeaders/core_utils.hpp"
#include "KalaHeaders/log_utils.hpp"

#if defined(__linux__)
#include "graphics/kw_window_global.hpp"
#endif

#include "core/kw_core.hpp"
#include "core/kw_crash.hpp"
#include "core/kw_input.hpp"
#include "graphics/kw_window.hpp"
#include "graphics/kw_menubar_windows.hpp"
#include "opengl/kw_opengl.hpp"
#include "opengl/kw_opengl_shader.hpp"

using KalaHeaders::KalaCore::ToVar;

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;
using KalaHeaders::KalaLog::TimeFormat;
using KalaHeaders::KalaLog::DateFormat;

#ifdef _WIN32
using KalaWindow::Graphics::MenuBar;
#endif

#if defined(__linux__)
using KalaWindow::Graphics::Window_Global;
#if defined(KW_USE_X11)
using KalaWindow::Graphics::X11GlobalData;
#endif
#endif

using KalaWindow::Core::CrashHandler;
using KalaWindow::Graphics::ProcessWindow;
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

		CrashHandler::SetForceCloseContent(
			target,
			reason);

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
#ifdef _WIN32
		MenuBar::GetRegistry().RemoveAllContent();
#endif
		ProcessWindow::GetRegistry().RemoveAllContent();

#if defined(__linux__) && defined(KW_USE_X11)
		const X11GlobalData& globalData = Window_Global::GetGlobalData();
		if (globalData.display)
		{
			Display* display = ToVar<Display*>(globalData.display);
			XCloseDisplay(display);
		}
#endif

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
			"KalaWindow shutting down with state = " + to_string(scast<int>(state)),
			"WINDOW",
			LogType::LOG_SUCCESS);

		if (useWindowShutdown) exit(0);
	}
}