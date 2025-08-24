//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef _WIN32

#include <Windows.h>
#include <string>
#include <sstream>

#include "KalaHeaders/logging.hpp"

#include "graphics/opengl/opengl.hpp"
#include "graphics/opengl/opengl_functions_core.hpp"
#include "graphics/opengl/opengl_functions_win.hpp"
#include "graphics/window.hpp"
#include "core/core.hpp"
#include "core/global_handles.hpp"

using KalaHeaders::Log;
using KalaHeaders::LogType;

using KalaWindow::Core::KalaWindowCore;
using KalaWindow::Core::GlobalHandle;
using KalaWindow::Graphics::Window;
using namespace KalaWindow::Graphics::OpenGLFunctions;

using std::string;
using std::to_string;
using std::ostringstream;

static bool IsCorrectVersion();

namespace KalaWindow::Graphics::OpenGL
{
	bool OpenGL_Renderer::Initialize(Window* targetWindow)
	{
		HMODULE module = ToVar<HMODULE>(GlobalHandle::GetOpenGLHandle());

		const WindowData& wData = targetWindow->GetWindowData();
		OpenGLData oData{};
		HWND windowRef = ToVar<HWND>(wData.hwnd);

		HDC hdc = GetDC(windowRef);
		oData.hdc = FromVar(hdc);

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
			KalaWindowCore::ForceClose(
				"OpenGL Window Error",
				"ChoosePixelFormat failed!");

			return false;
		}
		Log::Print(
			"Pixel Format Index: " + to_string(pixelFormat),
			"OPENGL_WINDOWS",
			LogType::LOG_DEBUG);

		if (!SetPixelFormat(hdc, pixelFormat, &pfd))
		{
			KalaWindowCore::ForceClose(
				"OpenGL Window Error",
				"SetPixelFormat failed!");

			return false;
		}
		Log::Print(
			"SetPixelFormat worked!",
			"OPENGL_WINDOWS",
			LogType::LOG_DEBUG);

		PIXELFORMATDESCRIPTOR actualPFD = {};
		int describeResult = DescribePixelFormat(hdc, pixelFormat, sizeof(actualPFD), &actualPFD);
		if (describeResult == 0)
		{
			KalaWindowCore::ForceClose(
				"OpenGL Window Error",
				"DescribePixelFormat failed!");

			return false;
		}
		Log::Print(
			"DescribePixelFormat value: " + to_string(describeResult),
			"OPENGL_WINDOWS",
			LogType::LOG_DEBUG);

		ostringstream ss{};
		ss << "Pixel Format Details:\n"
			<< "  ColorBits   = " << to_string(static_cast<int>(actualPFD.cColorBits)) << "\n"
			<< "  DepthBits   = " << to_string(static_cast<int>(actualPFD.cDepthBits)) << "\n"
			<< "  StencilBits = " << to_string(static_cast<int>(actualPFD.cStencilBits)) << "\n"
			<< "  Flags:\n"
			<< "    DRAW_TO_WINDOW = " << ((actualPFD.dwFlags & PFD_DRAW_TO_WINDOW) ? "Yes" : "No") << "\n"
			<< "    SUPPORT_OPENGL = " << ((actualPFD.dwFlags & PFD_SUPPORT_OPENGL) ? "Yes" : "No") << "\n"
			<< "    DOUBLEBUFFER   = " << ((actualPFD.dwFlags & PFD_DOUBLEBUFFER) ? "Yes" : "No");
		Log::Print(
			ss.str(),
			"OPENGL_WINDOWS",
			LogType::LOG_DEBUG);

		HGLRC dummyRC = wglCreateContext(hdc);
		wglMakeCurrent(hdc, dummyRC);

		//
		// INITIALIZE WGL EXTENSIONS
		//

		OpenGL_Functions_Windows::LoadWinFunction(
			"wglCreateContextAttribsARB");
		OpenGL_Functions_Windows::LoadWinFunction(
			"wglChoosePixelFormatARB");
		OpenGL_Functions_Windows::LoadWinFunction(
			"wglSwapIntervalEXT");

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

		oData.hglrc = FromVar(wglCreateContextAttribsARB(
			hdc, 
			0, 
			attribs));
		if (!oData.hglrc)
		{
			KalaWindowCore::ForceClose(
				"OpenGL Window Error",
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

		OpenGL_Functions_Core::LoadAllCoreFunctions();

		if (!IsCorrectVersion())
		{
			KalaWindowCore::ForceClose(
				"OpenGL Window Error",
				"OpenGL 3.3 or higher is required!");

			return false;
		}

		//and finally set opengl viewport size
		vec2 framebufferSize = targetWindow->GetFramebufferSize();
		glViewport(
			0, 
			0, 
			framebufferSize.x,
			framebufferSize.y);

		targetWindow->SetOpenGLData(oData);

		Log::Print(
			"Initialized OpenGL with version: " + string(reinterpret_cast<const char*>(glGetString(GL_VERSION))),
			"OPENGL_WINDOWS",
			LogType::LOG_SUCCESS);

		isInitialized = true;

		return true;
	}

	void OpenGL_Renderer::SetVSyncState(VSyncState newVSyncState)
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

	void OpenGL_Renderer::SwapOpenGLBuffers(Window* targetWindow)
	{
		if (!IsInitialized())
		{
			Log::Print(
				"Cannot swap opengl buffers because OpenGL is not initialized!",
				"OPENGL_WINDOWS",
				LogType::LOG_DEBUG);
			return;
		}

		const OpenGLData& oData = targetWindow->GetOpenGLData();
		HDC hdc = ToVar<HDC>(oData.hdc);
		SwapBuffers(hdc);
	}

	void OpenGL_Renderer::MakeContextCurrent(Window* window)
	{
		const OpenGLData& oData = window->GetOpenGLData();

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
	}

	bool OpenGL_Renderer::IsContextValid(Window* targetWindow)
	{
		const OpenGLData& oData = targetWindow->GetOpenGLData();

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

		return true;
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