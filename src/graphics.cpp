//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

//main log macro
#define WRITE_LOG(type, msg) std::cout << "[KALAKIT_WINDOW | " << type << "] " << msg << "\n"

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

#include "graphics.hpp"
#include "window.hpp"

using std::find;

namespace KalaKit
{
	//wgl extension typedefs
	typedef HGLRC(WINAPI* PFNWGLCREATECONTEXTATTRIBSARBPROC)(HDC, HGLRC, const int*);
	typedef BOOL(WINAPI* PFNWGLCHOOSEPIXELFORMATARBPROC)(
		HDC,
		const int*,
		const FLOAT*,
		UINT,
		int*,
		UINT*);

	bool Graphics::CreateOpenGLContext()
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
			string title = "OpenGL error";
			string message = "wglCreateContextAttribsARB not supported!";

			MessageBox(
				nullptr,
				message.c_str(),
				title.c_str(),
				MB_ICONERROR);

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
			string title = "OpenGL error";
			string message = "Failed to create OpenGL 3.3 context!";

			MessageBox(
				nullptr,
				message.c_str(),
				title.c_str(),
				MB_ICONERROR);

			return false;
		}

		//
		// CLEANUP AND SET REAL CONTEXT
		//

		wglMakeCurrent(nullptr, nullptr);
		wglDeleteContext(dummyRC);

		wglMakeCurrent(hdc, realRC);

		LOG_SUCCESS("OpenGL version: " << glGetString(GL_VERSION));

		return true;
	}

	void Graphics::LoadCoreOpenGLFunctions(const vector<OpenGLFunction>& functions)
	{
		for (const auto& entry : openGLFunctionTable)
		{
			if (find(functions.begin(), functions.end(), entry.id) != functions.end())
			{
				*entry.target = LoadOpenGLFunction<void*>(entry.name);
			}
		}
	}

	template <typename T>
	T Graphics::LoadOpenGLFunction(const char* name)
	{
		T func = reinterpret_cast<T>(wglGetProcAddress(name));
		if (!func)
		{
			LOG_ERROR("Failed to load OpenGL function: " << name);
		}
		return func;
	}

	const vector<Graphics::OpenGLFunctionEntry> Graphics::openGLFunctionTable = 
	{
		
	};
}