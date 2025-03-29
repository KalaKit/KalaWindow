//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <Windows.h>
#include <GL/gl.h>
#include <cstdint>
#include <cstddef>

#ifndef APIENTRY
    #define APIENTRY __stdcall
#endif

#ifndef APIENTRYP
    #define APIENTRYP APIENTRY *
#endif

#ifndef WGLENTRY
    #define WGLENTRY __stdcall
#endif

#ifndef WGLENTRYP
    #define WGLENTRYP WGLENTRY *
#endif

//shader types

#define GL_VERTEX_SHADER   0x8B31
#define GL_FRAGMENT_SHADER 0x8B30

//shader parameter enums

#define GL_COMPILE_STATUS  0x8B81
#define GL_LINK_STATUS     0x8B82

//uniform usage

#define GL_FALSE 0

//buffer targets

#define GL_ARRAY_BUFFER 0x8892

//buffer usage

#define GL_STATIC_DRAW 0x88E4

//vertex attribute types

#define GL_FLOAT 0x1406

#ifndef GL_TYPES_DEFINED
#define GL_TYPES_DEFINED

typedef unsigned int    GLenum;
typedef unsigned char   GLboolean;
typedef unsigned int    GLbitfield;
typedef void            GLvoid;
typedef signed char     GLbyte;
typedef short           GLshort;
typedef int             GLint;
typedef int             GLsizei;
typedef unsigned char   GLubyte;
typedef unsigned short  GLushort;
typedef unsigned int    GLuint;
typedef float           GLfloat;
typedef float           GLclampf;
typedef double          GLdouble;
typedef double          GLclampd;
typedef char            GLchar;
typedef ptrdiff_t       GLintptr;
typedef ptrdiff_t       GLsizeiptr;

#endif // GL_TYPES_DEFINED

//wgl extension typedefs

typedef HGLRC(WINAPI* PFNWGLCREATECONTEXTATTRIBSARBPROC)(
    HDC, 
    HGLRC, 
    const int*);
typedef BOOL(WINAPI* PFNWGLCHOOSEPIXELFORMATARBPROC)(
	HDC,
	const int*,
	const FLOAT*,
	UINT,
	int*,
	UINT*);

//geometry

typedef void (APIENTRYP PFNGLGENVERTEXARRAYSPROC)(
    GLsizei, 
    GLuint*);
typedef void (APIENTRYP PFNGLBINDVERTEXARRAYPROC)(
    GLuint);
typedef void (APIENTRYP PFNGLGENBUFFERSPROC)(
    GLsizei, 
    GLuint*);
typedef void (APIENTRYP PFNGLBINDBUFFERPROC)(
    GLenum, 
    GLuint);
typedef void (APIENTRYP PFNGLBUFFERDATAPROC)(
    GLenum, 
    GLsizeiptr, 
    const void*, 
    GLenum);
typedef void (APIENTRYP PFNGLENABLEVERTEXATTRIBARRAYPROC)(
    GLuint);
typedef void (APIENTRYP PFNGLVERTEXATTRIBPOINTERPROC)(
    GLuint index,
    GLint size,
    GLenum type,
    GLboolean normalized,
    GLsizei stride,
    const void* pointer);
typedef void (APIENTRYP PFNGLDRAWARRAYSPROC)(
    GLenum mode, 
    GLint first, 
    GLsizei count);
typedef void (APIENTRYP PFNGLDRAWELEMENTSPROC)(
    GLenum mode, 
    GLsizei count, 
    GLenum type, 
    const void* indices);

//shaders

typedef GLuint(APIENTRYP PFNGLCREATESHADERPROC)(
    GLenum type);
typedef void (APIENTRYP PFNGLSHADERSOURCEPROC)(
    GLuint shader, 
    GLsizei count, 
    const GLchar* const* string, 
    const GLint* length);
typedef void (APIENTRYP PFNGLCOMPILESHADERPROC)(
    GLuint shader);
typedef GLuint(APIENTRYP PFNGLCREATEPROGRAMPROC)(
    void);
typedef void (APIENTRYP PFNGLUSEPROGRAMPROC)(
    GLuint program);
typedef void (APIENTRYP PFNGLATTACHSHADERPROC)(
    GLuint program, 
    GLuint shader);
typedef void (APIENTRYP PFNGLLINKPROGRAMPROC)(
    GLuint program);
typedef void (APIENTRYP PFNGLDELETESHADERPROC)(
    GLuint shader);
typedef void (APIENTRYP PFNGLGETSHADERIVPROC)(
    GLuint shader, 
    GLenum pname, 
    GLint* params);
typedef void (APIENTRYP PFNGLGETSHADERINFOLOGPROC)(
    GLuint shader, 
    GLsizei bufSize, 
    GLsizei* length, 
    GLchar* infoLog);
typedef void (APIENTRYP PFNGLGETPROGRAMIVPROC)(
    GLuint program, 
    GLenum pname, 
    GLint* params);
typedef void (APIENTRYP PFNGLGETPROGRAMINFOLOGPROC)(
    GLuint program, 
    GLsizei bufSize, 
    GLsizei* length, 
    GLchar* infoLog);
typedef void (APIENTRYP PFNGLDELETEPROGRAMPROC)(
    GLuint program);

//uniforms

typedef GLint(APIENTRYP PFNGLGETUNIFORMLOCATIONPROC)(
    GLuint program,
    const GLchar* name);
typedef void (APIENTRYP PFNGLUNIFORM1IPROC)(
    GLint location,
    GLint v0);
typedef void (APIENTRYP PFNGLUNIFORM1FPROC)(
    GLint location,
    GLfloat v0);
typedef void (APIENTRYP PFNGLUNIFORM2FPROC)(
    GLint location,
    GLfloat v0,
    GLfloat v1);
typedef void (APIENTRYP PFNGLUNIFORM2FVPROC)(
    GLint location,
    GLsizei count,
    const GLfloat* value);
typedef void (APIENTRYP PFNGLUNIFORM3FPROC)(
    GLint location,
    GLfloat v0,
    GLfloat v1,
    GLfloat v2);
typedef void (APIENTRYP PFNGLUNIFORM3FVPROC)(
    GLint location,
    GLsizei count,
    const GLfloat* value);
typedef void (APIENTRYP PFNGLUNIFORM4FPROC)(
    GLint location,
    GLfloat v0,
    GLfloat v1,
    GLfloat v2,
    GLfloat v3);
typedef void (APIENTRYP PFNGLUNIFORM4FVPROC)(
    GLint location,
    GLsizei count,
    const GLfloat* value);
typedef void (APIENTRYP PFNGLUNIFORMMATRIX2FVPROC)(
    GLint location,
    GLsizei count,
    GLboolean transpose,
    const GLfloat* value);
typedef void (APIENTRYP PFNGLUNIFORMMATRIX3FVPROC)(
    GLint location,
    GLsizei count,
    GLboolean transpose,
    const GLfloat* value);
typedef void (APIENTRYP PFNGLUNIFORMMATRIX4FVPROC)(
    GLint location,
    GLsizei count,
    GLboolean transpose,
    const GLfloat* value);

//textures

typedef void (APIENTRYP PFNGLGENTEXTURESPROC)(
    GLsizei n, 
    GLuint* textures);
typedef void (APIENTRYP PFNGLBINDTEXTUREPROC)(
    GLenum target,
    GLuint texture);
typedef void (APIENTRYP PFNGLTEXIMAGE2DPROC)(
    GLenum target, 
    GLint level, 
    GLint internalFormat,
    GLsizei width, 
    GLsizei height, 
    GLint border,
    GLenum format, 
    GLenum type, 
    const void* data);
typedef void (APIENTRYP PFNGLTEXPARAMETERIPROC)(
    GLenum target, 
    GLenum pname, 
    GLint param);
typedef void (APIENTRYP PFNGLGENERATEMIPMAPPROC)(
    GLenum target);

//frame and render state

typedef void (APIENTRYP PFNGLVIEWPORTPROC)(
    GLint x,
    GLint y,
    GLsizei width,
    GLsizei height);
typedef void (APIENTRYP PFNGLDISABLEPROC)(
    GLenum cap);
typedef void (APIENTRYP PFNGLCLEARCOLORPROC)(
    GLfloat red, 
    GLfloat green, 
    GLfloat blue, 
    GLfloat alpha);
typedef void (APIENTRYP PFNGLCLEARPROC)(
    GLbitfield mask);