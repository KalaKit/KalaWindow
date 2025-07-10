//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef KALAWINDOW_SUPPORT_OPENGL

#define KALAKIT_MODULE "OPENGL"

#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <string>

#include "graphics/opengl/opengl.hpp"
#include "graphics/opengl/opengl_loader.hpp"
#include "graphics/render.hpp"
#include "graphics/window.hpp"
#include "core/enums.hpp"

using KalaWindow::Graphics::Render;
using KalaWindow::Graphics::ShutdownState;
using KalaWindow::Graphics::Window;
using KalaWindow::PopupAction;
using KalaWindow::PopupType;
using KalaWindow::PopupResult;

using std::string;

static void ForceClose(
	const string& title,
	const string& reason,
	ShutdownState state = ShutdownState::SHUTDOWN_FAILURE)
{
	LOG_ERROR(reason);

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
		LOG_DEBUG("Window HDC: " << hdc);

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
			LOG_ERROR("ChoosePixelFormat failed!");

			ForceClose(
				"OpenGL error",
				"[Initialize] ChoosePixelFormat failed!");

			return false;
		}
		LOG_DEBUG("Pixel Format Index: " << pixelFormat);

		if (!SetPixelFormat(hdc, pixelFormat, &pfd))
		{
			LOG_ERROR("SetPixelFormat failed!");

			ForceClose(
				"OpenGL error",
				"[Initialize] SetPixelFormat failed!");

			return false;
		}
		LOG_DEBUG("SetPixelFormat worked!");

		PIXELFORMATDESCRIPTOR actualPFD = {};
		int describeResult = DescribePixelFormat(hdc, pixelFormat, sizeof(actualPFD), &actualPFD);
		if (describeResult == 0)
		{
			LOG_ERROR("DescribePixelFormat failed!");

			ForceClose(
				"OpenGL error",
				"[Initialize] DescribePixelFormat failed!");

			return false;
		}
		else
		{
			LOG_DEBUG("DescribePixelFormat value: " << describeResult);
		}

		LOG_DEBUG("Pixel Format Details:");
		LOG_DEBUG("  ColorBits   = " << static_cast<int>(actualPFD.cColorBits));
		LOG_DEBUG("  DepthBits   = " << static_cast<int>(actualPFD.cDepthBits));
		LOG_DEBUG("  StencilBits = " << static_cast<int>(actualPFD.cStencilBits));
		LOG_DEBUG("  Flags:");
		LOG_DEBUG("    DRAW_TO_WINDOW = " << ((actualPFD.dwFlags & PFD_DRAW_TO_WINDOW) ? "Yes" : "No"));
		LOG_DEBUG("    SUPPORT_OPENGL = " << ((actualPFD.dwFlags & PFD_SUPPORT_OPENGL) ? "Yes" : "No"));
		LOG_DEBUG("    DOUBLEBUFFER    = " << ((actualPFD.dwFlags & PFD_DOUBLEBUFFER) ? "Yes" : "No"));

		HGLRC dummyRC = wglCreateContext(hdc);
		wglMakeCurrent(hdc, dummyRC);

		//
		// LOAD WGL EXTENSIONS
		//

		auto wglCreateContextAttribsARB =
			reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(
				wglGetProcAddress("wglCreateContextAttribsARB"));

		if (!wglCreateContextAttribsARB)
		{
			LOG_ERROR("wglCreateContextAttribsARB is not supported!");

			ForceClose(
				"OpenGL error",
				"[Initialize] wglCreateContextAttribsARB is not supported!");

			return false;
		}

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

		window.openglData.hglrc = FromVar<HGLRC>(wglCreateContextAttribsARB(hdc, 0, attribs));
		if (!window.openglData.hglrc)
		{
			LOG_ERROR("Failed to create OpenGL 3.3 context!");

			ForceClose(
				"OpenGL error",
				"[Initialize] Failed to create OpenGL 3.3 context!");

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
			LOG_ERROR("OpenGL 3.3 or higher is required!");

			string title = "OpenGL error detected!";
			string message = "OpenGL 3.3 or higher is required!";

			ForceClose(
				"OpenGL error",
				"[Initialize] OpenGL 3.3 or higher is required!");

			return false;
		}

		LOG_SUCCESS("OpenGL version: " << glGetString(GL_VERSION));

		OpenGLLoader::LoadAllFunctions();

		//and finally set opengl viewport size
		OpenGLLoader::glViewport(
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
}

#endif //KALAWINDOW_SUPPORT_OPENGL