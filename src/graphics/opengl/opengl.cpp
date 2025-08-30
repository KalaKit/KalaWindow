//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <string>

#include "KalaHeaders/logging.hpp"

#include "graphics/opengl/opengl.hpp"
#include "graphics/opengl/opengl_functions_core.hpp"
#include "graphics/window.hpp"
#include "core/core.hpp"
#include "core/containers.hpp"

using KalaHeaders::Log;
using KalaHeaders::LogType;

using KalaWindow::Graphics::Window;
using KalaWindow::Graphics::OpenGL::VSyncState;
using namespace KalaWindow::Graphics::OpenGLFunctions;
using KalaWindow::Core::KalaWindowCore;
using KalaWindow::Core::createdOpenGLTextures;
using KalaWindow::Core::createdOpenGLShaders;
using KalaWindow::Core::runtimeWindows;

using std::string;

namespace KalaWindow::Graphics::OpenGL
{
	bool OpenGL_Renderer::IsExtensionSupported(const string& name)
	{
		i32 numExtensions = 0;
		glGetIntegerv(
			GL_NUM_EXTENSIONS,
			&numExtensions);

		for (i32 i = 0; i < numExtensions; ++i)
		{
			const char* extName = reinterpret_cast<const char*>(glGetStringi(GL_EXTENSIONS, i));
			if (name == extName) return true;
		}

		return false;
	}

	void OpenGL_Renderer::GetError(const string& context)
	{
		GLenum error{};

		while ((error = glGetError()) != GL_NO_ERROR)
		{
			string msg{};
			switch (error)
			{
			case GL_INVALID_ENUM:                  msg = "GL_INVALID_ENUM"; break;
			case GL_INVALID_VALUE:                 msg = "GL_INVALID_VALUE"; break;
			case GL_INVALID_INDEX:                 msg = "GL_INVALID_INDEX"; break;

			case GL_INVALID_OPERATION:             msg = "GL_INVALID_OPERATION"; break;
			case GL_STACK_OVERFLOW:                msg = "GL_STACK_OVERFLOW"; break;
			case GL_STACK_UNDERFLOW:               msg = "GL_STACK_UNDERFLOW"; break;
			case GL_INVALID_FRAMEBUFFER_OPERATION: msg = "GL_INVALID_FRAMEBUFFER_OPERATION"; break;

			case GL_OUT_OF_MEMORY:                 msg = "GL_OUT_OF_MEMORY"; break;

			default:                               msg = "Unknown error"; break;
			}

			Log::Print(
				"OpenGL error in " + context + ": " + msg,
				"OPENGL",
				LogType::LOG_ERROR,
				2);
		}
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

		if (window->GetOpenGLData().hdc != NULL
			&& window->GetOpenGLData().hglrc != NULL)
		{
			OpenGLData oData = window->GetOpenGLData();
			if (oData.hdc != 0
				&& oData.hglrc != 0)
			{
				HDC hdc = ToVar<HDC>(oData.hdc);
				HGLRC hglrc = ToVar<HGLRC>(oData.hglrc);

#ifdef _WIN32
				//only clear the context if it's still active
				if (wglGetCurrentContext() == hglrc)
				{
					wglMakeCurrent(nullptr, nullptr);
				}
#elif __linux__
				//TODO: DEFINE
#endif
			}
		}
	}
}