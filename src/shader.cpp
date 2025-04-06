//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

//main log macro
#define WRITE_LOG(type, msg) std::cout << "[KALAKIT_SHADER | " << type << "] " << msg << "\n"

//log types
#if KALAWINDOW_DEBUG
	#define LOG_DEBUG(msg) WRITE_LOG("DEBUG", msg)
#else
	#define LOG_DEBUG(msg)
#endif
#define LOG_SUCCESS(msg) WRITE_LOG("SUCCESS", msg)
#define LOG_ERROR(msg) WRITE_LOG("ERROR", msg)

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "glm/gtc/type_ptr.hpp"

#include "shader.hpp"
#include "opengl.hpp"
#include "opengl_loader.hpp"
#include "window.hpp"

using std::ifstream;
using std::stringstream;
using std::hex;
using std::to_string;

namespace KalaKit
{
    Shader::Shader(
        const string& vertexPath, 
        const string& fragmentPath)
    {
        //
        // READ SHADER SOURCE FILES
        //

        ifstream vertexFile(vertexPath);
        if (!vertexFile.is_open())
        {
            string title = "Shader error detected!";
            string message = "Vertex shader '" + vertexPath + "' file is invalid! Close program?";

            if (KalaWindow::CreatePopup(
                    title,
                    message,
                    PopupAction::POPUP_ACTION_YES_NO,
                    PopupType::POPUP_TYPE_ERROR)
                    == PopupResult::POPUP_RESULT_YES)
            {
                KalaWindow::SetShouldCloseState(true);
            }

            LOG_ERROR("Failed to open vertex shader file: " << vertexPath);
            isValid = false;
            ID = 0;
            return;
        }

        ifstream fragmentFile(fragmentPath);
        if (!fragmentFile.is_open())
        {
            string title = "Shader error detected!";
            string message = "Fragment shader '" + fragmentPath + "' file is invalid! Close program?";

            if (KalaWindow::CreatePopup(
                title,
                message,
                PopupAction::POPUP_ACTION_YES_NO,
                PopupType::POPUP_TYPE_ERROR)
                == PopupResult::POPUP_RESULT_YES)
            {
                KalaWindow::SetShouldCloseState(true);
            }

            LOG_ERROR("Failed to open fragment shader file: " << fragmentPath);
            isValid = false;
            ID = 0;
            return;
        }

        stringstream vertexStream, fragmentStream;

        vertexStream << vertexFile.rdbuf();
        fragmentStream << fragmentFile.rdbuf();

        const string vertexCode = vertexStream.str();
        const string fragmentCode = fragmentStream.str();
        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();

        //
        // CREATE AND COMPILE VERTEX SHADER
        //

        LOG_DEBUG("Loading vertex shader: " << vertexPath);

        GLuint vertex = OpenGLLoader::glCreateShaderPtr(GL_VERTEX_SHADER);
        OpenGLLoader::glShaderSourcePtr(vertex, 1, &vShaderCode, nullptr);
        OpenGLLoader::glCompileShaderPtr(vertex);

        if (!CheckCompileErrors(vertex, "VERTEX"))
        {
            string title = "Shader error detected!";
            string message = "Vertex shader '" + vertexPath + "' failed to compile! Close program?";

            if (KalaWindow::CreatePopup(
                title,
                message,
                PopupAction::POPUP_ACTION_YES_NO,
                PopupType::POPUP_TYPE_ERROR)
                == PopupResult::POPUP_RESULT_YES)
            {
                KalaWindow::SetShouldCloseState(true);
            }

            LOG_ERROR("Vertex shader compilation failed!");
            isValid = false;
            ID = 0;
            return;
        }

        //
        // CREATE AND COMPILE FRAGMENT SHADER
        //

        LOG_DEBUG("Loading fragment shader: " << fragmentPath);

        GLuint fragment = OpenGLLoader::glCreateShaderPtr(GL_FRAGMENT_SHADER);
        OpenGLLoader::glShaderSourcePtr(fragment, 1, &fShaderCode, nullptr);
        OpenGLLoader::glCompileShaderPtr(fragment);

        if (!CheckCompileErrors(fragment, "FRAGMENT"))
        {
            LOG_ERROR("Fragment shader compilation failed!");

            string title = "Shader error detected!";
            string message = "Fragment shader '" + fragmentPath + "' failed to compile! Close program?";

            if (KalaWindow::CreatePopup(
                title,
                message,
                PopupAction::POPUP_ACTION_YES_NO,
                PopupType::POPUP_TYPE_ERROR)
                == PopupResult::POPUP_RESULT_YES)
            {
                KalaWindow::SetShouldCloseState(true);
            }
            
            isValid = false;
            ID = 0;
            return;
        }

        //
        // CREATE SHADER PROGRAM
        //

        ID = OpenGLLoader::glCreateProgramPtr();
        OpenGLLoader::glAttachShaderPtr(ID, vertex);
        OpenGLLoader::glAttachShaderPtr(ID, fragment);
        OpenGLLoader::glLinkProgramPtr(ID);

        GLint success = 0;
        OpenGLLoader::glGetProgramivPtr(ID, GL_LINK_STATUS, &success);

        if (success != GL_TRUE)
        {
            GLint logLength = 0;
            OpenGLLoader::glGetProgramivPtr(ID, GL_INFO_LOG_LENGTH, &logLength);

            if (logLength > 0)
            {
                std::vector<GLchar> log(logLength);
                OpenGLLoader::glGetProgramInfoLogPtr(ID, logLength, nullptr, log.data());
                LOG_ERROR("Shader link failed:\n" << log.data());
            }

            string title = "Shader error detected!";
            string message = "Failed to link vertex shader '" + vertexPath + "' and fragment shader '" + fragmentPath + "' to program! Close program?";

            if (KalaWindow::CreatePopup(
                title,
                message,
                PopupAction::POPUP_ACTION_YES_NO,
                PopupType::POPUP_TYPE_ERROR)
                == PopupResult::POPUP_RESULT_YES)
            {
                KalaWindow::SetShouldCloseState(true);
            }

            LOG_ERROR("Shader program linking failed!");
            isValid = false;
            ID = 0;
            return;
        }

        //validate the shader program before using it
        OpenGLLoader::glValidateProgramPtr(ID);
        GLint validated = 0;
        OpenGLLoader::glGetProgramivPtr(ID, GL_VALIDATE_STATUS, &validated);
        if (validated != GL_TRUE)
        {
            GLint logLength = 0;
            OpenGLLoader::glGetProgramivPtr(ID, GL_INFO_LOG_LENGTH, &logLength);
            if (logLength > 0)
            {
                std::vector<GLchar> log(logLength);
                OpenGLLoader::glGetProgramInfoLogPtr(ID, logLength, nullptr, log.data());
                LOG_ERROR("Shader::Use() failed! Shader program validation failed:\n" << log.data());
                isValid = false;
                ID = 0;
                return;
            }

            LOG_ERROR("Shader::Use() failed! Shader program validation failed!");
            isValid = false;
            ID = 0;
            return;
        }

        GLint valid = OpenGLLoader::glIsProgramPtr(ID);
        bool isProgramValid = valid == GL_TRUE;
        if (!isProgramValid)
        {
            LOG_ERROR("Shader program ID " << ID << " is not valid!\n");

            isValid = false;
            ID = 0;
            return;
        }
        else
        {
            LOG_DEBUG("Shader program ID " << ID << " is valid!\n");
        }

        //
        // CLEANUP
        //

        OpenGLLoader::glDeleteShaderPtr(vertex);
        OpenGLLoader::glDeleteShaderPtr(fragment);
    }

    Shader::~Shader()
    {
        if (ID != 0) OpenGLLoader::glDeleteProgramPtr(ID);
    }

    void Shader::Use() const
    {
        if (ID == 0)
        {
            LOG_ERROR("Shader::Use() failed! ID is 0.");
            return;
        }

        if (!OpenGL::IsContextValid())
        {
            LOG_ERROR("Shader::Use() failed! OpenGL context is invalid.");
            return;
        }

        OpenGLLoader::glUseProgramPtr(ID);

        GLenum err = glGetError();
        if (err != GL_NO_ERROR)
        {
            LOG_ERROR("glUseProgramPtr error: " << OpenGL::GetGLErrorString(err));
        }

        GLint activeProgram = 0;
        OpenGLLoader::glGetIntegervPtr(GL_CURRENT_PROGRAM, &activeProgram);

        if (activeProgram != (GLint)ID)
        {
            LOG_ERROR("Shader::Use() failed! Program ID not bound after glUseProgram. Expected ID: '" << ID << "', but got: '" << activeProgram << "'.");
        }
    }

    void Shader::SetBool(const string& name, bool value) const 
    {
        OpenGLLoader::glUniform1iPtr(OpenGLLoader::glGetUniformLocationPtr(ID, name.c_str()), (int)value);
    }

    void Shader::SetInt(const string& name, int value) const 
    {
        OpenGLLoader::glUniform1iPtr(OpenGLLoader::glGetUniformLocationPtr(ID, name.c_str()), value);
    }

    void Shader::SetFloat(const string& name, float value) const 
    {
        OpenGLLoader::glUniform1fPtr(OpenGLLoader::glGetUniformLocationPtr(ID, name.c_str()), value);
    }

    void Shader::SetVec2(const string& name, const glm::vec2& value) const 
    {
        OpenGLLoader::glUniform2fvPtr(OpenGLLoader::glGetUniformLocationPtr(ID, name.c_str()), 1, glm::value_ptr(value));
    }

    void Shader::SetVec2(const string& name, float x, float y) const 
    {
        OpenGLLoader::glUniform2fPtr(OpenGLLoader::glGetUniformLocationPtr(ID, name.c_str()), x, y);
    }

    void Shader::SetVec3(const string& name, const glm::vec3& value) const 
    {
        OpenGLLoader::glUniform3fvPtr(OpenGLLoader::glGetUniformLocationPtr(ID, name.c_str()), 1, glm::value_ptr(value));
    }

    void Shader::SetVec3(const string& name, float x, float y, float z) const 
    {
        OpenGLLoader::glUniform3fPtr(OpenGLLoader::glGetUniformLocationPtr(ID, name.c_str()), x, y, z);
    }

    void Shader::SetVec4(const string& name, const glm::vec4& value) const 
    {
        OpenGLLoader::glUniform4fvPtr(OpenGLLoader::glGetUniformLocationPtr(ID, name.c_str()), 1, glm::value_ptr(value));
    }

    void Shader::SetVec4(const string& name, float x, float y, float z, float w) const 
    {
        OpenGLLoader::glUniform4fPtr(OpenGLLoader::glGetUniformLocationPtr(ID, name.c_str()), x, y, z, w);
    }

    void Shader::SetMat2(const string& name, const glm::mat2& mat) const 
    {
        OpenGLLoader::glUniformMatrix2fvPtr(OpenGLLoader::glGetUniformLocationPtr(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
    }

    void Shader::SetMat3(const string& name, const glm::mat3& mat) const 
    {
        OpenGLLoader::glUniformMatrix3fvPtr(OpenGLLoader::glGetUniformLocationPtr(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
    }

    void Shader::SetMat4(const string& name, const glm::mat4& mat) const 
    {
        OpenGLLoader::glUniformMatrix4fvPtr(OpenGLLoader::glGetUniformLocationPtr(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
    }

    bool Shader::CheckCompileErrors(GLuint shader, const string& type)
    {
        GLint success = 0;
        GLchar infoLog[1024];

        if (type != "PROGRAM")
        {
            OpenGLLoader::glGetShaderivPtr(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                OpenGLLoader::glGetShaderInfoLogPtr(shader, 1024, nullptr, infoLog);
                LOG_ERROR("Shader compilation failed (" << type << "):\n" << infoLog);
                return false;
            }
        }
        else
        {
            OpenGLLoader::glGetProgramivPtr(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                OpenGLLoader::glGetProgramInfoLogPtr(shader, 1024, nullptr, infoLog);
                LOG_ERROR("Program linking failed:\n" << infoLog);
                return false;
            }
        }

        return true;
    }
}