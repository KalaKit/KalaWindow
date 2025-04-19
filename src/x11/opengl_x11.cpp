//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef KALAKIT_X11

#define KALAKIT_MODULE "OPENGL"

#include <sstream>
#include <vector>
#include <GL/glx.h>

//kalawindow
#include "opengl.hpp"
#include "opengl_loader.hpp"
#include "internal/window_x11.hpp"

using std::hex;
using std::dec;
using std::ostringstream;
using std::vector;

namespace KalaKit
{
	static void ErrorPopup(const string& message)
	{
		LOG_ERROR(message);
		if (KalaWindow::CreatePopup(
			"OpenGL error on X11", 
			message,
			PopupAction::POPUP_ACTION_OK, 
			PopupType::POPUP_TYPE_ERROR)
			== PopupResult::POPUP_RESULT_OK)
		{
			KalaWindow::SetShouldCloseState(true);
		}
	}

	bool OpenGL::Initialize(int width, int height)
	{
		int contextAttribs[] = 
		{
			GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
			GLX_CONTEXT_MINOR_VERSION_ARB, 3,
			GLX_CONTEXT_PROFILE_MASK_ARB,
			GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
			None
		};

		PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB =
			(PFNGLXCREATECONTEXTATTRIBSARBPROC)
			glXGetProcAddressARB((const GLubyte*)"glXCreateContextAttribsARB");

		if (!glXCreateContextAttribsARB)
		{
			ErrorPopup("glXCreateContextAttribsARB not available!");
			return false;
		}

		GLXContext context = glXCreateContextAttribsARB(
			Window_X11::newDisplay,
			Window_X11::glxFBConfig,
			nullptr,
			True,
			contextAttribs
		);
		if (!context)
		{
			ErrorPopup("glXCreateContextAttribsARB failed!");
			return false;
		}
		Window_X11::glxContext = context;

		if (!glXMakeCurrent(
			Window_X11::newDisplay,
			Window_X11::newWindow,
			context
		))
		{
			ErrorPopup("glxMakeCurrent failed!");
			return false;
		}

		LOG_SUCCESS("OpenGL version: " << glGetString(GL_VERSION));

		OpenGLLoader::LoadAllFunctions();

		//and finally set opengl viewport size
		OpenGLLoader::glViewportPtr(0, 0, width, height);

		return true;
	}

	bool OpenGL::IsContextValid()
	{
		return true;
	}
}

#endif // KALAKIT_X11