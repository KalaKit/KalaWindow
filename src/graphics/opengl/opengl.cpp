//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef _WIN32
#include <windows.h>
#elif __linux__
//TODO: ADD LINUX EQUIVALENT
#endif

#include <string>

#include "KalaHeaders/logging.hpp"

#include "graphics/opengl/opengl.hpp"
#include "graphics/opengl/opengl_functions_core.hpp"
#include "graphics/window.hpp"
#include "core/core.hpp"
#ifdef _WIN32
#include "graphics/opengl/opengl_functions_win.hpp"
#elif __linux__
#include "graphics/opengl/opengl_functions_linux.hpp"
#endif

using KalaHeaders::Log;
using KalaHeaders::LogType;

using KalaWindow::Graphics::Window;
using KalaWindow::Graphics::OpenGL::VSyncState;
using namespace KalaWindow::Graphics::OpenGLFunctions;
using KalaWindow::Core::KalaWindowCore;

using std::string;
using std::to_string;

//TODO: separate global init from window init
//TODO: move content from opengl_windows.cpp here so that this is the true origin of opengl initialization

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

	void Renderer_OpenGL::MakeContextCurrent(Window* window)
	{
		const OpenGLData& oData = window->GetOpenGLData();
#ifdef _WIN32
		if (oData.hdc == 0)
		{
			string title = "OpenGL Error";
			string reason = "Failed to get HDC for window '" + window->GetTitle() + "' during 'MakeContextCurrent' stage!";
			KalaWindowCore::ForceClose(title, reason);
			return;
		}
		HDC hdc = ToVar<HDC>(oData.hdc);

		if (oData.hglrc == 0)
		{
			string title = "OpenGL Error";
			string reason = "Failed to get HGLRC for window '" + window->GetTitle() + "' during 'MakeContextCurrent' stage!";
			KalaWindowCore::ForceClose(title, reason);
			return;
		}
		HGLRC hglrc = ToVar<HGLRC>(oData.hglrc);

		if (!wglMakeCurrent(hdc, hglrc))
		{
			DWORD err = GetLastError();
			KalaWindowCore::ForceClose(
				"OpenGL Error",
				"wglMakeCurrent failed with error: " + to_string(err));
		}
#elif __linux__
		//TODO: ADD LINUX EQUIVALENT
#endif
	}

	bool Renderer_OpenGL::IsContextValid(Window* targetWindow)
	{
		const OpenGLData& oData = targetWindow->GetOpenGLData();
#ifdef _WIN32
		if (oData.hglrc == 0)
		{
			string title = "OpenGL Error";
			string reason = "Failed to get HGLRC for window '" + targetWindow->GetTitle() + "' during 'IsContextValid' stage!";
			KalaWindowCore::ForceClose(title, reason);
			return false;
		}
		HGLRC hglrc = ToVar<HGLRC>(oData.hglrc);

		HGLRC current = wglGetCurrentContext();
		if (current == nullptr)
		{
			KalaWindowCore::ForceClose(
				"OpenGL Error",
				"Current OpenGL context is null!");
			return false;
		}

		if (current != hglrc)
		{
			KalaWindowCore::ForceClose(
				"OpenGL Error",
				"Current OpenGL context does not match stored context!");
			return false;
		}
#elif __linux__
		//TODO: set up for linux too
#endif
		return true;
	}

	VSyncState Renderer_OpenGL::GetVSyncState() { return vsyncState; }
	void Renderer_OpenGL::SetVSyncState(VSyncState newVSyncState)
	{
		vsyncState = newVSyncState;

		if (wglSwapIntervalEXT)
		{
			if (newVSyncState == VSyncState::VSYNC_ON)
			{
				wglSwapIntervalEXT(1);
			}
			else
			{
				wglSwapIntervalEXT(0);
			}
		}
		else
		{
			Log::Print(
				"wglSwapIntervalEXT not supported! VSync setting ignored.",
				"OPENGL",
				LogType::LOG_ERROR,
				2);
		}
	}
}