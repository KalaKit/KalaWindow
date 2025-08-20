//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <vector>

#include "KalaHeaders/api.hpp"
#include "KalaHeaders/core_types.hpp"
#include "OpenGL/glcorearb.h" //core opengl
#include "OpenGL/glext.h"     //extension tokens, enums and extra function pointers

namespace KalaWindow::Graphics::OpenGLFunctions
{
	using std::vector;

	struct GLFunction
	{
		string name;
		void* ptr;
	};

	//
	// DEBUGGING
	//

	using GLDEBUGPROC = void (LIB_APIENTRY*)(
		GLenum source,
		GLenum type,
		GLuint id,
		GLenum severity,
		GLsizei length,
		const char* message,
		const void* userParam);

	//Set OpenGL debug callback
	LIB_API extern PFNGLDEBUGMESSAGECALLBACKPROC glDebugMessageCallback;

	//Enable an OpenGL capability
	LIB_API extern PFNGLENABLEPROC glEnable;

	//Get OpenGL debug messages and send them to the logger
	LIB_API extern void DebugCallback(
		GLenum source,
		GLenum type,
		GLuint id,
		GLenum severity,
		GLsizei length,
		const char* message,
		const void* userParam);

	//
	// GEOMETRY
	//

	//Binds a named buffer to a specified buffer binding point
	LIB_API extern PFNGLBINDBUFFERPROC glBindBuffer;

	//Binds a vertex array object
	LIB_API extern PFNGLBINDVERTEXARRAYPROC glBindVertexArray;

	//Creates and initializes a buffer object's data store
	LIB_API extern PFNGLBUFFERDATAPROC glBufferData;

	//Deletes one or more named buffer objects
	LIB_API extern PFNGLDELETEBUFFERSPROC glDeleteBuffers;

	//Deletes one or more named vertex array objects
	LIB_API extern PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;

	//Draws non-indexed primitives from array data
	LIB_API extern PFNGLDRAWARRAYSPROC glDrawArrays;

	//Draws indexed primitives using array data and element indices
	LIB_API extern PFNGLDRAWELEMENTSPROC glDrawElements;

	//Enables a generic vertex attribute array
	LIB_API extern PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;

	//Generates buffer object names
	LIB_API extern PFNGLGENBUFFERSPROC glGenBuffers;

	//Generates vertex array object names
	LIB_API extern PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;

	//Retrieves parameter values for a vertex attribute array
	LIB_API extern PFNGLGETVERTEXATTRIBIVPROC glGetVertexAttribiv;

	//Retrieves a pointer to a vertex attribute array parameter
	LIB_API extern PFNGLGETVERTEXATTRIBPOINTERVPROC glGetVertexAttribPointerv;

	//Defines an array of generic vertex attribute data
	LIB_API extern PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;

	//
	// SHADERS
	//

	// Attaches a shader object to a program
	LIB_API extern PFNGLATTACHSHADERPROC glAttachShader;

	// Compiles a shader object
	LIB_API extern PFNGLCOMPILESHADERPROC glCompileShader;

	// Creates a new shader program object
	LIB_API extern PFNGLCREATEPROGRAMPROC glCreateProgram;

	// Creates a shader object of the specified type (GL_VERTEX_SHADER, etc.)
	LIB_API extern PFNGLCREATESHADERPROC glCreateShader;

	// Deletes a shader object
	LIB_API extern PFNGLDELETESHADERPROC glDeleteShader;

	// Deletes a program object
	LIB_API extern PFNGLDELETEPROGRAMPROC glDeleteProgram;

	// Detaches a shader object from a program
	LIB_API extern PFNGLDETACHSHADERPROC glDetachShader;

	// Retrieves information about an active attribute variable
	LIB_API extern PFNGLGETACTIVEATTRIBPROC glGetActiveAttrib;

	// Returns the attribute location within a shader program
	LIB_API extern PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation;

	// Retrieves a parameter from a program object
	LIB_API extern PFNGLGETPROGRAMIVPROC glGetProgramiv;

	// Returns the information log for a program object
	LIB_API extern PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;

	// Retrieves a parameter from a shader object
	LIB_API extern PFNGLGETSHADERIVPROC glGetShaderiv;

	// Returns the information log for a shader object
	LIB_API extern PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;

	// Links a program object
	LIB_API extern PFNGLLINKPROGRAMPROC glLinkProgram;

	// Sets the source code for a shader
	LIB_API extern PFNGLSHADERSOURCEPROC glShaderSource;

	// Activates a shader program for rendering
	LIB_API extern PFNGLUSEPROGRAMPROC glUseProgram;

	// Validates a program object to see if it's executable
	LIB_API extern PFNGLVALIDATEPROGRAMPROC glValidateProgram;

	// Returns whether a given program name is a valid program object
	LIB_API extern PFNGLISPROGRAMPROC glIsProgram;

	//
	// UNIFORMS
	//

	// Retrieves the location of a uniform variable within a shader program
	LIB_API extern PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;

	// Sets a single float uniform value
	LIB_API extern PFNGLUNIFORM1FPROC glUniform1f;

	// Sets a single integer uniform value
	LIB_API extern PFNGLUNIFORM1IPROC glUniform1i;

	// Sets a vec2 uniform (2 float components)
	LIB_API extern PFNGLUNIFORM2FPROC glUniform2f;

	// Sets a vec2 uniform from an array of values
	LIB_API extern PFNGLUNIFORM2FVPROC glUniform2fv;

	// Sets a vec3 uniform (3 float components)
	LIB_API extern PFNGLUNIFORM3FPROC glUniform3f;

	// Sets a vec3 uniform from an array of values
	LIB_API extern PFNGLUNIFORM3FVPROC glUniform3fv;

	// Sets a vec4 uniform (4 float components)
	LIB_API extern PFNGLUNIFORM4FPROC glUniform4f;

	// Sets a vec4 uniform from an array of values
	LIB_API extern PFNGLUNIFORM4FVPROC glUniform4fv;

	// Sets a 2×2 matrix uniform from an array of floats
	LIB_API extern PFNGLUNIFORMMATRIX2FVPROC glUniformMatrix2fv;

	// Sets a 3×3 matrix uniform from an array of floats
	LIB_API extern PFNGLUNIFORMMATRIX3FVPROC glUniformMatrix3fv;

	// Sets a 4×4 matrix uniform from an array of floats
	LIB_API extern PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;

	//
	// TEXTURES
	//

	// Binds a named texture to a texturing target
	LIB_API extern PFNGLBINDTEXTUREPROC glBindTexture;

	// Activates the specified texture unit
	LIB_API extern PFNGLACTIVETEXTUREPROC glActiveTexture;

	// Deletes one or more named textures
	LIB_API extern PFNGLDELETETEXTURESPROC glDeleteTextures;

	// Generates mipmaps for the currently bound texture
	LIB_API extern PFNGLGENERATEMIPMAPPROC glGenerateMipmap;

	// Generates texture object names
	LIB_API extern PFNGLGENTEXTURESPROC glGenTextures;

	// Specifies a two-dimensional texture image
	LIB_API extern PFNGLTEXIMAGE2DPROC glTexImage2D;

	// Sets texture parameters for the currently bound texture
	LIB_API extern PFNGLTEXPARAMETERIPROC glTexParameteri;

	// Specifies a subregion of an existing 2D texture image
	LIB_API extern PFNGLTEXSUBIMAGE2DPROC glTexSubImage2D;

	//
	// FRAMEBUFFERS AND RENDERBUFFERS
	//

	// Binds a renderbuffer to the renderbuffer target
	LIB_API extern PFNGLBINDRENDERBUFFERPROC glBindRenderbuffer;

	// Binds a framebuffer to a framebuffer target
	LIB_API extern PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer;

	// Checks the completeness status of a framebuffer
	LIB_API extern PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus;

	// Attaches a renderbuffer to a framebuffer attachment point
	LIB_API extern PFNGLFRAMEBUFFERRENDERBUFFERPROC glFramebufferRenderbuffer;

	// Attaches a 2D texture image to a framebuffer attachment point
	LIB_API extern PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D;

	// Generates renderbuffer object names
	LIB_API extern PFNGLGENRENDERBUFFERSPROC glGenRenderbuffers;

	// Generates framebuffer object names
	LIB_API extern PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers;

	// Establishes data storage format and dimensions for a renderbuffer
	LIB_API extern PFNGLRENDERBUFFERSTORAGEPROC glRenderbufferStorage;

	//
	// FRAME AND RENDER STATE
	//

	// Clears buffers to preset values
	LIB_API extern PFNGLCLEARPROC glClear;

	// Specifies the clear color for color buffers
	LIB_API extern PFNGLCLEARCOLORPROC glClearColor;

	// Disables a specific OpenGL capability
	LIB_API extern PFNGLDISABLEPROC glDisable;

	// Returns the last error flag raised
	LIB_API extern PFNGLGETERRORPROC glGetError;

	// Retrieves integer-valued parameters
	LIB_API extern PFNGLGETINTEGERVPROC glGetIntegerv;

	// Returns a string describing the current GL connection
	LIB_API extern PFNGLGETSTRINGPROC glGetString;

	// Sets the viewport transformation dimensions
	LIB_API extern PFNGLVIEWPORTPROC glViewport;

	class LIB_API OpenGL_Functions_Core
	{
	public:
		//Load all OpenGL general functions that are provided
		static void LoadAllFunctions();

		//Load a specific function, this won't be loaded again with LoadAllFunctions
		static void LoadFunction(void** target, const char* name);
	private:
		static inline vector<GLFunction> loadedFunctions{};
	};
}