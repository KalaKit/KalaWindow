//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef KALAWINDOW_SUPPORT_OPENGL

#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>

#ifdef _WIN32
#include <Windows.h>
#include "GL/gl.h"
#elif __linux__
#include <GL/glx.h>
#include <EGL/egl.h>
#endif

#include "graphics/window.hpp"
#include "graphics/opengl/shader_opengl.hpp"
#include "graphics/opengl/opengl.hpp"
#include "graphics/opengl/opengl_typedefs.hpp"
#include "graphics/opengl/opengl_loader.hpp"
#include "graphics/render.hpp"
#include "core/enums.hpp"
#include "core/log.hpp"

using KalaWindow::Graphics::OpenGLLoader;
using KalaWindow::Graphics::Render;
using KalaWindow::Graphics::ShutdownState;
using KalaWindow::Graphics::Window;
using KalaWindow::PopupAction;
using KalaWindow::PopupType;
using KalaWindow::PopupResult;
using KalaWindow::Core::Logger;
using KalaWindow::Core::LogType;

using std::string;
using std::to_string;
using std::ifstream;
using std::stringstream;
using std::make_unique;
using std::filesystem::exists;
using std::filesystem::path;

static bool CheckCompileErrors(GLuint shader, const string& type);

static void ForceClose(
    const string& title,
    const string& reason,
    ShutdownState state = ShutdownState::SHUTDOWN_FAILURE)
{
    Logger::Print(
        reason,
        "SHADER_OPENGL",
        LogType::LOG_ERROR,
        false,
        2);

    Window* mainWindow = Window::windows.front();
    if (mainWindow->CreatePopup(
        title,
        reason,
        PopupAction::POPUP_ACTION_OK,
        PopupType::POPUP_TYPE_ERROR)
        == PopupResult::POPUP_RESULT_OK)
    {
        Render::Shutdown(ShutdownState::SHUTDOWN_FAILURE);
    }
}

namespace KalaWindow::Graphics
{
    unique_ptr<Shader_OpenGL> Shader_OpenGL::CreateShader(
        const string& shaderName,
        const vector<ShaderStage>& shaderStages)
    {
        unique_ptr<Shader_OpenGL> newShader = make_unique<Shader_OpenGL>();
        ShaderData newShaderData{};
        ShaderStage newVertStage{};
        ShaderStage newFragStage{};
        ShaderStage newGeomStage{};

        if (shaderName.empty())
        {
            Logger::Print(
                "Cannot create a shader with no name!",
                "SHADER_OPENGL",
                LogType::LOG_ERROR,
                true,
                2);
            return nullptr;
        }
        for (const auto& [key, _] : createdShaders)
        {
            if (key == shaderName)
            {
                Logger::Print(
                    "Cannot create a shader with the name '" + shaderName 
                    + "' because a shader with that name already exists!",
                    "SHADER_OPENGL",
                    LogType::LOG_ERROR);
                return nullptr;
            }
        }

        if (shaderStages.empty())
        {
            Logger::Print(
                "Cannot create a shader with no stages!",
                "SHADER_OPENGL",
                LogType::LOG_ERROR,
                true,
                2);
            return nullptr;
        }

        for (const auto& stage : shaderStages)
        {
            string shaderType{};
            for (const auto& stage : shaderStages)
            {
                switch (stage.shaderType)
                {
                case ShaderType::Shader_Vertex:
                    shaderType = "vertex";
                    break;
                case ShaderType::Shader_Fragment:
                    shaderType = "fragment";
                    break;
                case ShaderType::Shader_Geometry:
                    shaderType = "geometry";
                    break;
                }
            }

            if (stage.shaderPath.empty())
            {
                Logger::Print(
                    "Shader '" + shaderName + "' with type '"
                    + shaderType + "' has no assigned path!",
                    "SHADER_OPENGL",
                    LogType::LOG_ERROR,
                    true,
                    2);
                return nullptr;
            }
            else if (!exists(stage.shaderPath))
            {
                Logger::Print(
                    "Shader '" + shaderName + "' with type '"
                    + shaderType + "' has an invalid path '" + stage.shaderPath + "'!",
                    "SHADER_OPENGL",
                    LogType::LOG_ERROR,
                    true,
                    2);
                return nullptr;
            }
            else
            {
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
                LogType::LOG_INFO,
                true,
                0);
        }
        else
        {
            Logger::Print(
                "Loading vertex shader: " + newVertStage.shaderPath,
                "SHADER_OPENGL",
                LogType::LOG_INFO,
                true,
                0);

            ifstream vertexFile(newVertStage.shaderPath);
            if (!vertexFile.is_open())
            {
                ForceClose(
                    "OpenGL error",
                    "[Shader_OpenGL] Failed to open vertex shader file: " + newVertStage.shaderPath);
                return nullptr;
            }

            stringstream vertexStream{};
            vertexStream << vertexFile.rdbuf();
            const string vertexCode = vertexStream.str();
            const char* vShaderCode = vertexCode.c_str();

            newVertStage.shaderID = OpenGLLoader::glCreateShader(GL_VERTEX_SHADER);
            OpenGLLoader::glShaderSource(
                newVertStage.shaderID,
                1, 
                &vShaderCode, 
                nullptr);
            OpenGLLoader::glCompileShader(newVertStage.shaderID);

            if (!CheckCompileErrors(newVertStage.shaderID, "VERTEX"))
            {
                OpenGLLoader::glDetachShader(newVertStage.shaderID);
                OpenGLLoader::glDeleteShader(newVertStage.shaderID);

                ForceClose(
                    "OpenGL error",
                    "[Shader_OpenGL] Vertex shader '" + newVertStage.shaderPath + "' failed to compile!");

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
                LogType::LOG_INFO,
                true,
                0);
        }
        else
        {
            Logger::Print(
                "Loading fragment shader: " + newFragStage.shaderPath,
                "SHADER_OPENGL",
                LogType::LOG_INFO,
                true,
                0);

            ifstream fragmentFile(newFragStage.shaderPath);
            if (!fragmentFile.is_open())
            {
                ForceClose(
                    "OpenGL error",
                    "[Shader_OpenGL] Failed to open fragment shader file: " + newFragStage.shaderPath);

                return nullptr;
            }

            stringstream fragmentStream{};
            fragmentStream << fragmentFile.rdbuf();
            const string fragmentCode = fragmentStream.str();
            const char* fShaderCode = fragmentCode.c_str();

            newFragStage.shaderID = OpenGLLoader::glCreateShader(GL_FRAGMENT_SHADER);
            OpenGLLoader::glShaderSource(
                newFragStage.shaderID,
                1, 
                &fShaderCode, 
                nullptr);
            OpenGLLoader::glCompileShader(newFragStage.shaderID);

            if (!CheckCompileErrors(newFragStage.shaderID, "FRAGMENT"))
            {
                OpenGLLoader::glDetachShader(newFragStage.shaderID);
                OpenGLLoader::glDeleteShader(newFragStage.shaderID);

                ForceClose(
                    "OpenGL error",
                    "[Shader_OpenGL] Fragment shader '" + newFragStage.shaderPath + "' failed to compile!");

                return nullptr;
            }
        }

        //
        // CREATE AND COMPILE GEOMETRY SHADER
        //

        if (!geomShaderExists)
        {
            Logger::Print(
                "Skipped loading fragment shader because it was not assigned as a shader stage.",
                "SHADER_OPENGL",
                LogType::LOG_INFO,
                true,
                0);
        }
        else
        {
            Logger::Print(
                "Loading geometry shader: " + newGeomStage.shaderPath,
                "SHADER_OPENGL",
                LogType::LOG_INFO,
                true,
                0);

            ifstream geometryFile(newGeomStage.shaderPath);
            if (!geometryFile.is_open())
            {
                ForceClose(
                    "OpenGL error",
                    "[Shader_OpenGL] Failed to open geometry shader file: " + newGeomStage.shaderPath);

                return nullptr;
            }

            stringstream geometryStream{};
            geometryStream << geometryFile.rdbuf();
            const string geometryCode = geometryStream.str();
            const char* gShaderCode = geometryCode.c_str();

            newGeomStage.shaderID = OpenGLLoader::glCreateShader(GL_GEOMETRY_SHADER);
            OpenGLLoader::glShaderSource(
                newGeomStage.shaderID,
                1, 
                &gShaderCode, 
                nullptr);
            OpenGLLoader::glCompileShader(newGeomStage.shaderID);

            if (!CheckCompileErrors(newGeomStage.shaderID, "GEOMETRY"))
            {
                OpenGLLoader::glDetachShader(newGeomStage.shaderID);
                OpenGLLoader::glDeleteShader(newGeomStage.shaderID);

                ForceClose(
                    "OpenGL error",
                    "[Shader_OpenGL] geometry shader '" + newGeomStage.shaderPath + "' failed to compile!");

                return nullptr;
            }
        }

        //
        // CREATE SHADER PROGRAM
        //

        newShaderData.programID = OpenGLLoader::glCreateProgram();

        OpenGLLoader::glAttachShader(
            newShaderData.programID, 
            newVertStage.shaderID);
        OpenGLLoader::glAttachShader(
            newShaderData.programID, 
            newFragStage.shaderID);
        if (geomShaderExists)
        {
            OpenGLLoader::glAttachShader(
                newShaderData.programID, 
                newGeomStage.shaderID);
        }
        OpenGLLoader::glLinkProgram(newShaderData.programID);

        GLint success = 0;
        OpenGLLoader::glGetProgramiv(
            newShaderData.programID, 
            GL_LINK_STATUS, 
            &success);

        if (success != GL_TRUE)
        {
            OpenGLLoader::glDetachShader(newVertStage.shaderID);
            OpenGLLoader::glDeleteShader(newVertStage.shaderID);

            OpenGLLoader::glDetachShader(newFragStage.shaderID);
            OpenGLLoader::glDeleteShader(newFragStage.shaderID);

            if (geomShaderExists)
            {
                OpenGLLoader::glDetachShader(newGeomStage.shaderID);
                OpenGLLoader::glDeleteShader(newGeomStage.shaderID);
            }

            GLint logLength = 0;
            OpenGLLoader::glGetProgramiv(
                newShaderData.programID, 
                GL_INFO_LOG_LENGTH, 
                &logLength);

            if (logLength > 0)
            {
                vector<GLchar> log(logLength);
                OpenGLLoader::glGetProgramInfoLog(
                    newShaderData.programID, 
                    logLength, 
                    nullptr, 
                    log.data());

                Logger::Print(
                    "Shader link failed:\n" + string(log.data()),
                    "SHADER_OPENGL",
                    LogType::LOG_ERROR,
                    true,
                    2);
            }

            if (!geomShaderExists)
            {
                ForceClose(
                    "OpenGL error",
                    "[Shader_OpenGL] Failed to link vertex shader '" +
                    newVertStage.shaderPath + "' and fragment shader '" +
                    newFragStage.shaderPath + "' to program!");
            }
            else
            {
                ForceClose(
                    "OpenGL error",
                    "[Shader_OpenGL] Failed to link vertex shader '" +
                    newVertStage.shaderPath + "', fragment shader '" +
                    newFragStage.shaderPath + "' and geometry shader '" +
                    newGeomStage.shaderPath + "' to program!");
            }

            return nullptr;
        }

        //validate the shader program before using it
        OpenGLLoader::glValidateProgram(newShaderData.programID);
        GLint validated = 0;
        OpenGLLoader::glGetProgramiv(
            newShaderData.programID, 
            GL_VALIDATE_STATUS, 
            &validated);
        if (validated != GL_TRUE)
        {
            OpenGLLoader::glDetachShader(newVertStage.shaderID);
            OpenGLLoader::glDeleteShader(newVertStage.shaderID);

            OpenGLLoader::glDetachShader(newFragStage.shaderID);
            OpenGLLoader::glDeleteShader(newFragStage.shaderID);

            if (geomShaderExists)
            {
                OpenGLLoader::glDetachShader(newGeomStage.shaderID);
                OpenGLLoader::glDeleteShader(newGeomStage.shaderID);
            }

            GLint logLength = 0;
            OpenGLLoader::glGetProgramiv(
                newShaderData.programID, 
                GL_INFO_LOG_LENGTH, 
                &logLength);
            if (logLength > 0)
            {
                vector<GLchar> log(logLength);
                OpenGLLoader::glGetProgramInfoLog(
                    newShaderData.programID, 
                    logLength, 
                    nullptr, 
                    log.data());
                
                string logStr(log.begin(), log.end());

                ForceClose(
                    "OpenGL error",
                    "[Shader_OpenGL] Shader program validation failed:\n" + logStr);

                return nullptr;
            }

            ForceClose(
                "OpenGL error",
                "[Shader_OpenGL] Shader program validation failed!");

            return nullptr;
        }

        GLint valid = OpenGLLoader::glIsProgram(newShaderData.programID);
        bool isProgramValid = valid == GL_TRUE;
        if (!isProgramValid)
        {
            OpenGLLoader::glDetachShader(newVertStage.shaderID);
            OpenGLLoader::glDeleteShader(newVertStage.shaderID);

            OpenGLLoader::glDetachShader(newFragStage.shaderID);
            OpenGLLoader::glDeleteShader(newFragStage.shaderID);

            if (geomShaderExists)
            {
                OpenGLLoader::glDetachShader(newGeomStage.shaderID);
                OpenGLLoader::glDeleteShader(newGeomStage.shaderID);
            }

            Logger::Print(
                "Shader program ID " + to_string(newShaderData.programID) + " is not valid!",
                "SHADER_OPENGL",
                LogType::LOG_ERROR,
                true,
                2);

            return nullptr;
        }
        else
        {
            Logger::Print(
                "Shader program ID " + to_string(newShaderData.programID) + " is valid!",
                "SHADER_OPENGL",
                LogType::LOG_ERROR,
                true,
                2);
        }

        //
        // CLEANUP
        //

        OpenGLLoader::glDetachShader(newVertStage.shaderID);
        OpenGLLoader::glDeleteShader(newVertStage.shaderID);

        OpenGLLoader::glDetachShader(newFragStage.shaderID);
        OpenGLLoader::glDeleteShader(newFragStage.shaderID);

        if (geomShaderExists)
        {
            OpenGLLoader::glDetachShader(newGeomStage.shaderID);
            OpenGLLoader::glDeleteShader(newGeomStage.shaderID);
        }

        if (vertShaderExists) newShaderData.stages.push_back(newVertStage);
        if (fragShaderExists) newShaderData.stages.push_back(newFragStage);
        if (geomShaderExists) newShaderData.stages.push_back(newGeomStage);

        newShader->name = shaderName;
        newShader->shaders.push_back(newShaderData);
        createdShaders[shaderName] = newShader.get();

        return newShader;
    }

    bool Shader_OpenGL::Bind(
        Window* window,
        const ShaderData& shaderData) const
    {
#ifdef _WIN32
        auto& oglData = window->GetWindow_Windows().openglData;
#elif __linux__
        auto& oglData = window->GetWindow_X11().openglData;
#endif
        unsigned int& lastProgramID = oglData.lastProgramID;
        unsigned int ID = shaderData.programID;

        if (ID == 0)
        {
            Logger::Print(
                "OpenGL shader bind failed! ID is 0.",
                "SHADER_OPENGL",
                LogType::LOG_ERROR,
                true,
                2);
            return false;
        }

        if (!Renderer_OpenGL::IsContextValid(window))
        {
            Logger::Print(
                "OpenGL shader bind failed! OpenGL context is invalid.",
                "SHADER_OPENGL",
                LogType::LOG_ERROR,
                true,
                2);
            return false;
        }

        if (ID == lastProgramID) return true;

        OpenGLLoader::glUseProgram(ID);

#ifdef _DEBUG
        GLenum err = glGetError();
        if (err != GL_NO_ERROR)
        {
            unsigned int errInt = static_cast<unsigned int>(err);
            const char* errorMsg = Renderer_OpenGL::GetGLErrorString(errInt);

            Logger::Print(
                "glUseProgram error: " + string(errorMsg),
                "SHADER_OPENGL",
                LogType::LOG_ERROR,
                true,
                2);
        }

        GLint activeProgram = 0;
        OpenGLLoader::glGetIntegerv(GL_CURRENT_PROGRAM, &activeProgram);

        if (activeProgram != (GLint)ID)
        {
            Logger::Print(
                "OpenGL shader bind failed! Program ID not bound after glUseProgram." +
                string("Expected ID: '") + to_string(ID) + "', but got: '" + to_string(activeProgram) + "'.",
                "SHADER_OPENGL",
                LogType::LOG_ERROR,
                true,
                2);
            return false;
        }
#endif

        lastProgramID = ID;

        return true;
    }

    void Shader_OpenGL::HotReload(Shader_OpenGL* shader)
    {
        if (shader == nullptr)
        {
            Logger::Print(
                "Cannot hot reload shader because it is null!",
                "SHADER_OPENGL",
                LogType::LOG_ERROR,
                true,
                2);
            return;
        }
        string shaderName = shader->name;

        //back up old data
        vector<ShaderData> oldShaders = shader->GetAllShaderData();

        //attepmt to recreate

        vector<ShaderStage> stagesToReload{};
        for (const auto& stage : oldShaders.front().stages)
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
            stagesToReload);
        if (!reloadedShader)
        {
            Logger::Print(
                "Hot reload failed for shader '" + shaderName + "'! Keeping old version.",
                "SHADER_OPENGL",
                LogType::LOG_ERROR,
                true,
                2);
            return;
        }

        //replace internal data
        shader->shaders = reloadedShader->shaders;

        Logger::Print(
            "Shader '" + shaderName + "' was hot reloaded!",
            "SHADER_OPENGL",
            LogType::LOG_SUCCESS,
            true,
            0);
    }

    void Shader_OpenGL::SetBool(
        unsigned int programID,
        const string& name, 
        bool value) const
    {
        OpenGLLoader::glUniform1i(OpenGLLoader::glGetUniformLocation(
            programID, 
            name.c_str()), 
            (int)value);
    }
    void Shader_OpenGL::SetInt(
        unsigned int programID,
        const string& name, 
        int value) const
    {
        OpenGLLoader::glUniform1i(OpenGLLoader::glGetUniformLocation(
            programID, 
            name.c_str()), 
            value);
    }
    void Shader_OpenGL::SetFloat(
        unsigned int programID,
        const string& name, 
        float value) const
    {
        OpenGLLoader::glUniform1f(OpenGLLoader::glGetUniformLocation(
            programID, 
            name.c_str()), 
            value);
    }

    void Shader_OpenGL::SetVec2(
        unsigned int programID,
        const string& name, 
        const kvec2& value) const
    {
        auto loc = OpenGLLoader::glGetUniformLocation(programID, name.c_str());
        OpenGLLoader::glUniform2fv(
            loc, 
            1, 
            &value.x);
    }
    void Shader_OpenGL::SetVec3(
        unsigned int programID,
        const string& name, 
        const kvec3& value) const
    {
        auto loc = OpenGLLoader::glGetUniformLocation(programID, name.c_str());
        OpenGLLoader::glUniform3fv(
            loc, 
            1, 
            &value.x);
    }
    void Shader_OpenGL::SetVec4(
        unsigned int programID,
        const string& name, 
        const kvec4& value) const
    {
        auto loc = OpenGLLoader::glGetUniformLocation(programID, name.c_str());
        OpenGLLoader::glUniform4fv(
            loc, 
            1, 
            &value.x);
    }

    void Shader_OpenGL::SetMat2(
        unsigned int programID,
        const string& name, 
        const kmat2& mat) const
    {
        auto loc = OpenGLLoader::glGetUniformLocation(programID, name.c_str());
        OpenGLLoader::glUniformMatrix2fv(
            loc, 
            1, 
            GL_FALSE, 
            &mat.columns[0].x);
    }
    void Shader_OpenGL::SetMat3(
        unsigned int programID,
        const string& name, 
        const kmat3& mat) const
    {
        auto loc = OpenGLLoader::glGetUniformLocation(programID, name.c_str());
        OpenGLLoader::glUniformMatrix3fv(
            loc, 
            1, 
            GL_FALSE, 
            &mat.columns[0].x);
    }
    void Shader_OpenGL::SetMat4(
        unsigned int programID,
        const string& name, 
        const kmat4& mat) const
    {
        auto loc = OpenGLLoader::glGetUniformLocation(programID, name.c_str());
        OpenGLLoader::glUniformMatrix4fv(
            loc, 
            1, 
            GL_FALSE, 
            &mat.columns[0].x);
    }

    void Shader_OpenGL::DestroyShader()
    {
        for (auto& shaderData : this->GetAllShaderData())
        {
            for (auto& shaderStage : shaderData.stages)
            {
                if (shaderStage.shaderID != 0)
                {
                    OpenGLLoader::glDetachShader(shaderStage.shaderID);
                    OpenGLLoader::glDeleteShader(shaderStage.shaderID);
                    shaderStage.shaderID = 0;
                }
            }
            if (shaderData.programID != 0)
            {
                OpenGLLoader::glDeleteProgram(shaderData.programID);
                shaderData.programID = 0;
            }
        }
        shaders.clear();
        createdShaders.erase(this->name);
    }
}

static bool CheckCompileErrors(GLuint shader, const string& type)
{
    GLint success = 0;
    GLchar infoLog[1024];

    if (type != "PROGRAM")
    {
        OpenGLLoader::glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            OpenGLLoader::glGetShaderInfoLog(shader, 1024, nullptr, infoLog);

            Logger::Print(
                "Shader compilation failed (" + type + "):\n" + infoLog,
                "SHADER_OPENGL",
                LogType::LOG_ERROR,
                true,
                2);
            return false;
        }
    }
    else
    {
        OpenGLLoader::glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            OpenGLLoader::glGetProgramInfoLog(shader, 1024, nullptr, infoLog);

            Logger::Print(
                "Program linking failed:\n" + string(infoLog),
                "SHADER_OPENGL",
                LogType::LOG_ERROR,
                true,
                2);
            return false;
        }
    }

    return true;
}

#endif //KALAWINDOW_SUPPORT_OPENGL