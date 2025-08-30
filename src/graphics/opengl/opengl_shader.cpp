//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <vector>
#include <memory>

#include "KalaHeaders/logging.hpp"

#include "graphics/window.hpp"
#include "graphics/opengl/opengl_shader.hpp"
#include "graphics/opengl/opengl.hpp"
#include "graphics/opengl/opengl_functions_core.hpp"
#include "core/core.hpp"
#include "core/containers.hpp"

using KalaHeaders::Log;
using KalaHeaders::LogType;

using KalaWindow::Graphics::Window;
using KalaWindow::Core::KalaWindowCore;
using KalaWindow::Graphics::OpenGL::OpenGL_Shader;
using KalaWindow::Graphics::OpenGL::ShaderType;
using KalaWindow::Graphics::OpenGL::ShaderStage;
using namespace KalaWindow::Graphics::OpenGLFunctions;
using KalaWindow::Core::globalID;
using KalaWindow::Core::createdOpenGLShaders;
using KalaWindow::Core::runtimeOpenGLShaders;

using std::string;
using std::to_string;
using std::ifstream;
using std::stringstream;
using std::unique_ptr;
using std::make_unique;
using std::filesystem::exists;
using std::filesystem::path;
using std::vector;

enum class ShaderCheckResult
{
    RESULT_OK,
    RESULT_INVALID,
    RESULT_ALREADY_EXISTS
};

static ShaderCheckResult IsShaderValid(
    const string& shaderName,
    const vector<ShaderStage>& shaderStages);

static bool CheckCompileErrors(u32 shader, const string& type);

static bool InitShader(
    ShaderType type,
    const string& shaderPath,
    u32& programID,
    u32& shaderID);

namespace KalaWindow::Graphics::OpenGL
{
    void OpenGL_Shader::SetName(const string& newName)
    {
        if (newName.empty())
        {
            Log::Print(
                "Cannot set shader name to empty name!",
                "OPENGL_SHADER",
                LogType::LOG_ERROR,
                2);
            return;
        }
        for (const auto& createdShader : createdOpenGLShaders)
        {
            string thisName = createdShader.second->GetName();
            if (newName == thisName)
            {
                Log::Print(
                    "Cannot set shader name to already existing shader name '" + thisName + "'!",
                    "OPENGL_SHADER",
                    LogType::LOG_ERROR,
                    2);
                return;
            }
        }
        name = newName;
    }

    OpenGL_Shader* OpenGL_Shader::CreateShader(
        const string& shaderName,
        const vector<ShaderStage>& shaderStages,
        Window* newWindow)
    {
        if (!OpenGL_Renderer::IsInitialized())
        {
            Log::Print(
                "Cannot create shader '" + shaderName + "' because OpenGL is not initialized!",
                "OPENGL_SHADER",
                LogType::LOG_ERROR,
                2);
            return nullptr;
        }

        ShaderCheckResult result = IsShaderValid(
            shaderName,
            shaderStages);

        if (result == ShaderCheckResult::RESULT_INVALID) return nullptr;
        else if (result == ShaderCheckResult::RESULT_ALREADY_EXISTS)
        {
            for (const auto& [key, value] : createdOpenGLShaders)
            {
                if (value->GetName() == shaderName) return value.get();
            }
        }

        Log::Print(
            "Creating shader '" + shaderName + "'.",
            "OPENGL_SHADER",
            LogType::LOG_DEBUG);

        unique_ptr<OpenGL_Shader> newShader = make_unique<OpenGL_Shader>();
        OpenGL_Shader* shaderPtr = newShader.get();

        ShaderStage newVertStage{};
        ShaderStage newFragStage{};
        ShaderStage newGeomStage{};

        for (const auto& stage : shaderStages)
        {
            string shaderType = GetShaderTypeName(stage.shaderType);
            string shaderPathName = path(stage.shaderPath).filename().string();

            if (stage.shaderPath.empty())
            {
                string title = "OpenGL Shader Error";
                string reason =
                    "Shader '" + shaderName + "' with type '"
                    + shaderType + "' has no assigned path!";

                KalaWindowCore::ForceClose(title, reason);

                return nullptr;
            }

            if (!exists(stage.shaderPath))
            {
                string title = "OpenGL Shader Error";
                string reason = 
                    "Shader '" + shaderName + "' with type '"
                    + shaderType + "' has an invalid path '" + shaderPathName + "'!";

                KalaWindowCore::ForceClose(title, reason);

                return nullptr;
            }
            
            switch (stage.shaderType)
            {
            case ShaderType::Shader_Vertex:
                newVertStage.shaderPath = stage.shaderPath;
                newVertStage.shaderType = stage.shaderType;
                break;
            case ShaderType::Shader_Fragment:
                newFragStage.shaderPath = stage.shaderPath;
                newFragStage.shaderType = stage.shaderType;
                break;
            case ShaderType::Shader_Geometry:
                newGeomStage.shaderPath = stage.shaderPath;
                newGeomStage.shaderType = stage.shaderType;
                break;
            }
        }

        bool vertShaderExists = !newVertStage.shaderPath.empty();
        bool fragShaderExists = !newFragStage.shaderPath.empty();
        bool geomShaderExists = !newGeomStage.shaderPath.empty();

        //
        // CREATE AND COMPILE VERTEX SHADER
        //

        if (!vertShaderExists)
        {
            if (isVerboseLoggingEnabled)
            {
                Log::Print(
                    "Skipped loading vertex shader because it was not assigned as a shader stage.",
                    "OPENGL_SHADER",
                    LogType::LOG_INFO);
            }
        }
        else
        {
            if (!InitShader(
                ShaderType::Shader_Vertex,
                newVertStage.shaderPath,
                shaderPtr->programID,
                newVertStage.shaderID))
            {
                return nullptr;
            }
        }

        //
        // CREATE AND COMPILE FRAGMENT SHADER
        //

        if (!fragShaderExists)
        {
            if (isVerboseLoggingEnabled)
            {
                Log::Print(
                    "Skipped loading fragment shader because it was not assigned as a shader stage.",
                    "OPENGL_SHADER",
                    LogType::LOG_INFO);
            }
        }
        else
        {
            if (!InitShader(
                ShaderType::Shader_Fragment,
                newFragStage.shaderPath,
                shaderPtr->programID,
                newFragStage.shaderID))
            {
                return nullptr;
            }
        }

        //
        // CREATE AND COMPILE GEOMETRY SHADER
        //

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
        else
        {
            if (!InitShader(
                ShaderType::Shader_Geometry,
                newGeomStage.shaderPath,
                shaderPtr->programID,
                newGeomStage.shaderID))
            {
                return nullptr;
            }
        }

        //
        // CREATE SHADER PROGRAM
        //

        shaderPtr->programID = glCreateProgram();

        glAttachShader(
            shaderPtr->programID, 
            newVertStage.shaderID);
        glAttachShader(
            shaderPtr->programID, 
            newFragStage.shaderID);
        if (geomShaderExists)
        {
            glAttachShader(
                shaderPtr->programID, 
                newGeomStage.shaderID);
        }
        glLinkProgram(shaderPtr->programID);

        i32 success = 0;
        glGetProgramiv(
            shaderPtr->programID, 
            GL_LINK_STATUS, 
            &success);

        if (success != GL_TRUE)
        {
            string vertShaderPathName = path(newVertStage.shaderPath).filename().string();
            string fragShaderPathName = path(newFragStage.shaderPath).filename().string();
            string geomShaderPathName = path(newGeomStage.shaderPath).filename().string();

            glDetachShader(
                shaderPtr->programID,
                newVertStage.shaderID);
            glDeleteShader(newVertStage.shaderID);

            glDetachShader(
                shaderPtr->programID,
                newFragStage.shaderID);
            glDeleteShader(newFragStage.shaderID);

            if (geomShaderExists)
            {
                glDetachShader(
                    shaderPtr->programID,
                    newGeomStage.shaderID);
                glDeleteShader(newGeomStage.shaderID);
            }

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
                    "OpenGL Shader Error",
                    "Shader link failed:\n" + string(log.data()));
            }
            else
            {
                KalaWindowCore::ForceClose(
                    "OpenGL Shader Error",
                    "Shader linking failed, but GL_INFO_LOG_LENGTH was 0 (no error message).");
            }

            if (!geomShaderExists)
            {
                KalaWindowCore::ForceClose(
                    "OpenGL Shader Error",
                    "Failed to link vertex shader '" +
                    vertShaderPathName + "' and fragment shader '" +
                    fragShaderPathName + "' to program!");
            }
            else
            {
                KalaWindowCore::ForceClose(
                    "OpenGL Shader Error",
                    "Failed to link vertex shader '" +
                    vertShaderPathName + "', fragment shader '" +
                    fragShaderPathName + "' and geometry shader '" +
                    geomShaderPathName + "' to program!");
            }

            return nullptr;
        }

        //validate the shader program before using it
        glValidateProgram(shaderPtr->programID);
        i32 validated = 0;
        glGetProgramiv(
            shaderPtr->programID, 
            GL_VALIDATE_STATUS, 
            &validated);
        if (validated != GL_TRUE)
        {
            glDetachShader(
                shaderPtr->programID,
                newVertStage.shaderID);
            glDeleteShader(newVertStage.shaderID);

            glDetachShader(
                shaderPtr->programID,
                newFragStage.shaderID);
            glDeleteShader(newFragStage.shaderID);

            if (geomShaderExists)
            {
                glDetachShader(
                    shaderPtr->programID,
                    newGeomStage.shaderID);
                glDeleteShader(newGeomStage.shaderID);
            }

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
                    "OpenGL Shader Error",
                    "Shader program validation failed for shader '" + shaderName + "'! Reason:\n" + logStr);

                return nullptr;
            }

            KalaWindowCore::ForceClose(
                "OpenGL Shader Error",
                "Shader program validation failed for shader '" + shaderName + "'! No log info was provided.");

            return nullptr;
        }

        i32 valid = glIsProgram(shaderPtr->programID);
        bool isProgramValid = valid == GL_TRUE;
        if (!isProgramValid)
        {
            glDetachShader(
                shaderPtr->programID,
                newVertStage.shaderID);
            glDeleteShader(newVertStage.shaderID);

            glDetachShader(
                shaderPtr->programID,
                newFragStage.shaderID);
            glDeleteShader(newFragStage.shaderID);

            if (geomShaderExists)
            {
                glDetachShader(
                    shaderPtr->programID,
                    newGeomStage.shaderID);
                glDeleteShader(newGeomStage.shaderID);
            }

            string title = "OpenGL Shader Error";
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

        glDetachShader(
            shaderPtr->programID,
            newVertStage.shaderID);
        glDeleteShader(newVertStage.shaderID);

        glDetachShader(
            shaderPtr->programID,
            newFragStage.shaderID);
        glDeleteShader(newFragStage.shaderID);

        if (geomShaderExists)
        {
            glDetachShader(
                shaderPtr->programID,
                newGeomStage.shaderID);
            glDeleteShader(newGeomStage.shaderID);
        }

        if (vertShaderExists) shaderPtr->shaders.push_back(newVertStage);
        if (fragShaderExists) shaderPtr->shaders.push_back(newFragStage);
        if (geomShaderExists) shaderPtr->shaders.push_back(newGeomStage);

        u32 newID = ++globalID;
        newShader->name = shaderName;
        newShader->ID = newID;
        newShader->targetWindow = newWindow;

        createdOpenGLShaders[newID] = move(newShader);
        runtimeOpenGLShaders.push_back(shaderPtr);

        Log::Print(
            "Created OpenGL shader '" + shaderName + "' with ID '" + to_string(newID) + "'!",
            "TEXTURE",
            LogType::LOG_SUCCESS);

        return shaderPtr;
    }

    bool OpenGL_Shader::Bind() const
    {
        if (!OpenGL_Renderer::IsInitialized())
        {
            Log::Print(
                "Cannot bind shader '" + name + "' because OpenGL is not initialized!",
                "OPENGL_SHADER",
                LogType::LOG_ERROR,
                2);
            return false;
        }

        if (targetWindow == nullptr)
        {
            Log::Print(
                "Cannot bind shader '" + name + "' because the window reference is invalid!",
                "OPENGL_SHADER",
                LogType::LOG_ERROR,
                2);
            return false;
        }

        OpenGLData oData = targetWindow->GetOpenGLData();

        u32& lastProgramID = oData.lastProgramID;
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

        OpenGL_Renderer::MakeContextCurrent(targetWindow);
        if (!OpenGL_Renderer::IsContextValid(targetWindow))
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
        /*
        Log::Print(
            "glUseProgram(" + to_string(ID) + ")",
            "OPENGL_SHADER",
            LogType::LOG_DEBUG);
        */

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
                LogType::LOG_DEBUG);
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
                LogType::LOG_DEBUG);
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

        OpenGL_Renderer::GetError("shader '" + name + "' bind");
#endif

        lastProgramID = ID;
        oData.lastProgramID = lastProgramID;

        return true;
    }

    void OpenGL_Shader::HotReload()
    {
        if (!OpenGL_Renderer::IsInitialized())
        {
            Log::Print(
                "Cannot hot reload shader '" + name + "' because OpenGL is not initialized!",
                "OPENGL_SHADER",
                LogType::LOG_ERROR,
                2);
            return;
        }

        string shaderName = name;

        //back up old data
        vector<ShaderStage> oldShaders = GetAllShaders();

        //attepmt to recreate

        vector<ShaderStage> stagesToReload{};
        for (const auto& stage : oldShaders)
        {
            stagesToReload.push_back(
                {
                    stage.shaderType,
                    stage.shaderPath,
                    0
                });
        }

        auto reloadedShader = OpenGL_Shader::CreateShader(
            shaderName,
            stagesToReload,
            targetWindow);
        if (!reloadedShader)
        {
            Log::Print(
                "Hot reload failed for shader '" + shaderName + "'! Keeping old version.",
                "OPENGL_SHADER",
                LogType::LOG_ERROR,
                2);
            return;
        }

        //replace internal data
        shaders = reloadedShader->shaders;

        Log::Print(
            "Shader '" + shaderName + "' was hot reloaded!",
            "OPENGL_SHADER",
            LogType::LOG_SUCCESS);
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
            &mat[0].x);
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
            &mat[0].x);
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
            &mat[0].x);
    }

    OpenGL_Shader::~OpenGL_Shader()
    {
        for (auto& shaderData : GetAllShaders())
        {
            for (auto& shaderStage : shaders)
            {
                if (shaderStage.shaderID != 0)
                {
                    u32 programID = GetProgramID();
                    if (programID != 0)
                    {
                        glDetachShader(
                            programID,
                            shaderStage.shaderID);
                    }
                    glDeleteShader(shaderStage.shaderID);
                    shaderStage.shaderID = 0;
                }
            }
            if (programID != 0)
            {
                glDeleteProgram(programID);
                programID = 0;
            }
        }
        shaders.clear();

        Log::Print(
            "Destroyed shader '" + GetName() + "'!",
            "OPENGL_SHADER",
            LogType::LOG_SUCCESS);
    }
}

ShaderCheckResult IsShaderValid(
    const string& shaderName,
    const vector<ShaderStage>& shaderStages)
{
    //shader name must be assigned

    if (shaderName.empty())
    {
        string title = "OpenGL Shader Error";
        string reason = "Cannot load a shader with no name!";

        KalaWindowCore::ForceClose(title, reason);

        return ShaderCheckResult::RESULT_INVALID;
    }

    //shader stages must not be empty

    if (shaderStages.empty())
    {
        string title = "OpenGL Shader Error";
        string reason = "Cannot load a shader with no stages!";

        KalaWindowCore::ForceClose(title, reason);

        return ShaderCheckResult::RESULT_INVALID;
    }

    vector<string> validExtensions =
    {
        ".vert",
        ".frag",
        ".geom"
    };

    for (const auto& stage : shaderStages)
    {
        //shader file path must not be empty

        if (stage.shaderPath.empty())
        {
            string title = "OpenGL Shader Error";
            string reason = "Cannot load shader '" + shaderName + "' with no file paths!";

            KalaWindowCore::ForceClose(title, reason);

            return ShaderCheckResult::RESULT_INVALID;
        }

        string shaderFileName = path(stage.shaderPath).filename().string();

        //shader file path must exist

        if (!exists(stage.shaderPath))
        {
            string title = "OpenGL Shader Error";
            string reason = "Shader '" + shaderName + "' path '" + shaderFileName + "' does not exist!";

            KalaWindowCore::ForceClose(title, reason);

            return ShaderCheckResult::RESULT_INVALID;
        }

        //shader file path must have extension

        if (!path(stage.shaderPath).has_extension())
        {
            string title = "OpenGL Shader Error";
            string reason = "Shader '" + shaderName + "' path '" + shaderFileName + "' has no extension. You must use .vert, .frag or .geom";

            KalaWindowCore::ForceClose(title, reason);

            return ShaderCheckResult::RESULT_INVALID;
        }

        string thisExtension = path(stage.shaderPath).extension().string();
        bool isExtensionValid =
            find(validExtensions.begin(),
                validExtensions.end(),
                thisExtension)
            != validExtensions.end();

        //extension must be .vert, .frag or .geom

        if (!isExtensionValid)
        {
            string title = "OpenGL Shader Error";
            string reason = "Shader '" + shaderName + "' path '" + shaderFileName + "' has an invalid extension '" + thisExtension + "'. Only .vert, .frag and .geom are allowed!";

            KalaWindowCore::ForceClose(title, reason);

            return ShaderCheckResult::RESULT_INVALID;
        }

        //vert type must match extension

        if (stage.shaderType == ShaderType::Shader_Vertex
            && thisExtension != ".vert")
        {
            string title = "OpenGL Shader Error";
            string reason = "Shader '" + shaderName + "' path '" + shaderFileName + "' has extension '" + thisExtension + "' but its type was set to 'Shader_Vertex'. Type and extension must always match!";

            KalaWindowCore::ForceClose(title, reason);

            return ShaderCheckResult::RESULT_INVALID;
        }

        //frag type must match extension

        if (stage.shaderType == ShaderType::Shader_Fragment
            && thisExtension != ".frag")
        {
            string title = "OpenGL Shader Error";
            string reason = "Shader '" + shaderName + "' path '" + shaderFileName + "' has extension '" + thisExtension + "' but its type was set to 'Shader_Fragment'. Type and extension must always match!";

            KalaWindowCore::ForceClose(title, reason);

            return ShaderCheckResult::RESULT_INVALID;
        }

        //geom type must match extension

        if (stage.shaderType == ShaderType::Shader_Geometry
            && thisExtension != ".geom")
        {
            string title = "OpenGL Shader Error";
            string reason = "Shader '" + shaderName + "' path '" + shaderFileName + "' has extension '" + thisExtension + "' but its type was set to 'Shader_Geometry'. Type and extension must always match!";

            KalaWindowCore::ForceClose(title, reason);

            return ShaderCheckResult::RESULT_INVALID;
        }
    }

    //pass existing one if shader with same name already exists

    for (const auto& [_, value] : createdOpenGLShaders)
    {
        if (value->GetName() == shaderName)
        {
            string reason =
                "Shader '" + shaderName + "' already exists!";

            Log::Print(
                reason,
                "OPENGL_SHADER",
                LogType::LOG_ERROR,
                2);

            return ShaderCheckResult::RESULT_ALREADY_EXISTS;
        }
    }

    return ShaderCheckResult::RESULT_OK;
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
                vector<char> infoLog(logLength);
                glGetProgramInfoLog(
                    shader,
                    logLength,
                    nullptr,
                    infoLog.data());

                KalaWindowCore::ForceClose(
                    "OpenGL Shader Error",
                    "Shader linking failed (" + type + "):\n" + string(infoLog.data()));
            }
            else
            {
                KalaWindowCore::ForceClose(
                    "OpenGL Shader Error",
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
                vector<char> infoLog(logLength);
                glGetShaderInfoLog(
                    shader,
                    logLength,
                    nullptr,
                    infoLog.data());

                KalaWindowCore::ForceClose(
                    "OpenGL Shader Error",
                    "Shader compilation failed (" + type + "):\n" + string(infoLog.data()));
            }
            else
            {
                KalaWindowCore::ForceClose(
                    "OpenGL Shader Error",
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

bool InitShader(
    ShaderType type,
    const string& shaderPath,
    u32& programID,
    u32& shaderID)
{
    string shaderType = OpenGL_Shader::GetShaderTypeName(type);
    string shaderName = path(shaderPath).filename().string();

    Log::Print(
        "Loading " + shaderType + " shader: " + shaderPath,
        "OPENGL_SHADER",
        LogType::LOG_INFO);

    ifstream shaderFile(shaderPath);
    if (!shaderFile.is_open())
    {
        KalaWindowCore::ForceClose(
            "OpenGL Shader Error",
            "Failed to open " + shaderType + " shader file '" + shaderName + "'!");
        return false;
    }

    stringstream shaderStream{};
    shaderStream << shaderFile.rdbuf();
    const string shaderCodeString = shaderStream.str();
    const char* shaderCodeChar = shaderCodeString.c_str();

    GLenum shaderEnum{};
    switch (type)
    {
    case ShaderType::Shader_Vertex:
        shaderEnum = GL_VERTEX_SHADER; break;
    case ShaderType::Shader_Fragment:
        shaderEnum = GL_FRAGMENT_SHADER; break;
    case ShaderType::Shader_Geometry:
        shaderEnum = GL_GEOMETRY_SHADER; break;
    }

    shaderID = glCreateShader(shaderEnum);
    glShaderSource(
        shaderID,
        1,
        &shaderCodeChar,
        nullptr);
    glCompileShader(shaderID);

    string capitalShaderName{};
    switch (type)
    {
    case ShaderType::Shader_Vertex:
        capitalShaderName = "VERTEX"; break;
    case ShaderType::Shader_Fragment:
        capitalShaderName = "FRAGMENT"; break;
    case ShaderType::Shader_Geometry:
        capitalShaderName = "GEOMETRY"; break;
    }

    if (!CheckCompileErrors(shaderID, capitalShaderName))
    {
        if (programID != 0)
        {
            glDetachShader(
                programID,
                shaderID);
        }
        glDeleteShader(shaderID);

        KalaWindowCore::ForceClose(
            "OpenGL Shader Error",
            "Failed to compile " + shaderType + " shader '" + shaderName + "'!");

        return false;
    }

    return true;
}