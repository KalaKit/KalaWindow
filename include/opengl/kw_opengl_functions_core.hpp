//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>

#include "OpenGL/glcorearb.h" //core opengl

#include "KalaHeaders/core_utils.hpp"

namespace KalaWindow::OpenGL::OpenGLFunctions
{
	using std::string;

#ifdef __linux__
	typedef void (*PFNGLDRAWARRAYSPROC)(GLenum mode, GLint first, GLsizei count);
	typedef void (*PFNGLDRAWELEMENTSPROC)(GLenum mode, GLsizei count, GLenum type, const void* indices);
	typedef void (*PFNGLBINDTEXTUREPROC)(GLenum target, GLuint texture);
	typedef void (*PFNGLDELETETEXTURESPROC)(GLsizei n, const GLuint* textures);
	typedef void (*PFNGLGENTEXTURESPROC)(GLsizei n, GLuint* textures);
	typedef void (*PFNGLTEXSUBIMAGE2DPROC)(
		GLenum target,
		GLint level,
		GLint xoffset,
		GLint yoffset,
		GLsizei width,
		GLsizei height,
		GLenum format,
		GLenum type,
		const void* pixels);
#endif

	struct GL_Core
	{
		//Set OpenGL debug callback
		PFNGLDEBUGMESSAGECALLBACKPROC glDebugMessageCallback;

		//Returns the last error flag raised
		PFNGLGETERRORPROC glGetError;

		//
		// GEOMETRY
		//

		//Maps a range of a buffer object's data store into the client's address space
		PFNGLMAPBUFFERRANGEPROC glMapBufferRange;

		//Creates an immutable data store for a buffer object (ARB_buffer_storage)
		PFNGLBUFFERSTORAGEPROC glBufferStorage;

		//Binds a named buffer to a specified buffer binding point
		PFNGLBINDBUFFERPROC glBindBuffer;

		//Binds a vertex array object
		PFNGLBINDVERTEXARRAYPROC glBindVertexArray;

		//Creates and initializes a buffer object's data store
		PFNGLBUFFERDATAPROC glBufferData;

		//Updates existing buffer object's data without reallocating storage
		PFNGLBUFFERSUBDATAPROC glBufferSubData;

		//Deletes one or more named buffer objects
		PFNGLDELETEBUFFERSPROC glDeleteBuffers;

		//Deletes one or more named vertex array objects
		PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;

		//Draws non-indexed primitives from array data
		PFNGLDRAWARRAYSPROC glDrawArrays;

		//Draws indexed primitives using array data and element indices
		PFNGLDRAWELEMENTSPROC glDrawElements;

		//Enables a generic vertex attribute array
		PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;

		//Generates buffer object names
		PFNGLGENBUFFERSPROC glGenBuffers;

		//Generates vertex array object names
		PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;

		//Retrieves parameter values for a vertex attribute array
		PFNGLGETVERTEXATTRIBIVPROC glGetVertexAttribiv;

		//Retrieves a pointer to a vertex attribute array parameter
		PFNGLGETVERTEXATTRIBPOINTERVPROC glGetVertexAttribPointerv;

		//Defines an array of generic vertex attribute data
		PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;

		//Disables a generic vertex attribute array
		PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;

		//Sets a constant integer value for a generic vertex attribute
		PFNGLVERTEXATTRIBI1IPROC glVertexAttribI1i;

		//Sets a constant integer vec2 value for a generic vertex attribute
		PFNGLVERTEXATTRIBI2IPROC glVertexAttribI2i;

		//Sets a constant integer vec3 value for a generic vertex attribute
		PFNGLVERTEXATTRIBI3IPROC glVertexAttribI3i;

		//Sets a constant integer vec4 value for a generic vertex attribute
		PFNGLVERTEXATTRIBI4IPROC glVertexAttribI4i;

		//Tells OpenGL which faces to not render before fragment shading
		PFNGLCULLFACEPROC glCullFace;

		//
		// SHADERS
		//

		//Attaches a shader object to a program
		PFNGLATTACHSHADERPROC glAttachShader;

		//Compiles a shader object
		PFNGLCOMPILESHADERPROC glCompileShader;

		//Creates a new shader program object
		PFNGLCREATEPROGRAMPROC glCreateProgram;

		//Creates a shader object of the specified type (GL_VERTEX_SHADER, etc.)
		PFNGLCREATESHADERPROC glCreateShader;

		//Deletes a shader object
		PFNGLDELETESHADERPROC glDeleteShader;

		//Deletes a program object
		PFNGLDELETEPROGRAMPROC glDeleteProgram;

		//Detaches a shader object from a program
		PFNGLDETACHSHADERPROC glDetachShader;

		//Retrieves information about an active attribute variable
		PFNGLGETACTIVEATTRIBPROC glGetActiveAttrib;

		//Returns the attribute location within a shader program
		PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation;

		//Retrieves a parameter from a program object
		PFNGLGETPROGRAMIVPROC glGetProgramiv;

		//Returns the information log for a program object
		PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;

		//Retrieves a parameter from a shader object
		PFNGLGETSHADERIVPROC glGetShaderiv;

		//Returns the information log for a shader object
		PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;

		//Links a program object
		PFNGLLINKPROGRAMPROC glLinkProgram;

		//Sets the source code for a shader
		PFNGLSHADERSOURCEPROC glShaderSource;

		//Activates a shader program for rendering
		PFNGLUSEPROGRAMPROC glUseProgram;

		//Validates a program object to see if it's executable
		PFNGLVALIDATEPROGRAMPROC glValidateProgram;

		//Returns whether a given program name is a valid program object
		PFNGLISPROGRAMPROC glIsProgram;

		//
		// UNIFORMS
		//

		//Retrieves the location of a uniform variable within a shader program
		PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;

		//Find block index by name
		PFNGLGETUNIFORMBLOCKINDEXPROC glGetUniformBlockIndex;

		//Bind block index to binding point
		PFNGLUNIFORMBLOCKBINDINGPROC glUniformBlockBinding;

		//Sets a float uniform
		PFNGLUNIFORM1FPROC glUniform1f;

		//Sets a integer uniform
		PFNGLUNIFORM1IPROC glUniform1i;

		//Sets a float uniform from an array
		PFNGLUNIFORM1FVPROC glUniform1fv;

		//Sets a int uniform from an array
		PFNGLUNIFORM1IVPROC glUniform1iv;

		//Sets a vec2 float uniform
		PFNGLUNIFORM2FPROC glUniform2f;

		//Sets a vec2 integer uniform
		PFNGLUNIFORM2IPROC glUniform2i;

		//Sets a vec2 float uniform from an array
		PFNGLUNIFORM2FVPROC glUniform2fv;

		//Sets a vec2 int uniform from an array
		PFNGLUNIFORM2IVPROC glUniform2iv;

		//Sets a vec3 float uniform
		PFNGLUNIFORM3FPROC glUniform3f;

		//Sets a vec3 integer uniform
		PFNGLUNIFORM3IPROC glUniform3i;

		//Sets a vec3 float uniform from an array
		PFNGLUNIFORM3FVPROC glUniform3fv;

		//Sets a vec3 int uniform from an array
		PFNGLUNIFORM3IVPROC glUniform3iv;

		//Sets a vec4 float uniform
		PFNGLUNIFORM4FPROC glUniform4f;

		//Sets a vec4 integer uniform
		PFNGLUNIFORM4IPROC glUniform4i;

		//Sets a vec4 float uniform from an array
		PFNGLUNIFORM4FVPROC glUniform4fv;

		//Sets a vec4 int uniform from an array
		PFNGLUNIFORM4IVPROC glUniform4iv;

		//Sets a 2�2 matrix uniform from an array of floats
		PFNGLUNIFORMMATRIX2FVPROC glUniformMatrix2fv;

		//Sets a 3�3 matrix uniform from an array of floats
		PFNGLUNIFORMMATRIX3FVPROC glUniformMatrix3fv;

		//Sets a 4�4 matrix uniform from an array of floats
		PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;

		//
		// TEXTURES
		//

		//Binds a named texture to a texturing target
		PFNGLBINDTEXTUREPROC glBindTexture;

		//Activates the specified texture unit
		PFNGLACTIVETEXTUREPROC glActiveTexture;

		//Deletes one or more named textures
		PFNGLDELETETEXTURESPROC glDeleteTextures;

		//Generates mipmaps for the currently bound texture
		PFNGLGENERATEMIPMAPPROC glGenerateMipmap;

		//Generates texture object names
		PFNGLGENTEXTURESPROC glGenTextures;

		//Specifies a 2D texture image
		PFNGLTEXIMAGE2DPROC glTexImage2D;

		//Specifies a 3D texture image
		PFNGLTEXIMAGE3DPROC glTexImage3D;

		//Specifies a compressed 2D texture image
		PFNGLCOMPRESSEDTEXIMAGE2DPROC glCompressedTexImage2D;

		//Specifies a compressed 3D or array texture image
		PFNGLCOMPRESSEDTEXIMAGE3DPROC glCompressedTexImage3D;

		//Allocates immutable storage for a 2D texture or cube map (all mip levels)
		PFNGLTEXSTORAGE2DPROC glTexStorage2D;

		//Allocates immutable storage for a 3D texture or 2D texture array (all mip levels)
		PFNGLTEXSTORAGE3DPROC glTexStorage3D;

		//Specifies a subregion of a 2D texture image (upload pixels into an allocated level)
		PFNGLTEXSUBIMAGE2DPROC glTexSubImage2D;

		//Specifies a subregion of a 3D or array texture image (upload pixels into an allocated level)
		PFNGLTEXSUBIMAGE3DPROC glTexSubImage3D;

		//Specifies a subregion of a compressed 2D texture image (upload compressed blocks into an allocated level)
		PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC glCompressedTexSubImage2D;

		//Specifies a subregion of a compressed 3D or array texture image (upload compressed blocks into an allocated level)
		PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC glCompressedTexSubImage3D;

		//Sets integer texture parameters for the currently bound texture
		PFNGLTEXPARAMETERIPROC glTexParameteri;

		//Sets integer/vector texture parameters for the currently bound texture
		PFNGLTEXPARAMETERIVPROC glTexParameteriv;

		//Sets float texture parameters for the currently bound texture
		PFNGLTEXPARAMETERFPROC glTexParameterf;

		//Sets float/vector texture parameters for the currently bound texture
		PFNGLTEXPARAMETERFVPROC glTexParameterfv;

		//Tells OpenGL how to step through rows of integer pixel data
		PFNGLPIXELSTOREIPROC glPixelStorei;

		//Tells OpenGL how to step through rows of float pixel data
		PFNGLPIXELSTOREFPROC glPixelStoref;

		//
		// FRAMEBUFFERS AND RENDERBUFFERS
		//

		//Binds a renderbuffer to the renderbuffer target
		PFNGLBINDRENDERBUFFERPROC glBindRenderbuffer;

		//Binds a framebuffer to a framebuffer target
		PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer;

		//Binds a buffer object to a binding point that shaders can use
		PFNGLBINDBUFFERBASEPROC glBindBufferBase;

		//Checks the completeness status of a framebuffer
		PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus;

		//Attaches a renderbuffer to a framebuffer attachment point
		PFNGLFRAMEBUFFERRENDERBUFFERPROC glFramebufferRenderbuffer;

		//Attaches a 2D texture image to a framebuffer attachment point
		PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D;

		//Generates renderbuffer object names
		PFNGLGENRENDERBUFFERSPROC glGenRenderbuffers;

		//Generates framebuffer object names
		PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers;

		//Establishes data storage format and dimensions for a renderbuffer
		PFNGLRENDERBUFFERSTORAGEPROC glRenderbufferStorage;

		//Sets the comparison function used for depth testing
		PFNGLDEPTHFUNCPROC glDepthFunc;

		//Toggles writing to the depth buffer
		PFNGLDEPTHMASKPROC glDepthMask;

		//Sets the constant blend color used with blending factors
		PFNGLBLENDCOLORPROC glBlendColor;

		//Sets the blending factors used to combine source and destination colors
		PFNGLBLENDFUNCPROC glBlendFunc;

		//Sets the blending factors used to combine source and destination colors for a specific color buffer
		PFNGLBLENDFUNCIPROC glBlendFunci;

		//Sets the blend equation used to combine source and destination colors
		PFNGLBLENDEQUATIONPROC glBlendEquation;

		//Sets the per-render-target blend equation used to combine source and destination colors
		PFNGLBLENDEQUATIONIPROC glBlendEquationi;

		//Sets the blend equation used to combine source and destination colors
		//as well as RGB and Alpha channels
		PFNGLBLENDEQUATIONSEPARATEPROC glBlendEquationSeparate;

		//Sets the per-render-target blend equation used to combine source and destination colors
		//as well as RGB and Alpha channels
		PFNGLBLENDEQUATIONSEPARATEIPROC glBlendEquationSeparatei;

		//Sets the stencil test function, reference value and mask
		PFNGLSTENCILFUNCPROC glStencilFunc;

		//Sets the stencil test function separately for front and back faces
		PFNGLSTENCILFUNCSEPARATEPROC glStencilFuncSeparate;

		//Sets the bitmask that controls which bits of the stencil buffer can be written
		PFNGLSTENCILMASKPROC glStencilMask;

		//Sets which bits of the stencil buffer can be written, separately per face
		PFNGLSTENCILMASKSEPARATEPROC glStencilMaskSeparate;

		//Sets the action to take on stencil failure, depth test failure and depth test pass
		PFNGLSTENCILOPPROC glStencilOp;

		//Sets stencil operations separately for front and back faces
		PFNGLSTENCILOPSEPARATEPROC glStencilOpSeparate;

		//
		// FRAME AND RENDER STATE
		//

		//Clears buffers to preset values
		PFNGLCLEARPROC glClear;

		//Specifies the clear color for color buffers
		PFNGLCLEARCOLORPROC glClearColor;

		//Enables a specific OpenGL capability
		PFNGLENABLEPROC glEnable;

		//Disables a specific OpenGL capability
		PFNGLDISABLEPROC glDisable;

		//Tells OpenGL how to decide which polygons are front-facing
		PFNGLFRONTFACEPROC glFrontFace;

		//Retrieves bool-valued parameters
		PFNGLGETBOOLEANVPROC glGetBooleanv;

		//Retrieves integer-valued parameters
		PFNGLGETINTEGERVPROC glGetIntegerv;

		//Retrieves float-valued parameters
		PFNGLGETFLOATVPROC glGetFloatv;

		//Retrieves double-valued parameters
		PFNGLGETDOUBLEVPROC glGetDoublev;

		//Returns a string describing the current GL connection
		PFNGLGETSTRINGPROC glGetString;

		//Returns a string extension for the given index
		PFNGLGETSTRINGIPROC glGetStringi;

		//Sets the viewport transformation dimensions
		PFNGLVIEWPORTPROC glViewport;
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

	//Get OpenGL debug messages and send them to the logger
	LIB_API extern void DebugCallback(
		GLenum source,
		GLenum type,
		GLuint id,
		GLenum severity,
		GLsizei length,
		const char* message,
		const void* userParam);

	class LIB_API OpenGL_Functions_Core
	{
	public:
		static const GL_Core* GetGLCore();

		//Load all OpenGL general functions that are provided
		static void LoadAllCoreFunctions();
	};
}
