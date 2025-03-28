//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>

namespace KalaKit
{
	using std::string;

	class Shader
	{
	public:
		unsigned int ID{};

		Shader(
			const string& vertexPath,
			const string& fragmentPath);
	};
}