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

#include "graphics/opengl/opengl.hpp"
#include "graphics/opengl/opengl_core.hpp"
#include "graphics/window.hpp"
#include "core/log.hpp"
#ifdef _WIN32
#include "graphics/opengl/opengl_win.hpp"
#elif __linux__
#include "graphics/opengl/opengl_linux.hpp"
#endif

using KalaWindow::Graphics::Render;
using KalaWindow::Graphics::ShutdownState;
using KalaWindow::Graphics::Window;
using KalaWindow::Graphics::PopupAction;
using KalaWindow::Graphics::PopupType;
using KalaWindow::Graphics::PopupResult;
using KalaWindow::Core::Logger;
using KalaWindow::Core::LogType;
using KalaWindow::Core::TimeFormat;
using KalaWindow::Core::DateFormat;
using KalaWindow::Graphics::OpenGL::VSyncState;

using std::string;
using std::to_string;

//If off, then all framerate is uncapped.
//Used in window.hpp
static VSyncState vsyncState = VSyncState::VSYNC_ON;

static void ForceClose(
	const string& title,
	const string& reason);

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
		Window_OpenGLData& oData = window->GetOpenGLStruct();
#ifdef _WIN32
		if (oData.hdc == 0)
		{
			string title = "OpenGL error [OpenGL]";
			string reason = "Failed to get HDC for window '" + window->GetTitle() + "' during 'MakeContextCurrent' stage!";
			ForceClose(title, reason);
			return;
		}
		HDC hdc = ToVar<HDC>(oData.hdc);

		if (oData.hglrc == 0)
		{
			string title = "OpenGL error [OpenGL]";
			string reason = "Failed to get HGLRC for window '" + window->GetTitle() + "' during 'MakeContextCurrent' stage!";
			ForceClose(title, reason);
			return;
		}
		HGLRC hglrc = ToVar<HGLRC>(oData.hglrc);

		if (!wglMakeCurrent(hdc, hglrc))
		{
			DWORD err = GetLastError();
			Logger::Print(
				"wglMakeCurrent failed with error: " + to_string(err),
				"OPENGL",
				LogType::LOG_ERROR);
		}
#elif __linux__
		//TODO: ADD LINUX EQUIVALENT
#endif
	}

	bool Renderer_OpenGL::IsContextValid(Window* targetWindow)
	{
		Window_OpenGLData& oData = targetWindow->GetOpenGLStruct();
#ifdef _WIN32
		if (oData.hglrc == 0)
		{
			string title = "OpenGL error [OpenGL]";
			string reason = "Failed to get HGLRC for window '" + targetWindow->GetTitle() + "' during 'IsContextValid' stage!";
			ForceClose(title, reason);
			return false;
		}
		HGLRC hglrc = ToVar<HGLRC>(oData.hglrc);

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

		if (current != hglrc)
		{
			Logger::Print(
				"Current OpenGL context does not match stored context!",
				"OPENGL",
				LogType::LOG_ERROR,
				2);
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
			Logger::Print(
				"wglSwapIntervalEXT not supported! VSync setting ignored.",
				"OPENGL",
				LogType::LOG_ERROR,
				2);
		}
	}
}

void ForceClose(
	const string& title,
	const string& reason)
{
	Logger::Print(
		reason,
		"OPENGL",
		LogType::LOG_ERROR,
		2,
		TimeFormat::TIME_NONE,
		DateFormat::DATE_NONE);

	Window* mainWindow = Window::windows.front();
	if (mainWindow->CreatePopup(
		title,
		reason,
		PopupAction::POPUP_ACTION_OK,
		PopupType::POPUP_TYPE_ERROR)
		== PopupResult::POPUP_RESULT_OK)
	{
		Render::Shutdown(ShutdownState::SHUTDOWN_FAILURE);
	}
}