//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>

#include "core/platform.hpp"
#include "core/log.hpp"
#include "graphics/window.hpp"

namespace KalaWindow::Graphics::OpenGL
{
	using KalaWindow::Core::Logger;
	using KalaWindow::Core::LogType;

	using std::string;

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
		u32 shaderID;
	};

	class KALAWINDOW_API Shader_OpenGL
	{
	public:
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
		void SetName(const string& newName);

		u32 GetID() const { return ID; }
		void SetID(u32 newID) { ID = newID; }

		Window* GetTargetWindow() const { return targetWindow; }

		u32 GetProgramID() const { return programID; }

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

		u32 GetShaderID(ShaderType type) const
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

		void SetBool(u32 programID, const string& name, bool value) const;
		void SetInt(u32 programID, const string& name, i32 value) const;
		void SetFloat(u32 programID, const string& name, f32 value) const;

		void SetVec2(u32 programID, const string& name, const vec2& value) const;
		void SetVec3(u32 programID, const string& name, const vec3& value) const;
		void SetVec4(u32 programID, const string& name, const vec4& value) const;

		void SetMat2(u32 programID, const string& name, const mat2& mat) const;
		void SetMat3(u32 programID, const string& name, const mat3& mat) const;
		void SetMat4(u32 programID, const string& name, const mat4& mat) const;

		//Destroys this created shader and its data
		~Shader_OpenGL();
	private:
		string name{};
		u32 ID{};

		Window* targetWindow{};

		u32 programID{};

		vector<ShaderStage> shaders{};
	};
}