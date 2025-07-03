//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef KALAWINDOW_SUPPORT_VULKAN

#pragma once

#include <string>
#include <memory>

#include "core/platform.hpp"

namespace KalaWindow::Graphics
{
	using std::string;
	using std::unique_ptr;

	struct ShaderData
	{
		void* vert = nullptr;
		void* frag = nullptr;
		void* pipeline = nullptr;
		void* layout = nullptr;
	};

	class KALAWINDOW_API Shader_Vulkan
	{
	public:
		//Creates a graphics pipeline from SPIR-V files
		static unique_ptr<Shader_Vulkan> CreateFromFiles(
			const string& vertexPath,
			const string& fragmentPath);

		//Binds the shader pipeline for use in the command buffer
		bool Bind(void* commandBuffer);

		//Destoys all associated resources
		void CleanResources();
	private:
		unique_ptr<ShaderData> shaderData;
	};
}

#endif //KALAWINDOW_SUPPORT_VULKAN