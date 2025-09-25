//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include "KalaHeaders/core_utils.hpp"
#include "OpenGL/glcorearb.h" //core opengl
#include "OpenGL/glext.h"     //extension tokens, enums and extra function pointers

namespace KalaWindow::Graphics::OpenGLFunctions
{
	using std::string;

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

	//Set OpenGL debug callback
	LIB_API extern PFNGLDEBUGMESSAGECALLBACKPROC glDebugMessageCallback;

	// Returns the last error flag raised
	LIB_API extern PFNGLGETERRORPROC glGetError;

	//
	// GEOMETRY
	//

	//Maps a range of a buffer object's data store into the client's address space
	LIB_API extern PFNGLMAPBUFFERRANGEPROC glMapBufferRange;

	//Creates an immutable data store for a buffer object (ARB_buffer_storage)
	LIB_API extern PFNGLBUFFERSTORAGEPROC glBufferStorage;

	//Binds a named buffer to a specified buffer binding point
	LIB_API extern PFNGLBINDBUFFERPROC glBindBuffer;

	//Binds a vertex array object
	LIB_API extern PFNGLBINDVERTEXARRAYPROC glBindVertexArray;

	//Creates and initializes a buffer object's data store
	LIB_API extern PFNGLBUFFERDATAPROC glBufferData;

	//Updates existing buffer object's data without reallocating storage
	LIB_API extern PFNGLBUFFERSUBDATAPROC glBufferSubData;

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

	// Tells OpenGL which faces to not render before fragment shading
	LIB_API extern PFNGLCULLFACEPROC glCullFace;

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

	// Find block index by name
	LIB_API extern PFNGLGETUNIFORMBLOCKINDEXPROC glGetUniformBlockIndex;

	// Bind block index to binding point
	LIB_API extern PFNGLUNIFORMBLOCKBINDINGPROC glUniformBlockBinding;

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

	// Generates texture object names
	LIB_API extern PFNGLGENTEXTURESPROC glGenTextures;

	// Specifies a 2D texture image
	LIB_API extern PFNGLTEXIMAGE2DPROC glTexImage2D;

	// Specifies a 3D texture image
	LIB_API extern PFNGLTEXIMAGE3DPROC glTexImage3D;

	// Specifies a compressed 2D texture image
	LIB_API extern PFNGLCOMPRESSEDTEXIMAGE2DPROC glCompressedTexImage2D;

	// Specifies a compressed 3D or array texture image
	LIB_API extern PFNGLCOMPRESSEDTEXIMAGE3DPROC glCompressedTexImage3D;

	// Allocates immutable storage for a 2D texture or cube map (all mip levels)
	LIB_API extern PFNGLTEXSTORAGE2DPROC glTexStorage2D;

	// Allocates immutable storage for a 3D texture or 2D texture array (all mip levels)
	LIB_API extern PFNGLTEXSTORAGE3DPROC glTexStorage3D;

	// Specifies a subregion of a 2D texture image (upload pixels into an allocated level)
	LIB_API extern PFNGLTEXSUBIMAGE2DPROC glTexSubImage2D;

	// Specifies a subregion of a 3D or array texture image (upload pixels into an allocated level)
	LIB_API extern PFNGLTEXSUBIMAGE3DPROC glTexSubImage3D;

	// Specifies a subregion of a compressed 2D texture image (upload compressed blocks into an allocated level)
	LIB_API extern PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC glCompressedTexSubImage2D;

	// Specifies a subregion of a compressed 3D or array texture image (upload compressed blocks into an allocated level)
	LIB_API extern PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC glCompressedTexSubImage3D;

	// Sets integer texture parameters for the currently bound texture
	LIB_API extern PFNGLTEXPARAMETERIPROC glTexParameteri;

	// Sets integer/vector texture parameters for the currently bound texture
	LIB_API extern PFNGLTEXPARAMETERIVPROC glTexParameteriv;

	// Sets float texture parameters for the currently bound texture
	LIB_API extern PFNGLTEXPARAMETERFPROC glTexParameterf;

	// Sets float/vector texture parameters for the currently bound texture
	LIB_API extern PFNGLTEXPARAMETERFVPROC glTexParameterfv;

	// Tells OpenGL how to step through rows of integer pixel data
	LIB_API extern PFNGLPIXELSTOREIPROC glPixelStorei;

	// Tells OpenGL how to step through rows of float pixel data
	LIB_API extern PFNGLPIXELSTOREFPROC glPixelStoref;

	//
	// FRAMEBUFFERS AND RENDERBUFFERS
	//

	// Binds a renderbuffer to the renderbuffer target
	LIB_API extern PFNGLBINDRENDERBUFFERPROC glBindRenderbuffer;

	// Binds a framebuffer to a framebuffer target
	LIB_API extern PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer;

	// Binds a buffer object to a binding point that shaders can use
	LIB_API extern PFNGLBINDBUFFERBASEPROC glBindBufferBase;

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

	// Sets the comparison function used for depth testing
	LIB_API extern PFNGLDEPTHFUNCPROC glDepthFunc;

	// Toggles writing to the depth buffer
	LIB_API extern PFNGLDEPTHMASKPROC glDepthMask;

	// Sets the constant blend color used with blending factors
	LIB_API extern PFNGLBLENDCOLORPROC glBlendColor;

	// Sets the blending factors used to combine source and destination colors
	LIB_API extern PFNGLBLENDFUNCPROC glBlendFunc;

	// Sets the blending factors used to combine source and destination colors for a specific color buffer
	LIB_API extern PFNGLBLENDFUNCIPROC glBlendFunci;

	// Sets the blend equation used to combine source and destination colors
	LIB_API extern PFNGLBLENDEQUATIONPROC glBlendEquation;

	// Sets the per-render-target blend equation used to combine source and destination colors
	LIB_API extern PFNGLBLENDEQUATIONIPROC glBlendEquationi;

	// Sets the blend equation used to combine source and destination colors
	// as well as RGB and Alpha channels
	LIB_API extern PFNGLBLENDEQUATIONSEPARATEPROC glBlendEquationSeparate;

	// Sets the per-render-target blend equation used to combine source and destination colors
	// as well as RGB and Alpha channels
	LIB_API extern PFNGLBLENDEQUATIONSEPARATEIPROC glBlendEquationSeparatei;

	// Sets the stencil test function, reference value and mask
	LIB_API extern PFNGLSTENCILFUNCPROC glStencilFunc;

	// Sets the stencil test function separately for front and back faces
	LIB_API extern PFNGLSTENCILFUNCSEPARATEPROC glStencilFuncSeparate;

	// Sets the bitmask that controls which bits of the stencil buffer can be written
	LIB_API extern PFNGLSTENCILMASKPROC glStencilMask;

	// Sets which bits of the stencil buffer can be written, separately per face
	LIB_API extern PFNGLSTENCILMASKSEPARATEPROC glStencilMaskSeparate;

	// Sets the action to take on stencil failure, depth test failure and depth test pass
	LIB_API extern PFNGLSTENCILOPPROC glStencilOp;

	// Sets stencil operations separately for front and back faces
	LIB_API extern PFNGLSTENCILOPSEPARATEPROC glStencilOpSeparate;

	//
	// FRAME AND RENDER STATE
	//

	// Clears buffers to preset values
	LIB_API extern PFNGLCLEARPROC glClear;

	// Specifies the clear color for color buffers
	LIB_API extern PFNGLCLEARCOLORPROC glClearColor;

	// Enables a specific OpenGL capability
	LIB_API extern PFNGLENABLEPROC glEnable;

	// Disables a specific OpenGL capability
	LIB_API extern PFNGLDISABLEPROC glDisable;

	// Tells OpenGL how to decide which polygons are front-facing
	LIB_API extern PFNGLFRONTFACEPROC glFrontFace;

	// Retrieves bool-valued parameters
	LIB_API extern PFNGLGETBOOLEANVPROC glGetBooleanv;

	// Retrieves integer-valued parameters
	LIB_API extern PFNGLGETINTEGERVPROC glGetIntegerv;

	// Retrieves float-valued parameters
	LIB_API extern PFNGLGETFLOATVPROC glGetFloatv;

	// Retrieves double-valued parameters
	LIB_API extern PFNGLGETDOUBLEVPROC glGetDoublev;

	// Returns a string describing the current GL connection
	LIB_API extern PFNGLGETSTRINGPROC glGetString;

	// Returns a string extension for the given index
	LIB_API extern PFNGLGETSTRINGIPROC glGetStringi;

	// Sets the viewport transformation dimensions
	LIB_API extern PFNGLVIEWPORTPROC glViewport;

	class LIB_API OpenGL_Functions_Core
	{
	public:
		//Load all OpenGL general functions that are provided
		static void LoadAllCoreFunctions();

		//Load a specific function, this won't be loaded again with LoadAllFunctions
		static void LoadCoreFunction(const char* name);
	};
}