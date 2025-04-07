//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <unordered_map>

//external
#include "glm/glm.hpp"
#include "glm/ext/vector_float2.hpp"

//kalawindow
#include "preprocessors.hpp"
#include "opengl_typedefs.hpp"

namespace KalaKit
{
	using std::string;
	using std::unordered_map;

	using glm::vec2;
	using glm::vec3;
	using glm::vec4;
	using glm::mat2;
	using glm::mat3;
	using glm::mat4;

	class KALAWINDOW_API Shader
	{
	public:
		unsigned int ID{};

		Shader() = default;

		Shader(
			const string& vertexPath,
			const string& fragmentPath);

		~Shader();

		bool IsValid() const { return isValid; }

		void Use() const;

		void SetBool(const string& name, bool value) const;
		void SetInt(const string& name, int value) const;
		void SetFloat(const string& name, float value) const;

		void SetVec2(const string& name, const vec2& value) const;
		void SetVec2(const string& name, float x, float y) const;

		void SetVec3(const string& name, const vec3& value) const;
		void SetVec3(const string& name, float x, float y, float z) const;

		void SetVec4(const string& name, const vec4& value) const;
		void SetVec4(const string& name, float x, float y, float z, float w) const;

		void SetMat2(const string& name, const mat2& mat) const;
		void SetMat3(const string& name, const mat3& mat) const;
		void SetMat4(const string& name, const mat4& mat) const;
	private:
		bool isValid = true;

		bool CheckCompileErrors(GLuint shader, const string& type);
	};
}