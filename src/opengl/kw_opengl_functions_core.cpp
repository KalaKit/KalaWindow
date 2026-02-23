//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <sstream>
#include <string>

#ifdef __linux__
#include <dlfcn.h>
#include <GL/glx.h>
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

using std::string;
using std::to_string;
using std::ostringstream;

#ifdef __linux__
using GLProc = void (*)();
#endif

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

    CoreGLFunction coreFunctions[] =
    {
        //
        // DEBUGGING
        //

        { "glDebugMessageCallback", rcast<void**>(&glCore.glDebugMessageCallback) },
        { "glGetError",             rcast<void**>(&glCore.glGetError) },

        //
        // GEOMETRY
        //

        { "glMapBufferRange",            rcast<void**>(&glCore.glMapBufferRange) },
        { "glBufferStorage",             rcast<void**>(&glCore.glBufferStorage) },
        { "glBindBuffer",                rcast<void**>(&glCore.glBindBuffer) },
        { "glBindVertexArray",           rcast<void**>(&glCore.glBindVertexArray) },
        { "glBufferData",                rcast<void**>(&glCore.glBufferData) },
        { "glBufferSubData",             rcast<void**>(&glCore.glBufferSubData) },
        { "glDeleteBuffers",             rcast<void**>(&glCore.glDeleteBuffers) },
        { "glDeleteVertexArrays",        rcast<void**>(&glCore.glDeleteVertexArrays) },
        { "glDrawArrays",               rcast<void**>(&glCore.glDrawArrays) },
        { "glDrawElements",             rcast<void**>(&glCore.glDrawElements) },
        { "glEnableVertexAttribArray",  rcast<void**>(&glCore.glEnableVertexAttribArray) },
        { "glGenBuffers",               rcast<void**>(&glCore.glGenBuffers) },
        { "glGenVertexArrays",          rcast<void**>(&glCore.glGenVertexArrays) },
        { "glGetVertexAttribiv",        rcast<void**>(&glCore.glGetVertexAttribiv) },
        { "glGetVertexAttribPointerv",  rcast<void**>(&glCore.glGetVertexAttribPointerv) },
        { "glVertexAttribPointer",      rcast<void**>(&glCore.glVertexAttribPointer) },
        { "glDisableVertexAttribArray", rcast<void**>(&glCore.glDisableVertexAttribArray) },
        { "glVertexAttribI1i",          rcast<void**>(&glCore.glVertexAttribI1i) },
        { "glVertexAttribI2i",          rcast<void**>(&glCore.glVertexAttribI2i) },
        { "glVertexAttribI3i",          rcast<void**>(&glCore.glVertexAttribI3i) },
        { "glVertexAttribI4i",          rcast<void**>(&glCore.glVertexAttribI4i) },
        { "glCullFace",                 rcast<void**>(&glCore.glCullFace) },

        //
        // SHADERS
        //

        { "glAttachShader",      rcast<void**>(&glCore.glAttachShader) },
        { "glCompileShader",     rcast<void**>(&glCore.glCompileShader) },
        { "glCreateProgram",     rcast<void**>(&glCore.glCreateProgram) },
        { "glCreateShader",      rcast<void**>(&glCore.glCreateShader) },
        { "glDeleteShader",      rcast<void**>(&glCore.glDeleteShader) },
        { "glDeleteProgram",     rcast<void**>(&glCore.glDeleteProgram) },
        { "glDetachShader",      rcast<void**>(&glCore.glDetachShader) },
        { "glGetActiveAttrib",   rcast<void**>(&glCore.glGetActiveAttrib) },
        { "glGetAttribLocation", rcast<void**>(&glCore.glGetAttribLocation) },
        { "glGetProgramiv",      rcast<void**>(&glCore.glGetProgramiv) },
        { "glGetProgramInfoLog", rcast<void**>(&glCore.glGetProgramInfoLog) },
        { "glGetShaderiv",       rcast<void**>(&glCore.glGetShaderiv) },
        { "glGetShaderInfoLog",  rcast<void**>(&glCore.glGetShaderInfoLog) },
        { "glLinkProgram",       rcast<void**>(&glCore.glLinkProgram) },
        { "glShaderSource",      rcast<void**>(&glCore.glShaderSource) },
        { "glUseProgram",        rcast<void**>(&glCore.glUseProgram) },
        { "glValidateProgram",   rcast<void**>(&glCore.glValidateProgram) },
        { "glIsProgram",         rcast<void**>(&glCore.glIsProgram) },

        //
        // UNIFORMS
        //

        { "glGetUniformLocation",   rcast<void**>(&glCore.glGetUniformLocation) },
        { "glGetUniformBlockIndex", rcast<void**>(&glCore.glGetUniformBlockIndex) },
        { "glUniformBlockBinding",  rcast<void**>(&glCore.glUniformBlockBinding) },
        { "glUniform1f",            rcast<void**>(&glCore.glUniform1f) },
        { "glUniform1i",            rcast<void**>(&glCore.glUniform1i) },
        { "glUniform1fv",           rcast<void**>(&glCore.glUniform1fv) },
        { "glUniform1iv",           rcast<void**>(&glCore.glUniform1iv) },
        { "glUniform2f",            rcast<void**>(&glCore.glUniform2f) },
        { "glUniform2i",            rcast<void**>(&glCore.glUniform2i) },
        { "glUniform2fv",           rcast<void**>(&glCore.glUniform2fv) },
        { "glUniform2iv",           rcast<void**>(&glCore.glUniform2iv) },
        { "glUniform3f",            rcast<void**>(&glCore.glUniform3f) },
        { "glUniform3i",            rcast<void**>(&glCore.glUniform3i) },
        { "glUniform3fv",           rcast<void**>(&glCore.glUniform3fv) },
        { "glUniform3iv",           rcast<void**>(&glCore.glUniform3iv) },
        { "glUniform4f",            rcast<void**>(&glCore.glUniform4f) },
        { "glUniform4i",            rcast<void**>(&glCore.glUniform4i) },
        { "glUniform4fv",           rcast<void**>(&glCore.glUniform4fv) },
        { "glUniform4iv",           rcast<void**>(&glCore.glUniform4iv) },
        { "glUniformMatrix2fv",     rcast<void**>(&glCore.glUniformMatrix2fv) },
        { "glUniformMatrix3fv",     rcast<void**>(&glCore.glUniformMatrix3fv) },
        { "glUniformMatrix4fv",     rcast<void**>(&glCore.glUniformMatrix4fv) },

        //
        // TEXTURES
        //

        { "glBindTexture",    rcast<void**>(&glCore.glBindTexture) },
        { "glActiveTexture",  rcast<void**>(&glCore.glActiveTexture) },
        { "glDeleteTextures", rcast<void**>(&glCore.glDeleteTextures) },
        { "glGenerateMipmap", rcast<void**>(&glCore.glGenerateMipmap) },
        { "glGenTextures",    rcast<void**>(&glCore.glGenTextures) },
        { "glTexImage2D",     rcast<void**>(&glCore.glTexImage2D) },
        { "glTexImage3D",     rcast<void**>(&glCore.glTexImage3D) },
        { "glCompressedTexImage2D", rcast<void**>(&glCore.glCompressedTexImage2D) },
        { "glCompressedTexImage3D", rcast<void**>(&glCore.glCompressedTexImage3D) },
        { "glTexStorage2D",   rcast<void**>(&glCore.glTexStorage2D) },
        { "glTexStorage3D",   rcast<void**>(&glCore.glTexStorage3D) },
        { "glTexSubImage2D",  rcast<void**>(&glCore.glTexSubImage2D) },
        { "glTexSubImage3D",  rcast<void**>(&glCore.glTexSubImage3D) },
        { "glCompressedTexSubImage2D",  rcast<void**>(&glCore.glCompressedTexSubImage2D) },
        { "glCompressedTexSubImage3D",  rcast<void**>(&glCore.glCompressedTexSubImage3D) },
        { "glTexParameteri",  rcast<void**>(&glCore.glTexParameteri) },
        { "glTexParameteriv", rcast<void**>(&glCore.glTexParameteriv) },
        { "glTexParameterf",  rcast<void**>(&glCore.glTexParameterf) },
        { "glTexParameterfv", rcast<void**>(&glCore.glTexParameterfv) },
        { "glPixelStorei",    rcast<void**>(&glCore.glPixelStorei) },
        { "glPixelStoref",    rcast<void**>(&glCore.glPixelStoref) },

        //
        // FRAMEBUFFERS AND RENDERBUFFERS
        //

        { "glBindRenderbuffer",        rcast<void**>(&glCore.glBindRenderbuffer) },
        { "glBindFramebuffer",         rcast<void**>(&glCore.glBindFramebuffer) },
        { "glBindBufferBase",          rcast<void**>(&glCore.glBindBufferBase) },
        { "glCheckFramebufferStatus",  rcast<void**>(&glCore.glCheckFramebufferStatus) },
        { "glFramebufferRenderbuffer", rcast<void**>(&glCore.glFramebufferRenderbuffer) },
        { "glFramebufferTexture2D",    rcast<void**>(&glCore.glFramebufferTexture2D) },
        { "glGenRenderbuffers",        rcast<void**>(&glCore.glGenRenderbuffers) },
        { "glGenFramebuffers",         rcast<void**>(&glCore.glGenFramebuffers) },
        { "glRenderbufferStorage",     rcast<void**>(&glCore.glRenderbufferStorage) },
        { "glDepthFunc",               rcast<void**>(&glCore.glDepthFunc) },
        { "glDepthMask",               rcast<void**>(&glCore.glDepthMask) },
        { "glBlendColor",              rcast<void**>(&glCore.glBlendColor) },
        { "glBlendFunc",               rcast<void**>(&glCore.glBlendFunc) },
        { "glBlendFunci",              rcast<void**>(&glCore.glBlendFunci) },
        { "glBlendEquation",           rcast<void**>(&glCore.glBlendEquation) },
        { "glBlendEquationi",         rcast<void**>(&glCore.glBlendEquationi) },
        { "glBlendEquationSeparate",  rcast<void**>(&glCore.glBlendEquationSeparate) },
        { "glBlendEquationSeparatei", rcast<void**>(&glCore.glBlendEquationSeparatei) },
        { "glStencilFunc",            rcast<void**>(&glCore.glStencilFunc) },
        { "glStencilFuncSeparate",    rcast<void**>(&glCore.glStencilFuncSeparate) },
        { "glStencilMask",            rcast<void**>(&glCore.glStencilMask) },
        { "glStencilMaskSeparate",    rcast<void**>(&glCore.glStencilMaskSeparate) },
        { "glStencilOp",              rcast<void**>(&glCore.glStencilOp) },
        { "glStencilOpSeparate",      rcast<void**>(&glCore.glStencilOpSeparate) },

        //
        // FRAME AND RENDER STATE
        //

        { "glClear",       rcast<void**>(&glCore.glClear) },
        { "glClearColor",  rcast<void**>(&glCore.glClearColor) },
        { "glEnable",      rcast<void**>(&glCore.glEnable) },
        { "glDisable",     rcast<void**>(&glCore.glDisable) },
        { "glFrontFace",   rcast<void**>(&glCore.glFrontFace) },
        { "glGetBooleanv", rcast<void**>(&glCore.glGetBooleanv) },
        { "glGetIntegerv", rcast<void**>(&glCore.glGetIntegerv) },
        { "glGetFloatv",   rcast<void**>(&glCore.glGetFloatv) },
        { "glGetDoublev",  rcast<void**>(&glCore.glGetDoublev) },
        { "glGetString",   rcast<void**>(&glCore.glGetString) },
        { "glGetStringi",  rcast<void**>(&glCore.glGetStringi) },
        { "glViewport",    rcast<void**>(&glCore.glViewport) }
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
        for (auto& entry : coreFunctions)
        {
            //try to load
            void* ptr = nullptr;

#ifdef _WIN32
            ptr = rcast<void*>(wglGetProcAddress(entry.name));
            if (!ptr)
            {
                HMODULE module = ToVar<HMODULE>(OpenGL_Global::GetOpenGLLibrary());
                ptr = rcast<void*>(GetProcAddress(module, entry.name));
            }
#else
            GLProc proc = rcast<GLProc>(glXGetProcAddress(
                rcast<const GLubyte*>(entry.name)));

            ptr = rcast<void*>(proc);

            if (!ptr)
            {
                void* module = ToVar<void*>(OpenGL_Global::GetOpenGLLibrary());
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
