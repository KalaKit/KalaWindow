//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef KALAWINDOW_SUPPORT_OPENGL

#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>

#include "core/platform.hpp"
#include "graphics/window.hpp"

namespace KalaWindow::Graphics
{
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
		static inline unordered_map<string, Shader_OpenGL*> createdShaders{};

		static unique_ptr<Shader_OpenGL> CreateShader(
			const string& shaderName,
			const vector<ShaderStage>& shaderStages);

		static Shader_OpenGL* GetShaderByName(const string& name)
		{
			auto it = createdShaders.find(name);
			return it != createdShaders.end() ? it->second : nullptr;
		}

		vector<ShaderStage> GetAllShaders() { return shaders; }

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

		bool Bind(Window* window) const;

		void HotReload(Shader_OpenGL* shader);

		void SetBool(unsigned int programID, const string& name, bool value) const;
		void SetInt(unsigned int programID, const string& name, int value) const;
		void SetFloat(unsigned int programID, const string& name, float value) const;

		void SetVec2(unsigned int programID, const string& name, const kvec2& value) const;
		void SetVec3(unsigned int programID, const string& name, const kvec3& value) const;
		void SetVec4(unsigned int programID, const string& name, const kvec4& value) const;

		void SetMat2(unsigned int programID, const string& name, const kmat2& mat) const;
		void SetMat3(unsigned int programID, const string& name, const kmat3& mat) const;
		void SetMat4(unsigned int programID, const string& name, const kmat4& mat) const;

		//Destroys this created shader and its data
		void DestroyShader();
	private:
		string name;

		unsigned int programID;

		vector<ShaderStage> shaders;
	};
}

#endif //KALAWINDOW_SUPPORT_OPENGL