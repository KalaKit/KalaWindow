//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include "graphics/render.hpp"
#include "graphics/window.hpp"
#include "core/input.hpp"

#ifdef KALAWINDOW_SUPPORT_OPENGL
#include "graphics/opengl/opengl.hpp"
using KalaWindow::Graphics::Renderer_OpenGL;
#elif KALAWINDOW_SUPPORT_VULKAN
#include "graphics/vulkan/vulkan.hpp"
using KalaWindow::Graphics::Renderer_Vulkan;
#endif //KALAWINDOW_SUPPORT_VULKAN

#ifdef _WIN32
#include <Windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#endif //_WIN32

using KalaWindow::Core::Input;

using std::exit;
using std::terminate;
using std::abort;

namespace KalaWindow::Graphics
{
	bool Render::Initialize()
	{
#ifdef _WIN32
		timeBeginPeriod(1);
#endif //_WIN32

		return true;
	}

	void Render::Shutdown(ShutdownState state)
	{
		for (const auto& window : Window::windows)
		{
			Window* winPtr = window.get();
			Window::DeleteWindow(winPtr);
		}

#ifdef KALAWINDOW_SUPPORT_VULKAN
		Renderer_Vulkan::Shutdown();
#endif //KALAWINDOW_SUPPORT_VULKAN

#ifdef _WIN32
		timeEndPeriod(1);
#endif //_WIN32

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