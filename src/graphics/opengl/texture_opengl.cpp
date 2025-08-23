//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <string>
#include <unordered_map>
#include <memory>
#include <filesystem>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"

#include "graphics/texture.hpp"
#include "graphics/opengl/texture_opengl.hpp"
#include "graphics/opengl/opengl_functions_core.hpp"
#include "core/core.hpp"
#include "core/containers.hpp"

using KalaHeaders::Log;
using KalaHeaders::LogType;

using KalaWindow::Graphics::Texture;
using namespace KalaWindow::Graphics::OpenGLFunctions;
using KalaWindow::Core::KalaWindowCore;
using KalaWindow::Core::globalID;
using KalaWindow::Core::createdOpenGLTextures;
using KalaWindow::Core::runtimeOpenGLTextures;

using std::string;
using std::to_string;
using std::unordered_map;
using std::unique_ptr;
using std::make_unique;
using std::filesystem::path;
using std::filesystem::exists;
using std::vector;

enum class TextureCheckResult
{
	RESULT_OK,
	RESULT_INVALID,
	RESULT_ALREADY_EXISTS
};

static vector<string> validExtensions =
{
	".png",
	".jpg",
	".jpeg"
};

static TextureCheckResult IsValidTexture(
	const string& textureName,
	const string& texturePath);

namespace KalaWindow::Graphics::OpenGL
{
	//TODO: NEEDS INTERNAL TOGLFORMAT

	Texture_OpenGL* Texture_OpenGL::LoadTexture(
		const string& name,
		const string& path,
		TextureType type,
		TextureFormat format,
		TextureUsage usage,
		vec2 size,
		u16 depth,
		u8 mipMapLevels)
	{
		//TODO: NEEDS TO UTILIZE ALL REMAINING PARAMETERS

		TextureCheckResult result = (IsValidTexture(name, path));

		if (result == TextureCheckResult::RESULT_INVALID) return nullptr;
		else if (result == TextureCheckResult::RESULT_ALREADY_EXISTS)
		{
			for (const auto& [key, value] : createdOpenGLTextures)
			{
				if (value->GetName() == name) return value.get();
			}
		}

		Log::Print(
			"Loading texture '" + name + "'.",
			"TEXTURE_OPENGL",
			LogType::LOG_DEBUG);

		unsigned int newTextureID{};
		glGenTextures(1, &newTextureID);
		glBindTexture(GL_TEXTURE_2D, newTextureID);

		glTexParameteri(
			GL_TEXTURE_2D,
			GL_TEXTURE_WRAP_S,
			GL_REPEAT);
		glTexParameteri(
			GL_TEXTURE_2D,
			GL_TEXTURE_WRAP_T,
			GL_REPEAT);

		glTexParameteri(
			GL_TEXTURE_2D,
			GL_TEXTURE_MIN_FILTER,
			GL_LINEAR);
		glTexParameteri(
			GL_TEXTURE_2D,
			GL_TEXTURE_MAG_FILTER,
			GL_LINEAR);

		int width{};
		int height{};
		int nrChannels{};
		stbi_set_flip_vertically_on_load(true);

		unsigned char* data = stbi_load(
			(path).c_str(),
			&width,
			&height,
			&nrChannels,
			0);

		GLenum formatGL{};
		if (nrChannels == 1) formatGL = GL_RED;
		else if (nrChannels == 3) formatGL = GL_RGB;
		else if (nrChannels == 4) formatGL = GL_RGBA;
		else
		{
			KalaWindowCore::ForceClose(
				"Texture error",
				"Unsupported channel count '" + to_string(nrChannels) + "'!");

			return nullptr;
		}

		if (width <= 0
			|| height <= 0)
		{
			KalaWindowCore::ForceClose(
				"Texture error",
				"Failed to load texture '" + path + "' because width or height is not above 0!");

			return nullptr;
		}

		if (data == nullptr)
		{
			KalaWindowCore::ForceClose(
				"Texture error",
				"Failed to load texture '" + path + "' because stbi data is nullptr!");

			return nullptr;
		}

		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			formatGL,
			width,
			height,
			0,
			formatGL,
			GL_UNSIGNED_BYTE,
			data);

		u32 newID = ++globalID;
		unique_ptr<Texture_OpenGL> newTexture = make_unique<Texture_OpenGL>();
		Texture_OpenGL* texturePtr = newTexture.get();

		newTexture->openGLID = newTextureID;
		newTexture->name = name;
		newTexture->path = path;
		newTexture->ID = newID;

		size_t dataSize = static_cast<size_t>(width) * height * nrChannels;
		vector<u8> pixels(data, data + dataSize);
		newTexture->pixels = pixels;

		stbi_image_free(data);

		createdOpenGLTextures[newID] = move(newTexture);
		runtimeOpenGLTextures.push_back(texturePtr);

		Log::Print(
			"Loaded OpenGL texture '" + name + "' with ID '" + to_string(newID) + "'!",
			"TEXTURE",
			LogType::LOG_SUCCESS);

		return texturePtr;
	}

	void Texture_OpenGL::HotReload()
	{
		//TODO: DEFINE
	}

	Texture_OpenGL::~Texture_OpenGL()
	{
		//TODO: DEFINE

		Log::Print(
			"Destroyed texture '" + GetName() + "'!",
			"TEXTURE_OPENGL",
			LogType::LOG_SUCCESS);
	}
}

TextureCheckResult IsValidTexture(
	const string& textureName,
	const string& texturePath)
{
	//texture name must not be empty

	if (textureName.empty())
	{
		string title = "OpenGL Texture Error";
		string reason = "Cannot load a texture with no name!";

		KalaWindowCore::ForceClose(title, reason);

		return TextureCheckResult::RESULT_INVALID;
	}

	//texture path must not be empty

	if (texturePath.empty())
	{
		string title = "OpenGL Texture Error";
		string reason = "Cannot load a texture with no path!";

		KalaWindowCore::ForceClose(title, reason);

		return TextureCheckResult::RESULT_INVALID;
	}

	string texturePathName = path(texturePath).filename().string();

	//texture file must exist

	if (!exists(texturePath))
	{
		string title = "OpenGL Texture Error";
		string reason = "Texture '" + textureName + "' path '" + texturePathName + "' does not exist!";

		KalaWindowCore::ForceClose(title, reason);

		return TextureCheckResult::RESULT_INVALID;
	}

	//texture file must have an extension

	if (!path(texturePath).has_extension())
	{
		string title = "OpenGL Texture Error";
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
		string title = "OpenGL Texture Error";
		string reason = "Texture '" + textureName + "' has an invalid extension '" + thisExtension + "'. Only png, jpg and jpeg are allowed!";

		KalaWindowCore::ForceClose(title, reason);

		return TextureCheckResult::RESULT_INVALID;
	}

	//pass existing texture if one with the same name or path already exists

	for (const auto& [_, value] : createdOpenGLTextures)
	{
		if (value->GetName() == textureName)
		{
			Log::Print(
				"Texture '" + textureName + "' already exists!",
				"TEXTURE_OPENGL",
				LogType::LOG_ERROR,
				2);

			return TextureCheckResult::RESULT_ALREADY_EXISTS;
		}

		if (value->GetPath() == texturePath)
		{
			Log::Print(
				"Texture '" + textureName + "' with path '" + texturePathName + "' has already been loaded!",
				"TEXTURE_OPENGL",
				LogType::LOG_ERROR,
				2);

			return TextureCheckResult::RESULT_ALREADY_EXISTS;
		}
	}

	return TextureCheckResult::RESULT_OK;
}