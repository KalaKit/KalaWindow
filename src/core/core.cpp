//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef _WIN32
#include <windows.h>
#include <mmsystem.h>
#elif __linux__
//TODO: ADD LINUX EQUIVALENT
#endif

#include <functional>

#include "core/core.hpp"
#include "core/log.hpp"
#include "graphics/window.hpp"
#include "graphics/opengl/opengl.hpp"
#include "graphics/opengl/shader_opengl.hpp"
#include "graphics/vulkan/vulkan.hpp"
#include "graphics/texture.hpp"
#include "core/containers.hpp"

using KalaWindow::Graphics::Window;
using KalaWindow::Graphics::OpenGL::Renderer_OpenGL;
using KalaWindow::Graphics::OpenGL::Shader_OpenGL;
using KalaWindow::Graphics::Vulkan::Renderer_Vulkan;
using KalaWindow::Graphics::Texture;

using std::function;
using std::exception;
using std::to_string;

static function<void()> userRegularShutdown;

namespace KalaWindow::Core
{
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
#elif __linux__
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
		Logger::Print(
			reason,
			"FORCE CLOSE",
			LogType::LOG_ERROR,
			2,
			TimeFormat::TIME_NONE,
			DateFormat::DATE_NONE);

		if (CreatePopup(
			title,
			reason,
			PopupAction::POPUP_ACTION_OK,
			PopupType::POPUP_TYPE_ERROR)
			== PopupResult::POPUP_RESULT_OK)
		{
			Shutdown(ShutdownState::SHUTDOWN_CRITICAL);
		}
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
		try
		{
			Logger::Print(
				"Attempting to run user provided regular shutdown function...",
				"WINDOW",
				LogType::LOG_DEBUG);

			if (userRegularShutdown) userRegularShutdown();
		}
		catch (const exception& e)
		{
			Logger::Print(
				"User-provided regular shutdown condition failed! Reason: " + string(e.what()),
				"WINDOW",
				LogType::LOG_ERROR,
				2);
		}

		createdOpenGLTextures.clear();
		createdVulkanTextures.clear();
		if (Renderer_Vulkan::IsVulkanInitialized())
		{
			Renderer_Vulkan::Shutdown();
		}
		if (Renderer_OpenGL::IsInitialized())
		{
			createdOpenGLShaders.clear();
		}

		createdWindows.clear();

#ifdef _WIN32
		timeEndPeriod(1);
#endif //_WIN32

		try
		{
			if (!useWindowShutdown && userShutdown) userShutdown();
		}
		catch (const exception& e)
		{
			Logger::Print(
				"User-provided post-render shutdown condition failed! Reason: " + string(e.what()),
				"WINDOW",
				LogType::LOG_ERROR,
				2);
		}

		Logger::Print(
			"KalaWindow shutting down with state = " + to_string(static_cast<int>(state)),
			"WINDOW",
			LogType::LOG_SUCCESS);

		if (useWindowShutdown)
		{
			switch (state)
			{
			case ShutdownState::SHUTDOWN_CLEAN:
				exit(0);
				break;
			case ShutdownState::SHUTDOWN_FAILURE:
				terminate();
				break;
			case ShutdownState::SHUTDOWN_CRITICAL:
				abort();
				break;
			}
		}
	}
}