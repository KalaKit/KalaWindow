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

#include "KalaHeaders/log_utils.hpp"

#include "core/core.hpp"
#include "core/audio.hpp"
#include "graphics/window.hpp"
#include "graphics/opengl/opengl.hpp"
#include "graphics/opengl/opengl_shader.hpp"
#include "graphics/opengl/opengl_texture.hpp"
#include "graphics/window_global.hpp"
#include "ui/font.hpp"

using KalaHeaders::Log;
using KalaHeaders::LogType;
using KalaHeaders::TimeFormat;
using KalaHeaders::DateFormat;

using KalaWindow::Core::AudioPlayer;
using KalaWindow::Graphics::Window;
using KalaWindow::Graphics::OpenGL::OpenGL_Shader;
using KalaWindow::Graphics::OpenGL::OpenGL_Texture;
using KalaWindow::Graphics::Window_Global;
using KalaWindow::Graphics::PopupAction;
using KalaWindow::Graphics::PopupResult;
using KalaWindow::Graphics::PopupType;
using KalaWindow::UI::Font;

using std::abort;
using std::quick_exit;
using std::function;
using std::exception;
using std::to_string;

static function<void()> userRegularShutdown;

namespace KalaWindow::Core
{
	u32 globalID{};

	void KalaWindowCore::ForceClose(
		const string& title,
		const string& reason)
	{
		Log::Print(
			reason,
			"FORCE CLOSE",
			LogType::LOG_ERROR,
			2,
			true,
			TimeFormat::TIME_NONE,
			DateFormat::DATE_NONE);

		DEBUG_ASSERT(false && reason.c_str());

#ifndef _DEBUG
		if (Window_Global::CreatePopup(
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
		Log::Print("\n====================", true);
		Log::Print("\nSHUTTING DOWN KALAWINDOW", true);
		Log::Print("\n====================\n", true);

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
		AudioPlayer::registry.RemoveAllContent();
		OpenGL_Texture::registry.RemoveAllContent();
		OpenGL_Shader::registry.RemoveAllContent();
		Font::registry.RemoveAllContent();
		Window::registry.RemoveAllContent();

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