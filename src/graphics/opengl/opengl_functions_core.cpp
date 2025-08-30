//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <sstream>
#include <vector>
#include <string>

#include "KalaHeaders/logging.hpp"
#include "KalaHeaders/core_types.hpp"

#include "graphics/opengl/opengl_functions_core.hpp"
#include "core/core.hpp"
#include "core/global_handles.hpp"

using KalaHeaders::Log;
using KalaHeaders::LogType;

using KalaWindow::Core::KalaWindowCore;
using KalaWindow::Core::GlobalHandle;
using namespace KalaWindow::Graphics::OpenGLFunctions;

using std::vector;
using std::string;
using std::to_string;
using std::ostringstream;

struct CoreGLFunction
{
    const char* name;
    void** target;
};

CoreGLFunction functions[] =
{
    //
    // DEBUGGING
    //

    { "glDebugMessageCallback", reinterpret_cast<void**>(&glDebugMessageCallback) },
    { "glEnable",               reinterpret_cast<void**>(&glEnable) },

    //
    // GEOMETRY
    //

    { "glBindBuffer",              reinterpret_cast<void**>(&glBindBuffer) },
    { "glBindVertexArray",         reinterpret_cast<void**>(&glBindVertexArray) },
    { "glBufferData",              reinterpret_cast<void**>(&glBufferData) },
    { "glDeleteBuffers",           reinterpret_cast<void**>(&glDeleteBuffers) },
    { "glDeleteVertexArrays",      reinterpret_cast<void**>(&glDeleteVertexArrays) },
    { "glDrawArrays",              reinterpret_cast<void**>(&glDrawArrays) },
    { "glDrawElements",            reinterpret_cast<void**>(&glDrawElements) },
    { "glEnableVertexAttribArray", reinterpret_cast<void**>(&glEnableVertexAttribArray) },
    { "glGenBuffers",              reinterpret_cast<void**>(&glGenBuffers) },
    { "glGenVertexArrays",         reinterpret_cast<void**>(&glGenVertexArrays) },
    { "glGetVertexAttribiv",       reinterpret_cast<void**>(&glGetVertexAttribiv) },
    { "glGetVertexAttribPointerv", reinterpret_cast<void**>(&glGetVertexAttribPointerv) },
    { "glVertexAttribPointer",     reinterpret_cast<void**>(&glVertexAttribPointer) },

    //
    // SHADERS
    //

    { "glAttachShader",      reinterpret_cast<void**>(&glAttachShader) },
    { "glCompileShader",     reinterpret_cast<void**>(&glCompileShader) },
    { "glCreateProgram",     reinterpret_cast<void**>(&glCreateProgram) },
    { "glCreateShader",      reinterpret_cast<void**>(&glCreateShader) },
    { "glDeleteShader",      reinterpret_cast<void**>(&glDeleteShader) },
    { "glDeleteProgram",     reinterpret_cast<void**>(&glDeleteProgram) },
    { "glDetachShader",      reinterpret_cast<void**>(&glDetachShader) },
    { "glGetActiveAttrib",   reinterpret_cast<void**>(&glGetActiveAttrib) },
    { "glGetAttribLocation", reinterpret_cast<void**>(&glGetAttribLocation) },
    { "glGetProgramiv",      reinterpret_cast<void**>(&glGetProgramiv) },
    { "glGetProgramInfoLog", reinterpret_cast<void**>(&glGetProgramInfoLog) },
    { "glGetShaderiv",       reinterpret_cast<void**>(&glGetShaderiv) },
    { "glGetShaderInfoLog",  reinterpret_cast<void**>(&glGetShaderInfoLog) },
    { "glLinkProgram",       reinterpret_cast<void**>(&glLinkProgram) },
    { "glShaderSource",      reinterpret_cast<void**>(&glShaderSource) },
    { "glUseProgram",        reinterpret_cast<void**>(&glUseProgram) },
    { "glValidateProgram",   reinterpret_cast<void**>(&glValidateProgram) },
    { "glIsProgram",         reinterpret_cast<void**>(&glIsProgram) },

    //
    // UNIFORMS
    //

    { "glGetUniformLocation", reinterpret_cast<void**>(&glGetUniformLocation) },
    { "glUniform1f",          reinterpret_cast<void**>(&glUniform1f) },
    { "glUniform1i",          reinterpret_cast<void**>(&glUniform1i) },
    { "glUniform2f",          reinterpret_cast<void**>(&glUniform2f) },
    { "glUniform2fv",         reinterpret_cast<void**>(&glUniform2fv) },
    { "glUniform3f",          reinterpret_cast<void**>(&glUniform3f) },
    { "glUniform3fv",         reinterpret_cast<void**>(&glUniform3fv) },
    { "glUniform4f",          reinterpret_cast<void**>(&glUniform4f) },
    { "glUniform4fv",         reinterpret_cast<void**>(&glUniform4fv) },
    { "glUniformMatrix2fv",   reinterpret_cast<void**>(&glUniformMatrix2fv) },
    { "glUniformMatrix3fv",   reinterpret_cast<void**>(&glUniformMatrix3fv) },
    { "glUniformMatrix4fv",   reinterpret_cast<void**>(&glUniformMatrix4fv) },

    //
    // TEXTURES
    //

    { "glBindTexture",    reinterpret_cast<void**>(&glBindTexture) },
    { "glActiveTexture",  reinterpret_cast<void**>(&glActiveTexture) },
    { "glDeleteTextures", reinterpret_cast<void**>(&glDeleteTextures) },
    { "glGenerateMipmap", reinterpret_cast<void**>(&glGenerateMipmap) },
    { "glGenTextures",    reinterpret_cast<void**>(&glGenTextures) },
    { "glTexImage2D",     reinterpret_cast<void**>(&glTexImage2D) },
    { "glTexImage3D",     reinterpret_cast<void**>(&glTexImage3D) },
    { "glCompressedTexImage2D", reinterpret_cast<void**>(&glCompressedTexImage2D) },
    { "glCompressedTexImage3D", reinterpret_cast<void**>(&glCompressedTexImage3D) },
    { "glTexStorage2D",   reinterpret_cast<void**>(&glTexStorage2D) },
    { "glTexStorage3D",   reinterpret_cast<void**>(&glTexStorage3D) },
    { "glTexSubImage2D",  reinterpret_cast<void**>(&glTexSubImage2D) },
    { "glTexSubImage3D",  reinterpret_cast<void**>(&glTexSubImage3D) },
    { "glCompressedTexSubImage2D",  reinterpret_cast<void**>(&glCompressedTexSubImage2D) },
    { "glCompressedTexSubImage3D",  reinterpret_cast<void**>(&glCompressedTexSubImage3D) },
    { "glTexParameteri",  reinterpret_cast<void**>(&glTexParameteri) },
    { "glTexParameteriv", reinterpret_cast<void**>(&glTexParameteriv) },
    { "glTexParameterf",  reinterpret_cast<void**>(&glTexParameterf) },
    { "glTexParameterfv", reinterpret_cast<void**>(&glTexParameterfv) },

    //
    // FRAMEBUFFERS AND RENDERBUFFERS
    //

    { "glBindRenderbuffer",        reinterpret_cast<void**>(&glBindRenderbuffer) },
    { "glBindFramebuffer",         reinterpret_cast<void**>(&glBindFramebuffer) },
    { "glCheckFramebufferStatus",  reinterpret_cast<void**>(&glCheckFramebufferStatus) },
    { "glFramebufferRenderbuffer", reinterpret_cast<void**>(&glFramebufferRenderbuffer) },
    { "glFramebufferTexture2D",    reinterpret_cast<void**>(&glFramebufferTexture2D) },
    { "glGenRenderbuffers",        reinterpret_cast<void**>(&glGenRenderbuffers) },
    { "glGenFramebuffers",         reinterpret_cast<void**>(&glGenFramebuffers) },
    { "glRenderbufferStorage",     reinterpret_cast<void**>(&glRenderbufferStorage) },

    //
    // FRAME AND RENDER STATE
    //

    { "glClear",       reinterpret_cast<void**>(&glClear) },
    { "glClearColor",  reinterpret_cast<void**>(&glClearColor) },
    { "glDisable",     reinterpret_cast<void**>(&glDisable) },
    { "glGetError",    reinterpret_cast<void**>(&glGetError) },
    { "glGetIntegerv", reinterpret_cast<void**>(&glGetIntegerv) },
    { "glGetString",   reinterpret_cast<void**>(&glGetString) },
    { "glGetStringi",   reinterpret_cast<void**>(&glGetStringi) },
    { "glViewport",    reinterpret_cast<void**>(&glViewport) }
};

static inline vector<CoreGLFunction> loadedCoreFunctions{};

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
    PFNGLTEXIMAGE3DPROC      glTexImage3D      = nullptr;
    PFNGLCOMPRESSEDTEXIMAGE2DPROC glCompressedTexImage2D = nullptr;
    PFNGLCOMPRESSEDTEXIMAGE3DPROC glCompressedTexImage3D = nullptr;
    PFNGLTEXSTORAGE2DPROC    glTexStorage2D    = nullptr;
    PFNGLTEXSTORAGE3DPROC    glTexStorage3D    = nullptr;
    PFNGLTEXSUBIMAGE2DPROC   glTexSubImage2D   = nullptr;
    PFNGLTEXSUBIMAGE3DPROC   glTexSubImage3D   = nullptr;
    PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC glCompressedTexSubImage2D = nullptr;
    PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC glCompressedTexSubImage3D = nullptr;
    PFNGLTEXPARAMETERIPROC   glTexParameteri   = nullptr;
    PFNGLTEXPARAMETERIVPROC  glTexParameteriv  = nullptr;
    PFNGLTEXPARAMETERFPROC   glTexParameterf   = nullptr;
    PFNGLTEXPARAMETERFVPROC  glTexParameterfv  = nullptr;

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
    PFNGLGETSTRINGIPROC   glGetStringi   = nullptr;
    PFNGLVIEWPORTPROC     glViewport     = nullptr;

	void OpenGL_Functions_Core::LoadAllCoreFunctions()
	{
        for (const auto& func : functions)
        {
            LoadCoreFunction(func.name);
        }
	}

    void OpenGL_Functions_Core::LoadCoreFunction(const char* name)
    {
        //check if already loaded
        auto it = find_if(
            loadedCoreFunctions.begin(),
            loadedCoreFunctions.end(),
            [name](const CoreGLFunction& rec) { return strcmp(rec.name, name) == 0; });

        //already loaded
        if (it != loadedCoreFunctions.end())
        {
            Log::Print(
                "Function '" + string(name) + "' is already loaded!",
                "OPENGL CORE FUNCTION",
                LogType::LOG_ERROR,
                2);

            return;
        }

        //find entry in registry
        CoreGLFunction* entry = nullptr;
        for (auto& f : functions)
        {
            if (strcmp(f.name, name) == 0)
            {
                entry = &f;
                break;
            }
        }
        if (!entry)
        {
            Log::Print(
                "Function '" + string(name) + "' does not exist!",
                "OPENGL CORE FUNCTION",
                LogType::LOG_ERROR,
                2);

            return;
        }

        //try to load
        void* ptr = nullptr;

#ifdef _WIN32
        ptr = reinterpret_cast<void*>(wglGetProcAddress(name));
        if (!ptr)
        {
            Log::Print(
                "Failed to load function '" + string(name) + "'! Trying again with handle.",
                "OPENGL CORE FUNCTION",
                LogType::LOG_DEBUG);

            HMODULE module = ToVar<HMODULE>(GlobalHandle::GetOpenGLHandle());
            ptr = reinterpret_cast<void*>(GetProcAddress(module, name));
        }
#else
        ptr = reinterpret_cast<void*>(glXGetProcAddress(
            reinterpret_cast<const GLubyte*>(name)));
        if (!ptr)
        {
            void* module = ToVar<void*>(GlobalHandle::GetOpenGLHandle());
            ptr = reinterpret_cast<void*>(GetProcAddress(module, name));
        }
#endif

        if (!ptr)
        {
            KalaWindowCore::ForceClose(
                "OpenGL Core function error",
                "Failed to load OpenGL error '" + string(name) + "'!");
        }

        //assign into the real extern global
        *entry->target = ptr;

        loadedCoreFunctions.push_back(CoreGLFunction
            {
                entry->name,
                entry->target
            });

        Log::Print(
            "Loaded '" + string(name) + "'!",
            "OPENGL CORE FUNCTION",
            LogType::LOG_DEBUG);
    }
}