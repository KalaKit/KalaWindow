//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef KALAWINDOW_SUPPORT_OPENGL

#pragma once

#include <cstdint>
#include <cstddef>
#ifdef _WIN32
    #include <Windows.h>
#endif
#include <GL/gl.h>

//kalawindow
#include "core/platform.hpp"

#ifdef _WIN32
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

    #ifndef WGL_CONTEXT_MAJOR_VERSION_ARB
        #define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
    #endif

    #ifndef WGL_CONTEXT_MINOR_VERSION_ARB
        #define WGL_CONTEXT_MINOR_VERSION_ARB 0x2092
    #endif

    #ifndef WGL_CONTEXT_PROFILE_MASK_ARB
        #define WGL_CONTEXT_PROFILE_MASK_ARB 0x9126
    #endif

    #ifndef WGL_CONTEXT_CORE_PROFILE_BIT_ARB
        #define WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001
    #endif
#else
    #ifndef APIENTRY
        #define APIENTRY
    #endif

    #ifndef APIENTRYP
        #define APIENTRYP *
    #endif

    #ifndef WGLENTRY
        #define WGLENTRY
    #endif

    #ifndef WGLENTRYP
        #define WGLENTRYP *
    #endif

    #ifdef __linux__
        #ifndef GLX_CONTEXT_MAJOR_VERSION_ARB
            #define GLX_CONTEXT_MAJOR_VERSION_ARB 0x2091
        #endif

        #ifndef GLX_CONTEXT_MINOR_VERSION_ARB
            #define GLX_CONTEXT_MINOR_VERSION_ARB 0x2092
        #endif

        #ifndef GLX_CONTEXT_PROFILE_MASK_ARB
            #define GLX_CONTEXT_PROFILE_MASK_ARB 0x9126
        #endif

        #ifndef GLX_CONTEXT_CORE_PROFILE_BIT_ARB
            #define GLX_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001
        #endif
    #endif
#endif

//opengl error codes

#define GL_NO_ERROR                      0
#define GL_INVALID_ENUM                  0x0500
#define GL_INVALID_VALUE                 0x0501
#define GL_INVALID_OPERATION             0x0502
#define GL_STACK_OVERFLOW                0x0503
#define GL_STACK_UNDERFLOW               0x0504
#define GL_OUT_OF_MEMORY                 0x0505
#define GL_INVALID_FRAMEBUFFER_OPERATION 0x0506

//shader types

#define GL_VERTEX_SHADER     0x8B31
#define GL_FRAGMENT_SHADER   0x8B30
#define GL_GEOMETRY_SHADER   0x8DD9

//shader parameter enums

#define GL_COMPILE_STATUS    0x8B81
#define GL_LINK_STATUS       0x8B82
#define GL_INFO_LOG_LENGTH   0x8B84
#define GL_ACTIVE_ATTRIBUTES 0x8B89
#define GL_ACTIVE_UNIFORMS   0x8B86

//shader interface queries

#define GL_CURRENT_PROGRAM   0x8B8D
#define GL_PROGRAM_INPUT     0x92E3
#define GL_PROGRAM_OUTPUT    0x92E4
#ifndef GL_ACTIVE_RESOURCES
    #define GL_ACTIVE_RESOURCES  0x929F
#endif
#define GL_NAME_LENGTH       0x92F9
#define GL_TYPE              0x92FA
#define GL_LOCATION          0x930E

#define GL_VALIDATE_STATUS   0x8B83

//uniform usage

#define GL_FALSE 0

//texture usage

#define GL_TEXTURE0           0x84C0
#define GL_CLAMP_TO_EDGE      0x812F
#define GL_TEXTURE_2D         0x0DE1
#define GL_TEXTURE_BASE_LEVEL 0x813C
#define GL_TEXTURE_MAX_LEVEL  0x813D

//buffer targets

#define GL_ARRAY_BUFFER 0x8892
#define GL_ARRAY_BUFFER_BINDING 0x8894

//buffer usage

#define GL_STATIC_DRAW 0x88E4

//vertex attribute types

#define GL_FLOAT 0x1406

//vertex attribute state queries

#define GL_VERTEX_ARRAY_BINDING 0x85B5

#define GL_VERTEX_ATTRIB_ARRAY_ENABLED    0x8622
#define GL_VERTEX_ATTRIB_ARRAY_SIZE       0x8623
#define GL_VERTEX_ATTRIB_ARRAY_STRIDE     0x8624
#define GL_VERTEX_ATTRIB_ARRAY_TYPE       0x8625
#define GL_VERTEX_ATTRIB_ARRAY_POINTER    0x8645

// Framebuffer object (FBO) defines

#ifndef GL_FRAMEBUFFER
    #define GL_FRAMEBUFFER 0x8D40
#endif

#ifndef GL_RENDERBUFFER
    #define GL_RENDERBUFFER 0x8D41
#endif

#ifndef GL_COLOR_ATTACHMENT0
    #define GL_COLOR_ATTACHMENT0 0x8CE0
#endif

#ifndef GL_DEPTH24_STENCIL8
    #define GL_DEPTH24_STENCIL8 0x88F0
#endif

#ifndef GL_DEPTH_STENCIL_ATTACHMENT
    #define GL_DEPTH_STENCIL_ATTACHMENT 0x821A
#endif

#ifndef GL_FRAMEBUFFER_COMPLETE
    #define GL_FRAMEBUFFER_COMPLETE 0x8CD5
#endif

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

#ifdef _WIN32
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
typedef BOOL(WINAPI* PFNWGLSWAPINTERVALEXTPROC)(int interval);
#endif

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
typedef void (APIENTRYP PFNGLDELETEVERTEXARRAYSPROC)(
    GLsizei n,
    const GLuint* arrays);
typedef void (APIENTRYP PFNGLDELETEBUFFERSPROC)(
    GLsizei n,
    const GLuint* buffers);
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
typedef void (APIENTRYP PFNGLGETVERTEXATTRIBIVPROC)(
    GLuint index,
    GLenum pname,
    GLint* params);
typedef void (APIENTRYP PFNGLGETVERTEXATTRIBPOINTERVPROC)(
    GLuint index,
    GLenum pname,
    void** pointer);
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
typedef void (APIENTRYP PFNGLDETACHSHADERPROC)(
    GLuint shader);
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
typedef void (APIENTRYP PFNGLGETACTIVEATTRIBPROC)(
    GLuint program,
    GLuint index,
    GLsizei bufSize,
    GLsizei* length,
    GLint* size,
    GLenum* type,
    GLchar* name);
typedef GLint(APIENTRYP PFNGLGETATTRIBLOCATIONPROC)(
    GLuint program,
    const GLchar* name);
typedef void (APIENTRYP PFNGLDELETEPROGRAMPROC)(
    GLuint program);
typedef void (APIENTRYP PFNGLVALIDATEPROGRAMPROC)(
    GLuint program);
typedef GLboolean(APIENTRYP PFNGLISPROGRAMPROC)(
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
typedef void (APIENTRYP PFNGLACTIVETEXTUREPROC)(
    GLenum texture);
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
typedef void (APIENTRYP PFNGLTEXSUBIMAGE2DPROC)(
    GLenum target,
    GLint level,
    GLint xoffset,
    GLint yoffset,
    GLsizei width,
    GLsizei height,
    GLenum format,
    GLenum type,
    const void* pixels);
typedef void (APIENTRYP PFNGLTEXPARAMETERIPROC)(
    GLenum target,
    GLenum pname,
    GLint param);
typedef void (APIENTRYP PFNGLGENERATEMIPMAPPROC)(
    GLenum target);
typedef void (APIENTRYP PFNGLDELETETEXTURESPROC)(
    GLsizei n,
    const GLuint* textures);

//framebuffers and renderbuffers

typedef void (APIENTRYP PFNGLGENFRAMEBUFFERSPROC)(
    GLsizei n,
    GLuint* framebuffers);
typedef void (APIENTRYP PFNGLBINDFRAMEBUFFERPROC)(
    GLenum target,
    GLuint framebuffer);
typedef void (APIENTRYP PFNGLFRAMEBUFFERTEXTURE2DPROC)(
    GLenum target,
    GLenum attachment,
    GLenum textarget,
    GLuint texture,
    GLint level);
typedef GLenum(APIENTRYP PFNGLCHECKFRAMEBUFFERSTATUSPROC)(
    GLenum target);
typedef void (APIENTRYP PFNGLGENRENDERBUFFERSPROC)(
    GLsizei n,
    GLuint* renderbuffers);
typedef void (APIENTRYP PFNGLBINDRENDERBUFFERPROC)(
    GLenum target,
    GLuint renderbuffer);
typedef void (APIENTRYP PFNGLRENDERBUFFERSTORAGEPROC)(
    GLenum target,
    GLenum internalformat,
    GLsizei width,
    GLsizei height);
typedef void (APIENTRYP PFNGLFRAMEBUFFERRENDERBUFFERPROC)(
    GLenum target,
    GLenum attachment,
    GLenum renderbuffertarget,
    GLuint renderbuffer);

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
typedef void (APIENTRYP PFNGLGETINTEGERVPROC)(
    GLenum pname,
    GLint* data);
typedef const GLubyte* (APIENTRYP PFNGLGETSTRINGPROC)(
    GLenum name);
typedef GLenum(APIENTRYP PFNGLGETERRORPROC)(
    void);
#endif //KALAWINDOW_SUPPORT_OPENGL