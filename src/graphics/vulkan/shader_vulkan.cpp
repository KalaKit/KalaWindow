//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef KALAWINDOW_SUPPORT_VULKAN

#define KALAKIT_MODULE = "VULKAN_SHADER"

#include <vector>
#include <string>
#include <fstream>

#include <vulkan/vulkan.h>

#include "graphics/vulkan/shader_vulkan.hpp"

using std::vector;
using std::string;
using std::ifstream;
using std::ios;

static vector<char> LoadBinaryFile(const string& path);
static VkShaderModule CreateShaderModule(VkDevice device, const vector<char>& code);

namespace KalaWindow::Graphics
{
	unique_ptr<Shader_Vulkan> Shader_Vulkan::CreateFromFiles(
		const string& vertexPath,
		const string& fragmentPath)
	{
		return nullptr;
	}

	bool Shader_Vulkan::Bind(void* commandBuffer)
	{
		return true;
	}

	void Shader_Vulkan::CleanResources()
	{

	}
}

vector<char> LoadBinaryFile(const string& path)
{
	return {};
}

VkShaderModule CreateShaderModule(VkDevice device, const vector<char>& code)
{
	return VK_NULL_HANDLE;
}

#endif //KALAWINDOW_SUPPORT_VULKAN