//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

//main log macro
#define WRITE_LOG(type, msg) std::cout << "[KALAKIT_OPENGL_LOADER | " << type << "] " << msg << "\n"

//log types
#if KALAWINDOW_DEBUG
	#define LOG_DEBUG(msg) WRITE_LOG("DEBUG", msg)
#else
	#define LOG_DEBUG(msg)
#endif
#define LOG_SUCCESS(msg) WRITE_LOG("SUCCESS", msg)
#define LOG_ERROR(msg) WRITE_LOG("ERROR", msg)

#include <iostream>

#include "opengl_loader.hpp"
#include "opengl_typedefs.hpp"

using std::find;

namespace KalaKit
{
	//geometry

	inline PFNGLGENVERTEXARRAYSPROC        OpenGLLoader::glGenVertexArraysPtr = nullptr;
	inline PFNGLBINDVERTEXARRAYPROC        OpenGLLoader::glBindVertexArrayPtr = nullptr;
	inline PFNGLGENBUFFERSPROC             OpenGLLoader::glGenBuffersPtr = nullptr;
	inline PFNGLBINDBUFFERPROC             OpenGLLoader::glBindBufferPtr = nullptr;
	inline PFNGLBUFFERDATAPROC             OpenGLLoader::glBufferDataPtr = nullptr;
	inline PFNGLENABLEVERTEXATTRIBARRAYPROC OpenGLLoader::glEnableVertexAttribArrayPtr = nullptr;
	inline PFNGLVERTEXATTRIBPOINTERPROC    OpenGLLoader::glVertexAttribPointerPtr = nullptr;
	inline PFNGLGETVERTEXATTRIBIVPROC      OpenGLLoader::glGetVertexAttribivPtr = nullptr;
	inline PFNGLGETVERTEXATTRIBPOINTERVPROC OpenGLLoader::glGetVertexAttribPointervPtr = nullptr;
	inline PFNGLDRAWARRAYSPROC             OpenGLLoader::glDrawArraysPtr = nullptr;
	inline PFNGLDRAWELEMENTSPROC           OpenGLLoader::glDrawElementsPtr = nullptr;

	//shaders

	inline PFNGLCREATESHADERPROC           OpenGLLoader::glCreateShaderPtr = nullptr;
	inline PFNGLSHADERSOURCEPROC           OpenGLLoader::glShaderSourcePtr = nullptr;
	inline PFNGLCOMPILESHADERPROC          OpenGLLoader::glCompileShaderPtr = nullptr;
	inline PFNGLCREATEPROGRAMPROC          OpenGLLoader::glCreateProgramPtr = nullptr;
	inline PFNGLUSEPROGRAMPROC             OpenGLLoader::glUseProgramPtr = nullptr;
	inline PFNGLATTACHSHADERPROC           OpenGLLoader::glAttachShaderPtr = nullptr;
	inline PFNGLLINKPROGRAMPROC            OpenGLLoader::glLinkProgramPtr = nullptr;
	inline PFNGLDELETESHADERPROC           OpenGLLoader::glDeleteShaderPtr = nullptr;
	inline PFNGLGETSHADERIVPROC            OpenGLLoader::glGetShaderivPtr = nullptr;
	inline PFNGLGETSHADERINFOLOGPROC       OpenGLLoader::glGetShaderInfoLogPtr = nullptr;
	inline PFNGLGETPROGRAMIVPROC           OpenGLLoader::glGetProgramivPtr = nullptr;
	inline PFNGLGETPROGRAMINFOLOGPROC      OpenGLLoader::glGetProgramInfoLogPtr = nullptr;
	inline PFNGLGETACTIVEATTRIBPROC        OpenGLLoader::glGetActiveAttribPtr = nullptr;
	inline PFNGLGETATTRIBLOCATIONPROC      OpenGLLoader::glGetAttribLocationPtr = nullptr;
	inline PFNGLDELETEPROGRAMPROC          OpenGLLoader::glDeleteProgramPtr = nullptr;
	inline PFNGLVALIDATEPROGRAMPROC        OpenGLLoader::glValidateProgramPtr = nullptr;
	inline PFNGLISPROGRAMPROC              OpenGLLoader::glIsProgramPtr = nullptr;

	//uniforms

	inline PFNGLGETUNIFORMLOCATIONPROC     OpenGLLoader::glGetUniformLocationPtr = nullptr;
	inline PFNGLUNIFORM1IPROC              OpenGLLoader::glUniform1iPtr = nullptr;
	inline PFNGLUNIFORM1FPROC              OpenGLLoader::glUniform1fPtr = nullptr;
	inline PFNGLUNIFORM2FPROC              OpenGLLoader::glUniform2fPtr = nullptr;
	inline PFNGLUNIFORM2FVPROC             OpenGLLoader::glUniform2fvPtr = nullptr;
	inline PFNGLUNIFORM3FPROC              OpenGLLoader::glUniform3fPtr = nullptr;
	inline PFNGLUNIFORM3FVPROC             OpenGLLoader::glUniform3fvPtr = nullptr;
	inline PFNGLUNIFORM4FPROC              OpenGLLoader::glUniform4fPtr = nullptr;
	inline PFNGLUNIFORM4FVPROC             OpenGLLoader::glUniform4fvPtr = nullptr;
	inline PFNGLUNIFORMMATRIX2FVPROC       OpenGLLoader::glUniformMatrix2fvPtr = nullptr;
	inline PFNGLUNIFORMMATRIX3FVPROC       OpenGLLoader::glUniformMatrix3fvPtr = nullptr;
	inline PFNGLUNIFORMMATRIX4FVPROC       OpenGLLoader::glUniformMatrix4fvPtr = nullptr;

	//textures

	inline PFNGLGENTEXTURESPROC            OpenGLLoader::glGenTexturesPtr = nullptr;
	inline PFNGLBINDTEXTUREPROC            OpenGLLoader::glBindTexturePtr = nullptr;
	inline PFNGLTEXIMAGE2DPROC             OpenGLLoader::glTexImage2DPtr = nullptr;
	inline PFNGLTEXPARAMETERIPROC          OpenGLLoader::glTexParameteriPtr = nullptr;
	inline PFNGLGENERATEMIPMAPPROC         OpenGLLoader::glGenerateMipmapPtr = nullptr;

	//frame and render state

	inline PFNGLVIEWPORTPROC               OpenGLLoader::glViewportPtr = nullptr;
	inline PFNGLDISABLEPROC                OpenGLLoader::glDisablePtr = nullptr;
	inline PFNGLCLEARCOLORPROC             OpenGLLoader::glClearColorPtr = nullptr;
	inline PFNGLCLEARPROC                  OpenGLLoader::glClearPtr = nullptr;
	inline PFNGLGETINTEGERVPROC            OpenGLLoader::glGetIntegervPtr = nullptr;
	inline PFNGLGETSTRINGPROC              OpenGLLoader::glGetStringPtr = nullptr;
	inline PFNGLGETERRORPROC               OpenGLLoader::glGetErrorPtr = nullptr;

	bool OpenGLLoader::IsFunctionAvailable(OpenGLFunction id)
	{
		for (const auto& entry : openGLFunctionTable)
		{
			if (entry.id == id)
			{
				return *entry.target != nullptr;
			}
		}

		return false;
	}

	void OpenGLLoader::LoadAllFunctions()
	{
		for (const auto& entry : openGLFunctionTable)
		{
			*entry.target = LoadOpenGLFunction<void*>(entry.name);
		}

		LOG_SUCCESS("Loaded all OpenGL functions!");
	}

	template <typename T>
	T OpenGLLoader::LoadOpenGLFunction(const char* name)
	{
		T func = reinterpret_cast<T>(wglGetProcAddress(name));

		//fall back to opengl32.dll for 1.1 core functions
		if (!func)
		{
			HMODULE openglModule = GetModuleHandleA("opengl32.dll");
			if (openglModule)
			{
				func = reinterpret_cast<T>(GetProcAddress(openglModule, name));
			}
		}

		if (!func)
		{
			LOG_ERROR("Failed to load OpenGL function: " << name);
		}
		return func;
	}

	const vector<OpenGLLoader::OpenGLFunctionEntry> OpenGLLoader::openGLFunctionTable =
	{
		//geometry

		{ OpenGLFunction::OPENGL_GENVERTEXARRAYS, "glGenVertexArrays", reinterpret_cast<void**>(&glGenVertexArraysPtr) },
		{ OpenGLFunction::OPENGL_BINDVERTEXARRAY, "glBindVertexArray", reinterpret_cast<void**>(&glBindVertexArrayPtr) },
		{ OpenGLFunction::OPENGL_GENBUFFERS, "glGenBuffers", reinterpret_cast<void**>(&glGenBuffersPtr) },
		{ OpenGLFunction::OPENGL_BINDBUFFER, "glBindBuffer", reinterpret_cast<void**>(&glBindBufferPtr) },
		{ OpenGLFunction::OPENGL_BUFFERDATA, "glBufferData", reinterpret_cast<void**>(&glBufferDataPtr) },
		{ OpenGLFunction::OPENGL_ENABLEVERTEXATTRIBARRAY, "glEnableVertexAttribArray", reinterpret_cast<void**>(&glEnableVertexAttribArrayPtr) },
		{ OpenGLFunction::OPENGL_VERTEXATTRIBPOINTER, "glVertexAttribPointer", reinterpret_cast<void**>(&glVertexAttribPointerPtr) },
		{ OpenGLFunction::OPENGL_GETVERTEXATTRIBIV, "glGetVertexAttribiv", reinterpret_cast<void**>(&glGetVertexAttribivPtr) },
		{ OpenGLFunction::OPENGL_GETVERTEXATTRIBPOINTERV, "glGetVertexAttribPointerv", reinterpret_cast<void**>(&glGetVertexAttribPointervPtr) },
		{ OpenGLFunction::OPENGL_DRAWARRAYS, "glDrawArrays", reinterpret_cast<void**>(&glDrawArraysPtr) },
		{ OpenGLFunction::OPENGL_DRAWELEMENTS, "glDrawElements", reinterpret_cast<void**>(&glDrawElementsPtr) },

		//shaders

		{ OpenGLFunction::OPENGL_CREATESHADER, "glCreateShader", reinterpret_cast<void**>(&glCreateShaderPtr) },
		{ OpenGLFunction::OPENGL_SHADERSOURCE, "glShaderSource", reinterpret_cast<void**>(&glShaderSourcePtr) },
		{ OpenGLFunction::OPENGL_COMPILESHADER, "glCompileShader", reinterpret_cast<void**>(&glCompileShaderPtr) },
		{ OpenGLFunction::OPENGL_CREATEPROGRAM, "glCreateProgram", reinterpret_cast<void**>(&glCreateProgramPtr) },
		{ OpenGLFunction::OPENGL_USEPROGRAM, "glUseProgram", reinterpret_cast<void**>(&glUseProgramPtr) },
		{ OpenGLFunction::OPENGL_ATTACHSHADER, "glAttachShader", reinterpret_cast<void**>(&glAttachShaderPtr) },
		{ OpenGLFunction::OPENGL_LINKPROGRAM, "glLinkProgram", reinterpret_cast<void**>(&glLinkProgramPtr) },
		{ OpenGLFunction::OPENGL_DELETESHADER, "glDeleteShader", reinterpret_cast<void**>(&glDeleteShaderPtr) },
		{ OpenGLFunction::OPENGL_GETSHADERIV, "glGetShaderiv", reinterpret_cast<void**>(&glGetShaderivPtr) },
		{ OpenGLFunction::OPENGL_GETSHADERINFOLOG, "glGetShaderInfoLog", reinterpret_cast<void**>(&glGetShaderInfoLogPtr) },
		{ OpenGLFunction::OPENGL_GETPROGRAMIV, "glGetProgramiv", reinterpret_cast<void**>(&glGetProgramivPtr) },
		{ OpenGLFunction::OPENGL_GETPROGRAMINFOLOG, "glGetProgramInfoLog", reinterpret_cast<void**>(&glGetProgramInfoLogPtr) },
		{ OpenGLFunction::OPENGL_GETACTIVEATTRIB, "glGetActiveAttrib", reinterpret_cast<void**>(&glGetActiveAttribPtr) },
		{ OpenGLFunction::OPENGL_GETATTRIBLOCATION, "glGetAttribLocation", reinterpret_cast<void**>(&glGetAttribLocationPtr) },
		{ OpenGLFunction::OPENGL_DELETEPROGRAM, "glDeleteProgram", reinterpret_cast<void**>(&glDeleteProgramPtr) },
		{ OpenGLFunction::OPENGL_VALIDATEPROGRAM, "glValidateProgram", reinterpret_cast<void**>(&glValidateProgramPtr) },
		{ OpenGLFunction::OPENGL_ISPROGRAM, "glIsProgram", reinterpret_cast<void**>(&glIsProgramPtr) },

		//uniforms

		{ OpenGLFunction::OPENGL_GETUNIFORMLOCATION, "glGetUniformLocation", reinterpret_cast<void**>(&glGetUniformLocationPtr) },
		{ OpenGLFunction::OPENGL_UNIFORM1I, "glUniform1i", reinterpret_cast<void**>(&glUniform1iPtr) },
		{ OpenGLFunction::OPENGL_UNIFORM1F, "glUniform1f", reinterpret_cast<void**>(&glUniform1fPtr) },
		{ OpenGLFunction::OPENGL_UNIFORM2F, "glUniform2f", reinterpret_cast<void**>(&glUniform2fPtr) },
		{ OpenGLFunction::OPENGL_UNIFORM2FV, "glUniform2fv", reinterpret_cast<void**>(&glUniform2fvPtr) },
		{ OpenGLFunction::OPENGL_UNIFORM3F, "glUniform3f", reinterpret_cast<void**>(&glUniform3fPtr) },
		{ OpenGLFunction::OPENGL_UNIFORM3FV, "glUniform3fv", reinterpret_cast<void**>(&glUniform3fvPtr) },
		{ OpenGLFunction::OPENGL_UNIFORM4F, "glUniform4f", reinterpret_cast<void**>(&glUniform4fPtr) },
		{ OpenGLFunction::OPENGL_UNIFORM4FV, "glUniform4fv", reinterpret_cast<void**>(&glUniform4fvPtr) },
		{ OpenGLFunction::OPENGL_UNIFORMMATRIX2FV, "glUniformMatrix2fv", reinterpret_cast<void**>(&glUniformMatrix2fvPtr) },
		{ OpenGLFunction::OPENGL_UNIFORMMATRIX3FV, "glUniformMatrix3fv", reinterpret_cast<void**>(&glUniformMatrix3fvPtr) },
		{ OpenGLFunction::OPENGL_UNIFORMMATRIX4FV, "glUniformMatrix4fv", reinterpret_cast<void**>(&glUniformMatrix4fvPtr) },

		//textures

		{ OpenGLFunction::OPENGL_GENTEXTURES, "glGenTextures", reinterpret_cast<void**>(&glGenTexturesPtr) },
		{ OpenGLFunction::OPENGL_BINDTEXTURE, "glBindTexture", reinterpret_cast<void**>(&glBindTexturePtr) },
		{ OpenGLFunction::OPENGL_TEXIMAGE2D, "glTexImage2D", reinterpret_cast<void**>(&glTexImage2DPtr) },
		{ OpenGLFunction::OPENGL_TEXPARAMETERI, "glTexParameteri", reinterpret_cast<void**>(&glTexParameteriPtr) },
		{ OpenGLFunction::OPENGL_GENERATEMIPMAP, "glGenerateMipmap", reinterpret_cast<void**>(&glGenerateMipmapPtr) },
	
		//frame and render state
		
		{ OpenGLFunction::OPENGL_VIEWPORT, "glViewport", reinterpret_cast<void**>(&glViewportPtr) },
		{ OpenGLFunction::OPENGL_DISABLE, "glDisable", reinterpret_cast<void**>(&glDisablePtr) },
		{ OpenGLFunction::OPENGL_CLEARCOLOR, "glClearColor", reinterpret_cast<void**>(&glClearColorPtr) },
		{ OpenGLFunction::OPENGL_CLEAR, "glClear", reinterpret_cast<void**>(&glClearPtr) },
		{ OpenGLFunction::OPENGL_GETINTEGERV, "glGetIntegerv", reinterpret_cast<void**>(&glGetIntegervPtr) },
		{ OpenGLFunction::OPENGL_GETSTRING, "glGetString", reinterpret_cast<void**>(&glGetStringPtr) },
		{ OpenGLFunction::OPENGL_GETERROR, "glGetError", reinterpret_cast<void**>(&glGetErrorPtr) }
	};
}