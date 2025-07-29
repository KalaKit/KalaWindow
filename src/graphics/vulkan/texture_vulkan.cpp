//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#define VK_NO_PROTOTYPES
#include <Volk/volk.h>

#include <string>
#include <unordered_map>
#include <memory>
#include <filesystem>
#include <vector>

#include "graphics/texture.hpp"
#include "graphics/vulkan/texture_vulkan.hpp"
#include "core/core.hpp"
#include "core/log.hpp"
#include "core/containers.hpp"

using KalaWindow::Graphics::Texture;
using KalaWindow::Core::KalaWindowCore;
using KalaWindow::Core::Logger;
using KalaWindow::Core::LogType;
using KalaWindow::Core::createdVulkanTextures;

using std::string;
using std::unordered_map;
using std::unique_ptr;
using std::filesystem::path;
using std::filesystem::exists;
using std::vector;

enum class TextureCheckResult
{
	RESULT_OK,
	RESULT_INVALID,
	RESULT_ALREADY_EXISTS
};

vector<string> validExtensions =
{
	".png",
	".jpg",
	".jpeg"
};

static TextureCheckResult IsValidTexture(
	const string& textureName,
	const string& texturePath);

namespace KalaWindow::Graphics::Vulkan
{
	//TODO: NEEDS INTERNAL TOVKFORMAT AND TOVKUSAGE

	Texture_Vulkan* Texture_Vulkan::LoadTexture(
		const string& name,
		const string& path,
		TextureType type,
		TextureFormat format,
		TextureUsage usage,
		vec2 size,
		u16 depth,
		u8 mipMapLevels)
	{
		//TODO: DEFINE
		return nullptr;
	}

	void Texture_Vulkan::HotReload()
	{
		//TODO: DEFINE
	}

	Texture_Vulkan::~Texture_Vulkan()
	{
		//TODO: DEFINE
	}
}

TextureCheckResult IsValidTexture(
	const string& textureName,
	const string& texturePath)
{
	//texture name must not be empty

	if (textureName.empty())
	{
		string title = "Vulkan Texture Error";
		string reason = "Cannot load a texture with no name!";

		KalaWindowCore::ForceClose(title, reason);

		return TextureCheckResult::RESULT_INVALID;
	}

	//texture path must not be empty

	if (texturePath.empty())
	{
		string title = "Vulkan Texture Error";
		string reason = "Cannot load a texture with no path!";

		KalaWindowCore::ForceClose(title, reason);

		return TextureCheckResult::RESULT_INVALID;
	}

	string texturePathName = path(texturePath).filename().string();

	//texture file must exist

	if (!exists(texturePath))
	{
		string title = "Vulkan Texture Error";
		string reason = "Texture '" + textureName + "' path '" + texturePathName + "' does not exist!";

		KalaWindowCore::ForceClose(title, reason);

		return TextureCheckResult::RESULT_INVALID;
	}

	//texture file must have an extension

	if (!path(texturePath).has_extension())
	{
		string title = "Vulkan Texture Error";
		string reason = "Texture '" + textureName + "' has no extension. You must use png, jpg or jpeg!";

		KalaWindowCore::ForceClose(title, reason);

		return TextureCheckResult::RESULT_INVALID;
	}

	string thisExtension = path(texturePath).extension().string();
	bool isExtensionValid =
		find(validExtensions.begin(),
			validExtensions.end(),
			thisExtension)
		!= validExtensions.end();

	//texture file must have a valid extension

	if (!isExtensionValid)
	{
		string title = "Vulkan Texture Error";
		string reason = "Texture '" + textureName + "' has an invalid extension '" + thisExtension + "'. Only png, jpg and jpeg are allowed!";

		KalaWindowCore::ForceClose(title, reason);

		return TextureCheckResult::RESULT_INVALID;
	}

	//pass existing texture if one with the same name or path already exists

	for (const auto& [_, value] : createdVulkanTextures)
	{
		if (value->GetName() == textureName)
		{
			Logger::Print(
				"Texture '" + textureName + "' already exists!",
				"TEXTURE_OPENGL",
				LogType::LOG_ERROR,
				2);

			return TextureCheckResult::RESULT_ALREADY_EXISTS;
		}

		if (value->GetPath() == texturePath)
		{
			Logger::Print(
				"Texture '" + textureName + "' with path '" + texturePathName + "' has already been loaded!",
				"TEXTURE_OPENGL",
				LogType::LOG_ERROR,
				2);

			return TextureCheckResult::RESULT_ALREADY_EXISTS;
		}
	}

	return TextureCheckResult::RESULT_OK;
}