//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include "graphics/opengl/opengl_core.hpp"
#ifdef _WIN32
#include "graphics/opengl/opengl_win.hpp"
#elif __linux__
#include "graphics/opengl/opengl_linux.hpp"
#endif

//
// DEBUGGING
//

//Set OpenGL debug callback
void (K_APIENTRY* glDebugMessageCallback)(
	GLDEBUGPROC callback,
	const void* userParam);

//Enable an OpenGL capability
void (K_APIENTRY* glEnable)(
	GLenum cap);

//
// GEOMETRY
//

//Binds a named buffer to a specified buffer binding point
void (K_APIENTRY* glBindBuffer)(
	GLenum target,
	GLuint buffer);

//Binds a vertex array object
void (K_APIENTRY* glBindVertexArray)(
	GLuint array);

//Creates and initializes a buffer object's data store
void (K_APIENTRY* glBufferData)(
	GLenum target,
	GLsizeiptr size,
	const void* data,
	GLenum usage);

//Deletes one or more named buffer objects
void (K_APIENTRY* glDeleteBuffers)(
	GLsizei n,
	const GLuint* buffers);

//Deletes one or more named vertex array objects
void (K_APIENTRY* glDeleteVertexArrays)(
	GLsizei n,
	const GLuint* arrays);

//Draws non-indexed primitives from array data
void (K_APIENTRY* glDrawArrays)(
	GLenum mode,
	GLint first,
	GLsizei count);

//Draws indexed primitives using array data and element indices
void (K_APIENTRY* glDrawElements)(
	GLenum mode,
	GLsizei count,
	GLenum type,
	const void* indices);

//Enables a generic vertex attribute array
void (K_APIENTRY* glEnableVertexAttribArray)(
	GLuint index);

//Generates buffer object names
void (K_APIENTRY* glGenBuffers)(
	GLsizei n,
	GLuint* buffers);

//Generates vertex array object names
void (K_APIENTRY* glGenVertexArrays)(
	GLsizei n,
	GLuint* arrays);

//Retrieves parameter values for a vertex attribute array
void (K_APIENTRY* glGetVertexAttribiv)(
	GLuint index,
	GLenum pname,
	GLint* params);

//Retrieves a pointer to a vertex attribute array parameter
void (K_APIENTRY* glGetVertexAttribPointerv)(
	GLuint index,
	GLenum pname,
	void** pointer);

//Defines an array of generic vertex attribute data
void (K_APIENTRY* glVertexAttribPointer)(
	GLuint index,
	GLint size,
	GLenum type,
	GLboolean normalized,
	GLsizei stride,
	const void* pointer);

//
// SHADERS
//

//Attaches a shader object to a program
void (K_APIENTRY* glAttachShader)(
	GLuint program,
	GLuint shader);

//Compiles a shader object
void (K_APIENTRY* glCompileShader)(
	GLuint shader);

//Creates a new shader program object
GLuint(K_APIENTRY* glCreateProgram)(
	void);

//Creates a shader object of the specified type
GLuint(K_APIENTRY* glCreateShader)(
	GLenum type);

//Deletes a shader object
void (K_APIENTRY* glDeleteShader)(
	GLuint shader);

//Deletes a program object
void (K_APIENTRY* glDeleteProgram)(
	GLuint program);

//Detaches a shader object from a program
void (K_APIENTRY* glDetachShader)(
	GLuint program,
	GLuint shader);

//Retrieves information about an active attribute variable
void (K_APIENTRY* glGetActiveAttrib)(
	GLuint program,
	GLuint index,
	GLsizei bufSize,
	GLsizei* length,
	GLint* size,
	GLenum* type,
	char* name);

//Returns the attribute location within a shader program
GLint(K_APIENTRY* glGetAttribLocation)(
	GLuint program,
	const char* name);

//Retrieves a parameter from a program object
void (K_APIENTRY* glGetProgramiv)(
	GLuint program,
	GLenum pname,
	GLint* params);

//Returns the information log for a program object
void (K_APIENTRY* glGetProgramInfoLog)(
	GLuint program,
	GLsizei bufSize,
	GLsizei* length,
	char* infoLog);

//Retrieves a parameter from a shader object
void (K_APIENTRY* glGetShaderiv)(
	GLuint shader,
	GLenum pname,
	GLint* params);

//Returns the information log for a shader object
void (K_APIENTRY* glGetShaderInfoLog)(
	GLuint shader,
	GLsizei bufSize,
	GLsizei* length,
	char* infoLog);

//Links a program object
void (K_APIENTRY* glLinkProgram)(
	GLuint program);

//Sets the source code for a shader
void (K_APIENTRY* glShaderSource)(
	GLuint shader,
	GLsizei count,
	const char* const* string,
	const GLint* length);

//Activates a shader program for rendering
void (K_APIENTRY* glUseProgram)(
	GLuint program);

//Validates a program object to see if it's executable
void (K_APIENTRY* glValidateProgram)(
	GLuint program);

//Returns whether a given program name is a valid program object
GLboolean(K_APIENTRY* glIsProgram)(
	GLuint program);

//
// UNIFORMS
//

//Retrieves the location of a uniform variable within a shader program
GLint(K_APIENTRY* glGetUniformLocation)(
	GLuint program,
	const char* name);

//Sets a single float uniform value
void (K_APIENTRY* glUniform1f)(
	GLint location,
	float v0);

//Sets a single integer uniform value
void (K_APIENTRY* glUniform1i)(
	GLint location,
	GLint v0);

//Sets a vec2 uniform (2 float components)
void (K_APIENTRY* glUniform2f)(
	GLint location,
	float v0,
	float v1);

//Sets a vec2 uniform from an array of values
void (K_APIENTRY* glUniform2fv)(
	GLint location,
	GLsizei count,
	const float* value);

//Sets a vec3 uniform (3 float components)
void (K_APIENTRY* glUniform3f)(
	GLint location,
	float v0,
	float v1,
	float v2);

//Sets a vec3 uniform from an array of values
void (K_APIENTRY* glUniform3fv)(
	GLint location,
	GLsizei count,
	const float* value);

//Sets a vec4 uniform (4 float components)
void (K_APIENTRY* glUniform4f)(
	GLint location,
	float v0,
	float v1,
	float v2,
	float v3);

//Sets a vec4 uniform from an array of values
void (K_APIENTRY* glUniform4fv)(
	GLint location,
	GLsizei count,
	const float* value);

//Sets a 2×2 matrix uniform from an array of floats
void (K_APIENTRY* glUniformMatrix2fv)(
	GLint location,
	GLsizei count,
	GLboolean transpose,
	const float* value);

//Sets a 3×3 matrix uniform from an array of floats
void (K_APIENTRY* glUniformMatrix3fv)(
	GLint location,
	GLsizei count,
	GLboolean transpose,
	const float* value);

//Sets a 4×4 matrix uniform from an array of floats
void (K_APIENTRY* glUniformMatrix4fv)(
	GLint location,
	GLsizei count,
	GLboolean transpose,
	const float* value);

//
// TEXTURES
//

//Binds a named texture to a texturing target
void (K_APIENTRY* glBindTexture)(
	GLenum target,
	GLuint texture);

//Activates the specified texture unit
void (K_APIENTRY* glActiveTexture)(
	GLenum texture);

//Deletes one or more named textures
void (K_APIENTRY* glDeleteTextures)(
	GLsizei n,
	const GLuint* textures);

//Generates mipmaps for the currently bound texture
void (K_APIENTRY* glGenerateMipmap)(
	GLenum target);

//Generates texture object names
void (K_APIENTRY* glGenTextures)(
	GLsizei n,
	GLuint* textures);

//Specifies a two-dimensional texture image
void (K_APIENTRY* glTexImage2D)(
	GLenum target,
	GLint level,
	GLint internalFormat,
	GLsizei width,
	GLsizei height,
	GLint border,
	GLenum format,
	GLenum type,
	const void* data);

//Sets texture parameters for the currently bound texture
void (K_APIENTRY* glTexParameteri)(
	GLenum target,
	GLenum pname,
	GLint param);

//Specifies a subregion of an existing 2D texture image
void (K_APIENTRY* glTexSubImage2D)(
	GLenum target,
	GLint level,
	GLint xoffset,
	GLint yoffset,
	GLsizei width,
	GLsizei height,
	GLenum format,
	GLenum type,
	const void* pixels);

//
// FRAMEBUFFERS AND RENDERBUFFERS
//

//Binds a renderbuffer to the renderbuffer target
void (K_APIENTRY* glBindRenderbuffer)(
	GLenum target,
	GLuint renderbuffer);

//Binds a framebuffer to a framebuffer target
void (K_APIENTRY* glBindFramebuffer)(
	GLenum target,
	GLuint framebuffer);

//Checks the completeness status of a framebuffer
GLenum(K_APIENTRY* glCheckFramebufferStatus)(
	GLenum target);

//Attaches a renderbuffer to a framebuffer attachment point
void (K_APIENTRY* glFramebufferRenderbuffer)(
	GLenum target,
	GLenum attachment,
	GLenum renderbuffertarget,
	GLuint renderbuffer);

//Attaches a 2D texture image to a framebuffer attachment point
void (K_APIENTRY* glFramebufferTexture2D)(
	GLenum target,
	GLenum attachment,
	GLenum textarget,
	GLuint texture,
	GLint level);

//Generates renderbuffer object names
void (K_APIENTRY* glGenRenderbuffers)(
	GLsizei n,
	GLuint* renderbuffers);

//Generates framebuffer object names
void (K_APIENTRY* glGenFramebuffers)(
	GLsizei n,
	GLuint* framebuffers);

//Establishes data storage format and dimensions for a renderbuffer
void (K_APIENTRY* glRenderbufferStorage)(
	GLenum target,
	GLenum internalformat,
	GLsizei width,
	GLsizei height);

//
// FRAME AND RENDER STATE
//

//Clears buffers to preset values
void (K_APIENTRY* glClear)(
	GLbitfield mask);

//Specifies the clear color for color buffers
void (K_APIENTRY* glClearColor)(
	float red,
	float green,
	float blue,
	float alpha);

//Disables a specific OpenGL capability
void (K_APIENTRY* glDisable)(
	GLenum cap);

//Returns the last error flag raised
GLenum(K_APIENTRY* glGetError)(
	void);

//Retrieves integer-valued parameters
void (K_APIENTRY* glGetIntegerv)(
	GLenum pname,
	GLint* data);

//Returns a string describing the current GL connection
const GLubyte* (K_APIENTRY* glGetString)(
	GLenum name);

//Sets the viewport transformation dimensions
void (K_APIENTRY* glViewport)(
	GLint x,
	GLint y,
	GLsizei width,
	GLsizei height);

#ifdef _WIN32

//
// WINDOWS GL FUNCTIONS
//

//Creates an OpenGL rendering context with specific attributes (version, profile)
HGLRC(WINAPI* wglCreateContextAttribsARB)(
	HDC hDC,
	HGLRC hShareContext,
	const int* attribList);

//Chooses a pixel format that matches specified attributes
BOOL(WINAPI* wglChoosePixelFormatARB)(
	HDC hDC,
	const int* attribIList,
	const FLOAT* attribFList,
	UINT maxFormats,
	int* formats,
	UINT* numFormats);

//Sets the swap interval for buffer swaps (vsync control)
BOOL(WINAPI* wglSwapIntervalEXT)(
	int interval);

#elif __linux

//
// LINUX GL FUNCTIONS
//

#endif