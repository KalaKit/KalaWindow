//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <sstream>

#include "KalaHeaders/logging.hpp"
#include "KalaHeaders/core_types.hpp"

#include "graphics/opengl/opengl_functions_core.hpp"
#include "core/core.hpp"
#include "core/global_handles.hpp"

using KalaHeaders::Log;

using KalaWindow::Core::KalaWindowCore;
using KalaWindow::Core::GlobalHandle;
using namespace KalaWindow::Graphics::OpenGLFunctions;

using std::string;
using std::to_string;
using std::ostringstream;

struct CoreFunctionCheck
{
    const char* name;
    const void* ptr;
};

CoreFunctionCheck checks[] =
{
    //
    // DEBUGGING
    //

    { "glDebugMessageCallback", glDebugMessageCallback },
    { "glEnable",               glEnable },

    //
    // GEOMETRY
    //

    { "glBindBuffer",              glBindBuffer },
    { "glBindVertexArray",         glBindVertexArray },
    { "glBufferData",              glBufferData },
    { "glDeleteBuffers",           glDeleteBuffers },
    { "glDeleteVertexArrays",      glDeleteVertexArrays },
    { "glDrawArrays",              glDrawArrays },
    { "glDrawElements",            glDrawElements },
    { "glEnableVertexAttribArray", glEnableVertexAttribArray },
    { "glGenBuffers",              glGenBuffers },
    { "glGenVertexArrays",         glGenVertexArrays },
    { "glGetVertexAttribiv",       glGetVertexAttribiv },
    { "glGetVertexAttribPointerv", glGetVertexAttribPointerv },
    { "glVertexAttribPointer",     glVertexAttribPointer },

    //
    // SHADERS
    //

    { "glAttachShader",      glAttachShader },
    { "glCompileShader",     glCompileShader },
    { "glCreateProgram",     glCreateProgram },
    { "glCreateShader",      glCreateShader },
    { "glDeleteShader",      glDeleteShader },
    { "glDeleteProgram",     glDeleteProgram },
    { "glDetachShader",      glDetachShader },
    { "glGetActiveAttrib",   glGetActiveAttrib },
    { "glGetAttribLocation", glGetAttribLocation },
    { "glGetProgramiv",      glGetProgramiv },
    { "glGetProgramInfoLog", glGetProgramInfoLog },
    { "glGetShaderiv",       glGetShaderiv },
    { "glGetShaderInfoLog",  glGetShaderInfoLog },
    { "glLinkProgram",       glLinkProgram },
    { "glShaderSource",      glShaderSource },
    { "glUseProgram",        glUseProgram },
    { "glValidateProgram",   glValidateProgram },
    { "glIsProgram",         glIsProgram },

    //
    // UNIFORMS
    //

    { "glGetUniformLocation", glGetUniformLocation },
    { "glUniform1f",          glUniform1f },
    { "glUniform1i",          glUniform1i },
    { "glUniform2f",          glUniform2f },
    { "glUniform2fv",         glUniform2fv },
    { "glUniform3f",          glUniform3f },
    { "glUniform3fv",         glUniform3fv },
    { "glUniform4f",          glUniform4f },
    { "glUniform4fv",         glUniform4fv },
    { "glUniformMatrix2fv",   glUniformMatrix2fv },
    { "glUniformMatrix3fv",   glUniformMatrix3fv },
    { "glUniformMatrix4fv",   glUniformMatrix4fv },

    //
    // TEXTURES
    //

    { "glBindTexture",    glBindTexture },
    { "glActiveTexture",  glActiveTexture },
    { "glDeleteTextures", glDeleteTextures },
    { "glGenerateMipmap", glGenerateMipmap },
    { "glGenTextures",    glGenTextures },
    { "glTexImage2D",     glTexImage2D },
    { "glTexParameteri",  glTexParameteri },
    { "glTexSubImage2D",  glTexSubImage2D },

    //
    // FRAMEBUFFERS AND RENDERBUFFERS
    //

    { "glBindRenderbuffer",        glBindRenderbuffer },
    { "glBindFramebuffer",         glBindFramebuffer },
    { "glCheckFramebufferStatus",  glCheckFramebufferStatus },
    { "glFramebufferRenderbuffer", glFramebufferRenderbuffer },
    { "glFramebufferTexture2D",    glFramebufferTexture2D },
    { "glGenRenderbuffers",        glGenRenderbuffers },
    { "glGenFramebuffers",         glGenFramebuffers },
    { "glRenderbufferStorage",     glRenderbufferStorage },

    //
    // FRAME AND RENDER STATE
    //

    { "glClear",       glClear },
    { "glClearColor",  glClearColor },
    { "glDisable",     glDisable },
    { "glGetError",    glGetError },
    { "glGetIntegerv", glGetIntegerv },
    { "glGetString",   glGetString },
    { "glViewport",    glViewport }
};

namespace KalaWindow::Graphics::OpenGLFunctions
{
    //
    // DEBUGGING
    //

    PFNGLDEBUGMESSAGECALLBACKPROC glDebugMessageCallback = nullptr;
    PFNGLENABLEPROC               glEnable               = nullptr;

    void LIB_APIENTRY DebugCallback(
        GLenum source,
        GLenum type,
        GLuint id,
        GLenum severity,
        GLsizei length,
        const char* message,
        const void* userParam)
    {
        string sourceValue{};
        switch (source)
        {
            case GL_DEBUG_SOURCE_API:             sourceValue = "API";        break;
            case GL_DEBUG_SOURCE_SHADER_COMPILER: sourceValue = "Shader";     break;
            case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   sourceValue = "WindowSys";  break;
            case GL_DEBUG_SOURCE_APPLICATION:     sourceValue = "App";        break;
            case GL_DEBUG_SOURCE_THIRD_PARTY:     sourceValue = "3rdParty";   break;
            default:                              sourceValue = "Other";      break;
        }

        string typeValue{};
        switch (type)
        {
            case GL_DEBUG_TYPE_ERROR:               typeValue = "Error";        break;
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: typeValue = "Deprecated";   break;
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  typeValue = "Undefined";    break;
            case GL_DEBUG_TYPE_PORTABILITY:         typeValue = "Portability";  break;
            case GL_DEBUG_TYPE_PERFORMANCE:         typeValue = "Performance";  break;
            case GL_DEBUG_TYPE_MARKER:              typeValue = "Marker";       break;
            case GL_DEBUG_TYPE_PUSH_GROUP:          typeValue = "PushGrp";      break;
            case GL_DEBUG_TYPE_POP_GROUP:           typeValue = "PopGrp";       break;
            default:                                typeValue = "Other";        break;
        }

        string severityValue{};
        switch (severity)
        {
            case GL_DEBUG_SEVERITY_HIGH:         severityValue = "HIGH";    break;
            case GL_DEBUG_SEVERITY_MEDIUM:       severityValue = "MEDIUM";  break;
            case GL_DEBUG_SEVERITY_LOW:          severityValue = "LOW";     break;
            case GL_DEBUG_SEVERITY_NOTIFICATION: severityValue = "NOTIFY";  break;
            default:                             severityValue = "UNKNOWN"; break;
        }


#ifndef _DEBUG
        //Skip Notification logging if not in Debug mode
        if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) return;
#endif

        ostringstream oss{};

        oss << "[OpenGL Debug] "
            << "[" << sourceValue << "] "
            << "[" << typeValue << "] "
            << "[" << severityValue << "] "
            << "[" << to_string(id) << "]:\n"
            << string(message) << "\n";

        Log::Print(oss.str());
    }

    //
    // GEOMETRY
    //

    PFNGLBINDBUFFERPROC             glBindBuffer             = nullptr;
    PFNGLBINDVERTEXARRAYPROC        glBindVertexArray        = nullptr;
    PFNGLBUFFERDATAPROC             glBufferData             = nullptr;
    PFNGLDELETEBUFFERSPROC          glDeleteBuffers          = nullptr;
    PFNGLDELETEVERTEXARRAYSPROC     glDeleteVertexArrays     = nullptr;
    PFNGLDRAWARRAYSPROC             glDrawArrays             = nullptr;
    PFNGLDRAWELEMENTSPROC           glDrawElements           = nullptr;
    PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray = nullptr;
    PFNGLGENBUFFERSPROC             glGenBuffers             = nullptr;
    PFNGLGENVERTEXARRAYSPROC        glGenVertexArrays        = nullptr;
    PFNGLGETVERTEXATTRIBIVPROC      glGetVertexAttribiv      = nullptr;
    PFNGLGETVERTEXATTRIBPOINTERVPROC glGetVertexAttribPointerv = nullptr;
    PFNGLVERTEXATTRIBPOINTERPROC    glVertexAttribPointer    = nullptr;

    //
    // SHADERS
    //

    PFNGLATTACHSHADERPROC       glAttachShader       = nullptr;
    PFNGLCOMPILESHADERPROC      glCompileShader      = nullptr;
    PFNGLCREATEPROGRAMPROC      glCreateProgram      = nullptr;
    PFNGLCREATESHADERPROC       glCreateShader       = nullptr;
    PFNGLDELETESHADERPROC       glDeleteShader       = nullptr;
    PFNGLDELETEPROGRAMPROC      glDeleteProgram      = nullptr;
    PFNGLDETACHSHADERPROC       glDetachShader       = nullptr;
    PFNGLGETACTIVEATTRIBPROC    glGetActiveAttrib    = nullptr;
    PFNGLGETATTRIBLOCATIONPROC  glGetAttribLocation  = nullptr;
    PFNGLGETPROGRAMIVPROC       glGetProgramiv       = nullptr;
    PFNGLGETPROGRAMINFOLOGPROC  glGetProgramInfoLog  = nullptr;
    PFNGLGETSHADERIVPROC        glGetShaderiv        = nullptr;
    PFNGLGETSHADERINFOLOGPROC   glGetShaderInfoLog   = nullptr;
    PFNGLLINKPROGRAMPROC        glLinkProgram        = nullptr;
    PFNGLSHADERSOURCEPROC       glShaderSource       = nullptr;
    PFNGLUSEPROGRAMPROC         glUseProgram         = nullptr;
    PFNGLVALIDATEPROGRAMPROC    glValidateProgram    = nullptr;
    PFNGLISPROGRAMPROC          glIsProgram          = nullptr;

    //
    // UNIFORMS
    //

    PFNGLGETUNIFORMLOCATIONPROC   glGetUniformLocation   = nullptr;
    PFNGLUNIFORM1FPROC            glUniform1f            = nullptr;
    PFNGLUNIFORM1IPROC            glUniform1i            = nullptr;
    PFNGLUNIFORM2FPROC            glUniform2f            = nullptr;
    PFNGLUNIFORM2FVPROC           glUniform2fv           = nullptr;
    PFNGLUNIFORM3FPROC            glUniform3f            = nullptr;
    PFNGLUNIFORM3FVPROC           glUniform3fv           = nullptr;
    PFNGLUNIFORM4FPROC            glUniform4f            = nullptr;
    PFNGLUNIFORM4FVPROC           glUniform4fv           = nullptr;
    PFNGLUNIFORMMATRIX2FVPROC     glUniformMatrix2fv     = nullptr;
    PFNGLUNIFORMMATRIX3FVPROC     glUniformMatrix3fv     = nullptr;
    PFNGLUNIFORMMATRIX4FVPROC     glUniformMatrix4fv     = nullptr;

    //
    // TEXTURES
    //

    PFNGLBINDTEXTUREPROC     glBindTexture     = nullptr;
    PFNGLACTIVETEXTUREPROC   glActiveTexture   = nullptr;
    PFNGLDELETETEXTURESPROC  glDeleteTextures  = nullptr;
    PFNGLGENERATEMIPMAPPROC  glGenerateMipmap  = nullptr;
    PFNGLGENTEXTURESPROC     glGenTextures     = nullptr;
    PFNGLTEXIMAGE2DPROC      glTexImage2D      = nullptr;
    PFNGLTEXPARAMETERIPROC   glTexParameteri   = nullptr;
    PFNGLTEXSUBIMAGE2DPROC   glTexSubImage2D   = nullptr;

    //
    // FRAMEBUFFERS AND RENDERBUFFERS
    //

    PFNGLBINDRENDERBUFFERPROC       glBindRenderbuffer         = nullptr;
    PFNGLBINDFRAMEBUFFERPROC        glBindFramebuffer          = nullptr;
    PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus   = nullptr;
    PFNGLFRAMEBUFFERRENDERBUFFERPROC glFramebufferRenderbuffer = nullptr;
    PFNGLFRAMEBUFFERTEXTURE2DPROC   glFramebufferTexture2D     = nullptr;
    PFNGLGENRENDERBUFFERSPROC       glGenRenderbuffers         = nullptr;
    PFNGLGENFRAMEBUFFERSPROC        glGenFramebuffers          = nullptr;
    PFNGLRENDERBUFFERSTORAGEPROC    glRenderbufferStorage      = nullptr;

    //
    // FRAME AND RENDER STATE
    //

    PFNGLCLEARPROC        glClear        = nullptr;
    PFNGLCLEARCOLORPROC   glClearColor   = nullptr;
    PFNGLDISABLEPROC      glDisable      = nullptr;
    PFNGLGETERRORPROC     glGetError     = nullptr;
    PFNGLGETINTEGERVPROC  glGetIntegerv  = nullptr;
    PFNGLGETSTRINGPROC    glGetString    = nullptr;
    PFNGLVIEWPORTPROC     glViewport     = nullptr;

	void OpenGL_Functions_Core::LoadAllCoreFunctions()
	{
        //
        // DEBUGGING
        //

        glDebugMessageCallback = reinterpret_cast<PFNGLDEBUGMESSAGECALLBACKPROC>(wglGetProcAddress("glDebugMessageCallback"));
        glEnable               = reinterpret_cast<PFNGLENABLEPROC>              (wglGetProcAddress("glEnable"));

        //
        // GEOMETRY
        //

        glBindBuffer              = reinterpret_cast<PFNGLBINDBUFFERPROC>             (wglGetProcAddress("glBindBuffer"));
        glBindVertexArray         = reinterpret_cast<PFNGLBINDVERTEXARRAYPROC>        (wglGetProcAddress("glBindVertexArray"));
        glBufferData              = reinterpret_cast<PFNGLBUFFERDATAPROC>             (wglGetProcAddress("glBufferData"));
        glDeleteBuffers           = reinterpret_cast<PFNGLDELETEBUFFERSPROC>          (wglGetProcAddress("glDeleteBuffers"));
        glDeleteVertexArrays      = reinterpret_cast<PFNGLDELETEVERTEXARRAYSPROC>     (wglGetProcAddress("glDeleteVertexArrays"));
        glDrawArrays              = reinterpret_cast<PFNGLDRAWARRAYSPROC>             (wglGetProcAddress("glDrawArrays"));
        glDrawElements            = reinterpret_cast<PFNGLDRAWELEMENTSPROC>           (wglGetProcAddress("glDrawElements"));
        glEnableVertexAttribArray = reinterpret_cast<PFNGLENABLEVERTEXATTRIBARRAYPROC>(wglGetProcAddress("glEnableVertexAttribArray"));
        glGenBuffers              = reinterpret_cast<PFNGLGENBUFFERSPROC>             (wglGetProcAddress("glGenBuffers"));
        glGenVertexArrays         = reinterpret_cast<PFNGLGENVERTEXARRAYSPROC>        (wglGetProcAddress("glGenVertexArrays"));
        glGetVertexAttribiv       = reinterpret_cast<PFNGLGETVERTEXATTRIBIVPROC>      (wglGetProcAddress("glGetVertexAttribiv"));
        glGetVertexAttribPointerv = reinterpret_cast<PFNGLGETVERTEXATTRIBPOINTERVPROC>(wglGetProcAddress("glGetVertexAttribPointerv"));
        glVertexAttribPointer     = reinterpret_cast<PFNGLVERTEXATTRIBPOINTERPROC>    (wglGetProcAddress("glVertexAttribPointer"));

        //
        // SHADERS
        //

        glAttachShader        = reinterpret_cast<PFNGLATTACHSHADERPROC>       (wglGetProcAddress("glAttachShader"));
        glCompileShader       = reinterpret_cast<PFNGLCOMPILESHADERPROC>      (wglGetProcAddress("glCompileShader"));
        glCreateProgram       = reinterpret_cast<PFNGLCREATEPROGRAMPROC>      (wglGetProcAddress("glCreateProgram"));
        glCreateShader        = reinterpret_cast<PFNGLCREATESHADERPROC>       (wglGetProcAddress("glCreateShader"));
        glDeleteShader        = reinterpret_cast<PFNGLDELETESHADERPROC>       (wglGetProcAddress("glDeleteShader"));
        glDeleteProgram       = reinterpret_cast<PFNGLDELETEPROGRAMPROC>      (wglGetProcAddress("glDeleteProgram"));
        glDetachShader        = reinterpret_cast<PFNGLDETACHSHADERPROC>       (wglGetProcAddress("glDetachShader"));
        glGetActiveAttrib     = reinterpret_cast<PFNGLGETACTIVEATTRIBPROC>    (wglGetProcAddress("glGetActiveAttrib"));
        glGetAttribLocation   = reinterpret_cast<PFNGLGETATTRIBLOCATIONPROC>  (wglGetProcAddress("glGetAttribLocation"));
        glGetProgramiv        = reinterpret_cast<PFNGLGETPROGRAMIVPROC>       (wglGetProcAddress("glGetProgramiv"));
        glGetProgramInfoLog   = reinterpret_cast<PFNGLGETPROGRAMINFOLOGPROC>  (wglGetProcAddress("glGetProgramInfoLog"));
        glGetShaderiv         = reinterpret_cast<PFNGLGETSHADERIVPROC>        (wglGetProcAddress("glGetShaderiv"));
        glGetShaderInfoLog    = reinterpret_cast<PFNGLGETSHADERINFOLOGPROC>   (wglGetProcAddress("glGetShaderInfoLog"));
        glLinkProgram         = reinterpret_cast<PFNGLLINKPROGRAMPROC>        (wglGetProcAddress("glLinkProgram"));
        glShaderSource        = reinterpret_cast<PFNGLSHADERSOURCEPROC>       (wglGetProcAddress("glShaderSource"));
        glUseProgram          = reinterpret_cast<PFNGLUSEPROGRAMPROC>         (wglGetProcAddress("glUseProgram"));
        glValidateProgram     = reinterpret_cast<PFNGLVALIDATEPROGRAMPROC>    (wglGetProcAddress("glValidateProgram"));
        glIsProgram           = reinterpret_cast<PFNGLISPROGRAMPROC>          (wglGetProcAddress("glIsProgram"));

        //
        // UNIFORMS
        //

        glGetUniformLocation  = reinterpret_cast<PFNGLGETUNIFORMLOCATIONPROC> (wglGetProcAddress("glGetUniformLocation"));
        glUniform1f           = reinterpret_cast<PFNGLUNIFORM1FPROC>          (wglGetProcAddress("glUniform1f"));
        glUniform1i           = reinterpret_cast<PFNGLUNIFORM1IPROC>          (wglGetProcAddress("glUniform1i"));
        glUniform2f           = reinterpret_cast<PFNGLUNIFORM2FPROC>          (wglGetProcAddress("glUniform2f"));
        glUniform2fv          = reinterpret_cast<PFNGLUNIFORM2FVPROC>         (wglGetProcAddress("glUniform2fv"));
        glUniform3f           = reinterpret_cast<PFNGLUNIFORM3FPROC>          (wglGetProcAddress("glUniform3f"));
        glUniform3fv          = reinterpret_cast<PFNGLUNIFORM3FVPROC>         (wglGetProcAddress("glUniform3fv"));
        glUniform4f           = reinterpret_cast<PFNGLUNIFORM4FPROC>          (wglGetProcAddress("glUniform4f"));
        glUniform4fv          = reinterpret_cast<PFNGLUNIFORM4FVPROC>         (wglGetProcAddress("glUniform4fv"));
        glUniformMatrix2fv    = reinterpret_cast<PFNGLUNIFORMMATRIX2FVPROC>   (wglGetProcAddress("glUniformMatrix2fv"));
        glUniformMatrix3fv    = reinterpret_cast<PFNGLUNIFORMMATRIX3FVPROC>   (wglGetProcAddress("glUniformMatrix3fv"));
        glUniformMatrix4fv    = reinterpret_cast<PFNGLUNIFORMMATRIX4FVPROC>   (wglGetProcAddress("glUniformMatrix4fv"));

        //
        // TEXTURES
        //

        glBindTexture   = reinterpret_cast<PFNGLBINDTEXTUREPROC>   (wglGetProcAddress("glBindTexture"));
        glActiveTexture = reinterpret_cast<PFNGLACTIVETEXTUREPROC> (wglGetProcAddress("glActiveTexture"));
        glDeleteTextures= reinterpret_cast<PFNGLDELETETEXTURESPROC>(wglGetProcAddress("glDeleteTextures"));
        glGenerateMipmap= reinterpret_cast<PFNGLGENERATEMIPMAPPROC>(wglGetProcAddress("glGenerateMipmap"));
        glGenTextures   = reinterpret_cast<PFNGLGENTEXTURESPROC>   (wglGetProcAddress("glGenTextures"));
        glTexImage2D    = reinterpret_cast<PFNGLTEXIMAGE2DPROC>    (wglGetProcAddress("glTexImage2D"));
        glTexParameteri = reinterpret_cast<PFNGLTEXPARAMETERIPROC> (wglGetProcAddress("glTexParameteri"));
        glTexSubImage2D = reinterpret_cast<PFNGLTEXSUBIMAGE2DPROC> (wglGetProcAddress("glTexSubImage2D"));

        //
        // FRAMEBUFFERS AND RENDERBUFFERS
        //

        glBindRenderbuffer       = reinterpret_cast<PFNGLBINDRENDERBUFFERPROC>      (wglGetProcAddress("glBindRenderbuffer"));
        glBindFramebuffer        = reinterpret_cast<PFNGLBINDFRAMEBUFFERPROC>       (wglGetProcAddress("glBindFramebuffer"));
        glCheckFramebufferStatus = reinterpret_cast<PFNGLCHECKFRAMEBUFFERSTATUSPROC>(wglGetProcAddress("glCheckFramebufferStatus"));
        glFramebufferRenderbuffer= reinterpret_cast<PFNGLFRAMEBUFFERRENDERBUFFERPROC>(wglGetProcAddress("glFramebufferRenderbuffer"));
        glFramebufferTexture2D   = reinterpret_cast<PFNGLFRAMEBUFFERTEXTURE2DPROC>  (wglGetProcAddress("glFramebufferTexture2D"));
        glGenRenderbuffers       = reinterpret_cast<PFNGLGENRENDERBUFFERSPROC>      (wglGetProcAddress("glGenRenderbuffers"));
        glGenFramebuffers        = reinterpret_cast<PFNGLGENFRAMEBUFFERSPROC>       (wglGetProcAddress("glGenFramebuffers"));
        glRenderbufferStorage    = reinterpret_cast<PFNGLRENDERBUFFERSTORAGEPROC>   (wglGetProcAddress("glRenderbufferStorage"));

        //
        // FRAME AND RENDER STATE
        //

        glClear       = reinterpret_cast<PFNGLCLEARPROC>      (wglGetProcAddress("glClear"));
        glClearColor  = reinterpret_cast<PFNGLCLEARCOLORPROC> (wglGetProcAddress("glClearColor"));
        glDisable     = reinterpret_cast<PFNGLDISABLEPROC>    (wglGetProcAddress("glDisable"));
        glGetError    = reinterpret_cast<PFNGLGETERRORPROC>   (wglGetProcAddress("glGetError"));
        glGetIntegerv = reinterpret_cast<PFNGLGETINTEGERVPROC>(wglGetProcAddress("glGetIntegerv"));
        glGetString   = reinterpret_cast<PFNGLGETSTRINGPROC>  (wglGetProcAddress("glGetString"));
        glViewport    = reinterpret_cast<PFNGLVIEWPORTPROC>   (wglGetProcAddress("glViewport"));

        for (auto& check : checks)
        {
            if (!check.ptr)
            {
                KalaWindowCore::ForceClose(
                    "OpenGL Core function error",
                    "Failed to load function '" + string(check.name) + "'");
            }
        }
	}

    void OpenGL_Functions_Core::LoadCoreFunction(void** target, const char* name)
    {
        //check if already loaded
        auto it = find_if(
            loadedCoreFunctions.begin(),
            loadedCoreFunctions.end(),
            [name](const CoreGLFunction& rec) { return rec.name == name; });

        //already loaded - return existing one
        if (it != loadedCoreFunctions.end())
        {
            *target = it->ptr;
            return;
        }

        //try to load
#ifdef _WIN32
        *target = reinterpret_cast<void*>(wglGetProcAddress(name));
        if (!*target)
        {
            HMODULE module = ToVar<HMODULE>(GlobalHandle::GetOpenGLHandle());
            *target = reinterpret_cast<void*>(GetProcAddress(module, name));
        }
#elif __linux__
        * target = reinterpret_cast<T>(glXGetProcAddress(
            reinterpret_cast<const GLubyte*>(name)));
        if (!*target)
        {
            void* module = ToVar<void*>(GlobalHandle::GetOpenGLHandle());
            *target = reinterpret_cast<void*>(GetProcAddress(module, name));
        }
#endif

        if (!*target)
        {
            KalaWindowCore::ForceClose(
                "OpenGL Core function error",
                "Failed to load OpenGL error '" + string(name) + "'!");
        }

        loadedCoreFunctions.push_back(
            {
                name,
                *target
            });
    }
}