//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#define KALAKIT_MODULE "OPENGL_LOADER"

//kalawindow
#include "opengl_loader.hpp"
#include "opengl_typedefs.hpp"

#ifdef KALAKIT_WINDOWS
#include "GL/gl.h"
#elif KALAKIT_X11
#include <GL/glx.h>
#include <EGL/egl.h>
#endif

using std::find;

namespace KalaKit
{
	//geometry

	inline PFNGLGENVERTEXARRAYSPROC        OpenGLLoader::glGenVertexArrays = nullptr;
	inline PFNGLBINDVERTEXARRAYPROC        OpenGLLoader::glBindVertexArray = nullptr;
	inline PFNGLGENBUFFERSPROC             OpenGLLoader::glGenBuffers = nullptr;
	inline PFNGLBINDBUFFERPROC             OpenGLLoader::glBindBuffer = nullptr;
	inline PFNGLDELETEVERTEXARRAYSPROC     OpenGLLoader::glDeleteVertexArrays = nullptr;
	inline PFNGLDELETEBUFFERSPROC          OpenGLLoader::glDeleteBuffers = nullptr;
	inline PFNGLBUFFERDATAPROC             OpenGLLoader::glBufferData = nullptr;
	inline PFNGLENABLEVERTEXATTRIBARRAYPROC OpenGLLoader::glEnableVertexAttribArray = nullptr;
	inline PFNGLVERTEXATTRIBPOINTERPROC    OpenGLLoader::glVertexAttribPointer = nullptr;
	inline PFNGLGETVERTEXATTRIBIVPROC      OpenGLLoader::glGetVertexAttribiv = nullptr;
	inline PFNGLGETVERTEXATTRIBPOINTERVPROC OpenGLLoader::glGetVertexAttribPointerv = nullptr;
	inline PFNGLDRAWARRAYSPROC             OpenGLLoader::glDrawArrays = nullptr;
	inline PFNGLDRAWELEMENTSPROC           OpenGLLoader::glDrawElements = nullptr;

	//shaders

	inline PFNGLCREATESHADERPROC           OpenGLLoader::glCreateShader = nullptr;
	inline PFNGLSHADERSOURCEPROC           OpenGLLoader::glShaderSource = nullptr;
	inline PFNGLCOMPILESHADERPROC          OpenGLLoader::glCompileShader = nullptr;
	inline PFNGLCREATEPROGRAMPROC          OpenGLLoader::glCreateProgram = nullptr;
	inline PFNGLUSEPROGRAMPROC             OpenGLLoader::glUseProgram = nullptr;
	inline PFNGLATTACHSHADERPROC           OpenGLLoader::glAttachShader = nullptr;
	inline PFNGLLINKPROGRAMPROC            OpenGLLoader::glLinkProgram = nullptr;
	inline PFNGLDELETESHADERPROC           OpenGLLoader::glDeleteShader = nullptr;
	inline PFNGLGETSHADERIVPROC            OpenGLLoader::glGetShaderiv = nullptr;
	inline PFNGLGETSHADERINFOLOGPROC       OpenGLLoader::glGetShaderInfoLog = nullptr;
	inline PFNGLGETPROGRAMIVPROC           OpenGLLoader::glGetProgramiv = nullptr;
	inline PFNGLGETPROGRAMINFOLOGPROC      OpenGLLoader::glGetProgramInfoLog = nullptr;
	inline PFNGLGETACTIVEATTRIBPROC        OpenGLLoader::glGetActiveAttrib = nullptr;
	inline PFNGLGETATTRIBLOCATIONPROC      OpenGLLoader::glGetAttribLocation = nullptr;
	inline PFNGLDELETEPROGRAMPROC          OpenGLLoader::glDeleteProgram = nullptr;
	inline PFNGLVALIDATEPROGRAMPROC        OpenGLLoader::glValidateProgram = nullptr;
	inline PFNGLISPROGRAMPROC              OpenGLLoader::glIsProgram = nullptr;

	//uniforms

	inline PFNGLGETUNIFORMLOCATIONPROC     OpenGLLoader::glGetUniformLocation = nullptr;
	inline PFNGLUNIFORM1IPROC              OpenGLLoader::glUniform1i = nullptr;
	inline PFNGLUNIFORM1FPROC              OpenGLLoader::glUniform1f = nullptr;
	inline PFNGLUNIFORM2FPROC              OpenGLLoader::glUniform2f = nullptr;
	inline PFNGLUNIFORM2FVPROC             OpenGLLoader::glUniform2fv = nullptr;
	inline PFNGLUNIFORM3FPROC              OpenGLLoader::glUniform3f = nullptr;
	inline PFNGLUNIFORM3FVPROC             OpenGLLoader::glUniform3fv = nullptr;
	inline PFNGLUNIFORM4FPROC              OpenGLLoader::glUniform4f = nullptr;
	inline PFNGLUNIFORM4FVPROC             OpenGLLoader::glUniform4fv = nullptr;
	inline PFNGLUNIFORMMATRIX2FVPROC       OpenGLLoader::glUniformMatrix2fv = nullptr;
	inline PFNGLUNIFORMMATRIX3FVPROC       OpenGLLoader::glUniformMatrix3fv = nullptr;
	inline PFNGLUNIFORMMATRIX4FVPROC       OpenGLLoader::glUniformMatrix4fv = nullptr;

	//textures

	inline PFNGLGENTEXTURESPROC            OpenGLLoader::glGenTextures = nullptr;
	inline PFNGLBINDTEXTUREPROC            OpenGLLoader::glBindTexture = nullptr;
	inline PFNGLACTIVETEXTUREPROC          OpenGLLoader::glActiveTexture = nullptr;
	inline PFNGLTEXIMAGE2DPROC             OpenGLLoader::glTexImage2D = nullptr;
	inline PFNGLTEXSUBIMAGE2DPROC          OpenGLLoader::glTexSubImage2D = nullptr;
	inline PFNGLTEXPARAMETERIPROC          OpenGLLoader::glTexParameteri = nullptr;
	inline PFNGLGENERATEMIPMAPPROC         OpenGLLoader::glGenerateMipmap = nullptr;
	inline PFNGLDELETETEXTURESPROC         OpenGLLoader::glDeleteTextures = nullptr;

	//framebuffers and renderbuffers

	inline PFNGLGENFRAMEBUFFERSPROC        OpenGLLoader::glGenFramebuffers = nullptr;
	inline PFNGLBINDFRAMEBUFFERPROC        OpenGLLoader::glBindFramebuffer = nullptr;
	inline PFNGLFRAMEBUFFERTEXTURE2DPROC   OpenGLLoader::glFramebufferTexture2D = nullptr;
	inline PFNGLCHECKFRAMEBUFFERSTATUSPROC OpenGLLoader::glCheckFramebufferStatus = nullptr;
	inline PFNGLGENRENDERBUFFERSPROC       OpenGLLoader::glGenRenderbuffers = nullptr;
	inline PFNGLBINDRENDERBUFFERPROC       OpenGLLoader::glBindRenderbuffer = nullptr;
	inline PFNGLRENDERBUFFERSTORAGEPROC    OpenGLLoader::glRenderbufferStorage = nullptr;
	inline PFNGLFRAMEBUFFERRENDERBUFFERPROC OpenGLLoader::glFramebufferRenderbuffer = nullptr;

	//frame and render state

	inline PFNGLVIEWPORTPROC               OpenGLLoader::glViewport = nullptr;
	inline PFNGLDISABLEPROC                OpenGLLoader::glDisable = nullptr;
	inline PFNGLCLEARCOLORPROC             OpenGLLoader::glClearColor = nullptr;
	inline PFNGLCLEARPROC                  OpenGLLoader::glClear = nullptr;
	inline PFNGLGETINTEGERVPROC            OpenGLLoader::glGetIntegerv = nullptr;
	inline PFNGLGETSTRINGPROC              OpenGLLoader::glGetString = nullptr;
	inline PFNGLGETERRORPROC               OpenGLLoader::glGetError = nullptr;

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
		T func = nullptr;

#ifdef KALAKIT_WINDOWS
		func = reinterpret_cast<T>(wglGetProcAddress(name));

		//fall back to opengl32.dll for 1.1 core functions
		if (!func)
		{
			HMODULE openglModule = GetModuleHandleA("opengl32.dll");
			if (openglModule)
			{
				func = reinterpret_cast<T>(GetProcAddress(openglModule, name));
			}
		}

#elif KALAKIT_X11
		func = reinterpret_cast<T>(glXGetProcAddressARB(
			reinterpret_cast<const GLubyte*>(name))
		);
#endif

		if (!func)
		{
			LOG_ERROR("Failed to load OpenGL function: " << name);
		}
		return func;
	}

	const vector<OpenGLLoader::OpenGLFunctionEntry> OpenGLLoader::openGLFunctionTable =
	{
		//geometry

		{ OpenGLFunction::OPENGL_GENVERTEXARRAYS, "glGenVertexArrays", reinterpret_cast<void**>(&glGenVertexArrays) },
		{ OpenGLFunction::OPENGL_BINDVERTEXARRAY, "glBindVertexArray", reinterpret_cast<void**>(&glBindVertexArray) },
		{ OpenGLFunction::OPENGL_GENBUFFERS, "glGenBuffers", reinterpret_cast<void**>(&glGenBuffers) },
		{ OpenGLFunction::OPENGL_BINDBUFFER, "glBindBuffer", reinterpret_cast<void**>(&glBindBuffer) },
		{ OpenGLFunction::OPENGL_DELETEVERTEXARRAY, "glDeleteVertexArrays", reinterpret_cast<void**>(&glDeleteVertexArrays) },
		{ OpenGLFunction::OPENGL_DELETEBUFFER,      "glDeleteBuffers",      reinterpret_cast<void**>(&glDeleteBuffers) },
		{ OpenGLFunction::OPENGL_BUFFERDATA, "glBufferData", reinterpret_cast<void**>(&glBufferData) },
		{ OpenGLFunction::OPENGL_ENABLEVERTEXATTRIBARRAY, "glEnableVertexAttribArray", reinterpret_cast<void**>(&glEnableVertexAttribArray) },
		{ OpenGLFunction::OPENGL_VERTEXATTRIBPOINTER, "glVertexAttribPointer", reinterpret_cast<void**>(&glVertexAttribPointer) },
		{ OpenGLFunction::OPENGL_GETVERTEXATTRIBIV, "glGetVertexAttribiv", reinterpret_cast<void**>(&glGetVertexAttribiv) },
		{ OpenGLFunction::OPENGL_GETVERTEXATTRIBPOINTERV, "glGetVertexAttribPointerv", reinterpret_cast<void**>(&glGetVertexAttribPointerv) },
		{ OpenGLFunction::OPENGL_DRAWARRAYS, "glDrawArrays", reinterpret_cast<void**>(&glDrawArrays) },
		{ OpenGLFunction::OPENGL_DRAWELEMENTS, "glDrawElements", reinterpret_cast<void**>(&glDrawElements) },

		//shaders

		{ OpenGLFunction::OPENGL_CREATESHADER, "glCreateShader", reinterpret_cast<void**>(&glCreateShader) },
		{ OpenGLFunction::OPENGL_SHADERSOURCE, "glShaderSource", reinterpret_cast<void**>(&glShaderSource) },
		{ OpenGLFunction::OPENGL_COMPILESHADER, "glCompileShader", reinterpret_cast<void**>(&glCompileShader) },
		{ OpenGLFunction::OPENGL_CREATEPROGRAM, "glCreateProgram", reinterpret_cast<void**>(&glCreateProgram) },
		{ OpenGLFunction::OPENGL_USEPROGRAM, "glUseProgram", reinterpret_cast<void**>(&glUseProgram) },
		{ OpenGLFunction::OPENGL_ATTACHSHADER, "glAttachShader", reinterpret_cast<void**>(&glAttachShader) },
		{ OpenGLFunction::OPENGL_LINKPROGRAM, "glLinkProgram", reinterpret_cast<void**>(&glLinkProgram) },
		{ OpenGLFunction::OPENGL_DELETESHADER, "glDeleteShader", reinterpret_cast<void**>(&glDeleteShader) },
		{ OpenGLFunction::OPENGL_GETSHADERIV, "glGetShaderiv", reinterpret_cast<void**>(&glGetShaderiv) },
		{ OpenGLFunction::OPENGL_GETSHADERINFOLOG, "glGetShaderInfoLog", reinterpret_cast<void**>(&glGetShaderInfoLog) },
		{ OpenGLFunction::OPENGL_GETPROGRAMIV, "glGetProgramiv", reinterpret_cast<void**>(&glGetProgramiv) },
		{ OpenGLFunction::OPENGL_GETPROGRAMINFOLOG, "glGetProgramInfoLog", reinterpret_cast<void**>(&glGetProgramInfoLog) },
		{ OpenGLFunction::OPENGL_GETACTIVEATTRIB, "glGetActiveAttrib", reinterpret_cast<void**>(&glGetActiveAttrib) },
		{ OpenGLFunction::OPENGL_GETATTRIBLOCATION, "glGetAttribLocation", reinterpret_cast<void**>(&glGetAttribLocation) },
		{ OpenGLFunction::OPENGL_DELETEPROGRAM, "glDeleteProgram", reinterpret_cast<void**>(&glDeleteProgram) },
		{ OpenGLFunction::OPENGL_VALIDATEPROGRAM, "glValidateProgram", reinterpret_cast<void**>(&glValidateProgram) },
		{ OpenGLFunction::OPENGL_ISPROGRAM, "glIsProgram", reinterpret_cast<void**>(&glIsProgram) },

		//uniforms

		{ OpenGLFunction::OPENGL_GETUNIFORMLOCATION, "glGetUniformLocation", reinterpret_cast<void**>(&glGetUniformLocation) },
		{ OpenGLFunction::OPENGL_UNIFORM1I, "glUniform1i", reinterpret_cast<void**>(&glUniform1i) },
		{ OpenGLFunction::OPENGL_UNIFORM1F, "glUniform1f", reinterpret_cast<void**>(&glUniform1f) },
		{ OpenGLFunction::OPENGL_UNIFORM2F, "glUniform2f", reinterpret_cast<void**>(&glUniform2f) },
		{ OpenGLFunction::OPENGL_UNIFORM2FV, "glUniform2fv", reinterpret_cast<void**>(&glUniform2fv) },
		{ OpenGLFunction::OPENGL_UNIFORM3F, "glUniform3f", reinterpret_cast<void**>(&glUniform3f) },
		{ OpenGLFunction::OPENGL_UNIFORM3FV, "glUniform3fv", reinterpret_cast<void**>(&glUniform3fv) },
		{ OpenGLFunction::OPENGL_UNIFORM4F, "glUniform4f", reinterpret_cast<void**>(&glUniform4f) },
		{ OpenGLFunction::OPENGL_UNIFORM4FV, "glUniform4fv", reinterpret_cast<void**>(&glUniform4fv) },
		{ OpenGLFunction::OPENGL_UNIFORMMATRIX2FV, "glUniformMatrix2fv", reinterpret_cast<void**>(&glUniformMatrix2fv) },
		{ OpenGLFunction::OPENGL_UNIFORMMATRIX3FV, "glUniformMatrix3fv", reinterpret_cast<void**>(&glUniformMatrix3fv) },
		{ OpenGLFunction::OPENGL_UNIFORMMATRIX4FV, "glUniformMatrix4fv", reinterpret_cast<void**>(&glUniformMatrix4fv) },

		//textures

		{ OpenGLFunction::OPENGL_GENTEXTURES,     "glGenTextures",     reinterpret_cast<void**>(&glGenTextures) },
		{ OpenGLFunction::OPENGL_BINDTEXTURE,     "glBindTexture",     reinterpret_cast<void**>(&glBindTexture) },
		{ OpenGLFunction::OPENGL_ACTIVETEXTURE,   "glActiveTexture",   reinterpret_cast<void**>(&glActiveTexture) },
		{ OpenGLFunction::OPENGL_TEXIMAGE2D,      "glTexImage2D",      reinterpret_cast<void**>(&glTexImage2D) },
		{ OpenGLFunction::OPENGL_TEXSUBIMAGE2D,   "glTexSubImage2D",   reinterpret_cast<void**>(&glTexSubImage2D) },
		{ OpenGLFunction::OPENGL_TEXPARAMETERI,   "glTexParameteri",   reinterpret_cast<void**>(&glTexParameteri) },
		{ OpenGLFunction::OPENGL_GENERATEMIPMAP,  "glGenerateMipmap",  reinterpret_cast<void**>(&glGenerateMipmap) },
		{ OpenGLFunction::OPENGL_DELETETEXTURES,  "glDeleteTextures",  reinterpret_cast<void**>(&glDeleteTextures) },

		//framebuffers and renderbuffers

		{ OpenGLFunction::OPENGL_GENFRAMEBUFFERS, "glGenFramebuffers", reinterpret_cast<void**>(&glGenFramebuffers) },
		{ OpenGLFunction::OPENGL_BINDFRAMEBUFFER, "glBindFramebuffer", reinterpret_cast<void**>(&glBindFramebuffer) },
		{ OpenGLFunction::OPENGL_FRAMEBUFFERTEXTURE2D, "glFramebufferTexture2D", reinterpret_cast<void**>(&glFramebufferTexture2D) },
		{ OpenGLFunction::OPENGL_CHECKFRAMEBUFFERSTATUS, "glCheckFramebufferStatus", reinterpret_cast<void**>(&glCheckFramebufferStatus) },

		{ OpenGLFunction::OPENGL_GENRENDERBUFFERS, "glGenRenderbuffers", reinterpret_cast<void**>(&glGenRenderbuffers) },
		{ OpenGLFunction::OPENGL_BINDRENDERBUFFER, "glBindRenderbuffer", reinterpret_cast<void**>(&glBindRenderbuffer) },
		{ OpenGLFunction::OPENGL_RENDERBUFFERSTORAGE, "glRenderbufferStorage", reinterpret_cast<void**>(&glRenderbufferStorage) },
	{ OpenGLFunction::OPENGL_FRAMEBUFFERRENDERBUFFER, "glFramebufferRenderbuffer", reinterpret_cast<void**>(&glFramebufferRenderbuffer) },

	
		//frame and render state
		
		{ OpenGLFunction::OPENGL_VIEWPORT, "glViewport", reinterpret_cast<void**>(&glViewport) },
		{ OpenGLFunction::OPENGL_DISABLE, "glDisable", reinterpret_cast<void**>(&glDisable) },
		{ OpenGLFunction::OPENGL_CLEARCOLOR, "glClearColor", reinterpret_cast<void**>(&glClearColor) },
		{ OpenGLFunction::OPENGL_CLEAR, "glClear", reinterpret_cast<void**>(&glClear) },
		{ OpenGLFunction::OPENGL_GETINTEGERV, "glGetIntegerv", reinterpret_cast<void**>(&glGetIntegerv) },
		{ OpenGLFunction::OPENGL_GETSTRING, "glGetString", reinterpret_cast<void**>(&glGetString) },
		{ OpenGLFunction::OPENGL_GETERROR, "glGetError", reinterpret_cast<void**>(&glGetError) }
	};
}