//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#if defined(__linux__) && defined(KW_USE_X11)

#include <string>

#include <GL/gl.h>
#include "OpenGL/glext.h"

#include "KalaHeaders/log_utils.hpp"

#include "opengl/kw_opengl.hpp"
#include "opengl/kw_opengl_functions_core.hpp"
#include "core/kw_core.hpp"
#include "graphics/kw_window.hpp"

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;

using KalaWindow::OpenGL::OpenGLFunctions::GL_Core;
using KalaWindow::OpenGL::OpenGLFunctions::OpenGL_Functions_Core;
using KalaWindow::Core::KalaWindowCore;
using KalaWindow::Graphics::ProcessWindow;

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
				LogType::LOG_ERROR);

			return;
		}

        //initialize here...

        Log::Print(
			"Initialized global OpenGL context!",
			"OPENGL",
			LogType::LOG_SUCCESS);

		isInitialized = true;
    }

    bool OpenGL_Global::IsInitialized() { return isInitialized; }

    void OpenGL_Global::SetOpenGLLibrary() {}
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
			const char* extName = reinterpret_cast<const char*>(
				coreFunc->glGetStringi(GL_EXTENSIONS, i));
			if (name == extName) return true;
		}

		return false;
	}

    void OpenGL_Global::MakeContextCurrent(
		OpenGL_Context* context,
		uintptr_t handle)
    {

    }
    void OpenGL_Global::MakeContextCurrent(
		uintptr_t context,
		uintptr_t handle)
    {

    }

    bool OpenGL_Global::IsContextValid(OpenGL_Context* context)
    {
        static bool cont{};
        return cont;
    }
    bool OpenGL_Global::IsContextValid(uintptr_t context)
    {
        static bool cont{};
        return cont;
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