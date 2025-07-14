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
		//geometry

		FN_ENTRY(kglBindBuffer,            TrapType::Void),
		FN_ENTRY(kglBindVertexArray,       TrapType::Void),
		FN_ENTRY(kglBufferData,            TrapType::Void),
		FN_ENTRY(kglDeleteBuffers,         TrapType::Void),
		FN_ENTRY(kglDeleteVertexArrays,    TrapType::Void),
		FN_ENTRY(kglDrawArrays,            TrapType::Void),
		FN_ENTRY(kglDrawElements,          TrapType::Void),
		FN_ENTRY(kglEnableVertexAttribArray, TrapType::Void),
		FN_ENTRY(kglGenBuffers,            TrapType::Void),
		FN_ENTRY(kglGenVertexArrays,       TrapType::Void),
		FN_ENTRY(kglGetVertexAttribiv,     TrapType::Void),
		FN_ENTRY(kglGetVertexAttribPointerv, TrapType::Void),
		FN_ENTRY(kglVertexAttribPointer,   TrapType::Void),

		//shaders

		FN_ENTRY(kglAttachShader,       TrapType::Void),
		FN_ENTRY(kglCompileShader,      TrapType::Void),
		FN_ENTRY(kglCreateProgram,      TrapType::Int),
		FN_ENTRY(kglCreateShader,       TrapType::Int),
		FN_ENTRY(kglDeleteShader,       TrapType::Void),
		FN_ENTRY(kglDeleteProgram,      TrapType::Void),
		FN_ENTRY(kglDetachShader,       TrapType::Void),
		FN_ENTRY(kglGetActiveAttrib,    TrapType::Void),
		FN_ENTRY(kglGetAttribLocation,  TrapType::Int),
		FN_ENTRY(kglGetProgramiv,       TrapType::Void),
		FN_ENTRY(kglGetProgramInfoLog,  TrapType::Void),
		FN_ENTRY(kglGetShaderiv,        TrapType::Void),
		FN_ENTRY(kglGetShaderInfoLog,   TrapType::Void),
		FN_ENTRY(kglLinkProgram,        TrapType::Void),
		FN_ENTRY(kglShaderSource,       TrapType::Void),
		FN_ENTRY(kglUseProgram,         TrapType::Void),
		FN_ENTRY(kglValidateProgram,    TrapType::Void),
		FN_ENTRY(kglIsProgram,          TrapType::Int),

		//uniforms

		FN_ENTRY(kglGetUniformLocation, TrapType::Int),
		FN_ENTRY(kglUniform1f,          TrapType::Void),
		FN_ENTRY(kglUniform1i,          TrapType::Void),
		FN_ENTRY(kglUniform2f,          TrapType::Void),
		FN_ENTRY(kglUniform2fv,         TrapType::Void),
		FN_ENTRY(kglUniform3f,          TrapType::Void),
		FN_ENTRY(kglUniform3fv,         TrapType::Void),
		FN_ENTRY(kglUniform4f,          TrapType::Void),
		FN_ENTRY(kglUniform4fv,         TrapType::Void),
		FN_ENTRY(kglUniformMatrix2fv,   TrapType::Void),
		FN_ENTRY(kglUniformMatrix3fv,   TrapType::Void),
		FN_ENTRY(kglUniformMatrix4fv,   TrapType::Void),

		//textures

		FN_ENTRY(kglBindTexture,     TrapType::Void),
		FN_ENTRY(kglActiveTexture,   TrapType::Void),
		FN_ENTRY(kglDeleteTextures,  TrapType::Void),
		FN_ENTRY(kglGenerateMipmap,  TrapType::Void),
		FN_ENTRY(kglGenTextures,     TrapType::Void),
		FN_ENTRY(kglTexImage2D,      TrapType::Void),
		FN_ENTRY(kglTexParameteri,   TrapType::Void),
		FN_ENTRY(kglTexSubImage2D,   TrapType::Void),

		//framebuffers and renderbuffers

		FN_ENTRY(kglBindRenderbuffer,        TrapType::Void),
		FN_ENTRY(kglBindFramebuffer,         TrapType::Void),
		FN_ENTRY_CAST(kglCheckFramebufferStatus, TrapType::Enum),
		FN_ENTRY(kglFramebufferRenderbuffer, TrapType::Void),
		FN_ENTRY(kglFramebufferTexture2D,    TrapType::Void),
		FN_ENTRY(kglGenRenderbuffers,        TrapType::Void),
		FN_ENTRY(kglGenFramebuffers,         TrapType::Void),
		FN_ENTRY(kglRenderbufferStorage,     TrapType::Void),

		//frame and render state

		FN_ENTRY(kglClear,          TrapType::Void),
		FN_ENTRY(kglClearColor,     TrapType::Void),
		FN_ENTRY(kglDisable,        TrapType::Void),
		FN_ENTRY_CAST(kglGetError,  TrapType::Enum),
		FN_ENTRY(kglGetIntegerv,    TrapType::Void),
		FN_ENTRY_CAST(kglGetString, TrapType::BytePtr),
		FN_ENTRY(kglViewport,       TrapType::Void),

#ifdef _WIN32 //WGL extensions (Windows only)
		FN_ENTRY(wglCreateContextAttribsARB, TrapType::Pointer),
		FN_ENTRY(wglChoosePixelFormatARB,    TrapType::Int),
		FN_ENTRY(wglSwapIntervalEXT,         TrapType::Int)
#elif __linux__

#endif
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
	void* OpenGLCore::GetGLProcAddress(const char* name)
	{
		return reinterpret_cast<void*>(wglGetProcAddress(name));
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

#endif //KALAWINDOW_SUPPORT_OPENGL