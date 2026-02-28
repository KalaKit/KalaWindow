//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.


#if defined(__linux__) && defined(KW_USE_X11)

#include <X11/Xlib.h>
#include <X11/X.h>
#include <X11/Xutil.h>
#include <dlfcn.h>

#include <GL/glx.h>
#include <OpenGL/glext.h>

#include <string>

#include "KalaHeaders/core_utils.hpp"
#include "KalaHeaders/log_utils.hpp"

#include "opengl/kw_opengl.hpp"
#include "opengl/kw_opengl_functions_core.hpp"
#include "opengl/kw_opengl_functions_linux.hpp"
#include "core/kw_core.hpp"
#include "graphics/kw_window.hpp"
#include "graphics/kw_window_global.hpp"

using KalaHeaders::KalaCore::ToVar;
using KalaHeaders::KalaCore::FromVar;

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;

using KalaWindow::OpenGL::OpenGLFunctions::GL_Core;
using KalaWindow::OpenGL::OpenGLFunctions::OpenGL_Functions_Core;
using KalaWindow::OpenGL::OpenGLFunctions::OpenGL_Functions_Linux;
using KalaWindow::Core::KalaWindowCore;
using KalaWindow::Graphics::ProcessWindow;
using KalaWindow::Graphics::Window_Global;
using KalaWindow::Graphics::X11GlobalData;

using std::string;
using std::to_string;
using i32 = int32_t;

namespace KalaWindow::OpenGL
{
	//
	// GLOBAL
	//

	static bool isInitialized{};
	static bool isVerboseLoggingEnabled{};

	static uintptr_t openGL32Lib{};

    void OpenGL_Global::SetVerboseLoggingState(bool newState) { isVerboseLoggingEnabled = newState; }
	bool OpenGL_Global::IsVerboseLoggingEnabled() { return isVerboseLoggingEnabled; }

	void OpenGL_Global::Initialize()
    {
		if (isInitialized)
		{
			Log::Print(
				"Cannot initialize global OpenGL more than once!",
				"OPENGL",
				LogType::LOG_ERROR,
				2);

			return;
		}

		if (!Window_Global::IsInitialized())
		{
			KalaWindowCore::ForceClose(
				"Global OpenGL init error",
				"Cannot initialize global OpenGL because global window manager has not been initialized!");

			return;
		}

		//
		// SET UP FBCONFIG
		//

		const X11GlobalData& globalData = Window_Global::GetGlobalData();
		Display* display = ToVar<Display*>(globalData.display);

		int visualAttribs[] =
		{
			GLX_X_RENDERABLE, True,
			GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
			GLX_RENDER_TYPE, GLX_RGBA_BIT,
			GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
			GLX_RED_SIZE, 8,
			GLX_GREEN_SIZE, 8,
			GLX_BLUE_SIZE, 8,
			GLX_ALPHA_SIZE, 8,
			GLX_DEPTH_SIZE, 24,
			GLX_STENCIL_SIZE, 8,
			GLX_DOUBLEBUFFER, True,
			None
		};

		int fbCount{};
		GLXFBConfig* fbConfigs =
			glXChooseFBConfig(
				display,
				DefaultScreen(display),
				visualAttribs,
				&fbCount);

		if (!fbConfigs
			|| fbCount == 0)
		{
			KalaWindowCore::ForceClose(
				"Global OpenGL init error",
				"Failed to choose FB config!");

			return;
		}

		XVisualInfo* vi = glXGetVisualFromFBConfig(display, fbConfigs[0]);

		if (!vi)
		{
			KalaWindowCore::ForceClose(
				"Global OpenGL init error",
				"Failed to get visual from FB config!");

			return;
		}

		//
		// CREATE DUMMY WINDOW FOR GLX
		//

		XSetWindowAttributes swa{};
		swa.colormap = XCreateColormap(
			display,
			RootWindow(display, vi->screen),
			vi->visual,
			AllocNone);

		swa.event_mask = StructureNotifyMask;

		Window dummyWindow = XCreateWindow(
			display,
			RootWindow(display, vi->screen),
			0, 0, 1, 1,
			0,
			vi->depth,
			InputOutput,
			vi->visual,
			CWColormap
			| CWEventMask,
			&swa);

		XMapWindow(display, dummyWindow);

		//
		// CREATE GLX CONTEXT
		//

		GLXContext dummyContext = 
			glXCreateNewContext(
				display,
				fbConfigs[0],
				GLX_RGBA_TYPE,
				nullptr,
				True);

		if (!dummyContext)
		{
			KalaWindowCore::ForceClose(
				"Global OpenGL init error",
				"Failed to create dummy context!");

			return;
		}

		if (!glXMakeCurrent(
			display,
			dummyWindow,
			dummyContext))
		{
			KalaWindowCore::ForceClose(
				"Global OpenGL init error",
				"Failed to make glx context current!");

			return;
		}

		//
		// LOAD OS AND CORE GL FUNCTIONS
		//

		OpenGL_Functions_Core::LoadAllCoreFunctions();
		OpenGL_Functions_Linux::LoadAllLinuxFunctions();

		//
		// CLEAN UP DUMMY
		//

		glXMakeCurrent(
			display,
			None,
			nullptr);
		glXDestroyContext(
			display,
			dummyContext);

		XDestroyWindow(
			display,
			dummyWindow);

		XFreeColormap(
			display,
			swa.colormap);
		XFree(vi);
		XFree(fbConfigs);

		Log::Print(
			"Initialized global OpenGL context!",
			"OPENGL",
			LogType::LOG_SUCCESS);

		isInitialized = true;
    }

    bool OpenGL_Global::IsInitialized() { return isInitialized; }

    void OpenGL_Global::SetOpenGLLibrary()
	{
		void* handle = dlopen(
			"libGL.so.1",
			RTLD_NOW
			| RTLD_GLOBAL);
		 if (!handle)
		{
			KalaWindowCore::ForceClose(
				"OpenGL error",
				"Failed to load 'libGL.so.1'");
		}

    	openGL32Lib = FromVar(handle);
	}
	uintptr_t OpenGL_Global::GetOpenGLLibrary()
    {
		if (!openGL32Lib) SetOpenGLLibrary();

		return openGL32Lib;
	}

    bool OpenGL_Global::IsExtensionSupported(const string& name)
	{
		const GL_Core* coreFunc = OpenGL_Functions_Core::GetGLCore();

		if (coreFunc->glGetIntegerv == nullptr)
		{
			KalaWindowCore::ForceClose(
				"OpenGL error",
				"OpenGL core function 'glGetIntegerv' is unassigned! Cannot call 'IsExtensionSupported'.");
				
			return false;
		}
		if (coreFunc->glGetStringi == nullptr)
		{
			KalaWindowCore::ForceClose(
				"OpenGL error",
				"OpenGL core function 'glGetStringi' is unassigned! Cannot call 'IsExtensionSupported'.");
				
			return false;
		}
		
		i32 numExtensions = 0;
		coreFunc->glGetIntegerv(
			GL_NUM_EXTENSIONS,
			&numExtensions);

		for (i32 i = 0; i < numExtensions; ++i)
		{
			const char* extName = rcast<const char*>(
				coreFunc->glGetStringi(GL_EXTENSIONS, i));
			if (name == extName) return true;
		}

		return false;
	}

    void OpenGL_Global::MakeContextCurrent(
		OpenGL_Context* context,
		uintptr_t handle)
    {
		if (!context)
		{
			Log::Print(
				"Cannot set OpenGL context because the attached context doesn't exist!",
				"OPENGL",
				LogType::LOG_ERROR,
				2);

			return;
		}
		
		if (OpenGL_Context::GetRegistry().runtimeContent.empty())
		{
			Log::Print(
				"Cannot set OpenGL context because no OpenGL contexts have been created!",
				"OPENGL",
				LogType::LOG_ERROR,
				2);

			return;
		}

		if (!Window_Global::IsInitialized())
		{
			KalaWindowCore::ForceClose(
				"OpenGL error",
				"Cannot set OpenGL context because global window manager has not been initialized!");

			return;
		}
			
		const X11GlobalData& globalData = Window_Global::GetGlobalData();
		Display* display = ToVar<Display*>(globalData.display);

		GLXContext stored = ToVar<GLXContext>(context->GetContext());
		GLXDrawable drawable = scast<GLXDrawable>(handle);

		if (glXGetCurrentContext() != stored
			&& !glXMakeCurrent(display, drawable, stored))
		{
			KalaWindowCore::ForceClose(
				"OpenGL error",
				"Failed to make OpenGL context current!");
		}
    }
    void OpenGL_Global::MakeContextCurrent(
		uintptr_t context,
		uintptr_t handle)
    {
		if (!context)
		{
			Log::Print(
				"Cannot set OpenGL context because the attached context doesn't exist!",
				"OPENGL",
				LogType::LOG_ERROR,
				2);

			return;
		}

		if (OpenGL_Context::GetRegistry().runtimeContent.empty())
		{
			Log::Print(
				"Cannot set OpenGL context because no OpenGL contexts have been created!",
				"OPENGL",
				LogType::LOG_ERROR,
				2);

			return;
		}

		if (!Window_Global::IsInitialized())
		{
			KalaWindowCore::ForceClose(
				"Global OpenGL error",
				"Cannot set OpenGL context because global window manager has not been initialized!");

			return;
		}

		const X11GlobalData& globalData = Window_Global::GetGlobalData();
		Display* display = ToVar<Display*>(globalData.display);

		GLXContext stored = ToVar<GLXContext>(context);
		GLXDrawable drawable = scast<GLXDrawable>(handle);

		if (glXGetCurrentContext() != stored
			&& !glXMakeCurrent(display, drawable, stored))
		{
			KalaWindowCore::ForceClose(
				"OpenGL error",
				"Failed to make OpenGL context current!");
		}
    }

    bool OpenGL_Global::IsContextValid(OpenGL_Context* context)
    {
        if (!context)
		{
			Log::Print(
				"Cannot check OpenGL context validity because the attached context doesn't exist!",
				"OPENGL",
				LogType::LOG_ERROR,
				2);

			return false;
		}
		
		if (OpenGL_Context::GetRegistry().runtimeContent.empty())
		{
			Log::Print(
				"Cannot check OpenGL context validity because no OpenGL contexts have been created!",
				"OPENGL",
				LogType::LOG_ERROR,
				2);

			return false;
		}

		if (!Window_Global::IsInitialized())
		{
			KalaWindowCore::ForceClose(
				"Global OpenGL error",
				"Cannot check context validity because global window manager has not been initialized!");

			return false;
		}
			
		GLXContext stored = ToVar<GLXContext>(context->GetContext());
		GLXContext current = glXGetCurrentContext();

		if (!current)
		{
			KalaWindowCore::ForceClose(
				"OpenGL error",
				"No context is currently bound!");

			return false;
		}

		if (current != stored)
		{
			KalaWindowCore::ForceClose(
				"OpenGL error",
				"Current OpenGL context does not match stored context!");

			return false;
		}

		return true;
    }
    bool OpenGL_Global::IsContextValid(uintptr_t context)
    {
        if (!context)
		{
			Log::Print(
				"Cannot check OpenGL context validity because the attached context doesn't exist!",
				"OPENGL",
				LogType::LOG_ERROR,
				2);

			return false;
		}
		
		if (OpenGL_Context::GetRegistry().runtimeContent.empty())
		{
			Log::Print(
				"Cannot check OpenGL context validity because no OpenGL contexts have been created!",
				"OPENGL",
				LogType::LOG_ERROR,
				2);

			return false;
		}

		if (!Window_Global::IsInitialized())
		{
			KalaWindowCore::ForceClose(
				"Global OpenGL error",
				"Cannot check context validity because global window manager has not been initialized!");

			return false;
		}
			
		GLXContext stored = ToVar<GLXContext>(context);
		GLXContext current = glXGetCurrentContext();

		if (!current)
		{
			KalaWindowCore::ForceClose(
				"OpenGL error",
				"No context is currently bound!");

			return false;
		}

		if (current != stored)
		{
			KalaWindowCore::ForceClose(
				"OpenGL error",
				"Current OpenGL context does not match stored context!");

			return false;
		}

		return true;
    }

    string OpenGL_Global::GetError()
	{	
		const GL_Core* coreFunc = OpenGL_Functions_Core::GetGLCore();

		GLenum error{};
		string errorVal{};

		while ((error = coreFunc->glGetError()) != GL_NO_ERROR)
		{
			switch (error)
			{
			case GL_INVALID_ENUM:                  errorVal = "GL_INVALID_ENUM"; break;
			case GL_INVALID_VALUE:                 errorVal = "GL_INVALID_VALUE"; break;
			case GL_INVALID_INDEX:                 errorVal = "GL_INVALID_INDEX"; break;

			case GL_INVALID_OPERATION:             errorVal = "GL_INVALID_OPERATION"; break;
			case GL_STACK_OVERFLOW:                errorVal = "GL_STACK_OVERFLOW"; break;
			case GL_STACK_UNDERFLOW:               errorVal = "GL_STACK_UNDERFLOW"; break;
			case GL_INVALID_FRAMEBUFFER_OPERATION: errorVal = "GL_INVALID_FRAMEBUFFER_OPERATION"; break;

			case GL_OUT_OF_MEMORY:                 errorVal = "GL_OUT_OF_MEMORY"; break;

			default:                               errorVal = "Unknown error"; break;
			}
		}

		return errorVal;
	}

    //
	// CONTEXT
	//

	static KalaWindowRegistry<OpenGL_Context> registry{};

	KalaWindowRegistry<OpenGL_Context>& OpenGL_Context::GetRegistry() { return registry; }

	OpenGL_Context* OpenGL_Context::Initialize(
		u32 windowID,
		u32 parentContext,
		MultiSampling msaa,
		SRGBMode srgb,
		ColorBufferBits cBits,
		DepthBufferBits dBits,
		StencilBufferBits sBits,
		AlphaChannel aChannel)
	{
        if (!OpenGL_Global::IsInitialized())
		{
			KalaWindowCore::ForceClose(
				"OpenGL error",
				"Cannot initialize OpenGL context because global OpenGL has not yet been initialized!");

			return nullptr;
		}

		ProcessWindow* window = ProcessWindow::GetRegistry().GetContent(windowID);

		if (!window
			|| !window->IsInitialized())
		{
			KalaWindowCore::ForceClose(
				"OpenGL error",
				"Cannot initialize OpenGL context because it's window was not found!");

			return nullptr;
		}

        //initialize here...

        return nullptr;
    }

    bool OpenGL_Context::IsInitialized() const { return isInitialized; }

	u32 OpenGL_Context::GetID() const { return ID; }
	u32 OpenGL_Context::GetWindowID() const { return windowID; }
	
	void OpenGL_Context::SwapOpenGLBuffers(uintptr_t handle)
	{

	}

	const string& OpenGL_Context::GetContextData() { return contextData; }

	uintptr_t OpenGL_Context::GetContext() const { return hglrc; }
	uintptr_t OpenGL_Context::GetParentContext() const { return parentHglrc; }

    void OpenGL_Context::SetVSyncState(VSyncState newValue)
	{		
		
	}
	VSyncState OpenGL_Context::GetVSyncState() const { return vsyncState; }

	OpenGL_Context::~OpenGL_Context()
	{
		if (!isInitialized)
		{
			Log::Print(
				"Cannot shut down OpenGL context because it is not initialized!",
				"OPENGL",
				LogType::LOG_ERROR,
				2);

			return;
		}

		ProcessWindow* window = ProcessWindow::GetRegistry().GetContent(windowID);

		if (!window
			|| !window->IsInitialized())
		{
			Log::Print(
				"Cannot shut down OpenGL context because its window was not found!",
				"OPENGL",
				LogType::LOG_ERROR,
				2);

			return;
		}

		Log::Print(
			"Destroying OpenGL context for window '" + window->GetTitle() + "' with ID '" + to_string(ID) + "'.",
			"OPENGL",
			LogType::LOG_DEBUG);

        /*
		HGLRC storedHGLRC = ToVar<HGLRC>(hglrc);

		if (storedHGLRC != NULL)
		{
			if (wglGetCurrentContext() == storedHGLRC) wglMakeCurrent(nullptr, nullptr);
			wglDeleteContext(storedHGLRC);
		}
        */
	}
}

#endif //__linux__ and KW_USE_X11