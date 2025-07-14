//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#ifdef KALAWINDOW_SUPPORT_OPENGL

#include <cstdint>
#include <cstddef>

#include "core/platform.hpp"

#ifdef _WIN32
#include "graphics/opengl/opengl_win.hpp"
#elif __linux__
#include "graphics/opengl/opengl_linux.hpp"
#endif

using std::uint32_t;
using std::int32_t;
using std::ptrdiff_t;

using GLbitfield = uint32_t;
using GLboolean = uint8_t;
using GLbyte = int8_t;
using GLenum = uint32_t;
using GLint = int32_t;
using GLintptr = ptrdiff_t;
using GLshort = int16_t;
using GLsizeiptr = ptrdiff_t;
using GLsizei = int32_t;
using GLuint = uint32_t;
using GLubyte = uint8_t;
using GLushort = uint16_t;

//OpenGL error codes

inline constexpr GLenum	GL_INVALID_ENUM = 0x0500; //An unacceptable value is specified for an enumerated argument
inline constexpr GLenum	GL_INVALID_FRAMEBUFFER_OPERATION = 0x0506; //Framebuffer object is not complete
inline constexpr GLenum	GL_INVALID_OPERATION = 0x0502; //The specified operation is not allowed in the current state
inline constexpr GLenum	GL_INVALID_VALUE = 0x0501; //A numeric argument is out of range
inline constexpr GLenum	GL_NO_ERROR = 0x0000; //No error has been recorded
inline constexpr GLenum	GL_OUT_OF_MEMORY = 0x0505; //There is not enough memory left to execute the command
inline constexpr GLenum	GL_STACK_OVERFLOW = 0x0503; //Function would cause a stack overflow
inline constexpr GLenum	GL_STACK_UNDERFLOW = 0x0504; //Function would cause a stack underflow

//Shader types

inline constexpr GLenum	GL_FRAGMENT_SHADER = 0x8B30; //Fragment shader type
inline constexpr GLenum	GL_GEOMETRY_SHADER = 0x8DD9; //Geometry shader type
inline constexpr GLenum	GL_VERTEX_SHADER = 0x8B31; //Vertex shader type

//Shader parameter enums

inline constexpr GLenum	GL_ACTIVE_ATTRIBUTES = 0x8B89; //Number of active attributes
inline constexpr GLenum	GL_ACTIVE_UNIFORMS = 0x8B86; //Number of active uniforms
inline constexpr GLenum	GL_COMPILE_STATUS = 0x8B81; //Shader compilation status
inline constexpr GLenum	GL_INFO_LOG_LENGTH = 0x8B84; //Length of the shader info log
inline constexpr GLenum	GL_LINK_STATUS = 0x8B82; //Shader program link status
inline constexpr GLenum	GL_VALIDATE_STATUS = 0x8B83; //Shader program validation status

//Shader interface queries

inline constexpr GLenum	GL_ACTIVE_RESOURCES = 0x929F; //Active resource query
inline constexpr GLenum	GL_CURRENT_PROGRAM = 0x8B8D; //Currently active program
inline constexpr GLenum	GL_LOCATION = 0x930E; //Shader variable location
inline constexpr GLenum	GL_NAME_LENGTH = 0x92F9; //Length of active resource name
inline constexpr GLenum	GL_PROGRAM_INPUT = 0x92E3; //Program input interface
inline constexpr GLenum	GL_PROGRAM_OUTPUT = 0x92E4; //Program output interface
inline constexpr GLenum	GL_TYPE = 0x92FA; //Type of resource

//Uniform usage

inline constexpr GLboolean	GL_FALSE = 0; //Boolean false (as GLboolean)

//Texture usage

inline constexpr GLenum	GL_CLAMP_TO_EDGE = 0x812F; //Texture clamp mode
inline constexpr GLenum	GL_TEXTURE0 = 0x84C0; //First texture unit
inline constexpr GLenum	GL_TEXTURE_2D = 0x0DE1; //2D texture target
inline constexpr GLenum	GL_TEXTURE_BASE_LEVEL = 0x813C; //Base mipmap level
inline constexpr GLenum	GL_TEXTURE_MAX_LEVEL = 0x813D; //Max mipmap level

//Buffer targets

inline constexpr GLenum	GL_ARRAY_BUFFER = 0x8892; //Vertex attribute buffer
inline constexpr GLenum	GL_ARRAY_BUFFER_BINDING = 0x8894; //Currently bound vertex buffer

//Buffer usage

inline constexpr GLenum	GL_STATIC_DRAW = 0x88E4; //Data modified once, used many times

//Vertex attribute types

inline constexpr GLenum	GL_FLOAT = 0x1406; //float vertex attribute type

//Vertex attribute state queries

inline constexpr GLenum	GL_VERTEX_ARRAY_BINDING = 0x85B5; //Currently bound vertex array object
inline constexpr GLenum	GL_VERTEX_ATTRIB_ARRAY_ENABLED = 0x8622; //Whether a vertex attribute array is enabled
inline constexpr GLenum	GL_VERTEX_ATTRIB_ARRAY_SIZE = 0x8623; //Number of components per vertex attribute
inline constexpr GLenum	GL_VERTEX_ATTRIB_ARRAY_STRIDE = 0x8624; //Byte offset between consecutive attributes
inline constexpr GLenum	GL_VERTEX_ATTRIB_ARRAY_TYPE = 0x8625; //Data type of attribute components
inline constexpr GLenum	GL_VERTEX_ATTRIB_ARRAY_POINTER = 0x8645; //Memory location of the vertex attribute data

//Framebuffer object (FBO) defines

inline constexpr GLenum	GL_FRAMEBUFFER = 0x8D40; //Framebuffer object target
inline constexpr GLenum	GL_RENDERBUFFER = 0x8D41; //Renderbuffer object target
inline constexpr GLenum	GL_COLOR_ATTACHMENT0 = 0x8CE0; //First color attachment point for framebuffer
inline constexpr GLenum	GL_DEPTH24_STENCIL8 = 0x88F0; //Combined depth+stencil internal format
inline constexpr GLenum	GL_DEPTH_STENCIL_ATTACHMENT = 0x821A; //Attachment point for combined depth/stencil
inline constexpr GLenum	GL_FRAMEBUFFER_COMPLETE = 0x8CD5; //Framebuffer is complete and ready for rendering

//
// GEOMETRY
//

//Binds a named buffer to a specified buffer binding point
static inline void (APIENTRY* glBindBuffer)(
	GLenum target,
	GLuint buffer) = nullptr;

//Binds a vertex array object
static inline void (APIENTRY* glBindVertexArray)(
	GLuint array) = nullptr;

//Creates and initializes a buffer object's data store
static inline void (APIENTRY* glBufferData)(
	GLenum target,
	GLsizeiptr size,
	const void* data,
	GLenum usage) = nullptr;

//Deletes one or more named buffer objects
static inline void (APIENTRY* glDeleteBuffers)(
	GLsizei n,
	const GLuint* buffers) = nullptr;

//Deletes one or more named vertex array objects
static inline void (APIENTRY* glDeleteVertexArrays)(
	GLsizei n,
	const GLuint* arrays) = nullptr;

//Draws non-indexed primitives from array data
static inline void (APIENTRY* glDrawArrays)(
	GLenum mode,
	GLint first,
	GLsizei count) = nullptr;

//Draws indexed primitives using array data and element indices
static inline void (APIENTRY* glDrawElements)(
	GLenum mode,
	GLsizei count,
	GLenum type,
	const void* indices) = nullptr;

//Enables a generic vertex attribute array
static inline void (APIENTRY* glEnableVertexAttribArray)(
	GLuint index) = nullptr;

//Generates buffer object names
static inline void (APIENTRY* glGenBuffers)(
	GLsizei n,
	GLuint* buffers) = nullptr;

//Generates vertex array object names
static inline void (APIENTRY* glGenVertexArrays)(
	GLsizei n,
	GLuint* arrays) = nullptr;

//Retrieves parameter values for a vertex attribute array
static inline void (APIENTRY* glGetVertexAttribiv)(
	GLuint index,
	GLenum pname,
	GLint* params) = nullptr;

//Retrieves a pointer to a vertex attribute array parameter
static inline void (APIENTRY* glGetVertexAttribPointerv)(
	GLuint index,
	GLenum pname,
	void** pointer) = nullptr;

//Defines an array of generic vertex attribute data
static inline void (APIENTRY* glVertexAttribPointer)(
	GLuint index,
	GLint size,
	GLenum type,
	GLboolean normalized,
	GLsizei stride,
	const void* pointer) = nullptr;

//
// SHADERS
//

//Attaches a shader object to a program
static inline void (APIENTRY* glAttachShader)(
	GLuint program,
	GLuint shader) = nullptr;

//Compiles a shader object
static inline void (APIENTRY* glCompileShader)(
	GLuint shader) = nullptr;

//Creates a new shader program object
static inline GLuint(APIENTRY* glCreateProgram)(
	void) = nullptr;

//Creates a shader object of the specified type
static inline GLuint(APIENTRY* glCreateShader)(
	GLenum type) = nullptr;

//Deletes a shader object
static inline void (APIENTRY* glDeleteShader)(
	GLuint shader) = nullptr;

//Deletes a program object
static inline void (APIENTRY* glDeleteProgram)(
	GLuint program) = nullptr;

//Detaches a shader object from a program
static inline void (APIENTRY* glDetachShader)(
	GLuint shader) = nullptr;

//Retrieves information about an active attribute variable
static inline void (APIENTRY* glGetActiveAttrib)(
	GLuint program,
	GLuint index,
	GLsizei bufSize,
	GLsizei* length,
	GLint* size,
	GLenum* type,
	char* name) = nullptr;

//Returns the attribute location within a shader program
static inline GLint(APIENTRY* glGetAttribLocation)(
	GLuint program,
	const char* name) = nullptr;

//Retrieves a parameter from a program object
static inline void (APIENTRY* glGetProgramiv)(
	GLuint program,
	GLenum pname,
	GLint* params) = nullptr;

//Returns the information log for a program object
static inline void (APIENTRY* glGetProgramInfoLog)(
	GLuint program,
	GLsizei bufSize,
	GLsizei* length,
	char* infoLog) = nullptr;

//Retrieves a parameter from a shader object
static inline void (APIENTRY* glGetShaderiv)(
	GLuint shader,
	GLenum pname,
	GLint* params) = nullptr;

//Returns the information log for a shader object
static inline void (APIENTRY* glGetShaderInfoLog)(
	GLuint shader,
	GLsizei bufSize,
	GLsizei* length,
	char* infoLog) = nullptr;

//Links a program object
static inline void (APIENTRY* glLinkProgram)(
	GLuint program) = nullptr;

//Sets the source code for a shader
static inline void (APIENTRY* glShaderSource)(
	GLuint shader,
	GLsizei count,
	const char* const* string,
	const GLint* length) = nullptr;

//Activates a shader program for rendering
static inline void (APIENTRY* glUseProgram)(
	GLuint program) = nullptr;

//Validates a program object to see if it's executable
static inline void (APIENTRY* glValidateProgram)(
	GLuint program) = nullptr;

//Returns whether a given program name is a valid program object
static inline GLboolean(APIENTRY* glIsProgram)(
	GLuint program) = nullptr;

//
// UNIFORMS
//

//Retrieves the location of a uniform variable within a shader program
static inline GLint(APIENTRY* glGetUniformLocation)(
	GLuint program,
	const char* name) = nullptr;

//Sets a single float uniform value
static inline void (APIENTRY* glUniform1f)(
	GLint location,
	float v0) = nullptr;

//Sets a single integer uniform value
static inline void (APIENTRY* glUniform1i)(
	GLint location,
	GLint v0) = nullptr;

//Sets a vec2 uniform (2 float components)
static inline void (APIENTRY* glUniform2f)(
	GLint location,
	float v0,
	float v1) = nullptr;

//Sets a vec2 uniform from an array of values
static inline void (APIENTRY* glUniform2fv)(
	GLint location,
	GLsizei count,
	const float* value) = nullptr;

//Sets a vec3 uniform (3 float components)
static inline void (APIENTRY* glUniform3f)(
	GLint location,
	float v0,
	float v1,
	float v2) = nullptr;

//Sets a vec3 uniform from an array of values
static inline void (APIENTRY* glUniform3fv)(
	GLint location,
	GLsizei count,
	const float* value) = nullptr;

//Sets a vec4 uniform (4 float components)
static inline void (APIENTRY* glUniform4f)(
	GLint location,
	float v0,
	float v1,
	float v2,
	float v3) = nullptr;

//Sets a vec4 uniform from an array of values
static inline void (APIENTRY* glUniform4fv)(
	GLint location,
	GLsizei count,
	const float* value) = nullptr;

//Sets a 2×2 matrix uniform from an array of floats
static inline void (APIENTRY* glUniformMatrix2fv)(
	GLint location,
	GLsizei count,
	GLboolean transpose,
	const float* value) = nullptr;

//Sets a 3×3 matrix uniform from an array of floats
static inline void (APIENTRY* glUniformMatrix3fv)(
	GLint location,
	GLsizei count,
	GLboolean transpose,
	const float* value) = nullptr;

//Sets a 4×4 matrix uniform from an array of floats
static inline void (APIENTRY* glUniformMatrix4fv)(
	GLint location,
	GLsizei count,
	GLboolean transpose,
	const float* value) = nullptr;

//
// TEXTURES
//

//Binds a named texture to a texturing target
static inline void (APIENTRY* glBindTexture)(
	GLenum target,
	GLuint texture) = nullptr;

//Activates the specified texture unit
static inline void (APIENTRY* glActiveTexture)(
	GLenum texture) = nullptr;

//Deletes one or more named textures
static inline void (APIENTRY* glDeleteTextures)(
	GLsizei n,
	const GLuint* textures) = nullptr;

//Generates mipmaps for the currently bound texture
static inline void (APIENTRY* glGenerateMipmap)(
	GLenum target) = nullptr;

//Generates texture object names
static inline void (APIENTRY* glGenTextures)(
	GLsizei n,
	GLuint* textures) = nullptr;

//Specifies a two-dimensional texture image
static inline void (APIENTRY* glTexImage2D)(
	GLenum target,
	GLint level,
	GLint internalFormat,
	GLsizei width,
	GLsizei height,
	GLint border,
	GLenum format,
	GLenum type,
	const void* data) = nullptr;

//Sets texture parameters for the currently bound texture
static inline void (APIENTRY* glTexParameteri)(
	GLenum target,
	GLenum pname,
	GLint param) = nullptr;

//Specifies a subregion of an existing 2D texture image
static inline void (APIENTRY* glTexSubImage2D)(
	GLenum target,
	GLint level,
	GLint xoffset,
	GLint yoffset,
	GLsizei width,
	GLsizei height,
	GLenum format,
	GLenum type,
	const void* pixels) = nullptr;

//
// FRAMEBUFFERS AND RENDERBUFFERS
//

//Binds a renderbuffer to the renderbuffer target
static inline void (APIENTRY* glBindRenderbuffer)(
	GLenum target,
	GLuint renderbuffer) = nullptr;

//Binds a framebuffer to a framebuffer target
static inline void (APIENTRY* glBindFramebuffer)(
	GLenum target,
	GLuint framebuffer) = nullptr;

//Checks the completeness status of a framebuffer
static inline GLenum(APIENTRY* glCheckFramebufferStatus)(
	GLenum target) = nullptr;

//Attaches a renderbuffer to a framebuffer attachment point
static inline void (APIENTRY* glFramebufferRenderbuffer)(
	GLenum target,
	GLenum attachment,
	GLenum renderbuffertarget,
	GLuint renderbuffer) = nullptr;

//Attaches a 2D texture image to a framebuffer attachment point
static inline void (APIENTRY* glFramebufferTexture2D)(
	GLenum target,
	GLenum attachment,
	GLenum textarget,
	GLuint texture,
	GLint level) = nullptr;

//Generates renderbuffer object names
static inline void (APIENTRY* glGenRenderbuffers)(
	GLsizei n,
	GLuint* renderbuffers) = nullptr;

//Generates framebuffer object names
static inline void (APIENTRY* glGenFramebuffers)(
	GLsizei n,
	GLuint* framebuffers) = nullptr;

//Establishes data storage format and dimensions for a renderbuffer
static inline void (APIENTRY* glRenderbufferStorage)(
	GLenum target,
	GLenum internalformat,
	GLsizei width,
	GLsizei height) = nullptr;

//
// FRAME AND RENDER STATE
//

//Clears buffers to preset values
static inline void (APIENTRY* glClear)(
	GLbitfield mask) = nullptr;

//Specifies the clear color for color buffers
static inline void (APIENTRY* glClearColor)(
	float red,
	float green,
	float blue,
	float alpha) = nullptr;

//Disables a specific OpenGL capability
static inline void (APIENTRY* glDisable)(
	GLenum cap) = nullptr;

//Returns the last error flag raised
static inline GLenum(APIENTRY* glGetError)(
	void) = nullptr;

//Retrieves integer-valued parameters
static inline void (APIENTRY* glGetIntegerv)(
	GLenum pname,
	GLint* data) = nullptr;

//Returns a string describing the current GL connection
static inline const GLubyte* (APIENTRY* glGetString)(
	GLenum name) = nullptr;

//Sets the viewport transformation dimensions
static inline void (APIENTRY* glViewport)(
	GLint x,
	GLint y,
	GLsizei width,
	GLsizei height) = nullptr;

namespace KalaWindow::Graphics::OpenGL
{
	class KALAWINDOW_API OpenGLCore
	{
	public:
		//Initializes all OpenGL 3.3 functions
		static void InitializeAllFunctions();

		//Initializes a single OpenGL 3.3 function
		static void InitializeFunction(const char* name);

		//Returns true if the function is available
		static bool IsFunctionAvailable(const char* name);

		//Returns the address of an OpenGL function. Requires a current context.
		void* GetGLProcAddress(const char* name);
	};
}

#endif //KALAWINDOW_SUPPORT_OPENGL