//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <unordered_map>

//kalawindow
#include "platform.hpp"
#include "opengl_typedefs.hpp"

namespace KalaKit
{
	using std::string;
	using std::unordered_map;

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

		void SetVec2(const string& name, const kvec2& value) const;
		void SetVec3(const string& name, const kvec3& value) const;
		void SetVec4(const string& name, const kvec4& value) const;

		void SetMat2(const string& name, const kmat2& mat) const;
		void SetMat3(const string& name, const kmat3& mat) const;
		void SetMat4(const string& name, const kmat4& mat) const;
	private:
		bool isValid = true;

		bool CheckCompileErrors(GLuint shader, const string& type);
	};
}