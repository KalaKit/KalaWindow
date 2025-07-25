//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <vector>

#include "graphics/window.hpp"
#include "graphics/opengl/shader_opengl.hpp"
#include "graphics/opengl/opengl.hpp"
#include "graphics/opengl/opengl_core.hpp"
#include "core/log.hpp"

using KalaWindow::Graphics::ShutdownState;
using KalaWindow::Graphics::Window;
using KalaWindow::Graphics::PopupAction;
using KalaWindow::Graphics::PopupType;
using KalaWindow::Graphics::PopupResult;
using KalaWindow::Core::Logger;
using KalaWindow::Core::LogType;
using KalaWindow::Core::TimeFormat;
using KalaWindow::Core::DateFormat;
using KalaWindow::Graphics::OpenGL::Shader_OpenGL;
using KalaWindow::Graphics::OpenGL::ShaderType;
using KalaWindow::Graphics::OpenGL::ShaderStage;

using std::string;
using std::to_string;
using std::ifstream;
using std::stringstream;
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

static void ForceClose(
    const string& title,
    const string& reason);

static ShaderCheckResult IsShaderValid(
    const string& shaderName,
    const vector<ShaderStage>& shaderStages);

static bool CheckCompileErrors(GLuint shader, const string& type);

static bool InitShader(
    ShaderType type,
    const string& shaderPath,
    unsigned int& programID,
    unsigned int& shaderID);

namespace KalaWindow::Graphics::OpenGL
{
    Shader_OpenGL* Shader_OpenGL::CreateShader(
        const string& shaderName,
        const vector<ShaderStage>& shaderStages,
        Window* newWindow)
    {
        if (!Renderer_OpenGL::IsInitialized())
        {
            Logger::Print(
                "Cannot create shader '" + shaderName + "' because OpenGL is not initialized!",
                "SHADER_OPENGL",
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
            return createdShaders[shaderName].get();
        }

        unique_ptr<Shader_OpenGL> newShader = make_unique<Shader_OpenGL>();
        Shader_OpenGL* shaderPtr = newShader.get();
        ShaderStage newVertStage{};
        ShaderStage newFragStage{};
        ShaderStage newGeomStage{};

        for (const auto& stage : shaderStages)
        {
            string shaderType = GetShaderTypeName(stage.shaderType);
            string shaderPathName = path(stage.shaderPath).filename().string();

            if (stage.shaderPath.empty())
            {
                string title = "OpenGL error [shader_opengl]";
                string reason =
                    "Shader '" + shaderName + "' with type '"
                    + shaderType + "' has no assigned path!";

                ForceClose(title, reason);

                return nullptr;
            }

            if (!exists(stage.shaderPath))
            {
                string title = "OpenGL error [shader_opengl]";
                string reason = 
                    "Shader '" + shaderName + "' with type '"
                    + shaderType + "' has an invalid path '" + shaderPathName + "'!";

                ForceClose(title, reason);

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
            Logger::Print(
                "Skipped loading vertex shader because it was not assigned as a shader stage.",
                "SHADER_OPENGL",
                LogType::LOG_INFO);
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
            Logger::Print(
                "Skipped loading fragment shader because it was not assigned as a shader stage.",
                "SHADER_OPENGL",
                LogType::LOG_INFO);
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
            Logger::Print(
                "Skipped loading geometry shader because it was not assigned as a shader stage.",
                "SHADER_OPENGL",
                LogType::LOG_INFO);
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

        GLint success = 0;
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

            GLint logLength = 0;
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

                Logger::Print(
                    "Shader link failed:\n" + string(log.data()),
                    "SHADER_OPENGL",
                    LogType::LOG_ERROR,
                    2);
            }
            else
            {
                Logger::Print(
                    "Shader linking failed, but GL_INFO_LOG_LENGTH was 0 (no error message).",
                    "SHADER_OPENGL",
                    LogType::LOG_ERROR,
                    2);
            }

            if (!geomShaderExists)
            {
                ForceClose(
                    "OpenGL error [shader_opengl]",
                    "Failed to link vertex shader '" +
                    vertShaderPathName + "' and fragment shader '" +
                    fragShaderPathName + "' to program!");
            }
            else
            {
                ForceClose(
                    "OpenGL error [shader_opengl]",
                    "Failed to link vertex shader '" +
                    vertShaderPathName + "', fragment shader '" +
                    fragShaderPathName + "' and geometry shader '" +
                    geomShaderPathName + "' to program!");
            }

            return nullptr;
        }

        //validate the shader program before using it
        glValidateProgram(shaderPtr->programID);
        GLint validated = 0;
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

            GLint logLength = 0;
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

                ForceClose(
                    "OpenGL error [shader_opengl]",
                    "Shader program validation failed for shader '" + shaderName + "'! Reason:\n" + logStr);

                return nullptr;
            }

            ForceClose(
                "OpenGL error [shader_opengl]",
                "Shader program validation failed for shader '" + shaderName + "'! No log info was provided.");

            return nullptr;
        }

        GLint valid = glIsProgram(shaderPtr->programID);
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

            string title = "OpenGL error [shader_opengl]";
            string reason = "Shader program ID " + to_string(shaderPtr->programID) + " for shader '" + shaderName + "' is not valid!";

            ForceClose(title, reason);

            return nullptr;
        }
        else
        {
            Logger::Print(
                "Shader program ID " + to_string(shaderPtr->programID) + " for shader '" + shaderName + "' is valid!",
                "SHADER_OPENGL",
                LogType::LOG_SUCCESS);
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

        newShader->name = shaderName;
        newShader->targetWindow = newWindow;
        createdShaders[shaderName] = move(newShader);

        return shaderPtr;
    }

    bool Shader_OpenGL::Bind() const
    {
        if (!Renderer_OpenGL::IsInitialized())
        {
            Logger::Print(
                "Cannot bind shader '" + name + "' because OpenGL is not initialized!",
                "SHADER_OPENGL",
                LogType::LOG_ERROR,
                2);
            return false;
        }

        if (targetWindow == nullptr)
        {
            Logger::Print(
                "Cannot bind shader '" + name + "' because the window reference is invalid!",
                "SHADER_OPENGL",
                LogType::LOG_ERROR,
                2);
            return false;
        }

        OpenGLData& oData = targetWindow->GetOpenGLData();

        unsigned int& lastProgramID = oData.lastProgramID;
        unsigned int ID = programID;

        if (ID == 0)
        {
            Logger::Print(
                "OpenGL shader bind failed! ID is 0.",
                "SHADER_OPENGL",
                LogType::LOG_ERROR,
                2);
            return false;
        }

        Renderer_OpenGL::MakeContextCurrent(targetWindow);
        if (!Renderer_OpenGL::IsContextValid(targetWindow))
        {
            Logger::Print(
                "OpenGL shader bind failed! OpenGL context is invalid.",
                "SHADER_OPENGL",
                LogType::LOG_ERROR,
                2);
            return false;
        }

        if (ID == lastProgramID) return true;

#ifdef _DEBUG
        Logger::Print(
            "glUseProgram(" + to_string(ID) + ")",
            "SHADER_OPENGL",
            LogType::LOG_DEBUG);

        GLint linked = 0;
        glGetProgramiv(
            ID,
            GL_LINK_STATUS,
            &linked);
        if (linked != GL_TRUE)
        {
            Logger::Print(
                "GL_LINK_STATUS = " + to_string(linked),
                "SHADER_OPENGL",
                LogType::LOG_DEBUG);
        }

        GLint validated = 0;
        glGetProgramiv(
            ID,
            GL_VALIDATE_STATUS,
            &validated);
        if (validated != GL_TRUE)
        {
            Logger::Print(
                "GL_VALIDATE_STATUS = " + to_string(validated),
                "SHADER_OPENGL",
                LogType::LOG_DEBUG);
        }

        {
            GLenum e;
            while ((e = glGetError()) != GL_NO_ERROR) 
            {
                // discard
            }
        }
#endif

        glUseProgram(ID);

#ifdef _DEBUG
        GLenum err = glGetError();
        GLint activeProgram = 0;
        glGetIntegerv(
            GL_CURRENT_PROGRAM,
            &activeProgram);

        bool programMisMatch = activeProgram != (GLint)ID;
        bool hasError = err != GL_NO_ERROR;

        if (programMisMatch
            || hasError)
        {
            if (hasError)
            {
                unsigned int errInt = static_cast<unsigned int>(err);
                const char* errorMsg = Renderer_OpenGL::GetGLErrorString(errInt);

                Logger::Print(
                    "glUseProgram error: " + string(errorMsg),
                    "SHADER_OPENGL",
                    LogType::LOG_ERROR,
                    2);
            }

            if (activeProgram != (GLint)ID)
            {
                Logger::Print(
                    "OpenGL shader bind failed! Program ID not bound after glUseProgram." +
                    string("Expected ID: '") + to_string(ID) + "', but got: '" + to_string(activeProgram) + "'.",
                    "SHADER_OPENGL",
                    LogType::LOG_ERROR,
                    2);
                return false;
            }
        }
#endif

        lastProgramID = ID;
        oData.lastProgramID = lastProgramID;

        return true;
    }

    void Shader_OpenGL::HotReload()
    {
        if (!Renderer_OpenGL::IsInitialized())
        {
            Logger::Print(
                "Cannot hot reload shader '" + name + "' because OpenGL is not initialized!",
                "SHADER_OPENGL",
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

        auto reloadedShader = Shader_OpenGL::CreateShader(
            shaderName,
            stagesToReload,
            targetWindow);
        if (!reloadedShader)
        {
            Logger::Print(
                "Hot reload failed for shader '" + shaderName + "'! Keeping old version.",
                "SHADER_OPENGL",
                LogType::LOG_ERROR,
                2);
            return;
        }

        //replace internal data
        shaders = reloadedShader->shaders;

        Logger::Print(
            "Shader '" + shaderName + "' was hot reloaded!",
            "SHADER_OPENGL",
            LogType::LOG_SUCCESS);
    }

    void Shader_OpenGL::SetBool(
        unsigned int programID,
        const string& name, 
        bool value) const
    {
        glUniform1i(glGetUniformLocation(
            programID, 
            name.c_str()), 
            (int)value);
    }
    void Shader_OpenGL::SetInt(
        unsigned int programID,
        const string& name, 
        int value) const
    {
        glUniform1i(glGetUniformLocation(
            programID, 
            name.c_str()), 
            value);
    }
    void Shader_OpenGL::SetFloat(
        unsigned int programID,
        const string& name, 
        float value) const
    {
        glUniform1f(glGetUniformLocation(
            programID, 
            name.c_str()), 
            value);
    }

    void Shader_OpenGL::SetVec2(
        unsigned int programID,
        const string& name, 
        const vec2& value) const
    {
        auto loc = glGetUniformLocation(programID, name.c_str());
        glUniform2fv(
            loc, 
            1, 
            &value.x);
    }
    void Shader_OpenGL::SetVec3(
        unsigned int programID,
        const string& name, 
        const vec3& value) const
    {
        auto loc = glGetUniformLocation(programID, name.c_str());
        glUniform3fv(
            loc, 
            1, 
            &value.x);
    }
    void Shader_OpenGL::SetVec4(
        unsigned int programID,
        const string& name, 
        const vec4& value) const
    {
        auto loc = glGetUniformLocation(programID, name.c_str());
        glUniform4fv(
            loc, 
            1, 
            &value.x);
    }

    void Shader_OpenGL::SetMat2(
        unsigned int programID,
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
    void Shader_OpenGL::SetMat3(
        unsigned int programID,
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
    void Shader_OpenGL::SetMat4(
        unsigned int programID,
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

    Shader_OpenGL::~Shader_OpenGL()
    {
        for (auto& shaderData : GetAllShaders())
        {
            for (auto& shaderStage : shaders)
            {
                if (shaderStage.shaderID != 0)
                {
                    unsigned int programID = GetProgramID();
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
    }
}

void ForceClose(
    const string& title,
    const string& reason)
{
    Logger::Print(
        reason,
        "SHADER_OPENGL",
        LogType::LOG_ERROR,
        2,
        TimeFormat::TIME_NONE,
        DateFormat::DATE_NONE);

    Window* mainWindow = Window::windows.front();
    if (mainWindow->CreatePopup(
        title,
        reason,
        PopupAction::POPUP_ACTION_OK,
        PopupType::POPUP_TYPE_ERROR)
        == PopupResult::POPUP_RESULT_OK)
    {
        Window::Shutdown(ShutdownState::SHUTDOWN_FAILURE);
    }
}

ShaderCheckResult IsShaderValid(
    const string& shaderName,
    const vector<ShaderStage>& shaderStages)
{
    //shader name must be assigned

    if (shaderName.empty())
    {
        string title = "OpenGL error [shader_opengl]";
        string reason = "Cannot load a shader with no name!";

        ForceClose(title, reason);

        return ShaderCheckResult::RESULT_INVALID;
    }

    //shader stages must not be empty

    if (shaderStages.empty())
    {
        string title = "OpenGL error [shader_opengl]";
        string reason = "Cannot load a shader with no stages!";

        ForceClose(title, reason);

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
            string title = "OpenGL error [shader_opengl]";
            string reason = "Cannot load shader '" + shaderName + "' with no file paths!";

            ForceClose(title, reason);

            return ShaderCheckResult::RESULT_INVALID;
        }

        string shaderFileName = path(stage.shaderPath).filename().string();

        //shader file path must exist

        if (!exists(stage.shaderPath))
        {
            string title = "OpenGL error [shader_opengl]";
            string reason = "Shader '" + shaderName + "' path '" + shaderFileName + "' does not exist!";

            ForceClose(title, reason);

            return ShaderCheckResult::RESULT_INVALID;
        }

        //shader file path must have extension

        if (!path(stage.shaderPath).has_extension())
        {
            string title = "OpenGL error [shader_opengl]";
            string reason = "Shader '" + shaderName + "' path '" + shaderFileName + "' has no extension. You must use .vert, .frag or .geom";

            ForceClose(title, reason);

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
            string title = "OpenGL error [texture]";
            string reason = "Shader '" + shaderName + "' path '" + shaderFileName + "' has an invalid extension '" + thisExtension + "'. Only .vert, .frag and .geom are allowed!";

            ForceClose(title, reason);

            return ShaderCheckResult::RESULT_INVALID;
        }

        //vert type must match extension

        if (stage.shaderType == ShaderType::Shader_Vertex
            && thisExtension != ".vert")
        {
            string title = "OpenGL error [texture]";
            string reason = "Shader '" + shaderName + "' path '" + shaderFileName + "' has extension '" + thisExtension + "' but its type was set to 'Shader_Vertex'. Type and extension must always match!";

            ForceClose(title, reason);

            return ShaderCheckResult::RESULT_INVALID;
        }

        //frag type must match extension

        if (stage.shaderType == ShaderType::Shader_Fragment
            && thisExtension != ".frag")
        {
            string title = "OpenGL error [texture]";
            string reason = "Shader '" + shaderName + "' path '" + shaderFileName + "' has extension '" + thisExtension + "' but its type was set to 'Shader_Fragment'. Type and extension must always match!";

            ForceClose(title, reason);

            return ShaderCheckResult::RESULT_INVALID;
        }

        //geom type must match extension

        if (stage.shaderType == ShaderType::Shader_Geometry
            && thisExtension != ".geom")
        {
            string title = "OpenGL error [texture]";
            string reason = "Shader '" + shaderName + "' path '" + shaderFileName + "' has extension '" + thisExtension + "' but its type was set to 'Shader_Geometry'. Type and extension must always match!";

            ForceClose(title, reason);

            return ShaderCheckResult::RESULT_INVALID;
        }
    }

    //pass existing one if shader with same name already exists

    for (const auto& [key, _] : Shader_OpenGL::createdShaders)
    {
        if (key == shaderName)
        {
            string reason =
                "Shader '" + shaderName + "' already exists!";

            Logger::Print(
                reason,
                "SHADER_OPENGL",
                LogType::LOG_ERROR,
                2);

            return ShaderCheckResult::RESULT_ALREADY_EXISTS;
        }
    }

    return ShaderCheckResult::RESULT_OK;
}

bool CheckCompileErrors(GLuint shader, const string& type)
{
    GLint success = 0;
    GLint logLength = 0;

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

                Logger::Print(
                    "Shader linking failed (" + type + "):\n" + string(infoLog.data()),
                    "SHADER_OPENGL",
                    LogType::LOG_ERROR,
                    2);
            }
            else
            {
                Logger::Print(
                    "Shader linking failed (" + type + "), but no log was returned.",
                    "SHADER_OPENGL",
                    LogType::LOG_ERROR,
                    2);
            }
            return false;
        }
        else
        {
            Logger::Print(
                "Shader linking succeeded (" + type + ")",
                "SHADER_OPENGL",
                LogType::LOG_SUCCESS);
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

                Logger::Print(
                    "Shader compilation failed (" + type + "):\n" + string(infoLog.data()),
                    "SHADER_OPENGL",
                    LogType::LOG_ERROR,
                    2);
            }
            else
            {
                Logger::Print(
                    "Shader compilation failed (" + type + "), but no log was returned.",
                    "SHADER_OPENGL",
                    LogType::LOG_ERROR,
                    2);
            }
            return false;
        }
        else
        {
            Logger::Print(
                "Shader compilation succeeded (" + type + ")",
                "SHADER_OPENGL",
                LogType::LOG_SUCCESS);
        }
    }

    return true;
}

bool InitShader(
    ShaderType type,
    const string& shaderPath,
    unsigned int& programID,
    unsigned int& shaderID)
{
    string shaderType = Shader_OpenGL::GetShaderTypeName(type);
    string shaderName = path(shaderPath).filename().string();

    Logger::Print(
        "Loading " + shaderType + " shader: " + shaderPath,
        "SHADER_OPENGL",
        LogType::LOG_INFO);

    ifstream shaderFile(shaderPath);
    if (!shaderFile.is_open())
    {
        ForceClose(
            "OpenGL error [shader_opengl]",
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

        ForceClose(
            "OpenGL error [shader_opengl]",
            "Failed to compile " + shaderType + " shader '" + shaderName + "'!");

        return false;
    }

    return true;
}