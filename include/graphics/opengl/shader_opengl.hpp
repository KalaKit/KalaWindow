//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>

#include "core/platform.hpp"
#include "core/log.hpp"
#include "graphics/window.hpp"

namespace KalaWindow::Graphics::OpenGL
{
	using KalaWindow::Core::Logger;
	using KalaWindow::Core::LogType;

	using std::string;
	using std::unordered_map;
	using std::vector;

	enum class ShaderType
	{
		Shader_Vertex,
		Shader_Fragment,
		Shader_Geometry,
	};

	struct ShaderStage
	{
		ShaderType shaderType;
		string shaderPath;
		unsigned int shaderID;
	};

	class KALAWINDOW_API Shader_OpenGL
	{
	public:
		static inline unordered_map<string, unique_ptr<Shader_OpenGL>> createdShaders{};

		static Shader_OpenGL* CreateShader(
			const string& shaderName,
			const vector<ShaderStage>& shaderStages,
			Window* targetWindow);

		static string GetShaderTypeName(ShaderType type)
		{
			switch (type)
			{
			case ShaderType::Shader_Vertex:
				return "vertex";
			case ShaderType::Shader_Fragment:
				return "fragment";
			case ShaderType::Shader_Geometry:
				return "geometry";
			}

			return "";
		}

		const string& GetName() const { return name; }
		void SetName(const string& newName)
		{
			if (newName.empty())
			{
				Logger::Print(
					"Cannot set shader name to empty name!",
					"SHADER_OPENGL",
					LogType::LOG_ERROR,
					2);
				return;
			}
			for (const auto& createdShader : createdShaders)
			{
				string thisName = createdShader.first.c_str();
				if (newName == thisName)
				{
					Logger::Print(
						"Cannot set shader name to already existing shader name '" + thisName + "'!",
						"SHADER_OPENGL",
						LogType::LOG_ERROR,
						2);
					return;
				}
			}
			name = newName;
		}

		Window* GetTargetWindow() const { return targetWindow; }

		unsigned int GetProgramID() const { return programID; }

		const vector<ShaderStage>& GetAllShaders() const { return shaders; }

		void SetShaderPath(
			const string& path,
			ShaderType type)
		{
			if (path.empty())
			{
				Logger::Print(
					"Cannot set shader path to empty path!",
					"SHADER_OPENGL",
					LogType::LOG_ERROR,
					2);
				return;
			}

			for (auto& stage : shaders)
			{
				if (stage.shaderType == type)
				{
					stage.shaderPath = path;
					break;
				}
			}
		}

		unsigned int GetShaderID(ShaderType type) const
		{
			for (const auto& stage : shaders)
			{
				if (stage.shaderType == type)
				{
					return stage.shaderID;
				}
			}

			string typeStr = GetShaderTypeName(type);

			Logger::Print(
				"Shader with type '" + typeStr + "' was not assigned! Returning ID 0.",
				"SHADER_OPENGL",
				LogType::LOG_ERROR,
				2);
			return 0;
		}
		const string& GetShaderPath(ShaderType type) const
		{
			static string shaderPath{};

			for (const auto& stage : shaders)
			{
				if (stage.shaderType == type)
				{
					return stage.shaderPath;
					break;
				}
			}

			string typeStr = GetShaderTypeName(type);

			Logger::Print(
				"Shader with type '" + typeStr + "' was not assigned! Returning empty path.",
				"SHADER_OPENGL",
				LogType::LOG_ERROR,
				2);
			return shaderPath;
		}

		//Returns true if this shader is loaded
		bool IsShaderLoaded(ShaderType targetType)
		{
			if (shaders.empty()
				|| programID == 0)
			{
				return false;
			}

			for (const auto& stage : shaders)
			{
				if (stage.shaderType == targetType
					&& !stage.shaderPath.empty()
					&& stage.shaderID != 0)
				{
					return true;
				}
			}

			return false;
		}
		//Returns true if the shader path of this shader type exists
		bool ShaderExists(ShaderType targetType)
		{
			if (shaders.empty()
				|| programID == 0)
			{
				return false;
			}

			for (const auto& stage : shaders)
			{
				if (stage.shaderType == targetType
					&& !stage.shaderPath.empty())
				{
					return true;
				}
			}

			return false;
		}

		bool Bind() const;

		void HotReload();

		void SetBool(unsigned int programID, const string& name, bool value) const;
		void SetInt(unsigned int programID, const string& name, int value) const;
		void SetFloat(unsigned int programID, const string& name, float value) const;

		void SetVec2(unsigned int programID, const string& name, const vec2& value) const;
		void SetVec3(unsigned int programID, const string& name, const vec3& value) const;
		void SetVec4(unsigned int programID, const string& name, const vec4& value) const;

		void SetMat2(unsigned int programID, const string& name, const mat2& mat) const;
		void SetMat3(unsigned int programID, const string& name, const mat3& mat) const;
		void SetMat4(unsigned int programID, const string& name, const mat4& mat) const;

		//Destroys this created shader and its data
		~Shader_OpenGL();
	private:
		string name{};

		Window* targetWindow{};

		unsigned int programID{};

		vector<ShaderStage> shaders{};
	};
}