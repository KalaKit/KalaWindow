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
#include "core/global_handles.hpp"

using KalaHeaders::Log;
using KalaHeaders::LogType;

using KalaWindow::Graphics::Window;
using KalaWindow::Graphics::OpenGL::VSyncState;
using namespace KalaWindow::Graphics::OpenGLFunctions;
using KalaWindow::Core::KalaWindowCore;
using KalaWindow::Core::createdOpenGLTextures;
using KalaWindow::Core::createdOpenGLShaders;
using KalaWindow::Core::runtimeWindows;
using KalaWindow::Core::GlobalHandle;

using std::string;
using std::to_string;

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

	bool OpenGL_Renderer::GetError(const string& context)
	{
		GLenum error{};
		bool hadError = false;

		while ((error = glGetError()) != GL_NO_ERROR)
		{
			hadError = true;

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
				"OpenGL error in " + context + ": " + msg + " (0x" + to_string(error) + ")",
				"OPENGL",
				LogType::LOG_ERROR,
				2);
		}

		return hadError;
	}
}