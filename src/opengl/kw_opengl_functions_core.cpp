//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <sstream>
#include <string>

#ifdef __linux__
#include <dlfcn.h>
#endif

#include "KalaHeaders/log_utils.hpp"
#include "KalaHeaders/core_utils.hpp"

#include "opengl/kw_opengl_functions_core.hpp"
#include "opengl/kw_opengl.hpp"
#include "core/kw_core.hpp"

using KalaHeaders::KalaCore::ToVar;
using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;

using KalaWindow::Core::KalaWindowCore;
using namespace KalaWindow::OpenGL::OpenGLFunctions;
using KalaWindow::OpenGL::OpenGL_Global;

using std::string;
using std::to_string;
using std::ostringstream;

struct CoreGLFunction
{
    const char* name;
    void** target;
};

namespace KalaWindow::OpenGL::OpenGLFunctions
{
    static GL_Core glCore{};

    const GL_Core* OpenGL_Functions_Core::GetGLCore()
    {
        return &glCore;
    }

    //
    // LOADED GL FUNCTIONS
    //

    CoreGLFunction functions[] =
    {
        //
        // DEBUGGING
        //

        { "glDebugMessageCallback", reinterpret_cast<void**>(&glCore.glDebugMessageCallback) },
        { "glGetError",             reinterpret_cast<void**>(&glCore.glGetError) },

        //
        // GEOMETRY
        //

        { "glMapBufferRange",          reinterpret_cast<void**>(&glCore.glMapBufferRange) },
        { "glBufferStorage",           reinterpret_cast<void**>(&glCore.glBufferStorage) },
        { "glBindBuffer",              reinterpret_cast<void**>(&glCore.glBindBuffer) },
        { "glBindVertexArray",         reinterpret_cast<void**>(&glCore.glBindVertexArray) },
        { "glBufferData",              reinterpret_cast<void**>(&glCore.glBufferData) },
        { "glBufferSubData",           reinterpret_cast<void**>(&glCore.glBufferSubData) },
        { "glDeleteBuffers",           reinterpret_cast<void**>(&glCore.glDeleteBuffers) },
        { "glDeleteVertexArrays",      reinterpret_cast<void**>(&glCore.glDeleteVertexArrays) },
        { "glDrawArrays",              reinterpret_cast<void**>(&glCore.glDrawArrays) },
        { "glDrawElements",            reinterpret_cast<void**>(&glCore.glDrawElements) },
        { "glEnableVertexAttribArray", reinterpret_cast<void**>(&glCore.glEnableVertexAttribArray) },
        { "glGenBuffers",              reinterpret_cast<void**>(&glCore.glGenBuffers) },
        { "glGenVertexArrays",         reinterpret_cast<void**>(&glCore.glGenVertexArrays) },
        { "glGetVertexAttribiv",       reinterpret_cast<void**>(&glCore.glGetVertexAttribiv) },
        { "glGetVertexAttribPointerv", reinterpret_cast<void**>(&glCore.glGetVertexAttribPointerv) },
        { "glVertexAttribPointer",     reinterpret_cast<void**>(&glCore.glVertexAttribPointer) },
        { "glCullFace",                reinterpret_cast<void**>(&glCore.glCullFace) },

        //
        // SHADERS
        //

        { "glAttachShader",      reinterpret_cast<void**>(&glCore.glAttachShader) },
        { "glCompileShader",     reinterpret_cast<void**>(&glCore.glCompileShader) },
        { "glCreateProgram",     reinterpret_cast<void**>(&glCore.glCreateProgram) },
        { "glCreateShader",      reinterpret_cast<void**>(&glCore.glCreateShader) },
        { "glDeleteShader",      reinterpret_cast<void**>(&glCore.glDeleteShader) },
        { "glDeleteProgram",     reinterpret_cast<void**>(&glCore.glDeleteProgram) },
        { "glDetachShader",      reinterpret_cast<void**>(&glCore.glDetachShader) },
        { "glGetActiveAttrib",   reinterpret_cast<void**>(&glCore.glGetActiveAttrib) },
        { "glGetAttribLocation", reinterpret_cast<void**>(&glCore.glGetAttribLocation) },
        { "glGetProgramiv",      reinterpret_cast<void**>(&glCore.glGetProgramiv) },
        { "glGetProgramInfoLog", reinterpret_cast<void**>(&glCore.glGetProgramInfoLog) },
        { "glGetShaderiv",       reinterpret_cast<void**>(&glCore.glGetShaderiv) },
        { "glGetShaderInfoLog",  reinterpret_cast<void**>(&glCore.glGetShaderInfoLog) },
        { "glLinkProgram",       reinterpret_cast<void**>(&glCore.glLinkProgram) },
        { "glShaderSource",      reinterpret_cast<void**>(&glCore.glShaderSource) },
        { "glUseProgram",        reinterpret_cast<void**>(&glCore.glUseProgram) },
        { "glValidateProgram",   reinterpret_cast<void**>(&glCore.glValidateProgram) },
        { "glIsProgram",         reinterpret_cast<void**>(&glCore.glIsProgram) },

        //
        // UNIFORMS
        //

        { "glGetUniformLocation",   reinterpret_cast<void**>(&glCore.glGetUniformLocation) },
        { "glGetUniformBlockIndex", reinterpret_cast<void**>(&glCore.glGetUniformBlockIndex) },
        { "glUniformBlockBinding",  reinterpret_cast<void**>(&glCore.glUniformBlockBinding) },
        { "glUniform1f",            reinterpret_cast<void**>(&glCore.glUniform1f) },
        { "glUniform1i",            reinterpret_cast<void**>(&glCore.glUniform1i) },
        { "glUniform1fv",           reinterpret_cast<void**>(&glCore.glUniform1fv) },
        { "glUniform1iv",           reinterpret_cast<void**>(&glCore.glUniform1iv) },
        { "glUniform2f",            reinterpret_cast<void**>(&glCore.glUniform2f) },
        { "glUniform2i",            reinterpret_cast<void**>(&glCore.glUniform2i) },
        { "glUniform2fv",           reinterpret_cast<void**>(&glCore.glUniform2fv) },
        { "glUniform2iv",           reinterpret_cast<void**>(&glCore.glUniform2iv) },
        { "glUniform3f",            reinterpret_cast<void**>(&glCore.glUniform3f) },
        { "glUniform3i",            reinterpret_cast<void**>(&glCore.glUniform3i) },
        { "glUniform3fv",           reinterpret_cast<void**>(&glCore.glUniform3fv) },
        { "glUniform3iv",           reinterpret_cast<void**>(&glCore.glUniform3iv) },
        { "glUniform4f",            reinterpret_cast<void**>(&glCore.glUniform4f) },
        { "glUniform4i",            reinterpret_cast<void**>(&glCore.glUniform4i) },
        { "glUniform4fv",           reinterpret_cast<void**>(&glCore.glUniform4fv) },
        { "glUniform4iv",           reinterpret_cast<void**>(&glCore.glUniform4iv) },
        { "glUniformMatrix2fv",     reinterpret_cast<void**>(&glCore.glUniformMatrix2fv) },
        { "glUniformMatrix3fv",     reinterpret_cast<void**>(&glCore.glUniformMatrix3fv) },
        { "glUniformMatrix4fv",     reinterpret_cast<void**>(&glCore.glUniformMatrix4fv) },

        //
        // TEXTURES
        //

        { "glBindTexture",    reinterpret_cast<void**>(&glCore.glBindTexture) },
        { "glActiveTexture",  reinterpret_cast<void**>(&glCore.glActiveTexture) },
        { "glDeleteTextures", reinterpret_cast<void**>(&glCore.glDeleteTextures) },
        { "glGenerateMipmap", reinterpret_cast<void**>(&glCore.glGenerateMipmap) },
        { "glGenTextures",    reinterpret_cast<void**>(&glCore.glGenTextures) },
        { "glTexImage2D",     reinterpret_cast<void**>(&glCore.glTexImage2D) },
        { "glTexImage3D",     reinterpret_cast<void**>(&glCore.glTexImage3D) },
        { "glCompressedTexImage2D", reinterpret_cast<void**>(&glCore.glCompressedTexImage2D) },
        { "glCompressedTexImage3D", reinterpret_cast<void**>(&glCore.glCompressedTexImage3D) },
        { "glTexStorage2D",   reinterpret_cast<void**>(&glCore.glTexStorage2D) },
        { "glTexStorage3D",   reinterpret_cast<void**>(&glCore.glTexStorage3D) },
        { "glTexSubImage2D",  reinterpret_cast<void**>(&glCore.glTexSubImage2D) },
        { "glTexSubImage3D",  reinterpret_cast<void**>(&glCore.glTexSubImage3D) },
        { "glCompressedTexSubImage2D",  reinterpret_cast<void**>(&glCore.glCompressedTexSubImage2D) },
        { "glCompressedTexSubImage3D",  reinterpret_cast<void**>(&glCore.glCompressedTexSubImage3D) },
        { "glTexParameteri",  reinterpret_cast<void**>(&glCore.glTexParameteri) },
        { "glTexParameteriv", reinterpret_cast<void**>(&glCore.glTexParameteriv) },
        { "glTexParameterf",  reinterpret_cast<void**>(&glCore.glTexParameterf) },
        { "glTexParameterfv", reinterpret_cast<void**>(&glCore.glTexParameterfv) },
        { "glPixelStorei",    reinterpret_cast<void**>(&glCore.glPixelStorei) },
        { "glPixelStoref",    reinterpret_cast<void**>(&glCore.glPixelStoref) },

        //
        // FRAMEBUFFERS AND RENDERBUFFERS
        //

        { "glBindRenderbuffer",        reinterpret_cast<void**>(&glCore.glBindRenderbuffer) },
        { "glBindFramebuffer",         reinterpret_cast<void**>(&glCore.glBindFramebuffer) },
        { "glBindBufferBase",          reinterpret_cast<void**>(&glCore.glBindBufferBase) },
        { "glCheckFramebufferStatus",  reinterpret_cast<void**>(&glCore.glCheckFramebufferStatus) },
        { "glFramebufferRenderbuffer", reinterpret_cast<void**>(&glCore.glFramebufferRenderbuffer) },
        { "glFramebufferTexture2D",    reinterpret_cast<void**>(&glCore.glFramebufferTexture2D) },
        { "glGenRenderbuffers",        reinterpret_cast<void**>(&glCore.glGenRenderbuffers) },
        { "glGenFramebuffers",         reinterpret_cast<void**>(&glCore.glGenFramebuffers) },
        { "glRenderbufferStorage",     reinterpret_cast<void**>(&glCore.glRenderbufferStorage) },
        { "glDepthFunc",               reinterpret_cast<void**>(&glCore.glDepthFunc) },
        { "glDepthMask",               reinterpret_cast<void**>(&glCore.glDepthMask) },
        { "glBlendColor",              reinterpret_cast<void**>(&glCore.glBlendColor) },
        { "glBlendFunc",               reinterpret_cast<void**>(&glCore.glBlendFunc) },
        { "glBlendFunci",              reinterpret_cast<void**>(&glCore.glBlendFunci) },
        { "glBlendEquation",           reinterpret_cast<void**>(&glCore.glBlendEquation) },
        { "glBlendEquationi",          reinterpret_cast<void**>(&glCore.glBlendEquationi) },
        { "glBlendEquationSeparate",   reinterpret_cast<void**>(&glCore.glBlendEquationSeparate) },
        { "glBlendEquationSeparatei",  reinterpret_cast<void**>(&glCore.glBlendEquationSeparatei) },
        { "glStencilFunc",             reinterpret_cast<void**>(&glCore.glStencilFunc) },
        { "glStencilFuncSeparate",     reinterpret_cast<void**>(&glCore.glStencilFuncSeparate) },
        { "glStencilMask",             reinterpret_cast<void**>(&glCore.glStencilMask) },
        { "glStencilMaskSeparate",     reinterpret_cast<void**>(&glCore.glStencilMaskSeparate) },
        { "glStencilOp",               reinterpret_cast<void**>(&glCore.glStencilOp) },
        { "glStencilOpSeparate",       reinterpret_cast<void**>(&glCore.glStencilOpSeparate) },

        //
        // FRAME AND RENDER STATE
        //

        { "glClear",       reinterpret_cast<void**>(&glCore.glClear) },
        { "glClearColor",  reinterpret_cast<void**>(&glCore.glClearColor) },
        { "glEnable",      reinterpret_cast<void**>(&glCore.glEnable) },
        { "glDisable",     reinterpret_cast<void**>(&glCore.glDisable) },
        { "glFrontFace",   reinterpret_cast<void**>(&glCore.glFrontFace) },
        { "glGetBooleanv", reinterpret_cast<void**>(&glCore.glGetBooleanv) },
        { "glGetIntegerv", reinterpret_cast<void**>(&glCore.glGetIntegerv) },
        { "glGetFloatv",   reinterpret_cast<void**>(&glCore.glGetFloatv) },
        { "glGetDoublev",  reinterpret_cast<void**>(&glCore.glGetDoublev) },
        { "glGetString",   reinterpret_cast<void**>(&glCore.glGetString) },
        { "glGetStringi",  reinterpret_cast<void**>(&glCore.glGetStringi) },
        { "glViewport",    reinterpret_cast<void**>(&glCore.glViewport) }
    };

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
        //skip Notification logging if not in debug
        if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
        {
            return;
        }
#endif

        ostringstream oss{};

        oss << "[OpenGL Debug] "
            << "[" << sourceValue << "] "
            << "[" << typeValue << "] "
            << "[" << severityValue << "] "
            << "[" << to_string(id) << "]:\n"
            << string(message) << "\n";

        Log::Print(oss.str());

        if (severity == GL_DEBUG_SEVERITY_HIGH)
        {
            KalaWindowCore::ForceClose(
                "OpenGL critical error",
                oss.str());

            return;
        }
    }

	void OpenGL_Functions_Core::LoadAllCoreFunctions()
	{
        for (auto& entry : functions)
        {
            //try to load
            void* ptr = nullptr;

#ifdef _WIN32
            ptr = reinterpret_cast<void*>(wglGetProcAddress(entry.name));
            if (!ptr)
            {
                HMODULE module = ToVar<HMODULE>(OpenGL_Global::GetOpenGLLibrary());
                ptr = reinterpret_cast<void*>(GetProcAddress(module, entry.name));
            }
#else
            ptr = reinterpret_cast<void*>(glXGetProcAddress(
                reinterpret_cast<const GLubyte*>(entry.name)));

            if (!ptr)
            {
                void* module = ToVar<void*>(OpenGL_Global::GetOpenGLHandle());
                ptr = dlsym(module, entry.name);
            }
#endif

            if (!ptr)
            {
                KalaWindowCore::ForceClose(
                    "OpenGL Core function error",
                    "Failed to load OpenGL function '" + string(entry.name) + "'!");
            }

            //assign into GL_Core dispatch table
            *entry.target = ptr;

            Log::Print(
                "Loaded '" + string(entry.name) + "'!",
                "OPENGL_CORE",
                LogType::LOG_DEBUG);
        }
	}
}