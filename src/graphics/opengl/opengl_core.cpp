//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <unordered_map>
#include <string_view>
#include <string>
#include <format>
#include <sstream>

#include "graphics/opengl/opengl_core.hpp"
#include "graphics/window.hpp"
#include "core/log.hpp"

#ifdef _WIN32
#include <windows.h>
#include "graphics/opengl/opengl_win.hpp"
#elif __linux__
#include "graphics/opengl/opengl_linux.hpp"
#endif

#include "core/log.hpp"
#include "core/core.hpp"

using KalaWindow::Core::Logger;
using KalaWindow::Core::LogType;
using KalaWindow::Core::KalaWindowCore;
using KalaWindow::Graphics::Window;
using KalaWindow::Core::Logger;
using KalaWindow::Core::LogType;

using std::unordered_map;
using std::string_view;
using std::string;
using std::to_string;
using std::format;
using std::stringstream;

HMODULE module{};

void DebugCallback(
	GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const char* message,
	const void* userParam)
{
	auto GetSourceStr = [](GLenum source)
		{
			switch (source)
			{
			default:return "UNKNOWN";

			case GL_DEBUG_SOURCE_API:
				return "API";
			case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
				return "WINDOW_SYSTEM";
			case GL_DEBUG_SOURCE_SHADER_COMPILER:
				return "SHADER_COMPILER";
			case GL_DEBUG_SOURCE_THIRD_PARTY:
				return "THIRD_PARTY";
			case GL_DEBUG_SOURCE_APPLICATION:
				return "APPLICATION";
			case GL_DEBUG_SOURCE_OTHER:
				return "OTHER";
			}
		};

	auto GetTypeStr = [](GLenum type)
		{
			switch (type)
			{
			default: return "UNKNOWN";
			case GL_DEBUG_TYPE_ERROR:
				return "ERROR";
			case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
				return "DEPRECATED";
			case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
				return "UNDEFINED";
			case GL_DEBUG_TYPE_PORTABILITY:
				return "PORTABILITY";
			case GL_DEBUG_TYPE_PERFORMANCE:
				return "PERFORMANCE";
			case GL_DEBUG_TYPE_MARKER:
				return "MARKER";
			case GL_DEBUG_TYPE_PUSH_GROUP:
				return "PUSH_GROUP";
			case GL_DEBUG_TYPE_POP_GROUP:
				return "POP_GROUP";
			case GL_DEBUG_TYPE_OTHER:
				return "OTHER";
			}
		};

	auto GetSeverityStr = [](GLenum severity)
		{
			switch (severity)
			{
			default: return "UNKNOWN";

			case GL_DEBUG_SEVERITY_HIGH:
				return "HIGH";
			case GL_DEBUG_SEVERITY_MEDIUM:
				return "MEDIUM";
			case GL_DEBUG_SEVERITY_LOW:
				return "LOW";
			case GL_DEBUG_SEVERITY_NOTIFICATION:
				return "NOTIFICATION";
			}
		};

	stringstream ss{};
	ss << "\nSource: " << GetSourceStr(source)
		<< "\nType: " << GetTypeStr(type)
		<< "\nSeverity: " << GetSeverityStr(severity)
		<< "\nID: " << to_string(id)
		<< "\nMessage: " << message << "\n";

	Logger::Print(
		ss.str(),
		"OPENGL DEBUG MESSAGE",
		LogType::LOG_DEBUG);
}

namespace KalaWindow::Graphics::OpenGL
{
#define FN_ENTRY(name, trap) { #name, reinterpret_cast<void**>(&name), trap }
#define FN_ENTRY_CAST(name, trap) { #name, reinterpret_cast<void**>(reinterpret_cast<void*>(&name)), trap }

	static void* (*getProc)(const char*) = OpenGLCore::GetGLProcAddress;

	static unordered_map<string_view, void*> functionRegistry;
	static const char* lastFailedFunctionName{};

	enum class TrapType
	{
		Void,
		Int,
		Enum,
		BytePtr,
		Pointer
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
		//debugging

		FN_ENTRY(glDebugMessageCallback,  TrapType::Void),
		FN_ENTRY(glEnable,                TrapType::Void),

		//geometry

		FN_ENTRY(glBindBuffer,            TrapType::Void),
		FN_ENTRY(glBindVertexArray,       TrapType::Void),
		FN_ENTRY(glBufferData,            TrapType::Void),
		FN_ENTRY(glDeleteBuffers,         TrapType::Void),
		FN_ENTRY(glDeleteVertexArrays,    TrapType::Void),
		FN_ENTRY(glDrawArrays,            TrapType::Void),
		FN_ENTRY(glDrawElements,          TrapType::Void),
		FN_ENTRY(glEnableVertexAttribArray, TrapType::Void),
		FN_ENTRY(glGenBuffers,            TrapType::Void),
		FN_ENTRY(glGenVertexArrays,       TrapType::Void),
		FN_ENTRY(glGetVertexAttribiv,     TrapType::Void),
		FN_ENTRY(glGetVertexAttribPointerv, TrapType::Void),
		FN_ENTRY(glVertexAttribPointer,   TrapType::Void),

		//shaders

		FN_ENTRY(glAttachShader,       TrapType::Void),
		FN_ENTRY(glCompileShader,      TrapType::Void),
		FN_ENTRY(glCreateProgram,      TrapType::Int),
		FN_ENTRY(glCreateShader,       TrapType::Int),
		FN_ENTRY(glDeleteShader,       TrapType::Void),
		FN_ENTRY(glDeleteProgram,      TrapType::Void),
		FN_ENTRY(glDetachShader,       TrapType::Void),
		FN_ENTRY(glGetActiveAttrib,    TrapType::Void),
		FN_ENTRY(glGetAttribLocation,  TrapType::Int),
		FN_ENTRY(glGetProgramiv,       TrapType::Void),
		FN_ENTRY(glGetProgramInfoLog,  TrapType::Void),
		FN_ENTRY(glGetShaderiv,        TrapType::Void),
		FN_ENTRY(glGetShaderInfoLog,   TrapType::Void),
		FN_ENTRY(glLinkProgram,        TrapType::Void),
		FN_ENTRY(glShaderSource,       TrapType::Void),
		FN_ENTRY(glUseProgram,         TrapType::Void),
		FN_ENTRY(glValidateProgram,    TrapType::Void),
		FN_ENTRY(glIsProgram,          TrapType::Int),

		//uniforms

		FN_ENTRY(glGetUniformLocation, TrapType::Int),
		FN_ENTRY(glUniform1f,          TrapType::Void),
		FN_ENTRY(glUniform1i,          TrapType::Void),
		FN_ENTRY(glUniform2f,          TrapType::Void),
		FN_ENTRY(glUniform2fv,         TrapType::Void),
		FN_ENTRY(glUniform3f,          TrapType::Void),
		FN_ENTRY(glUniform3fv,         TrapType::Void),
		FN_ENTRY(glUniform4f,          TrapType::Void),
		FN_ENTRY(glUniform4fv,         TrapType::Void),
		FN_ENTRY(glUniformMatrix2fv,   TrapType::Void),
		FN_ENTRY(glUniformMatrix3fv,   TrapType::Void),
		FN_ENTRY(glUniformMatrix4fv,   TrapType::Void),

		//textures

		FN_ENTRY(glBindTexture,     TrapType::Void),
		FN_ENTRY(glActiveTexture,   TrapType::Void),
		FN_ENTRY(glDeleteTextures,  TrapType::Void),
		FN_ENTRY(glGenerateMipmap,  TrapType::Void),
		FN_ENTRY(glGenTextures,     TrapType::Void),
		FN_ENTRY(glTexImage2D,      TrapType::Void),
		FN_ENTRY(glTexParameteri,   TrapType::Void),
		FN_ENTRY(glTexSubImage2D,   TrapType::Void),

		//framebuffers and renderbuffers

		FN_ENTRY(glBindRenderbuffer,        TrapType::Void),
		FN_ENTRY(glBindFramebuffer,         TrapType::Void),
		FN_ENTRY_CAST(glCheckFramebufferStatus, TrapType::Enum),
		FN_ENTRY(glFramebufferRenderbuffer, TrapType::Void),
		FN_ENTRY(glFramebufferTexture2D,    TrapType::Void),
		FN_ENTRY(glGenRenderbuffers,        TrapType::Void),
		FN_ENTRY(glGenFramebuffers,         TrapType::Void),
		FN_ENTRY(glRenderbufferStorage,     TrapType::Void),

		//frame and render state

		FN_ENTRY(glClear,          TrapType::Void),
		FN_ENTRY(glClearColor,     TrapType::Void),
		FN_ENTRY(glDisable,        TrapType::Void),
		FN_ENTRY_CAST(glGetError,  TrapType::Enum),
		FN_ENTRY(glGetIntegerv,    TrapType::Void),
		FN_ENTRY_CAST(glGetString, TrapType::BytePtr),
		FN_ENTRY(glViewport,       TrapType::Void),

#ifdef _WIN32 //WGL extensions (Windows only)
		FN_ENTRY(wglCreateContextAttribsARB, TrapType::Pointer),
		FN_ENTRY(wglChoosePixelFormatARB,    TrapType::Int),
		FN_ENTRY(wglSwapIntervalEXT,         TrapType::Int)
#elif __linux__

#endif
	};

	void OpenGLCore::InitializeAllFunctions()
	{
		if (!module)
		{
			module = ToVar<HMODULE>(Window::GetOpenGLLib());

			if (!module)
			{
				KalaWindowCore::ForceClose(
					"OpenGL Core Error",
					"Failed to get module 'opengl32.dll' because it was invalid!");
			}
		}

		for (const auto& entry : functionTable)
		{
			if (string(entry.name).find("wgl") != string::npos) continue;

			void* ptr = getProc(entry.name);
			*entry.pointer = ptr
				? ptr
				: GetTrapForType(entry.trapType, entry.name);

			if (ptr == nullptr)
			{
				Logger::Print(
					string("Function '") + entry.name + "' is not available!",
					"OPENGL_CORE",
					LogType::LOG_ERROR);
				continue;
			}

			functionRegistry[entry.name] = ptr;

			string formatStr = format(
				"{:#x}",
				reinterpret_cast<uintptr_t>(ptr));

			Logger::Print(
				string("Initialized function '") + entry.name + "' with hex value '" + formatStr + "'!",
				"OPENGL_CORE",
				LogType::LOG_DEBUG);
		}
	}

	void OpenGLCore::InitializeFunction(const char* name)
	{
		if (!module)
		{
			module = ToVar<HMODULE>(Window::GetOpenGLLib());

			if (!module)
			{
				KalaWindowCore::ForceClose(
					"OpenGL Core Error",
					"Failed to get module 'opengl32.dll' because it was invalid!");
			}
		}

		for (const auto& entry : functionTable)
		{
			if (strcmp(entry.name, name) == 0)
			{
				void* ptr = getProc(name);
				*entry.pointer = ptr
					? ptr
					: GetTrapForType(entry.trapType, entry.name);

				if (ptr == nullptr)
				{
					Logger::Print(
						string("Function '") + entry.name + "' is not available!",
						"OPENGL_CORE",
						LogType::LOG_WARNING);
					break;
				}

				functionRegistry[name] = ptr;

				string formatStr = format(
					"{:#x}",
					reinterpret_cast<uintptr_t>(ptr));

				Logger::Print(
					string("Initialized function '") + entry.name + "' with hex value '" + formatStr + "'!",
					"OPENGL_CORE",
					LogType::LOG_DEBUG);

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
	void* OpenGLCore::GetGLProcAddress(const char* name)
	{
		void* p = reinterpret_cast<void*>(wglGetProcAddress(name));

		auto IsBadFunction = [](void* p) 
		{
		return 
			p == nullptr
			|| p == reinterpret_cast<void*>(1)
			|| p == reinterpret_cast<void*>(2)
			|| p == reinterpret_cast<void*>(3)
			|| p == reinterpret_cast<void*>(-1);
		};
		if (!IsBadFunction(p))
		{
			return p;
		}

		Logger::Print(
			"Function " + string(name) + " was nullptr, trying to look for it from opengl32.dll",
			"OPENGL_CORE",
			LogType::LOG_WARNING);

		p = reinterpret_cast<void*>(GetProcAddress(module, name));
		if (!IsBadFunction(p)) return p;

		string title = "OpenGL Core Error";
		string reason = "Failed to find function '" + string(name) + "'!";

		KalaWindowCore::ForceClose(title, reason);
		return nullptr;
	}
#elif __linux__
	void* OpenGLCore::GetGLProcAddress(const char* name)
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

	static const void* FunctionNotLoadedPointerTrap()
	{
		string name = string(lastFailedFunctionName);

		Logger::Print(
			"OpenGL const pointer-returning function '" + name + "' was not loaded.",
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
		case TrapType::Pointer: return reinterpret_cast<void*>(&FunctionNotLoadedPointerTrap);
		}
		return nullptr;
	}
}