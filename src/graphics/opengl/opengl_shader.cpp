//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <array>
#include <vector>
#include <memory>

#include "KalaHeaders/log_utils.hpp"

#include "graphics/window.hpp"
#include "graphics/opengl/opengl_shader.hpp"
#include "graphics/opengl/opengl.hpp"
#include "graphics/opengl/opengl_functions_core.hpp"
#include "core/core.hpp"

using KalaHeaders::Log;
using KalaHeaders::LogType;

using KalaWindow::Graphics::Window;
using KalaWindow::Graphics::OpenGL::OpenGL_Context;
using KalaWindow::Graphics::OpenGL::OpenGL_Shader;
using KalaWindow::Graphics::OpenGL::ShaderType;
using KalaWindow::Graphics::OpenGL::ShaderData;
using namespace KalaWindow::Graphics::OpenGLFunctions;
using KalaWindow::Core::KalaWindowCore;
using KalaWindow::Core::globalID;

using std::string;
using std::to_string;
using std::ifstream;
using std::stringstream;
using std::unique_ptr;
using std::make_unique;
using std::filesystem::exists;
using std::filesystem::path;
using std::array;
using std::vector;

static void CheckShaderData(
    const string& shaderName,
    const array<ShaderData, 3>& shaderData);

static bool CheckCompileErrors(u32 shader, const string& type);

static void InitShader(ShaderData& data);

static string GetShaderTypeString(ShaderType shaderType)
{
    static string empty{};

    switch (shaderType)
    {
    case ShaderType::SHADER_VERTEX:   return "VERTEX";
    case ShaderType::SHADER_FRAGMENT: return "FRAGMENT";
    case ShaderType::SHADER_GEOMETRY: return "GEOMETRY";
    }

    return empty;
}

static void DeleteShader(
    u32 programID,
    const array<ShaderData, 3>& shaderData)
{
    for (const auto& s : shaderData)
    {
        if (s.ID)
        {
            glDetachShader(programID, s.ID);
            glDeleteShader(s.ID);
        }
    }
}

namespace KalaWindow::Graphics::OpenGL
{
    OpenGL_Shader* OpenGL_Shader::CreateShader(
        u32 windowID,
        const string& shaderName,
        const array<ShaderData, 3>& shaderData)
    {
        if (!OpenGL_Global::IsInitialized())
        {
            KalaWindowCore::ForceClose(
                "Shader error",
                "Cannot create shader '" + shaderName + "' because OpenGL is not initialized!");

            return nullptr;
        }

        Window* window = Window::registry.GetContent(windowID);

        if (!window
            || !window->IsInitialized())
        {
            KalaWindowCore::ForceClose(
                "Shader error",
                "Cannot create shader '" + shaderName + "' because its window was not found!");

            return nullptr;
        }

        vector<OpenGL_Context*> contexts = OpenGL_Context::registry.GetAllWindowContent(windowID);
        OpenGL_Context* context = contexts.empty() ? nullptr : contexts.front();

        if (!context
            || !context->IsInitialized()
            || !context->IsContextValid())
        {
            KalaWindowCore::ForceClose(
                "Shader error",
                "Cannot create shader '" + shaderName + "' because window '" + window->GetTitle() + "' has no valid OpenGL context!");

            return nullptr;
        }

        u32 newID = ++globalID;
        unique_ptr<OpenGL_Shader> newShader = make_unique<OpenGL_Shader>();
        OpenGL_Shader* shaderPtr = newShader.get();

        Log::Print(
            "Creating shader '" + shaderName + "' with ID '" + to_string(newID) + "'.",
            "OPENGL_SHADER",
            LogType::LOG_DEBUG);

        CheckShaderData(
            shaderName,
            shaderData);

        bool vertShaderExists{};
        bool fragShaderExists{};
        bool geomShaderExists{};

        bool vertDuplicateExists{};
        bool fragDuplicateExists{};
        bool geomDuplicateExists{};

        ShaderData newVertData{};
        ShaderData newFragData{};
        ShaderData newGeomData{};

        for (const auto& s : shaderData)
        {
            if (s.type == ShaderType::SHADER_VERTEX
                && vertShaderExists)
            {
                vertDuplicateExists = true;
                break;
            }

            if (s.type == ShaderType::SHADER_FRAGMENT
                && fragShaderExists)
            {
                fragDuplicateExists = true;
                break;
            }

            if (s.type == ShaderType::SHADER_GEOMETRY
                && geomShaderExists)
            {
                geomDuplicateExists = true;
                break;
            }

            if (s.type == ShaderType::SHADER_VERTEX
                && (!s.shaderData.empty()
                || !s.shaderPath.empty()))
            {
                newVertData.shaderPath = s.shaderPath;
                newVertData.shaderData = s.shaderData;
                newVertData.type = s.type;

                vertShaderExists = true;
                continue;
            }

            if (s.type == ShaderType::SHADER_FRAGMENT
                && (!s.shaderData.empty()
                || !s.shaderPath.empty()))
            {
                newFragData.shaderPath = s.shaderPath;
                newFragData.shaderData = s.shaderData;
                newFragData.type = s.type;

                fragShaderExists = true;
                continue;
            }

            if (s.type == ShaderType::SHADER_GEOMETRY
                && (!s.shaderData.empty()
                || !s.shaderPath.empty()))
            {
                newGeomData.shaderPath = s.shaderPath;
                newGeomData.shaderData = s.shaderData;
                newGeomData.type = s.type;

                geomShaderExists = true;
                continue;
            }
        }

        if (!vertShaderExists)
        {
            Log::Print(
                "Cannot create shader '" + shaderName + "' because its vertex data is missing!",
                "OPENGL_SHADER",
                LogType::LOG_ERROR);

            return nullptr;
        }

        if (!fragShaderExists)
        {
            Log::Print(
                "Cannot create shader '" + shaderName + "' because its fragment data is missing!",
                "OPENGL_SHADER",
                LogType::LOG_ERROR);

            return nullptr;
        }

        if (vertDuplicateExists)
        {
            Log::Print(
                "Cannot create shader '" + shaderName + "' because more than one vertex shader was added!",
                "OPENGL_SHADER",
                LogType::LOG_ERROR);

            return nullptr;
        }

        if (fragDuplicateExists)
        {
            Log::Print(
                "Cannot create shader '" + shaderName + "' because more than one fragment shader was added!",
                "OPENGL_SHADER",
                LogType::LOG_ERROR);

            return nullptr;
        }

        if (geomDuplicateExists)
        {
            Log::Print(
                "Cannot create shader '" + shaderName + "' because more than one geometry shader was added!",
                "OPENGL_SHADER",
                LogType::LOG_ERROR);

            return nullptr;
        }

        InitShader(newVertData);
        InitShader(newFragData);

        if (!geomShaderExists)
        {
            if (isVerboseLoggingEnabled)
            {
                Log::Print(
                    "Skipped loading geometry shader because it was not assigned as a shader stage.",
                    "OPENGL_SHADER",
                    LogType::LOG_INFO);
            }
        }
        else InitShader(newGeomData);

        //
        // CREATE SHADER PROGRAM
        //

        shaderPtr->programID = glCreateProgram();

        glAttachShader(
            shaderPtr->programID, 
            newVertData.ID);
        glAttachShader(
            shaderPtr->programID, 
            newFragData.ID);
        if (geomShaderExists)
        {
            glAttachShader(
                shaderPtr->programID, 
                newGeomData.ID);
        }
        glLinkProgram(shaderPtr->programID);

        i32 success = 0;
        glGetProgramiv(
            shaderPtr->programID, 
            GL_LINK_STATUS, 
            &success);

        if (success != GL_TRUE)
        {
            string vertShaderPathName = path(newVertData.shaderPath).filename().string();
            string fragShaderPathName = path(newFragData.shaderPath).filename().string();
            string geomShaderPathName = path(newGeomData.shaderPath).filename().string();

            DeleteShader(
                shaderPtr->programID,
                { {
                    {newVertData},
                    {newFragData},
                    {newGeomData}
                } });

            i32 logLength = 0;
            glGetProgramiv(
                shaderPtr->programID, 
                GL_INFO_LOG_LENGTH, 
                &logLength);

            if (logLength > 0)
            {
                vector<char> log(logLength);
                glGetProgramInfoLog(
                    shaderPtr->programID, 
                    logLength, 
                    nullptr, 
                    log.data());

                KalaWindowCore::ForceClose(
                    "OpenGL shader error",
                    "Shader link failed:\n" + string(log.data()));
            }
            else
            {
                KalaWindowCore::ForceClose(
                    "OpenGL shader error",
                    "Shader linking failed, but GL_INFO_LOG_LENGTH was 0 (no error message).");
            }

            if (!geomShaderExists)
            {
                KalaWindowCore::ForceClose(
                    "OpenGL shader error",
                    "Failed to link vertex shader '" +
                    vertShaderPathName + "' and fragment shader '" +
                    fragShaderPathName + "' to program!");
            }
            else
            {
                KalaWindowCore::ForceClose(
                    "OpenGL shader error",
                    "Failed to link vertex shader '" +
                    vertShaderPathName + "', fragment shader '" +
                    fragShaderPathName + "' and geometry shader '" +
                    geomShaderPathName + "' to program!");
            }

            return nullptr;
        }

        //
        // VALIDATE THE SHADER PROGRAM BEFORE USING IT
        //

        glValidateProgram(shaderPtr->programID);
        i32 validated = 0;
        glGetProgramiv(
            shaderPtr->programID, 
            GL_VALIDATE_STATUS, 
            &validated);
        if (validated != GL_TRUE)
        {
            DeleteShader(
                shaderPtr->programID,
                { {
                    {newVertData},
                    {newFragData},
                    {newGeomData}
                } });

            i32 logLength = 0;
            glGetProgramiv(
                shaderPtr->programID, 
                GL_INFO_LOG_LENGTH, 
                &logLength);

            if (logLength > 0)
            {
                vector<char> log(logLength);
                glGetProgramInfoLog(
                    shaderPtr->programID, 
                    logLength, 
                    nullptr, 
                    log.data());
                
                string logStr(log.begin(), log.end());

                KalaWindowCore::ForceClose(
                    "OpenGL shader error",
                    "Shader program validation failed for shader '" + shaderName + "'! Reason:\n" + logStr);

                return nullptr;
            }

            KalaWindowCore::ForceClose(
                "OpenGL shader error",
                "Shader program validation failed for shader '" + shaderName + "'! No log info was provided.");

            return nullptr;
        }

        i32 valid = glIsProgram(shaderPtr->programID);
        bool isProgramValid = valid == GL_TRUE;
        if (!isProgramValid)
        {
            DeleteShader(
                shaderPtr->programID,
                { {
                    {newVertData},
                    {newFragData},
                    {newGeomData}
                } });

            string title = "OpenGL shader error";
            string reason = "Shader program ID " + to_string(shaderPtr->programID) + " for shader '" + shaderName + "' is not valid!";

            KalaWindowCore::ForceClose(title, reason);

            return nullptr;
        }
        else
        {
            if (isVerboseLoggingEnabled)
            {
                Log::Print(
                    "Shader program ID " + to_string(shaderPtr->programID) + " for shader '" + shaderName + "' is valid!",
                    "OPENGL_SHADER",
                    LogType::LOG_SUCCESS);
            }
        }

        //
        // CLEANUP
        //

        DeleteShader(
            shaderPtr->programID,
            { {
                {newVertData},
                {newFragData},
                {newGeomData}
            } });

        if (vertShaderExists) shaderPtr->vertData = newVertData;
        if (fragShaderExists) shaderPtr->fragData = newFragData;
        if (geomShaderExists) shaderPtr->geomData = newGeomData;

        if (!shaderPtr->SetName(shaderName))
        {
            Log::Print(
                "Shader name cannot be empty or longer than 50 characters!",
                "OPENGL_SHADER",
                LogType::LOG_ERROR,
                2);

            return nullptr;
        }
        shaderPtr->ID = newID;
        shaderPtr->windowID = windowID;

        shaderPtr->isInitialized = true;

        registry.AddContent(newID, move(newShader));

        Log::Print(
            "Created OpenGL shader '" + shaderName + "' with ID '" + to_string(newID) + "'!",
            "OPENGL_SHADER",
            LogType::LOG_SUCCESS);

        return shaderPtr;
    }

    bool OpenGL_Shader::Bind() const
    {
        if (!OpenGL_Global::IsInitialized())
        {
            Log::Print(
                "Cannot bind shader '" + name + "' because OpenGL is not initialized!",
                "OPENGL_SHADER",
                LogType::LOG_ERROR,
                2);
            return false;
        }

        Window* window = Window::registry.GetContent(windowID);

        if (!window
            || !window->IsInitialized())
        {
            Log::Print(
                "Cannot bind shader '" + name + "' because its window was not found!",
                "OPENGL_SHADER",
                LogType::LOG_ERROR,
                2);
            return false;
        }

        vector<OpenGL_Context*> contexts = OpenGL_Context::registry.GetAllWindowContent(windowID);
        OpenGL_Context* context = contexts.empty() ? nullptr : contexts.front();

        if (!context
            || !context->IsInitialized()
            || !context->IsContextValid())
        {
            Log::Print(
                "Cannot bind shader '" + name + "' for window '" + window->GetTitle() + "' because it has no valid OpenGL context!",
                "OPENGL_WINDOWS",
                LogType::LOG_ERROR);

            return false;
        }

        u32 lastProgramID = context->GetLastProgramID();
        u32 ID = programID;

        if (ID == 0)
        {
            Log::Print(
                "OpenGL shader bind failed! ID is 0.",
                "OPENGL_SHADER",
                LogType::LOG_ERROR,
                2);
            return false;
        }

        context->MakeContextCurrent();
        if (!context->IsContextValid())
        {
            Log::Print(
                "OpenGL shader bind failed! OpenGL context is invalid.",
                "OPENGL_SHADER",
                LogType::LOG_ERROR,
                2);
            return false;
        }

        if (ID == lastProgramID) return true;

#ifdef _DEBUG
        i32 linked = 0;
        glGetProgramiv(
            ID,
            GL_LINK_STATUS,
            &linked);
        if (linked != GL_TRUE)
        {
            Log::Print(
                "GL_LINK_STATUS = " + to_string(linked),
                "OPENGL_SHADER",
                LogType::LOG_WARNING);
        }

        i32 validated = 0;
        glGetProgramiv(
            ID,
            GL_VALIDATE_STATUS,
            &validated);
        if (validated != GL_TRUE)
        {
            Log::Print(
                "GL_VALIDATE_STATUS = " + to_string(validated),
                "OPENGL_SHADER",
                LogType::LOG_WARNING);
        }
#endif

        glUseProgram(ID);

#ifdef _DEBUG
        i32 activeProgram = 0;
        glGetIntegerv(
            GL_CURRENT_PROGRAM,
            &activeProgram);

        if (activeProgram != (i32)ID)
        {
            Log::Print(
                "OpenGL shader bind failed! Program ID not bound after glUseProgram." +
                string("Expected ID: '") + to_string(ID) + "', but got: '" + to_string(activeProgram) + "'.",
                "OPENGL_SHADER",
                LogType::LOG_ERROR,
                2);
            return false;
        }

        string errorVal = OpenGL_Global::GetError();
        if (!errorVal.empty())
        {
            KalaWindowCore::ForceClose(
                "OpenGL shader error",
                "Failed to bind shader '" + name + "'! Reason: " + errorVal);

            return false;
        }
#endif

        context->SetLastProgramID(ID);

        return true;
    }

    bool OpenGL_Shader::HotReload()
    {
        //back up old data
        array<ShaderData, 3> shaders = GetAllShaders();

        for (const auto& shader : shaders)
        {
            if (shader.shaderPath.empty()
                && shader.shaderData.empty())
            {
                Log::Print(
                    "Hot reload failed for shader '" + name
                    + "' because one or more shader types had invalid data! Keeping old version.",
                    "OPENGL_SHADER",
                    LogType::LOG_ERROR,
                    2);

                return false;
            }
        }

        auto reloadedShader = OpenGL_Shader::CreateShader(
            windowID,
            name,
            shaders);

        if (!reloadedShader)
        {
            Log::Print(
                "Hot reload failed for shader '" + name
                + "' because the new shader failed to be created! Keeping old version.",
                "OPENGL_SHADER",
                LogType::LOG_ERROR,
                2);

            return false;
        }

        //replace internal data
        for (const auto& sh : shaders)
        {
            if (sh.type == ShaderType::SHADER_VERTEX) vertData = sh;
            else if (sh.type == ShaderType::SHADER_FRAGMENT) fragData = sh;
            else if (sh.type == ShaderType::SHADER_GEOMETRY
                && (!sh.shaderPath.empty()
                || !sh.shaderData.empty()))
            {
                geomData = sh;
            }
        }

        Log::Print(
            "Shader '" + name + "' was hot reloaded!",
            "OPENGL_SHADER",
            LogType::LOG_SUCCESS);

        return true;
    }

    void OpenGL_Shader::SetBool(
        u32 programID,
        const string& name, 
        bool value) const
    {
        glUniform1i(glGetUniformLocation(
            programID, 
            name.c_str()), 
            (i32)value);
    }
    void OpenGL_Shader::SetInt(
        u32 programID,
        const string& name, 
        i32 value) const
    {
        glUniform1i(glGetUniformLocation(
            programID, 
            name.c_str()), 
            value);
    }
    void OpenGL_Shader::SetFloat(
        u32 programID,
        const string& name, 
        f32 value) const
    {
        glUniform1f(glGetUniformLocation(
            programID, 
            name.c_str()), 
            value);
    }

    void OpenGL_Shader::SetVec2(
        u32 programID,
        const string& name, 
        const vec2& value) const
    {
        auto loc = glGetUniformLocation(programID, name.c_str());
        glUniform2fv(
            loc, 
            1, 
            &value.x);
    }
    void OpenGL_Shader::SetVec3(
        u32 programID,
        const string& name, 
        const vec3& value) const
    {
        auto loc = glGetUniformLocation(programID, name.c_str());
        glUniform3fv(
            loc, 
            1, 
            &value.x);
    }
    void OpenGL_Shader::SetVec4(
        u32 programID,
        const string& name, 
        const vec4& value) const
    {
        auto loc = glGetUniformLocation(programID, name.c_str());
        glUniform4fv(
            loc, 
            1, 
            &value.x);
    }

    void OpenGL_Shader::SetMat2(
        u32 programID,
        const string& name, 
        const mat2& mat) const
    {
        auto loc = glGetUniformLocation(programID, name.c_str());
        glUniformMatrix2fv(
            loc, 
            1, 
            GL_FALSE, 
            &mat.m00);
    }
    void OpenGL_Shader::SetMat3(
        u32 programID,
        const string& name, 
        const mat3& mat) const
    {
        auto loc = glGetUniformLocation(programID, name.c_str());
        glUniformMatrix3fv(
            loc, 
            1, 
            GL_FALSE, 
            &mat.m00);
    }
    void OpenGL_Shader::SetMat4(
        u32 programID,
        const string& name, 
        const mat4& mat) const
    {
        auto loc = glGetUniformLocation(programID, name.c_str());
        glUniformMatrix4fv(
            loc, 
            1, 
            GL_FALSE, 
            &mat.m00);
    }

    OpenGL_Shader::~OpenGL_Shader()
    {
        Log::Print(
            "Destroying shader '" + name + "' with ID '" + to_string(ID) + "'.",
            "OPENGL_SHADER",
            LogType::LOG_INFO);

        auto DestroyShader = [&](u32& ID)
            {
                if (ID == 0) return;

                if (programID != 0)
                {
                    glDetachShader(
                        programID,
                        ID);
                }
                glDeleteShader(ID);
                ID = 0;
            };

        DestroyShader(vertData.ID);
        DestroyShader(fragData.ID);
        DestroyShader(geomData.ID);

        if (programID != 0)
        {
            glDeleteProgram(programID);
            programID = 0;
        }
    }
}

void CheckShaderData(
    const string& shaderName,
    const array<ShaderData, 3>& shaderData)
{
    //shader data must not be empty
    if (shaderData.empty())
    {
        KalaWindowCore::ForceClose(
            "OpenGL shader error",
            "Shader '" + shaderName + "' has no data to load!");

        return;
    }

    array<string, 3> validExtensions =
    {
        ".vert",
        ".frag",
        ".geom"
    };

    for (const auto& shader : shaderData)
    {
        string type = GetShaderTypeString(shader.type);

        //shader file path or shader data must have content
        if (shader.type != ShaderType::SHADER_NONE
            && shader.shaderData.empty()
            && shader.shaderPath.empty())
        {
            KalaWindowCore::ForceClose(
                "OpenGL shader error",
                "Shader '" + shaderName + "' with type '" + type + "' has no file paths or shader data to load data from!");

            return;
        }

        if (!shader.shaderPath.empty())
        {
            string shaderFileName = path(shader.shaderPath).filename().string();

            //shader file path must exist
            if (!exists(shader.shaderPath))
            {
                KalaWindowCore::ForceClose(
                    "OpenGL shader error",
                    "Shader '" + shaderName + "' path '" + shaderFileName + "' does not exist!");

                return;
            }

            //shader file path must have extension
            if (!path(shader.shaderPath).has_extension())
            {
                KalaWindowCore::ForceClose(
                    "OpenGL shader error",
                    "Shader '" + shaderName + "' path '" + shaderFileName + "' has no extension!");

                return;
            }

            string thisExtension = path(shader.shaderPath).extension().string();
            bool isExtensionValid =
                find(validExtensions.begin(),
                    validExtensions.end(),
                    thisExtension)
                != validExtensions.end();

            //extension must be .vert, .frag or .geom
            if (!isExtensionValid)
            {
                KalaWindowCore::ForceClose(
                    "OpenGL shader error",
                    "Shader '" + shaderName + "' path '" + shaderFileName + "' has an invalid extension '" + thisExtension + "'!");

                return;
            }

            //vert type must match extension
            if (shader.type == ShaderType::SHADER_VERTEX
                && thisExtension != ".vert")
            {
                KalaWindowCore::ForceClose(
                    "OpenGL shader error",
                    "Shader '" + shaderName + "' path '" + shaderFileName + "' has extension '" + thisExtension + "' but its type was set to 'SHADER_VERTEX'! Only '.vert' is allowed for vertex shaders.");

                return;
            }

            //frag type must match extension

            if (shader.type == ShaderType::SHADER_FRAGMENT
                && thisExtension != ".frag")
            {
                KalaWindowCore::ForceClose(
                    "OpenGL shader error",
                    "Shader '" + shaderName + "' path '" + shaderFileName + "' has extension '" + thisExtension + "' but its type was set to 'SHADER_FRAGMENT'! Only '.frag' is allowed for fragment shaders.");

                return;
            }

            //geom type must match extension
            if (shader.type == ShaderType::SHADER_GEOMETRY
                && thisExtension != ".geom")
            {
                KalaWindowCore::ForceClose(
                    "OpenGL shader error",
                    "Shader '" + shaderName + "' path '" + shaderFileName + "' has extension '" + thisExtension + "' but its type was set to 'SHADER_GEOMETRY'! Only '.geom' is allowed for geometry shaders.");

                return;
            }
        }
    }
}

bool CheckCompileErrors(u32 shader, const string& type)
{
    i32 success = 0;
    i32 logLength = 0;

    if (type == "PROGRAM")
    {
        glGetProgramiv(
            shader,
            GL_LINK_STATUS,
            &success);
        glGetProgramiv(
            shader,
            GL_INFO_LOG_LENGTH,
            &logLength);
        if (!success)
        {
            if (logLength > 0)
            {
                const GLsizei safeLength = min(logLength, 4096);

                vector<char> infoLog(safeLength);
                glGetProgramInfoLog(
                    shader,
                    safeLength,
                    nullptr,
                    infoLog.data());

                KalaWindowCore::ForceClose(
                    "OpenGL shader error",
                    "Shader linking failed (" + type + "):\n" + string(infoLog.data()));
            }
            else
            {
                KalaWindowCore::ForceClose(
                    "OpenGL shader error",
                    "Shader linking failed (" + type + "), but no log was returned.");
            }
            return false;
        }
        else
        {
            if (OpenGL_Shader::IsVerboseLoggingEnabled())
            {
                Log::Print(
                    "Shader linking succeeded (" + type + ")",
                    "OPENGL_SHADER",
                    LogType::LOG_SUCCESS);
            }
        }
    }
    else
    {
        glGetShaderiv(
            shader,
            GL_COMPILE_STATUS,
            &success);
        glGetShaderiv(
            shader,
            GL_INFO_LOG_LENGTH,
            &logLength);
        if (!success)
        {
            if (logLength > 0)
            {
                const GLsizei safeLength = min(logLength, 4096);

                vector<char> infoLog(safeLength);
                glGetShaderInfoLog(
                    shader,
                    safeLength,
                    nullptr,
                    infoLog.data());

                KalaWindowCore::ForceClose(
                    "OpenGL shader error",
                    "Shader compilation failed (" + type + "):\n" + string(infoLog.data()));
            }
            else
            {
                KalaWindowCore::ForceClose(
                    "OpenGL shader error",
                    "Shader compilation failed (" + type + "), but no log was returned.");
            }
            return false;
        }
        else
        {
            if (OpenGL_Shader::IsVerboseLoggingEnabled())
            {
                Log::Print(
                    "Shader compilation succeeded (" + type + ")",
                    "OPENGL_SHADER",
                    LogType::LOG_SUCCESS);
            }
        }
    }

    return true;
}

void InitShader(ShaderData& data)
{
    string shaderPath = data.shaderPath;
    string shaderData = data.shaderData;
    ShaderType type = data.type;

    string shaderType = GetShaderTypeString(type);
    string shaderName = path(shaderPath).filename().string();

    string shaderCodeString{};
    if (!shaderPath.empty())
    {
        ifstream shaderFile(shaderPath);
        if (!shaderFile.is_open())
        {
            KalaWindowCore::ForceClose(
                "OpenGL shader error",
                "Failed to read " + shaderType + " shader file '" + shaderName + "'!");

            return;
        }

        stringstream shaderStream{};
        shaderStream << shaderFile.rdbuf();
        shaderCodeString = shaderStream.str();
    }
    else shaderCodeString = shaderData;

    const char* shaderCodeChar = shaderCodeString.c_str();

    GLenum shaderEnum{};
    switch (type)
    {
    case ShaderType::SHADER_VERTEX:
        shaderEnum = GL_VERTEX_SHADER; break;
    case ShaderType::SHADER_FRAGMENT:
        shaderEnum = GL_FRAGMENT_SHADER; break;
    case ShaderType::SHADER_GEOMETRY:
        shaderEnum = GL_GEOMETRY_SHADER; break;
    }

    data.ID = glCreateShader(shaderEnum);
    glShaderSource(
        data.ID,
        1,
        &shaderCodeChar,
        nullptr);
    glCompileShader(data.ID);

    string capitalShaderName{};
    switch (type)
    {
    case ShaderType::SHADER_VERTEX:
        capitalShaderName = "VERTEX"; break;
    case ShaderType::SHADER_FRAGMENT:
        capitalShaderName = "FRAGMENT"; break;
    case ShaderType::SHADER_GEOMETRY:
        capitalShaderName = "GEOMETRY"; break;
    }

    if (!CheckCompileErrors(data.ID, capitalShaderName))
    {
        glDeleteShader(data.ID);

        KalaWindowCore::ForceClose(
            "OpenGL shader error",
            "Failed to compile " + shaderType + " shader '" + shaderName + "'!");

        return;
    }

    if (OpenGL_Shader::IsVerboseLoggingEnabled())
    {
        Log::Print(
            "Initialized " + shaderType + " shader!",
            "OPENGL_SHADER",
            LogType::LOG_SUCCESS);
    }
}