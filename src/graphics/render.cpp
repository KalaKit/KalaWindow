//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef _WIN32
#include <Windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#endif //_WIN32
#include <functional>
#include <string>

#include "graphics/render.hpp"
#include "graphics/window.hpp"
#include "core/input.hpp"
#include "core/log.hpp"
#include "graphics/opengl/opengl.hpp"
#include "graphics/vulkan/vulkan.hpp"

using KalaWindow::Core::Input;
using KalaWindow::Core::Logger;
using KalaWindow::Core::LogType;
using KalaWindow::Graphics::OpenGL::Renderer_OpenGL;
using KalaWindow::Graphics::Vulkan::Renderer_Vulkan;

using std::exit;
using std::terminate;
using std::abort;
using std::exception;
using std::to_string;
using std::function;

static function<void()> userRegularShutdown;

namespace KalaWindow::Graphics
{
	bool Render::Initialize()
	{
#ifdef _WIN32
		timeBeginPeriod(1);
#endif //_WIN32

		return true;
	}

	void Render::SetUserShutdownFunction(function<void()> regularShutdown)
	{
		userRegularShutdown = regularShutdown;
	}

	void Render::Shutdown(
		ShutdownState state,
		bool useWindowShutdown,
		bool userEarlyShutdown,
		function<void()> userShutdown)
	{
		try
		{
			if (userRegularShutdown) userRegularShutdown();
		}
		catch (const exception& e)
		{
			Logger::Print(
				"User-provided regular shutdown condition failed! Reason: " + string(e.what()),
				"RENDER",
				LogType::LOG_ERROR,
				2);
		}

		try
		{
			if (userEarlyShutdown && userShutdown) userShutdown();
		}
		catch (const exception& e)
		{
			Logger::Print(
				"User-provided early shutdown condition failed! Reason: " + string(e.what()),
				"RENDER",
				LogType::LOG_ERROR,
				2);
		}

		for (const auto& window : Window::windows)
		{
			Window* winPtr = window;
			Window::DeleteWindow(winPtr);
		}

		Renderer_Vulkan::Shutdown();

		try
		{
			if (!userEarlyShutdown && userShutdown) userShutdown();
		}
		catch (const exception& e)
		{
			Logger::Print(
				"User-provided early shutdown condition failed! Reason: " + string(e.what()),
				"RENDER",
				LogType::LOG_ERROR,
				2);
		}

#ifdef _WIN32
		timeEndPeriod(1);
#endif //_WIN32

		Logger::Print(
			"KalaWindow shutting down with state = " + to_string(static_cast<int>(state)),
			"RENDER",
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