//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <vector>

#include "KalaHeaders/core_utils.hpp"

#include "core/glm_global.hpp"

namespace KalaWindow::Graphics::OpenGL
{
	using std::string;
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

		//path to external shader file,
		//use this if you have a shader file you want to read
		string shaderPath{};

		//raw string data to shader code,
		//use this if you want to skip loading external shader files
		//and you have a string ready for reading
		string shaderText{};

		u32 shaderID;
	};

	class LIB_API OpenGL_Shader
	{
	public:
		static OpenGL_Shader* CreateShader(
			u32 windowID,
			const string& shaderName,
			const vector<ShaderStage>& shaderStages);

		inline bool IsInitialized() const { return isInitialized; }

		//Toggle verbose logging. If true, then usually frequently updated runtime values like
		//vertex, fragment and geometry shader compilation messages will dump their logs into the console.
		static inline void SetVerboseLoggingState(bool newState) { isVerboseLoggingEnabled = newState; }
		static inline bool IsVerboseLoggingEnabled() { return isVerboseLoggingEnabled; }

		static inline string GetShaderTypeName(ShaderType type)
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

		inline const string& GetName() const { return name; }
		inline void SetName(const string& newName);

		inline u32 GetID() const { return ID; }

		inline u32 GetProgramID() const { return programID; }

		inline const vector<ShaderStage>& GetAllShaders() const { return shaders; }

		inline u32 GetShaderID(ShaderType type) const
		{
			for (const auto& stage : shaders)
			{
				if (stage.shaderType == type)
				{
					return stage.shaderID;
				}
			}

			string typeStr = GetShaderTypeName(type);

			return 0;
		}
		inline const string& GetShaderPath(ShaderType type) const
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

			return shaderPath;
		}

		//Returns true if this shader is loaded
		inline bool IsShaderLoaded(ShaderType targetType)
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
		inline bool ShaderExists(ShaderType targetType)
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

		//Do not destroy manually, erase from containers.hpp instead
		~OpenGL_Shader();
	private:
		static inline bool isVerboseLoggingEnabled{};

		bool isInitialized{};

		string name{};
		u32 ID{};

		u32 windowID{};

		u32 programID{};

		vector<ShaderStage> shaders{};
	};
}