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
static HWND CreateDummyWindow();

namespace KalaWindow::Graphics::OpenGL
{
	bool OpenGL_Renderer::GlobalInitialize()
	{
		if (isInitialized
			&& GlobalHandle::GetOpenGLWinContext() != NULL)
		{
			Log::Print(
				"Cannot initialize global OpenGL more than once!",
				"OPENGL_WINDOWS",
				LogType::LOG_SUCCESS);

			return false;
		}

		//
		// CREATE A DUMMY CONTEXT TO LOAD WGL EXTENSIONS
		//

		HWND dummyHWND = CreateDummyWindow();
		HDC dummyDC = GetDC(dummyHWND);

		//
		// SET PIXEL FORMAT FOR DUMMY WINDOW
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

		int pf = ChoosePixelFormat(dummyDC, &pfd);
		if (pf == 0)
		{
			KalaWindowCore::ForceClose(
				"OpenGL Window Error",
				"ChoosePixelFormat failed during global OpenGL init!");

			return false;
		}

		if (!SetPixelFormat(dummyDC, pf, &pfd))
		{
			KalaWindowCore::ForceClose(
				"OpenGL Window Error",
				"SetPixelFormat failed during global OpenGL init!");

			return false;
		}

		HGLRC dummyRC = wglCreateContext(dummyDC);
		wglMakeCurrent(dummyDC, dummyRC);

		//
		// LOAD WGL AND CORE EXTENSIONS
		//

		OpenGL_Functions_Windows::LoadWinFunction("wglCreateContextAttribsARB");
		OpenGL_Functions_Windows::LoadWinFunction("wglChoosePixelFormatARB");
		OpenGL_Functions_Windows::LoadWinFunction("wglSwapIntervalEXT");

		OpenGL_Functions_Core::LoadAllCoreFunctions();

		//
		// CLEAN UP DUMMY
		//

		wglMakeCurrent(nullptr, nullptr);
		wglDeleteContext(dummyRC);
		ReleaseDC(dummyHWND, dummyDC);
		DestroyWindow(dummyHWND);

		Log::Print(
			"Initialized OpenGL!",
			"OPENGL_WINDOWS",
			LogType::LOG_SUCCESS);

		isInitialized = true;

		return true;
	}

	bool OpenGL_Renderer::Initialize(Window* targetWindow)
	{
		if (targetWindow->GetOpenGLData().hdc != NULL)
		{
			Log::Print(
				"Cannot initialize OpenGL more than once per window!",
				"OPENGL_WINDOWS",
				LogType::LOG_SUCCESS);

			return false;
		}

		const WindowData& wData = targetWindow->GetWindowData();
		HWND windowRef = ToVar<HWND>(wData.hwnd);

		OpenGLData data{};

		HDC hdc = GetDC(windowRef);

		data.hdc = FromVar(hdc);

		//
		// SET PIXEL FORMAT FOR WINDOW
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

		int pf = ChoosePixelFormat(hdc, &pfd);
		if (pf == 0)
		{
			KalaWindowCore::ForceClose(
				"OpenGL Window Error",
				"ChoosePixelFormat failed during per-window OpenGL init!");

			return false;
		}
		if (isVerboseLoggingEnabled)
		{
			Log::Print(
				"Pixel Format Index: " + to_string(pf),
				"OPENGL_WINDOWS",
				LogType::LOG_INFO);
		}

		if (!SetPixelFormat(hdc, pf, &pfd))
		{
			KalaWindowCore::ForceClose(
				"OpenGL Window Error",
				"SetPixelFormat failed during per-window OpenGL init!");

			return false;
		}
		if (isVerboseLoggingEnabled)
		{
			Log::Print(
				"SetPixelFormat worked!",
				"OPENGL_WINDOWS",
				LogType::LOG_INFO);
		}

		PIXELFORMATDESCRIPTOR actualPFD = {};
		int describeResult = DescribePixelFormat(
			hdc, 
			pf, 
			sizeof(actualPFD), 
			&actualPFD);

		if (describeResult == 0)
		{
			KalaWindowCore::ForceClose(
				"OpenGL Window Error",
				"DescribePixelFormat failed!");

			return false;
		}
		if (isVerboseLoggingEnabled)
		{
			Log::Print(
				"DescribePixelFormat value: " + to_string(describeResult),
				"OPENGL_WINDOWS",
				LogType::LOG_INFO);

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
				LogType::LOG_INFO);
		}

		int attribs[] =
		{
			WGL_CONTEXT_MAJOR_VERSION_ARB, 
			3,
			WGL_CONTEXT_MINOR_VERSION_ARB, 
			3,
			WGL_CONTEXT_PROFILE_MASK_ARB,
			WGL_CONTEXT_CORE_PROFILE_BIT_ARB, 
			0
		};

		HGLRC existing = ToVar<HGLRC>(GlobalHandle::GetOpenGLWinContext());

		HGLRC hglrc = wglCreateContextAttribsARB(
			hdc,
			existing,
			attribs);

		if (existing == NULL) GlobalHandle::SetOpenGLWinContext(FromVar(hglrc));

		data.hglrc = FromVar(hglrc);

		wglMakeCurrent(hdc, hglrc);

		//and finally set opengl viewport size
		vec2 framebufferSize = targetWindow->GetFramebufferSize();
		glViewport(
			0,
			0,
			framebufferSize.x,
			framebufferSize.y);

		targetWindow->SetOpenGLData(data);

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
			if (newVSyncState == VSyncState::VSYNC_ON) wglSwapIntervalEXT(1);
			else                                       wglSwapIntervalEXT(0);
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
				LogType::LOG_ERROR);
			return;
		}

		const OpenGLData& oData = targetWindow->GetOpenGLData();
		HDC hdc = ToVar<HDC>(oData.hdc);
		SwapBuffers(hdc);
	}

	void OpenGL_Renderer::MakeContextCurrent(Window* window)
	{
		const OpenGLData& oData = window->GetOpenGLData();

		if (oData.hdc == 0
			|| oData.hglrc == 0)
		{
			Log::Print(
				"Cannot make OpenGL context current for window '" + window->GetTitle() + "' because its hdc or hglrc is not assigned!",
				"OPENGL_WINDOWS",
				LogType::LOG_ERROR);

			return;
		}

		HDC hdc = ToVar<HDC>(oData.hdc);
		HGLRC hglrc = ToVar<HGLRC>(oData.hglrc);

		if (wglGetCurrentContext() != hglrc) wglMakeCurrent(hdc, hglrc);
	}

	bool OpenGL_Renderer::IsContextValid(Window* targetWindow)
	{
		const OpenGLData& oData = targetWindow->GetOpenGLData();

		if (oData.hglrc == NULL)
		{
			string title = "OpenGL Error";
			string reason = "Failed to get HGLRC for window '" + targetWindow->GetTitle() + "' during 'IsContextValid' stage!";
			KalaWindowCore::ForceClose(title, reason);
			return false;
		}
		HGLRC hglrcReal = ToVar<HGLRC>(oData.hglrc);

		HGLRC current = wglGetCurrentContext();
		if (current == nullptr)
		{
			Log::Print(
				"OpenGL context is null!",
				"OPENGL_WINDOWS",
				LogType::LOG_ERROR,
				2);

			return false;
		}

		if (current != hglrcReal)
		{
			KalaWindowCore::ForceClose(
				"OpenGL Error",
				"Current OpenGL context does not match stored context!");
			return false;
		}

		return true;
	}

	void OpenGL_Renderer::Shutdown(Window* window)
	{
		if (!isInitialized)
		{
			Log::Print(
				"Failed to shut down OpenGL because it has not yet been initialized!",
				"OPENGL",
				LogType::LOG_ERROR,
				2);

			return;
		}

		if (window == nullptr)
		{
			Log::Print(
				"Failed to shut down OpenGL because its target window is invalid!",
				"OPENGL",
				LogType::LOG_ERROR,
				2);

			return;
		}

#ifdef _WIN32
		OpenGLData oData = window->GetOpenGLData();
		
		HGLRC hglrc = ToVar<HGLRC>(oData.hglrc);
		HDC hdc = ToVar<HDC>(oData.hdc);
		HWND hwnd = ToVar<HWND>(window->GetWindowData().hwnd);

		if (hglrc != NULL)
		{
			if (wglGetCurrentContext() == hglrc) wglMakeCurrent(nullptr, nullptr);
			wglDeleteContext(hglrc);
		}
		if (hdc != NULL) ReleaseDC(hwnd, hdc);
#elif __linux__
		//TODO: DEFINE
#endif
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

HWND CreateDummyWindow()
{
	WNDCLASSA wc = {};
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = DefWindowProcA;
	wc.hInstance = GetModuleHandleA(nullptr);
	wc.lpszClassName = "KalaWindowDummy";

	RegisterClassA(&wc);

	//create the hidden dummy window
	HWND hwnd = CreateWindowExA(
		0,
		wc.lpszClassName,
		"dummy opengl window",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 
		CW_USEDEFAULT,
		CW_USEDEFAULT, 
		CW_USEDEFAULT,
		nullptr, 
		nullptr,
		wc.hInstance,
		nullptr);

	return hwnd;
}

#endif //_WIN32