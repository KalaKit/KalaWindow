//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <memory>

#include "core/platform.hpp"
#include "graphics/window.hpp"

namespace KalaWindow::Graphics::OpenGL
{
	using std::unique_ptr;

	enum VSyncState
	{
		VSYNC_ON, //Framerate is capped to monitor refresh rate.
		VSYNC_OFF //Framerate is uncapped, runs as fast as render loop allows, introduces tearing.
	};

	class KALAWINDOW_API Renderer_OpenGL
	{
	public:
		/// <summary>
		/// Initializes OpenGL.
		/// </summary>
		static bool Initialize(Window* targetWindow);

		static const char* GetGLErrorString(unsigned int err);

		static bool IsContextValid(Window* window);

		/// <summary>
		/// Call at the end of your render loop.
		/// </summary>
		static void SwapOpenGLBuffers(Window* targetWindow);

		//Checks if vsync is enabled or not.
		VSyncState GetVSyncState();
		//Allows to set vsync true or false.
		void SetVSyncState(VSyncState vsyncState);
	private:
		/// <summary>
		/// Checks whether user has OpenGL 3.3 or higher.
		/// </summary>
		static bool IsCorrectVersion();
	};
}