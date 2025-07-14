//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef KALAWINDOW_SUPPORT_OPENGL

#include <unordered_map>
#include <string_view>
#include <string>

#include "graphics/opengl/opengl_core.hpp"

#ifdef _WIN32
#include <windows.h>
#include "graphics/opengl/opengl_win.hpp"
#elif __linux__
#include "graphics/opengl/opengl_linux.hpp"
#endif

#include "core/log.hpp"

using KalaWindow::Core::Logger;
using KalaWindow::Core::LogType;

using std::unordered_map;
using std::string_view;
using std::string;

namespace KalaWindow::Graphics::OpenGL
{
	static void* (*getProc)(const char*) = GetGLProcAddress;

	static unordered_map<string_view, void*> functionRegistry;
	static const char* lastFailedFunctionName{};

	enum class TrapType
	{
		Void,
		Int,
		Enum,
		BytePtr
	};

	static void* GetTrapForType(
		TrapType type,
		const char* name);

	struct FunctionEntry
	{
		const char* name;
		void** pointer;
		TrapType trapType;
	};

	static FunctionEntry functionTable[] =
	{
		//geometry

		{ "glBindBuffer",            reinterpret_cast<void**>(&glBindBuffer),            TrapType::Void },
		{ "glBindVertexArray",       reinterpret_cast<void**>(&glBindVertexArray),       TrapType::Void },
		{ "glBufferData",            reinterpret_cast<void**>(&glBufferData),            TrapType::Void },
		{ "glDeleteBuffers",         reinterpret_cast<void**>(&glDeleteBuffers),         TrapType::Void },
		{ "glDeleteVertexArrays",    reinterpret_cast<void**>(&glDeleteVertexArrays),    TrapType::Void },
		{ "glDrawArrays",            reinterpret_cast<void**>(&glDrawArrays),            TrapType::Void },
		{ "glDrawElements",          reinterpret_cast<void**>(&glDrawElements),          TrapType::Void },
		{ "glEnableVertexAttribArray", reinterpret_cast<void**>(&glEnableVertexAttribArray), TrapType::Void },
		{ "glGenBuffers",            reinterpret_cast<void**>(&glGenBuffers),            TrapType::Void },
		{ "glGenVertexArrays",       reinterpret_cast<void**>(&glGenVertexArrays),       TrapType::Void },
		{ "glGetVertexAttribiv",     reinterpret_cast<void**>(&glGetVertexAttribiv),     TrapType::Void },
		{ "glGetVertexAttribPointerv", reinterpret_cast<void**>(&glGetVertexAttribPointerv), TrapType::Void },
		{ "glVertexAttribPointer",   reinterpret_cast<void**>(&glVertexAttribPointer),   TrapType::Void },

		//shaders

		{ "glAttachShader",       reinterpret_cast<void**>(&glAttachShader),       TrapType::Void },
		{ "glCompileShader",      reinterpret_cast<void**>(&glCompileShader),      TrapType::Void },
		{ "glCreateProgram",      reinterpret_cast<void**>(&glCreateProgram),      TrapType::Int  },
		{ "glCreateShader",       reinterpret_cast<void**>(&glCreateShader),       TrapType::Int  },
		{ "glDeleteShader",       reinterpret_cast<void**>(&glDeleteShader),       TrapType::Void },
		{ "glDeleteProgram",      reinterpret_cast<void**>(&glDeleteProgram),      TrapType::Void },
		{ "glDetachShader",       reinterpret_cast<void**>(&glDetachShader),       TrapType::Void },
		{ "glGetActiveAttrib",    reinterpret_cast<void**>(&glGetActiveAttrib),    TrapType::Void },
		{ "glGetAttribLocation",  reinterpret_cast<void**>(&glGetAttribLocation),  TrapType::Int  },
		{ "glGetProgramiv",       reinterpret_cast<void**>(&glGetProgramiv),       TrapType::Void },
		{ "glGetProgramInfoLog",  reinterpret_cast<void**>(&glGetProgramInfoLog),  TrapType::Void },
		{ "glGetShaderiv",        reinterpret_cast<void**>(&glGetShaderiv),        TrapType::Void },
		{ "glGetShaderInfoLog",   reinterpret_cast<void**>(&glGetShaderInfoLog),   TrapType::Void },
		{ "glLinkProgram",        reinterpret_cast<void**>(&glLinkProgram),        TrapType::Void },
		{ "glShaderSource",       reinterpret_cast<void**>(&glShaderSource),       TrapType::Void },
		{ "glUseProgram",         reinterpret_cast<void**>(&glUseProgram),         TrapType::Void },
		{ "glValidateProgram",    reinterpret_cast<void**>(&glValidateProgram),    TrapType::Void },
		{ "glIsProgram",          reinterpret_cast<void**>(&glIsProgram),          TrapType::Int  },

		//uniforms

		{ "glGetUniformLocation", reinterpret_cast<void**>(&glGetUniformLocation), TrapType::Int  },
		{ "glUniform1f",          reinterpret_cast<void**>(&glUniform1f),          TrapType::Void },
		{ "glUniform1i",          reinterpret_cast<void**>(&glUniform1i),          TrapType::Void },
		{ "glUniform2f",          reinterpret_cast<void**>(&glUniform2f),          TrapType::Void },
		{ "glUniform2fv",         reinterpret_cast<void**>(&glUniform2fv),         TrapType::Void },
		{ "glUniform3f",          reinterpret_cast<void**>(&glUniform3f),          TrapType::Void },
		{ "glUniform3fv",         reinterpret_cast<void**>(&glUniform3fv),         TrapType::Void },
		{ "glUniform4f",          reinterpret_cast<void**>(&glUniform4f),          TrapType::Void },
		{ "glUniform4fv",         reinterpret_cast<void**>(&glUniform4fv),         TrapType::Void },
		{ "glUniformMatrix2fv",   reinterpret_cast<void**>(&glUniformMatrix2fv),   TrapType::Void },
		{ "glUniformMatrix3fv",   reinterpret_cast<void**>(&glUniformMatrix3fv),   TrapType::Void },
		{ "glUniformMatrix4fv",   reinterpret_cast<void**>(&glUniformMatrix4fv),   TrapType::Void },

		//textures

		{ "glBindTexture",     reinterpret_cast<void**>(&glBindTexture),     TrapType::Void },
		{ "glActiveTexture",   reinterpret_cast<void**>(&glActiveTexture),   TrapType::Void },
		{ "glDeleteTextures",  reinterpret_cast<void**>(&glDeleteTextures),  TrapType::Void },
		{ "glGenerateMipmap",  reinterpret_cast<void**>(&glGenerateMipmap),  TrapType::Void },
		{ "glGenTextures",     reinterpret_cast<void**>(&glGenTextures),     TrapType::Void },
		{ "glTexImage2D",      reinterpret_cast<void**>(&glTexImage2D),      TrapType::Void },
		{ "glTexParameteri",   reinterpret_cast<void**>(&glTexParameteri),   TrapType::Void },
		{ "glTexSubImage2D",   reinterpret_cast<void**>(&glTexSubImage2D),   TrapType::Void },

		//framebuffers and renderbuffers

		{ "glBindRenderbuffer",        reinterpret_cast<void**>(&glBindRenderbuffer),        TrapType::Void },
		{ "glBindFramebuffer",         reinterpret_cast<void**>(&glBindFramebuffer),         TrapType::Void },
		{ "glCheckFramebufferStatus", reinterpret_cast<void**>(reinterpret_cast<void*>(&glCheckFramebufferStatus)), TrapType::Enum },
		{ "glFramebufferRenderbuffer", reinterpret_cast<void**>(&glFramebufferRenderbuffer), TrapType::Void },
		{ "glFramebufferTexture2D",   reinterpret_cast<void**>(&glFramebufferTexture2D),     TrapType::Void },
		{ "glGenRenderbuffers",       reinterpret_cast<void**>(&glGenRenderbuffers),         TrapType::Void },
		{ "glGenFramebuffers",        reinterpret_cast<void**>(&glGenFramebuffers),          TrapType::Void },
		{ "glRenderbufferStorage",    reinterpret_cast<void**>(&glRenderbufferStorage),      TrapType::Void },

		//frame and render state

		{ "glClear",         reinterpret_cast<void**>(&glClear),         TrapType::Void },
		{ "glClearColor",    reinterpret_cast<void**>(&glClearColor),    TrapType::Void },
		{ "glDisable",       reinterpret_cast<void**>(&glDisable),       TrapType::Void },
		{ "glGetError",      reinterpret_cast<void**>(reinterpret_cast<void*>(&glGetError)), TrapType::Enum },
		{ "glGetIntegerv",   reinterpret_cast<void**>(&glGetIntegerv),   TrapType::Void },
		{ "glGetString",     reinterpret_cast<void**>(reinterpret_cast<void*>(&glGetString)), TrapType::BytePtr },
		{ "glViewport",      reinterpret_cast<void**>(&glViewport),      TrapType::Void },
	};

	void OpenGLCore::InitializeAllFunctions()
	{
		for (const auto& entry : functionTable)
		{
			void* ptr = getProc(entry.name);
			if (ptr)
			{
				*entry.pointer = ptr;
				functionRegistry[entry.name] = ptr;
			}
			else
			{
				*entry.pointer = GetTrapForType(entry.trapType, entry.name);
				Logger::Print(
					string("Function '") + entry.name + "' is not available!",
					"OPENGL_CORE",
					LogType::LOG_DEBUG);
			}
		}
	}

	void OpenGLCore::InitializeFunction(const char* name)
	{
		for (const auto& entry : functionTable)
		{
			if (strcmp(entry.name, name) == 0)
			{
				void* ptr = getProc(name);
				if (ptr)
				{
					*entry.pointer = ptr;
					functionRegistry[name] = ptr;
				}
				else
				{
					*entry.pointer = GetTrapForType(entry.trapType, entry.name);
					Logger::Print(
						string("Function '") + entry.name + "' is not available!",
						"OPENGL_CORE",
						LogType::LOG_DEBUG);
				}
				break;
			}
		}
	}

	bool OpenGLCore::IsFunctionAvailable(const char* name)
	{
		auto it = functionRegistry.find(name);
		return it != functionRegistry.end()
			&& it->second != nullptr;
	}

#ifdef _WIN32
	void* GetGLProcAddress(const char* name)
	{
		return reinterpret_cast<void*>(wglGetProcAddress(name));
	}
#elif __linux__
	void* GetGLProcAddress(const char* name)
	{
		return reinterpret_cast<void*>(
			glXGetProcAddressARB(reinterpret_cast<const GLuByte*>(name)));
	}
#endif

	//
	// TRAP THE END USER HERE
	// IF ANY OF THE FUNCTIONS FAIL TO LOAD
	//

	static void FunctionNotLoadedVoidTrap()
	{
		string name = string(lastFailedFunctionName);

		Logger::Print(
			"OpenGL void function '" + name + "' was not loaded.",
			"OPENGL_CORE",
			LogType::LOG_ERROR);
	}

	static GLint FunctionNotLoadedIntTrap()
	{
		string name = string(lastFailedFunctionName);

		Logger::Print(
			"OpenGL int-returning function '" + name + "' was not loaded.",
			"OPENGL_CORE",
			LogType::LOG_ERROR);
		return -1;
	}

	static GLenum FunctionNotLoadedEnumTrap()
	{
		string name = string(lastFailedFunctionName);

		Logger::Print(
			"OpenGL enum-returning function '" + name + "' was not loaded.",
			"OPENGL_CORE",
			LogType::LOG_ERROR);
		return 0;
	}

	static const GLubyte* FunctionNotLoadedBytePtrTrap()
	{
		string name = string(lastFailedFunctionName);

		Logger::Print(
			"OpenGL const GLubyte* function '" + name + "' was not loaded.",
			"OPENGL_CORE",
			LogType::LOG_ERROR);
		return nullptr;
	}

	static void* GetTrapForType(
		TrapType type,
		const char* name)
	{
		lastFailedFunctionName = name;

		switch (type)
		{
		case TrapType::Void:    return reinterpret_cast<void*>(&FunctionNotLoadedVoidTrap);
		case TrapType::Int:     return reinterpret_cast<void*>(&FunctionNotLoadedIntTrap);
		case TrapType::Enum:    return reinterpret_cast<void*>(&FunctionNotLoadedEnumTrap);
		case TrapType::BytePtr: return reinterpret_cast<void*>(&FunctionNotLoadedBytePtrTrap);
		}
		return nullptr;
	}
}

#endif //KALAWINDOW_SUPPORT_OPENGL