//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <array>

#include "KalaHeaders/core_utils.hpp"
#include "KalaHeaders/math_utils.hpp"

#include "core/registry.hpp"

namespace KalaWindow::Graphics::OpenGL
{
	using std::string;
	using std::array;

	using KalaHeaders::kvec2;
	using KalaHeaders::kvec3;
	using KalaHeaders::kvec4;
	using KalaHeaders::kmat2;
	using KalaHeaders::kmat3;
	using KalaHeaders::kmat4;

	using KalaWindow::Core::Registry;

	enum class ShaderType
	{
		SHADER_NONE,
		SHADER_VERTEX,
		SHADER_FRAGMENT,
		SHADER_GEOMETRY
	};

	struct ShaderData
	{
		//either fill shader data or shader path,
		//it picks data by default if both are filled
		string shaderPath{};

		//either fill shader data or shader path,
		//it picks data by default if both are filled
		string shaderData{};

		//assign to something else than the default 'SHADER_NONE'
		ShaderType type = ShaderType::SHADER_NONE;

		//ID of the shader, does not need to be filled by user
		u32 ID{};
	};

	class LIB_API OpenGL_Shader
	{
	public:
		static inline Registry<OpenGL_Shader> registry{};

		//Create a new shader with up to three types of shader files.
		//Geometry shaders are optional but vert and frag shader must always be filled
		static OpenGL_Shader* CreateShader(
			u32 windowID,
			const string& shaderName,
			const array<ShaderData, 3>& shaderData);

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

		inline u32 GetID() const { return ID; }
		inline u32 GetWindowID() const { return windowID; }

		inline u32 GetProgramID() const { return programID; }

		//Returns true if this shader is loaded
		inline bool IsShaderLoaded(ShaderType targetType) const
		{
			if (programID == 0) return false;

			switch (targetType)
			{
			case ShaderType::SHADER_VERTEX: return vertData.ID != 0;
			case ShaderType::SHADER_FRAGMENT: return fragData.ID != 0;
			case ShaderType::SHADER_GEOMETRY: return geomData.ID != 0;
			}

			return false;
		}

		//Returns shader data assigned to shader type
		inline const string& GetShaderData(ShaderType targetType) const
		{
			static const string empty{};

			if (programID == 0) return empty;

			switch (targetType)
			{
			case ShaderType::SHADER_VERTEX: return vertData.shaderData;
			case ShaderType::SHADER_FRAGMENT: return fragData.shaderData;
			case ShaderType::SHADER_GEOMETRY: return geomData.shaderData;
			}

			return empty;
		}
		//Returns shader path assigned to shader type
		inline const string& GetShaderPath(ShaderType targetType) const
		{
			static const string empty{};

			if (programID == 0) return empty;

			switch (targetType)
			{
			case ShaderType::SHADER_VERTEX: return vertData.shaderPath;
			case ShaderType::SHADER_FRAGMENT: return fragData.shaderPath;
			case ShaderType::SHADER_GEOMETRY: return geomData.shaderPath;
			}

			return empty;
		}
		inline u32 GetShaderID(ShaderType targetType) const
		{
			if (programID == 0) return 0;

			switch (targetType)
			{
			case ShaderType::SHADER_VERTEX: return vertData.ID;
			case ShaderType::SHADER_FRAGMENT: return fragData.ID;
			case ShaderType::SHADER_GEOMETRY: return geomData.ID;
			}

			return 0;
		}

		inline const array<ShaderData, 3>& GetAllShaders() const
		{
			static array<ShaderData, 3> dataOut{};

			dataOut[0] = vertData;
			dataOut[1] = fragData;
			dataOut[2] = geomData;

			return dataOut;
		}

		bool Bind() const;

		bool HotReload();

		void SetBool(u32 programID, const string& name, bool value) const;
		void SetInt(u32 programID, const string& name, i32 value) const;
		void SetFloat(u32 programID, const string& name, f32 value) const;

		void SetVec2(u32 programID, const string& name, const kvec2& value) const;
		void SetVec3(u32 programID, const string& name, const kvec3& value) const;
		void SetVec4(u32 programID, const string& name, const kvec4& value) const;

		void SetMat2(u32 programID, const string& name, const kmat2& mat) const;
		void SetMat3(u32 programID, const string& name, const kmat3& mat) const;
		void SetMat4(u32 programID, const string& name, const kmat4& mat) const;

		//Do not destroy manually, erase from registry instead
		~OpenGL_Shader();
	private:
		static inline bool isVerboseLoggingEnabled{};

		bool isInitialized{};

		string name{};

		u32 ID{};
		u32 windowID{};

		u32 programID{};

		ShaderData vertData{};
		ShaderData fragData{};
		ShaderData geomData{};
	};
}