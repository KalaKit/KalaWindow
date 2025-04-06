//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

//main log macro
#define WRITE_LOG(type, msg) std::cout << "[KALAKIT_OPENGL | " << type << "] " << msg << "\n"

//log types
#if KALAWINDOW_DEBUG
#define LOG_DEBUG(msg) WRITE_LOG("DEBUG", msg)
#else
#define LOG_DEBUG(msg)
#endif
#define LOG_SUCCESS(msg) WRITE_LOG("SUCCESS", msg)
#define LOG_ERROR(msg) WRITE_LOG("ERROR", msg)

#ifndef WGL_CONTEXT_MAJOR_VERSION_ARB
	#define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
#endif

#ifndef WGL_CONTEXT_MINOR_VERSION_ARB
	#define WGL_CONTEXT_MINOR_VERSION_ARB 0x2092
#endif

#ifndef WGL_CONTEXT_PROFILE_MASK_ARB
	#define WGL_CONTEXT_PROFILE_MASK_ARB 0x9126
#endif

#ifndef WGL_CONTEXT_CORE_PROFILE_BIT_ARB
	#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001
#endif

#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <iostream>

#include "opengl.hpp"
#include "window.hpp"
#include "opengl_loader.hpp"

namespace KalaKit
{
	bool OpenGL::Initialize()
	{
		HDC hdc = GetDC(KalaWindow::window);

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

		int pixelFormat = ChoosePixelFormat(hdc, &pfd);
		SetPixelFormat(hdc, pixelFormat, &pfd);

		HGLRC dummyRC = wglCreateContext(hdc);
		wglMakeCurrent(hdc, dummyRC);

		//
		// LOAD WGL EXTENSIONS
		//

		auto wglCreateContextAttribsARB = reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(
			wglGetProcAddress("wglCreateContextAttribsARB"));

		if (!wglCreateContextAttribsARB)
		{
			LOG_ERROR("wglCreateContextAttribsARB is not supported!");

			string title = "OpenGL error!";
			string message = "wglCreateContextAttribsARB is not supported!";

			if (KalaWindow::CreatePopup(
				title,
				message,
				PopupAction::POPUP_ACTION_OK,
				PopupType::POPUP_TYPE_ERROR)
				== PopupResult::POPUP_RESULT_OK)
			{
				KalaWindow::SetShouldCloseState(true);
			}

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

		HGLRC realRC = wglCreateContextAttribsARB(hdc, 0, attribs);
		if (!realRC)
		{
			LOG_ERROR("Failed to create OpenGL 3.3 context!");

			string title = "OpenGL error!";
			string message = "Failed to create OpenGL 3.3 context!";

			if (KalaWindow::CreatePopup(
				title,
				message,
				PopupAction::POPUP_ACTION_OK,
				PopupType::POPUP_TYPE_ERROR)
				== PopupResult::POPUP_RESULT_OK)
			{
				KalaWindow::SetShouldCloseState(true);
			}

			return false;
		}

		//
		// CLEANUP AND SET REAL CONTEXT
		//

		wglMakeCurrent(nullptr, nullptr);
		wglDeleteContext(dummyRC);

		wglMakeCurrent(hdc, realRC);

		if (!IsCorrectVersion())
		{
			LOG_ERROR("OpenGL 3.3 or higher is required!");

			string title = "OpenGL error!";
			string message = "OpenGL 3.3 or higher is required!";

			if (KalaWindow::CreatePopup(
				title,
				message,
				PopupAction::POPUP_ACTION_OK,
				PopupType::POPUP_TYPE_ERROR)
				== PopupResult::POPUP_RESULT_OK)
			{
				KalaWindow::SetShouldCloseState(true);
			}

			return false;
		}

		LOG_SUCCESS("OpenGL version: " << glGetString(GL_VERSION));

		OpenGLLoader::LoadAllFunctions();

		return true;
	}

	bool OpenGL::IsCorrectVersion()
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