//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef _WIN32

#include <Windows.h>
#include <string>
#include <sstream>

#include "graphics/opengl/opengl.hpp"
#include "graphics/opengl/opengl_core.hpp"
#include "graphics/render.hpp"
#include "graphics/window.hpp"
#include "core/log.hpp"
#include "graphics/opengl/opengl_win.hpp"

using KalaWindow::Graphics::Render;
using KalaWindow::Graphics::ShutdownState;
using KalaWindow::Graphics::Window;
using KalaWindow::Graphics::PopupAction;
using KalaWindow::Graphics::PopupType;
using KalaWindow::Graphics::PopupResult;
using KalaWindow::Graphics::OpenGL::OpenGLCore;
using KalaWindow::Core::Logger;
using KalaWindow::Core::LogType;
using KalaWindow::Core::TimeFormat;
using KalaWindow::Core::DateFormat;

using std::string;
using std::to_string;
using std::stringstream;

static bool IsCorrectVersion();

static void ForceClose(
	const string& title,
	const string& reason);

namespace KalaWindow::Graphics::OpenGL
{
	bool Renderer_OpenGL::Initialize(Window* targetWindow)
	{
		WindowStruct_Windows wData = targetWindow->GetWindow_Windows();
		Window_OpenGLData oData{};
		HWND windowRef = ToVar<HWND>(wData.hwnd);

		HDC hdc = GetDC(windowRef);
		oData.hdc = FromVar<HDC>(hdc);

		//
		// CREATE A DUMMY CONTEXT TO LOAD WGL EXTENSIONS
		//

		PIXELFORMATDESCRIPTOR pfd = {};
		pfd.nSize = sizeof(pfd);
		pfd.nVersion = 1;
		pfd.dwFlags =
			PFD_DRAW_TO_WINDOW
			| PFD_SUPPORT_OPENGL
			| PFD_DOUBLEBUFFER;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = 32;
		pfd.cDepthBits = 24;
		pfd.cStencilBits = 8;

		int pixelFormat = ChoosePixelFormat(hdc, &pfd);
		if (pixelFormat == 0)
		{
			ForceClose(
				"OpenGL initialization error [opengl_windows]",
				"ChoosePixelFormat failed!");

			return false;
		}
		Logger::Print(
			"Pixel Format Index: " + to_string(pixelFormat),
			"OPENGL_WINDOWS",
			LogType::LOG_DEBUG);

		if (!SetPixelFormat(hdc, pixelFormat, &pfd))
		{
			ForceClose(
				"OpenGL initialization error [opengl_windows]",
				"SetPixelFormat failed!");

			return false;
		}
		Logger::Print(
			"SetPixelFormat worked!",
			"OPENGL_WINDOWS",
			LogType::LOG_DEBUG);

		PIXELFORMATDESCRIPTOR actualPFD = {};
		int describeResult = DescribePixelFormat(hdc, pixelFormat, sizeof(actualPFD), &actualPFD);
		if (describeResult == 0)
		{
			ForceClose(
				"OpenGL initialization error [opengl_windows]",
				"DescribePixelFormat failed!");

			return false;
		}
		Logger::Print(
			"DescribePixelFormat value: " + to_string(describeResult),
			"OPENGL_WINDOWS",
			LogType::LOG_DEBUG);

		stringstream ss{};
		ss << "Pixel Format Details:\n"
			<< "  ColorBits   = " << to_string(static_cast<int>(actualPFD.cColorBits)) << "\n"
			<< "  DepthBits   = " << to_string(static_cast<int>(actualPFD.cDepthBits)) << "\n"
			<< "  StencilBits = " << to_string(static_cast<int>(actualPFD.cStencilBits)) << "\n"
			<< "  Flags:\n"
			<< "    DRAW_TO_WINDOW = " << ((actualPFD.dwFlags & PFD_DRAW_TO_WINDOW) ? "Yes" : "No") << "\n"
			<< "    SUPPORT_OPENGL = " << ((actualPFD.dwFlags & PFD_SUPPORT_OPENGL) ? "Yes" : "No") << "\n"
			<< "    DOUBLEBUFFER   = " << ((actualPFD.dwFlags & PFD_DOUBLEBUFFER) ? "Yes" : "No");
		Logger::Print(
			ss.str(),
			"OPENGL_WINDOWS",
			LogType::LOG_DEBUG);

		HGLRC dummyRC = wglCreateContext(hdc);
		wglMakeCurrent(hdc, dummyRC);

		//
		// INITIALIZE WGL EXTENSIONS
		//

		OpenGLCore::InitializeFunction("wglCreateContextAttribsARB");
		OpenGLCore::InitializeFunction("wglChoosePixelFormatARB");
		OpenGLCore::InitializeFunction("wglSwapIntervalEXT");

		//
		// CREATE REAL OPENGL 3.3 CONTEXT
		//

		int attribs[] =
		{
			WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
			WGL_CONTEXT_MINOR_VERSION_ARB, 3,
			WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
			0
		};

		oData.hglrc = FromVar<HGLRC>(wglCreateContextAttribsARB(
			hdc, 
			0, 
			attribs));
		if (!oData.hglrc)
		{
			ForceClose(
				"OpenGL initialization error [opengl_windows]",
				"Failed to create OpenGL 3.3 context!");

			return false;
		}

		//
		// CLEANUP AND SET REAL CONTEXT
		//

		wglMakeCurrent(nullptr, nullptr);
		wglDeleteContext(dummyRC);

		HGLRC hglrc = ToVar<HGLRC>(oData.hglrc);
		wglMakeCurrent(hdc, hglrc);

		OpenGLCore::InitializeAllFunctions();

		if (!IsCorrectVersion())
		{
			ForceClose(
				"OpenGL initialization error [opengl_windows]",
				"OpenGL 3.3 or higher is required!");

			return false;
		}

		//and finally set opengl viewport size
		glViewport(
			0, 
			0, 
			targetWindow->GetSize().x,
			targetWindow->GetSize().y);

		targetWindow->SetOpenGLStruct(oData);

		Logger::Print(
			"Initialized OpenGL with version: " + string(reinterpret_cast<const char*>(glGetString(GL_VERSION))),
			"OPENGL_WINDOWS",
			LogType::LOG_SUCCESS);

		isInitialized = true;

		return true;
	}

	void Renderer_OpenGL::SwapOpenGLBuffers(Window* targetWindow)
	{
		if (!IsInitialized())
		{
			Logger::Print(
				"Cannot swap opengl buffers because OpenGL is not initialized!",
				"OPENGL_WINDOWS",
				LogType::LOG_DEBUG);
			return;
		}

		Window_OpenGLData& oData = targetWindow->GetOpenGLStruct();
		HDC hdc = ToVar<HDC>(oData.hdc);
		SwapBuffers(hdc);
	}
}

void ForceClose(
	const string& title,
	const string& reason)
{
	Logger::Print(
		reason,
		"OPENGL_WINDOWS",
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

bool IsCorrectVersion()
{
	const char* versionStr = reinterpret_cast<const char*>(glGetString(GL_VERSION));
	if (!versionStr) return false;

	int major = 0;
	int minor = 0;
	if (sscanf_s(versionStr, "%d.%d", &major, &minor) != 2)
	{
		return false;
	}

	return
		(major > 3)
		|| (major == 3
		&& minor >= 3);
}

#endif //_WIN32