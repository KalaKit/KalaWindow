//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include "graphics/render.hpp"
#include "graphics/window.hpp"
#include "core/input.hpp"
#include "core/log.hpp"

#ifdef _WIN32
#include <Windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#endif //_WIN32

#include <string>

#ifdef KALAWINDOW_SUPPORT_OPENGL
#include "graphics/opengl/opengl.hpp"
using KalaWindow::Graphics::Renderer_OpenGL;
#elif KALAWINDOW_SUPPORT_VULKAN
#include "graphics/vulkan/vulkan.hpp"
using KalaWindow::Graphics::Renderer_Vulkan;
#endif //KALAWINDOW_SUPPORT_VULKAN

using KalaWindow::Core::Input;
using KalaWindow::Core::Logger;
using KalaWindow::Core::LogType;

using std::exit;
using std::terminate;
using std::abort;
using std::exception;
using std::to_string;

namespace KalaWindow::Graphics
{
	bool Render::Initialize()
	{
#ifdef _WIN32
		timeBeginPeriod(1);
#endif //_WIN32

		return true;
	}

	void Render::Shutdown(
		ShutdownState state,
		bool useWindowShutdown,
		bool userEarlyShutdown,
		function<void()> userShutdown)
	{
		try
		{
			if (userEarlyShutdown && userShutdown) userShutdown();
		}
		catch (const exception& e)
		{
			Logger::Print(
				"User-provided shutdown condition failed! Reason: " + string(e.what()),
				"RENDER",
				LogType::LOG_ERROR,
				2);
		}

		for (const auto& window : Window::windows)
		{
			Window* winPtr = window;
			Window::DeleteWindow(winPtr);
		}

#ifdef KALAWINDOW_SUPPORT_VULKAN
		Renderer_Vulkan::Shutdown();
#endif //KALAWINDOW_SUPPORT_VULKAN

		try
		{
			if (!userEarlyShutdown && userShutdown) userShutdown();
		}
		catch (const exception& e)
		{
			Logger::Print(
				"User-provided shutdown condition failed! Reason: " + string(e.what()),
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
			LogType::LOG_SUCCESS,
			0);

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