//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef _WIN32
#include <windows.h>
#elif __linux__

#endif

#include "graphics/opengl/opengl.hpp"
#include "graphics/opengl/opengl_core.hpp"
#include "graphics/window.hpp"
#include "core/log.hpp"
#ifdef _WIN32
#include "graphics/opengl/opengl_win.hpp"
#elif __linux__
#include "graphics/opengl/opengl_linux.hpp"
#endif

using KalaWindow::Graphics::Window;
using KalaWindow::Graphics::VSyncState;
using KalaWindow::Core::Logger;
using KalaWindow::Core::LogType;

//If off, then all framerate is uncapped.
//Used in window.hpp
static VSyncState vsyncState = VSyncState::VSYNC_ON;

namespace KalaWindow::Graphics::OpenGL
{
	const char* Renderer_OpenGL::GetGLErrorString(unsigned int err)
	{
		GLenum glErr = static_cast<GLenum>(err);

		switch (glErr)
		{
		case GL_INVALID_ENUM: return "GL_INVALID_ENUM";
		case GL_INVALID_VALUE: return "GL_INVALID_VALUE";
		case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION";
		case GL_STACK_OVERFLOW: return "GL_STACK_OVERFLOW";
		case GL_STACK_UNDERFLOW: return "GL_STACK_UNDERFLOW";
		case GL_OUT_OF_MEMORY: return "GL_OUT_OF_MEMORY";
		case GL_INVALID_FRAMEBUFFER_OPERATION: return "GL_INVALID_FRAMEBUFFER_OPERATION";
		default: return "Unknown error";
		}
	}

	bool Renderer_OpenGL::IsContextValid(Window* targetWindow)
	{
#ifdef _WIN32
		WindowStruct_Windows& win = targetWindow->GetWindow_Windows();
		HGLRC hglrc = ToVar<HGLRC>(win.openglData.hglrc);

		HGLRC current = wglGetCurrentContext();
		if (current == nullptr)
		{
			Logger::Print(
				"Current OpenGL context is null!",
				"OPENGL",
				LogType::LOG_ERROR,
				2);
			return false;
		}
#elif __linux__
		//TODO: set up for linux too
#endif

		if (current != hglrc)
		{
			Logger::Print(
				"Current OpenGL context does not match stored context!",
				"OPENGL",
				LogType::LOG_ERROR,
				2);
			return false;
		}

		return true;
	}

	//
	// EXTERNAL
	//

	VSyncState Window::GetVSyncState() { return vsyncState; }
	void Window::SetVSyncState(VSyncState newVSyncState)
	{
		vsyncState = newVSyncState;

		if (wglSwapIntervalEXT)
		{
			if (newVSyncState == VSyncState::VSYNC_ON)
			{
				wglSwapIntervalEXT(1);
			}
			else if (newVSyncState == VSyncState::VSYNC_OFF)
			{
				wglSwapIntervalEXT(0);
			}
			else
			{
				Logger::Print(
					"Cannot set vsync to 'TRIPLE BUFFERING' because it is not supported on OpenGL! Falling back to 'ON'.",
					"OPENGL",
					LogType::LOG_WARNING,
					2);
				wglSwapIntervalEXT(1);
			}
		}
		else
		{
			Logger::Print(
				"wglSwapIntervalEXT not supported! VSync setting ignored.",
				"OPENGL",
				LogType::LOG_ERROR,
				2);
		}
	}
}