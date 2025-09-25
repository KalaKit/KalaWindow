//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef _WIN32
#include <windows.h>
#include <mmsystem.h>
#else
//TODO: ADD LINUX EQUIVALENT
#endif

#include <functional>
#include <cstdint>

#include "core/core.hpp"
#include "graphics/window.hpp"
#include "graphics/opengl/opengl.hpp"
#include "graphics/opengl/opengl_shader.hpp"
#include "graphics/vulkan/vulkan.hpp"
#include "graphics/texture.hpp"
#include "ui/debug_ui.hpp"
#include "core/containers.hpp"
#include "core/audio.hpp"
#include "core/global_handles.hpp"

using KalaHeaders::Log;
using KalaHeaders::LogType;
using KalaHeaders::TimeFormat;
using KalaHeaders::DateFormat;

using KalaWindow::Graphics::Window;
using KalaWindow::Graphics::OpenGLData;
using KalaWindow::Graphics::OpenGL::OpenGL_Renderer;
using KalaWindow::Graphics::OpenGL::OpenGL_Shader;
using KalaWindow::Graphics::Texture;
using KalaWindow::UI::DebugUI;
using KalaWindow::Core::GlobalHandle;

using std::abort;
using std::quick_exit;
using std::function;
using std::exception;
using std::to_string;

static u32 version_windows{};

static function<void()> userRegularShutdown;

static void SetVersion();

namespace KalaWindow::Core
{
	u32 KalaWindowCore::GetVersion()
	{
#ifdef WIN32
		if (version_windows == 0)
		{
			string title = "Window version check fail";

			typedef LONG(WINAPI* RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);

			HMODULE hMod = GetModuleHandleW(L"ntdll.dll");
			if (!hMod)
			{
				ForceClose(
					title,
					"Failed to get 'ntdll.dll'");

				return 0;
			}

			auto pRtlGetVersion = (RtlGetVersionPtr)GetProcAddress(hMod, "RtlGetVersion");
			if (!pRtlGetVersion)
			{
				ForceClose(
					title,
					"Failed to resolve address of 'RtlGetVersion'");

				return 0;
			}

			RTL_OSVERSIONINFOW rovi = { sizeof(rovi) };
			if (pRtlGetVersion(&rovi) != 0)
			{
				ForceClose(
					title,
					"Call to 'RtlGetVersion' failed");

				return 0;
			}

			u32 major = rovi.dwMajorVersion;
			u32 build = rovi.dwBuildNumber;

			//Windows 11 reports as 10.0  but build >= 22000
			if (major == 10
				&& build >= 22000)
			{
				major = 11;
			}

			version_windows = major * 1000000 + build;
			return version_windows;
		}

		return version_windows;
#elif __linux__
		return 0;
#endif
		return 0;
	}

	PopupResult KalaWindowCore::CreatePopup(
		const string& title,
		const string& message,
		PopupAction action,
		PopupType type)
	{
		int flags = 0;

#ifdef _WIN32
		switch (action)
		{
		case PopupAction::POPUP_ACTION_OK:            flags |= MB_OK; break;
		case PopupAction::POPUP_ACTION_OK_CANCEL:     flags |= MB_OKCANCEL; break;
		case PopupAction::POPUP_ACTION_YES_NO:        flags |= MB_YESNO; break;
		case PopupAction::POPUP_ACTION_YES_NO_CANCEL: flags |= MB_YESNOCANCEL; break;
		case PopupAction::POPUP_ACTION_RETRY_CANCEL:  flags |= MB_RETRYCANCEL; break;
		default:                                      flags |= MB_OK; break;
		}

		switch (type)
		{
		case PopupType::POPUP_TYPE_INFO:     flags |= MB_ICONINFORMATION; break;
		case PopupType::POPUP_TYPE_WARNING:  flags |= MB_ICONWARNING; break;
		case PopupType::POPUP_TYPE_ERROR:    flags |= MB_ICONERROR; break;
		case PopupType::POPUP_TYPE_QUESTION: flags |= MB_ICONQUESTION; break;
		default:                             flags |= MB_ICONINFORMATION; break;
		}
#else
		//TODO: ADD LINUX EQUIVALENT
#endif

		int result = MessageBox(
			nullptr,
			message.c_str(),
			title.c_str(),
			flags);

		switch (result)
		{
		case IDOK:     return PopupResult::POPUP_RESULT_OK;
		case IDCANCEL: return PopupResult::POPUP_RESULT_CANCEL;
		case IDYES:    return PopupResult::POPUP_RESULT_YES;
		case IDNO:     return PopupResult::POPUP_RESULT_NO;
		case IDRETRY:  return PopupResult::POPUP_RESULT_RETRY;
		default:       return PopupResult::POPUP_RESULT_NONE;
		}
	}

	void KalaWindowCore::ForceClose(
		const string& title,
		const string& reason)
	{
		Log::Print(
			reason,
			"FORCE CLOSE",
			LogType::LOG_ERROR,
			2,
			TimeFormat::TIME_NONE,
			DateFormat::DATE_NONE);

		DEBUG_ASSERT(false && reason.c_str());

#ifndef _DEBUG
		if (CreatePopup(
			title,
			reason,
			PopupAction::POPUP_ACTION_OK,
			PopupType::POPUP_TYPE_ERROR)
			== PopupResult::POPUP_RESULT_OK)
		{
			Shutdown(ShutdownState::SHUTDOWN_CRITICAL);
		}
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
		Log::Print("\n====================");
		Log::Print("\nSHUTTING DOWN KALAWINDOW");
		Log::Print("\n====================\n");

		if (state == ShutdownState::SHUTDOWN_CRITICAL)
		{
#ifdef _DEBUG
			//skip all cleanup if a critical error was detected, we have no time to waste with cleanup here
			abort();  
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

		if (Audio::IsInitialized()) Audio::Shutdown();

		for (const auto& window : runtimeWindows) OpenGL_Renderer::Shutdown(window);

		createdOpenGLTextures.clear();
		createdOpenGLShaders.clear();

		if (DebugUI::IsInitialized()) DebugUI::Shutdown();

		HGLRC hglrc = ToVar<HGLRC>(GlobalHandle::GetOpenGLWinContext());
		if (hglrc != NULL)
		{
			if (wglGetCurrentContext() == hglrc) wglMakeCurrent(nullptr, nullptr);
			wglDeleteContext(hglrc);
			GlobalHandle::SetOpenGLWinContext(NULL);
		}

		createdWindows.clear();

#ifdef _WIN32
		timeEndPeriod(1);
#endif //_WIN32

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