//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef KALAWINDOW_SUPPORT_OPENGL

#include <Windows.h>
#include <string>
#include <sstream>

#include "graphics/opengl/opengl.hpp"
#include "graphics/opengl/opengl_core.hpp"
#include "graphics/render.hpp"
#include "graphics/window.hpp"
#include "core/log.hpp"

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

static void ForceClose(
	const string& title,
	const string& reason,
	ShutdownState state = ShutdownState::SHUTDOWN_FAILURE)
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

namespace KalaWindow::Graphics
{
	bool Renderer_OpenGL::Initialize(
		Window* targetWindow)
	{
		WindowStruct_Windows& window = targetWindow->GetWindow_Windows();
		HWND windowRef = ToVar<HWND>(window.hwnd);

		HDC hdc = GetDC(windowRef);
		window.openglData.hdc = FromVar<HDC>(hdc);

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
			Logger::Print(
				"ChoosePixelFormat failed!",
				"OPENGL_WINDOWS",
				LogType::LOG_ERROR,
				2);

			ForceClose(
				"OpenGL error",
				"ChoosePixelFormat failed!");

			return false;
		}
		Logger::Print(
			"Pixel Format Index: " + to_string(pixelFormat),
			"OPENGL_WINDOWS",
			LogType::LOG_DEBUG);

		if (!SetPixelFormat(hdc, pixelFormat, &pfd))
		{
			Logger::Print(
				"SetPixelFormat failed!",
				"OPENGL_WINDOWS",
				LogType::LOG_ERROR,
				2);

			ForceClose(
				"OpenGL error",
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
			Logger::Print(
				"DescribePixelFormat failed!",
				"OPENGL_WINDOWS",
				LogType::LOG_ERROR,
				2);

			ForceClose(
				"OpenGL error",
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
		// CREATE REAL OPENGL 3.3 CONTEXT
		//

		int attribs[] =
		{
			WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
			WGL_CONTEXT_MINOR_VERSION_ARB, 3,
			WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
			0
		};

		window.openglData.hglrc = FromVar<HGLRC>(wglCreateContextAttribsARB(
			hdc, 
			0, 
			attribs));
		if (!window.openglData.hglrc)
		{
			Logger::Print(
				"Failed to create OpenGL 3.3 context!",
				"OPENGL_WINDOWS",
				LogType::LOG_ERROR,
				2);

			ForceClose(
				"OpenGL error",
				"Failed to create OpenGL 3.3 context!");

			return false;
		}

		//
		// CLEANUP AND SET REAL CONTEXT
		//

		wglMakeCurrent(nullptr, nullptr);
		wglDeleteContext(dummyRC);

		wglMakeCurrent(hdc, reinterpret_cast<HGLRC>(window.openglData.hglrc));

		if (!IsCorrectVersion())
		{
			Logger::Print(
				"OpenGL 3.3 or higher is required!",
				"OPENGL_WINDOWS",
				LogType::LOG_ERROR,
				2);

			ForceClose(
				"OpenGL error",
				"OpenGL 3.3 or higher is required!");

			return false;
		}

		Logger::Print(
			"OpenGL version: " + string(reinterpret_cast<const char*>(kglGetString(GL_VERSION))),
			"OPENGL_WINDOWS",
			LogType::LOG_SUCCESS);

		OpenGLCore::InitializeAllFunctions();

		//and finally set opengl viewport size
		kglViewport(
			0, 
			0, 
			targetWindow->GetSize().x,
			targetWindow->GetSize().y);

		return true;
	}

	void Renderer_OpenGL::SwapOpenGLBuffers(Window* targetWindow)
	{
		WindowStruct_Windows& window = targetWindow->GetWindow_Windows();
		HDC hdc = ToVar<HDC>(window.openglData.hdc);
		SwapBuffers(hdc);
	}

	bool Renderer_OpenGL::IsCorrectVersion()
	{
		const char* versionStr = reinterpret_cast<const char*>(kglGetString(GL_VERSION));
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
}

#endif //KALAWINDOW_SUPPORT_OPENGL