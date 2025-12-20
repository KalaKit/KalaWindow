//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <array>

#include "KalaHeaders/core_utils.hpp"
#include "KalaHeaders/math_utils.hpp"

#include "opengl/kw_opengl.hpp"
#include "core/kw_registry.hpp"

namespace KalaWindow::OpenGL
{
	using std::string;
	using std::array;

	using KalaHeaders::KalaMath::vec2;
	using KalaHeaders::KalaMath::vec3;
	using KalaHeaders::KalaMath::vec4;
	using KalaHeaders::KalaMath::mat2;
	using KalaHeaders::KalaMath::mat3;
	using KalaHeaders::KalaMath::mat4;
	
	using KalaWindow::OpenGL::OpenGL_Context;
	using KalaWindow::Core::KalaWindowRegistry;

	enum class OpenGL_ShaderType
	{
		SHADER_NONE,
		SHADER_VERTEX,
		SHADER_FRAGMENT,
		SHADER_GEOMETRY
	};

	struct LIB_API OpenGL_ShaderData
	{
		//either fill shader data or shader path,
		//it picks data by default if both are filled
		string shaderPath{};

		//either fill shader data or shader path,
		//it picks data by default if both are filled
		string shaderData{};

		//assign to something else than the default 'SHADER_NONE'
		OpenGL_ShaderType type = OpenGL_ShaderType::SHADER_NONE;

		//ID of the shader, does not need to be filled by user
		u32 ID{};
	};

	class LIB_API OpenGL_Shader
	{
	public:
		static inline KalaWindowRegistry<OpenGL_Shader> registry{};

		//Create a new shader with up to three types of shader files.
		//Geometry shaders are optional but vert and frag shader must always be filled
		static OpenGL_Shader* Initialize(
			OpenGL_Context* glContext,
			const string& shaderName,
			const array<OpenGL_ShaderData, 3>& shaderData);

		inline bool IsInitialized() const { return isInitialized; }

		//Toggle verbose logging. If true, then usually frequently updated runtime values like
		//vertex, fragment and geometry shader compilation messages will dump their logs into the console.
		static inline void SetVerboseLoggingState(bool newState) { isVerboseLoggingEnabled = newState; }
		static inline bool IsVerboseLoggingEnabled() { return isVerboseLoggingEnabled; }

		inline const string& GetName() const { return name; }
		inline bool SetName(const string& newName)
		{
			if (newName.empty()
				|| newName.size() > 50)
			{
				return false;
			}

			name = newName;

			return true;
		}

		//Returns the global ID of this shader
		inline u32 GetID() const { return ID; }
		//Returns the OpenGL program ID of this shader
		inline u32 GetProgramID() const { return programID; }
		
		//Returns the OpenGL context of this shader
		inline OpenGL_Context* GetGLContext() const { return glContext; }

		//Returns shader data assigned to shader type
		inline const string& GetShaderData(OpenGL_ShaderType targetType) const
		{
			static const string empty{};

			if (programID == 0) return empty;

			switch (targetType)
			{
			case OpenGL_ShaderType::SHADER_VERTEX: return vertData.shaderData;
			case OpenGL_ShaderType::SHADER_FRAGMENT: return fragData.shaderData;
			case OpenGL_ShaderType::SHADER_GEOMETRY: return geomData.shaderData;
			}

			return empty;
		}
		//Returns shader path assigned to shader type
		inline const string& GetShaderPath(OpenGL_ShaderType targetType) const
		{
			static const string empty{};

			if (programID == 0) return empty;

			switch (targetType)
			{
			case OpenGL_ShaderType::SHADER_VERTEX: return vertData.shaderPath;
			case OpenGL_ShaderType::SHADER_FRAGMENT: return fragData.shaderPath;
			case OpenGL_ShaderType::SHADER_GEOMETRY: return geomData.shaderPath;
			}

			return empty;
		}
		inline u32 GetShaderID(OpenGL_ShaderType targetType) const
		{
			if (programID == 0) return 0;

			switch (targetType)
			{
			case OpenGL_ShaderType::SHADER_VERTEX: return vertData.ID;
			case OpenGL_ShaderType::SHADER_FRAGMENT: return fragData.ID;
			case OpenGL_ShaderType::SHADER_GEOMETRY: return geomData.ID;
			}

			return 0;
		}

		//Bind current shader
		bool Bind();

		bool HotReload();

		void SetBool(const string& name, bool value) const;
		void SetInt(const string& name, i32 value) const;
		void SetFloat(const string& name, f32 value) const;

		void SetVec2(const string& name, const vec2& value) const;
		void SetVec3(const string& name, const vec3& value) const;
		void SetVec4(const string& name, const vec4& value) const;

		void SetMat2(const string& name, const mat2& mat) const;
		void SetMat3(const string& name, const mat3& mat) const;
		void SetMat4(const string& name, const mat4& mat) const;

		//Do not destroy manually, erase from registry instead
		~OpenGL_Shader();
	private:
		static inline bool isVerboseLoggingEnabled{};

		bool isInitialized{};

		string name{};

		u32 ID{};
		u32 programID{};
		
		OpenGL_Context* glContext{};

		OpenGL_ShaderData vertData{};
		OpenGL_ShaderData fragData{};
		OpenGL_ShaderData geomData{};
	};
}