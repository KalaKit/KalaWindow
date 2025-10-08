//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <string>
#include <unordered_map>
#include <memory>
#include <filesystem>
#include <vector>
#include <sstream>
#include <algorithm>
#include <array>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb/stb_image_resize2.h"

#include "KalaHeaders/log_utils.hpp"

#include "graphics/texture.hpp"
#include "graphics/opengl/opengl.hpp"
#include "graphics/opengl/opengl_texture.hpp"
#include "graphics/opengl/opengl_functions_core.hpp"
#include "core/core.hpp"
#include "core/containers.hpp"

using KalaHeaders::Log;
using KalaHeaders::LogType;

using KalaWindow::Graphics::OpenGL::OpenGL_Global;
using KalaWindow::Graphics::OpenGL::OpenGL_Texture;
using KalaWindow::Graphics::Texture;
using KalaWindow::Graphics::TextureType;
using KalaWindow::Graphics::TextureFormat;
using namespace KalaWindow::Graphics::OpenGLFunctions;
using namespace KalaWindow::Core;

using std::string;
using std::string_view;
using std::to_string;
using std::unordered_map;
using std::unique_ptr;
using std::make_unique;
using std::filesystem::path;
using std::filesystem::exists;
using std::vector;
using std::ostringstream;
using std::clamp;
using std::max;
using std::floor;
using std::log2;
using std::array;
using std::transform;
using std::tolower;

enum class TextureCheckResult
{
	RESULT_OK,
	RESULT_INVALID,
	RESULT_ALREADY_EXISTS
};

constexpr array<string_view, 4> validExtensions =
{
	".png",
	".jpg",
	".jpeg",
	".ktx"
};

static u32 TEXTURE_MAX_SIZE{};

static OpenGL_Texture* fallbackTexture{};

constexpr string_view fallbackTextureName = "FallbackTexture";

//The actual fallback texture raw pixel data
static array<u8, 32 * 32 * 4> fallbackPixels{};

//Helper that builds the full checkerboard
static const array<u8, 32 * 32 * 4>& GetFallbackPixels()
{
	if (fallbackPixels[0] != 0) return fallbackPixels;

	constexpr int texSize = 32;
	constexpr int blockSize = 8;

	for (int y = 0; y < texSize; ++y)
	{
		for (int x = 0; x < texSize; ++x)
		{
			//determine block color - alternate every 8 pixels
			bool isMagenta = (((x / blockSize) + (y / blockSize)) % 2 == 0);

			int idx = (y * texSize + x) * 4;
			using index_t = array<u8, 32 * 32 * 4>::size_type;

			auto Push = [idx](u8 adder, u8 result)
				{
					fallbackPixels[static_cast<index_t>(idx) + adder] = result;
				};

			Push(0, isMagenta ? 255 : 0); //R
			Push(1, 0);                   //G
			Push(2, isMagenta ? 255 : 0); //B
			Push(3, 255);                 //A
		}
	}

	return fallbackPixels;
}

static OpenGL_Texture* InitTexture(
	const string& name,
	const string& filePath,
	TextureType type,
	TextureFormat formatIn,
	TextureFormat& formatOut,
	bool flipVertically,
	vec2& size,
	vector<u8>& data,
	int& nrChannels);

static TextureCheckResult IsValidTexture(
	const string& textureName,
	const string& texturePath);

static bool IsCorrectFormat(
	TextureFormat format,
	int nrChannels);

static bool IsCompressed(TextureFormat f)
{
	return f == TextureFormat::Format_BC1 
		|| f == TextureFormat::Format_BC3 
		|| f == TextureFormat::Format_BC4 
		|| f == TextureFormat::Format_BC5 
		|| f == TextureFormat::Format_BC7;
}
static bool IsUncompressed(TextureFormat f)
{
	return f == TextureFormat::Format_R8
		|| f == TextureFormat::Format_RG8
		|| f == TextureFormat::Format_RGB8
		|| f == TextureFormat::Format_RGBA8
		|| f == TextureFormat::Format_SRGB8
		|| f == TextureFormat::Format_SRGB8A8
		|| f == TextureFormat::Format_R16F
		|| f == TextureFormat::Format_RG16F
		|| f == TextureFormat::Format_RGBA16F
		|| f == TextureFormat::Format_R32F
		|| f == TextureFormat::Format_RG32F
		|| f == TextureFormat::Format_RGBA32F;
}

static bool IsExtensionSupported(const string& filePath)
{
	string extension = path(filePath).extension().string();

	return find(validExtensions.begin(),
			validExtensions.end(),
			extension)
			!= validExtensions.end();
}

static bool AllTextureExtensionsMatch(
	const vector<string>& filePaths)
{
	string firstExtension = path(filePaths.front()).extension().string();

	for (const auto& filePath : filePaths)
	{
		string thisExtension = path(filePath).extension().string();

		if (thisExtension != firstExtension) return false;
	}

	return true;
}

static u8 GetBytesPerChannel(TextureFormat format)
{
	switch (format)
	{
	case TextureFormat::Format_R8:
	case TextureFormat::Format_RG8:
	case TextureFormat::Format_RGB8:
	case TextureFormat::Format_RGBA8:
	case TextureFormat::Format_SRGB8:
	case TextureFormat::Format_SRGB8A8:
		return 1;
	case TextureFormat::Format_R16F:
	case TextureFormat::Format_RG16F:
	case TextureFormat::Format_RGBA16F:
		return 2;
	case TextureFormat::Format_R32F:
	case TextureFormat::Format_RG32F:
	case TextureFormat::Format_RGBA32F:
		return 4;
	default:
		Log::Print(
			"GetBytesPerChannel called on invalid format! Compressed formats don't have per-channel bytes.",
			"OPENGL_TEXTURE",
			LogType::LOG_ERROR,
			2);

		return 0;
	}
}

static string ToLower(string in)
{
	transform(
		in.begin(),
		in.end(),
		in.begin(),
		[](unsigned char c) 
		{ 
			return static_cast<char>(tolower(c)); 
		});

	return in;
}

struct GLFormatInfo {
	GLint internalFormat;
	GLenum format;
	GLenum type;
};

static stbir_pixel_layout ToStbirLayout(
	TextureFormat format,
	const string& textureName);

static GLenum ToGLTarget(TextureType type);

static GLFormatInfo ToGLFormat(TextureFormat fmt);

namespace KalaWindow::Graphics
{
	u8 Texture::GetMaxMipMapLevels(
		vec2 size,
		u16 depth,
		u8 mipmapLevels)
	{
		u32 maxDim = static_cast<u32>(max({ size.x, size.y, (f32)depth }));
		u32 maxPossibleLevels = 1u + static_cast<u32>(floor(log2(maxDim)));

		u8 clamped = clamp(
			mipmapLevels,
			static_cast<u8>(1),
			static_cast<u8>(maxPossibleLevels));

		return clamped;
	}
}

namespace KalaWindow::Graphics::OpenGL
{
	OpenGL_Texture* OpenGL_Texture::LoadTexture(
		const string& name,
		const string& filePath,
		TextureType type,
		TextureFormat format,
		bool flipVertically,
		u16 depth,
		u8 mipMapLevels)
	{
		//ensure a fallback texture always exists
		if (!fallbackTexture) LoadFallbackTexture();

		if (!OpenGL_Global::IsInitialized())
		{
			KalaWindowCore::ForceClose(
				"Texture error",
				"Cannot load texture '" + name + "' because OpenGL is not initialized!");

			return nullptr;
		}

		if (runtimeWindows.empty())
		{
			KalaWindowCore::ForceClose(
				"Texture error",
				"Cannot load texture '" + name + "' because no windows exist!");

			return nullptr;
		}

		bool foundCont = false;
		for (const Window* w : runtimeWindows)
		{
			OpenGL_Context* cont = GetValueByID<OpenGL_Context>(w->GetOpenGLID());

			if (cont
				&& cont->IsContextValid())
			{
				foundCont = true;
				break;
			}
		}

		if (!foundCont)
		{
			KalaWindowCore::ForceClose(
				"Texture error",
				"Cannot load texture '" + name + "' because no created windows have a valid OpenGL context!");
		}

		vector<u8> data{};
		int nrChannels{};
		vec2 size{};

		TextureFormat resolvedFormat = format;

		OpenGL_Texture* result = InitTexture(
			name,
			filePath,
			type,
			format,
			resolvedFormat,
			flipVertically,
			size,
			data,
			nrChannels);

		if (result != nullptr) return result;

		//
		// BIND TEXTURE
		//

		unsigned int newTextureID{};
		glGenTextures(1, &newTextureID);

		GLenum target = ToGLTarget(type);
		GLFormatInfo fmt = ToGLFormat(resolvedFormat);

		glBindTexture(target, newTextureID);

		glTexParameteri(
			target,
			GL_TEXTURE_WRAP_S,
			GL_REPEAT);
		glTexParameteri(
			target,
			GL_TEXTURE_WRAP_T,
			GL_REPEAT);

		if (target == GL_TEXTURE_3D)
		{
			glTexParameteri(
				target,
				GL_TEXTURE_WRAP_R,
				GL_REPEAT);
		}

		//
		// FILTERING
		//

		glTexParameteri(
			target,
			GL_TEXTURE_MIN_FILTER,
			mipMapLevels > 1
			? GL_LINEAR_MIPMAP_LINEAR
			: GL_LINEAR);
		glTexParameteri(
			target,
			GL_TEXTURE_MAG_FILTER,
			GL_LINEAR);

		//
		// STORE DATA AND INIT TEXTURE
		//

		u32 newID = ++globalID;
		unique_ptr<OpenGL_Texture> newTexture = make_unique<OpenGL_Texture>();
		OpenGL_Texture* texturePtr = newTexture.get();

		newTexture->name = name;
		newTexture->ID = newID;
		newTexture->filePath = filePath;
		newTexture->size = size,
		newTexture->openGLID = newTextureID;
		newTexture->type = type;
		newTexture->format = resolvedFormat;

		u16 clampedDepth = depth;
		if (type == TextureType::Type_2D) clampedDepth = 1;
		else
		{
			clampedDepth = clamp(
				clampedDepth, 
				static_cast<u16>(1), 
				static_cast<u16>(8192));
		}
		newTexture->depth = clampedDepth;

		newTexture->mipMapLevels = GetMaxMipMapLevels(
			newTexture->size,
			clampedDepth,
			mipMapLevels);

		newTexture->pixels = move(data);

		createdOpenGLTextures[newID] = move(newTexture);
		runtimeOpenGLTextures.push_back(texturePtr);

		string errorVal = OpenGL_Global::GetError();
		if (!errorVal.empty())
		{
			KalaWindowCore::ForceClose(
				"OpenGL texture error",
				"Failed to load texture '" + name + "'! Reason: " + errorVal);

			return nullptr;
		}

		texturePtr->HotReload();

		Log::Print(
			"Loaded OpenGL texture '" + name + "' with ID '" + to_string(newID) + "'!",
			"OPENGL_TEXTURE",
			LogType::LOG_SUCCESS);

		return texturePtr;
	}

	OpenGL_Texture* OpenGL_Texture::LoadCubeMapTexture(
		const string& name,
		const array<string, 6>& texturePaths,
		TextureFormat format,
		bool flipVertically,
		u8 mipMapLevels)
	{
		//ensure a fallback texture always exists
		if (!fallbackTexture) LoadFallbackTexture();

		vector<string> paths(begin(texturePaths), end(texturePaths));
		if (!AllTextureExtensionsMatch(paths))
		{
			Log::Print(
				"Failed to load cube map texture '" + name + "' because its extensions don't match!",
				"OPENGL_TEXTURE",
				LogType::LOG_ERROR,
				2);

			return GetFallbackTexture();
		}

		vec2 baseSize{};
		vector<vector<u8>> cubePixelData{};
		TextureFormat resolvedFormat = format;
		int nrChannels{};

		for (const auto& thisPath : texturePaths)
		{
			vector<u8> data{};
			vec2 size{};

			OpenGL_Texture* result = InitTexture(
				name,
				thisPath,
				TextureType::Type_Cube,
				format,
				resolvedFormat,
				flipVertically,
				size,
				data,
				nrChannels);

			if (result != nullptr) return result;

			cubePixelData.push_back(move(data));

			if (baseSize == vec2(0)) baseSize = size;

			string sizeX = to_string(static_cast<int>(size.x));
			string sizeY = to_string(static_cast<int>(size.y));

			if (size.x != size.y)
			{
				ostringstream oss{};
				oss << "failed to create cube map texture '" << name
					<< "' because texture '" << name << "' size x '" << sizeX << "' does not match y '" << sizeY
					<< "'! X and Y must be exactly the same for 'Type_Cube' textures.";

				Log::Print(
					oss.str(),
					"OPENGL_TEXTURE",
					LogType::LOG_ERROR,
					2);

				return GetFallbackTexture();
			}

			if (size != baseSize)
			{
				ostringstream oss{};
				oss << "failed to create cube map texture '" << name
					<< "' because its size '" << sizeX << "x " << sizeY 
					<< "' does not match base size '" << to_string(baseSize.x) << "x" << to_string(baseSize.y)
					<< "'! Each subtexture size must match if you want to create a 'Type_Cube' texture.";

				Log::Print(
					oss.str(),
					"OPENGL_TEXTURE",
					LogType::LOG_ERROR,
					2);

				return GetFallbackTexture();
			}
		}

		//
		// BIND TEXTURE
		//

		unsigned int newTextureID{};
		glGenTextures(1, &newTextureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, newTextureID);

		glTexParameteri(
			GL_TEXTURE_CUBE_MAP,
			GL_TEXTURE_WRAP_S,
			GL_CLAMP_TO_EDGE);
		glTexParameteri(
			GL_TEXTURE_CUBE_MAP,
			GL_TEXTURE_WRAP_T,
			GL_CLAMP_TO_EDGE);
		glTexParameteri(
			GL_TEXTURE_CUBE_MAP,
			GL_TEXTURE_WRAP_R,
			GL_CLAMP_TO_EDGE);

		//
		// FILTERING
		//

		glTexParameteri(
			GL_TEXTURE_CUBE_MAP,
			GL_TEXTURE_MIN_FILTER,
			mipMapLevels > 1 
				? GL_LINEAR_MIPMAP_LINEAR
				: GL_LINEAR);
		glTexParameteri(
			GL_TEXTURE_CUBE_MAP,
			GL_TEXTURE_MAG_FILTER,
			GL_LINEAR);

		//
		// STORE DATA AND INIT TEXTURE
		//

		u32 newID = ++globalID;
		unique_ptr<OpenGL_Texture> newTexture = make_unique<OpenGL_Texture>();
		OpenGL_Texture* texturePtr = newTexture.get();

		texturePtr->name = name;
		texturePtr->ID = newID;
		texturePtr->openGLID = newTextureID;
		texturePtr->size = baseSize;
		texturePtr->type = TextureType::Type_Cube;
		texturePtr->format = resolvedFormat;
		texturePtr->depth = 6; //depth is always 6 for cubemaps

		texturePtr->mipMapLevels = GetMaxMipMapLevels(
			baseSize,
			6, //depth
			mipMapLevels);

		texturePtr->cubePixels = move(cubePixelData);

		createdOpenGLTextures[newID] = move(newTexture);
		runtimeOpenGLTextures.push_back(texturePtr);

		string errorVal = OpenGL_Global::GetError();
		if (!errorVal.empty())
		{
			KalaWindowCore::ForceClose(
				"OpenGL texture error",
				"Failed to load cubemap texture '" + name + "'! Reason: " + errorVal);

			return nullptr;
		}

		texturePtr->HotReload();

		Log::Print(
			"Created OpenGL cube texture '" + name + "' with ID '" + to_string(newID) + "'!",
			"OPENGL_TEXTURE",
			LogType::LOG_SUCCESS);

		return texturePtr;
	}

	OpenGL_Texture* OpenGL_Texture::Load2DArrayTexture(
		const string& name,
		const vector<string>& texturePaths,
		TextureFormat format,
		bool flipVertically,
		u8 mipMapLevels)
	{
		//ensure a fallback texture always exists
		if (!fallbackTexture) LoadFallbackTexture();

		if (!AllTextureExtensionsMatch(texturePaths))
		{
			Log::Print(
				"Failed to load 2D array texture '" + name + "' because its extensions don't match!",
				"OPENGL_TEXTURE",
				LogType::LOG_ERROR,
				2);

			return GetFallbackTexture();
		}

		vec2 baseSize{};
		vector<vector<u8>> layerPixelData{};
		TextureFormat resolvedFormat{};
		int nrChannels{};

		for (const auto& thisPath : texturePaths)
		{
			vector<u8> data{};
			vec2 size{};

			OpenGL_Texture* result = InitTexture(
				name,
				thisPath,
				TextureType::Type_2DArray,
				format,
				resolvedFormat,
				flipVertically,
				size,
				data,
				nrChannels);

			if (result != nullptr) return result;

			layerPixelData.push_back(move(data));

			if (baseSize == vec2(0)) baseSize = size;

			string sizeX = to_string(static_cast<int>(size.x));
			string sizeY = to_string(static_cast<int>(size.y));

			if (size != baseSize)
			{
				ostringstream oss{};
				oss << "failed to create 2D array texture '" << name
					<< "' because its size '" << sizeX << "x " << sizeY
					<< "' does not match base size '" << to_string(baseSize.x) << "x" << to_string(baseSize.y)
					<< "'! Each subtexture size must match if you want to create a 'Type_2D_Array' texture.";

				Log::Print(
					oss.str(),
					"OPENGL_TEXTURE",
					LogType::LOG_ERROR,
					2);

				return GetFallbackTexture();
			}
		}

		//
		// BIND TEXTURE
		//

		unsigned int newTextureID{};
		glGenTextures(1, &newTextureID);
		glBindTexture(GL_TEXTURE_2D_ARRAY, newTextureID);

		glTexParameteri(
			GL_TEXTURE_2D_ARRAY,
			GL_TEXTURE_WRAP_S,
			GL_REPEAT);
		glTexParameteri(
			GL_TEXTURE_2D_ARRAY,
			GL_TEXTURE_WRAP_T,
			GL_REPEAT);
		glTexParameteri(
			GL_TEXTURE_2D_ARRAY,
			GL_TEXTURE_WRAP_R,
			GL_CLAMP_TO_EDGE);

		//
		// FILTERING
		//

		glTexParameteri(
			GL_TEXTURE_2D_ARRAY,
			GL_TEXTURE_MIN_FILTER,
			mipMapLevels > 1
			? GL_LINEAR_MIPMAP_LINEAR
			: GL_LINEAR);
		glTexParameteri(
			GL_TEXTURE_2D_ARRAY,
			GL_TEXTURE_MAG_FILTER,
			GL_LINEAR);

		//
		// STORE DATA AND INIT TEXTURE
		//

		u32 newID = ++globalID;
		unique_ptr<OpenGL_Texture> newTexture = make_unique<OpenGL_Texture>();
		OpenGL_Texture* texturePtr = newTexture.get();

		texturePtr->name = name;
		texturePtr->ID = newID;
		texturePtr->openGLID = newTextureID;
		texturePtr->size = baseSize;
		texturePtr->type = TextureType::Type_2DArray;
		texturePtr->format = resolvedFormat;
		texturePtr->depth = texturePaths.size();

		texturePtr->mipMapLevels = GetMaxMipMapLevels(
			baseSize,
			texturePaths.size(), //size of texturePaths vector
			mipMapLevels);

		texturePtr->layerPixels = move(layerPixelData);

		createdOpenGLTextures[newID] = move(newTexture);
		runtimeOpenGLTextures.push_back(texturePtr);

		string errorVal = OpenGL_Global::GetError();
		if (!errorVal.empty())
		{
			KalaWindowCore::ForceClose(
				"OpenGL texture error",
				"Failed to create 2D array texture '" + name + "'! Reason: " + errorVal);

			return nullptr;
		}

		texturePtr->HotReload();

		Log::Print(
			"Created OpenGL 2D array texture '" + name + "' with ID '" + to_string(newID) + "'!",
			"OPENGL_TEXTURE",
			LogType::LOG_SUCCESS);

		return texturePtr;
	}

	void OpenGL_Texture::LoadFallbackTexture()
	{
		while (glGetError() != GL_NO_ERROR) {} //clear old errors

		if (fallbackTexture != nullptr)
		{
			Log::Print(
				"Fallback texture has already been assigned! Do not call this function manually.",
				"OPENGL_TEXTURE",
				LogType::LOG_ERROR,
				2);

			return;
		}

		unsigned int newTextureID{};
		glGenTextures(1, &newTextureID);

		glBindTexture(GL_TEXTURE_2D, newTextureID);

		//reset state
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexStorage2D(
			GL_TEXTURE_2D,
			1,
			GL_RGBA8,
			32,
			32);

		glTexSubImage2D(
			GL_TEXTURE_2D,
			0,
			0,
			0,
			32,
			32,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			GetFallbackPixels().data());

		glGenerateMipmap(GL_TEXTURE_2D);

		u32 newID = ++globalID;
		unique_ptr<OpenGL_Texture> newTexture = make_unique<OpenGL_Texture>();
		OpenGL_Texture* texturePtr = newTexture.get();

		newTexture->name = string(fallbackTextureName);
		newTexture->ID = newID;
		newTexture->size = { 32.0f, 32.0f };
		newTexture->openGLID = newTextureID;
		const auto& pixelData = GetFallbackPixels();
		newTexture->pixels.assign(pixelData.begin(), pixelData.end());
		newTexture->type = TextureType::Type_2D;
		newTexture->format = TextureFormat::Format_RGBA8;

		createdOpenGLTextures[newID] = move(newTexture);
		runtimeOpenGLTextures.push_back(texturePtr);

		string errorVal = OpenGL_Global::GetError();
		if (!errorVal.empty())
		{
			KalaWindowCore::ForceClose(
				"OpenGL texture error",
				"Failed to load fallback texture! Reason: " + errorVal);

			return;
		}

		fallbackTexture = texturePtr;

		Log::Print(
			"Loaded fallback OpenGL texture with ID '" + to_string(newID) + "'!",
			"OPENGL_TEXTURE",
			LogType::LOG_SUCCESS);
	}

	OpenGL_Texture* OpenGL_Texture::GetFallbackTexture()
	{
		if (!fallbackTexture)
		{
			KalaWindowCore::ForceClose(
				"OpenGL texture error",
				"Failed to get fallback texture!");
		}

		return fallbackTexture;
	}

	void OpenGL_Texture::Rescale(
		vec2 newSize,
		TextureResizeType resizeType)
	{
		if (pixels.empty()
			|| size.x <= 0
			|| size.y <= 0)
		{
			Log::Print(
				"Failed to resize texture '" + name + "' because its pixel data is empty or size is invalid!",
				"OPENGL_TEXTURE",
				LogType::LOG_ERROR,
				2);

			return;
		}

		string sizeX = to_string(static_cast<int>(newSize.x));
		string sizeY = to_string(static_cast<int>(newSize.y));

		if (newSize.x <= 0
			|| newSize.y <= 0)
		{
			ostringstream oss{};
			oss << "failed to resize texture '" << name
				<< "' because the user-defined new size '" << sizeX << "x" << sizeY
				<< "' is too small! Size cannot be below or equal to '0x0' pixels!";

			Log::Print(
				oss.str(),
				"OPENGL_TEXTURE",
				LogType::LOG_ERROR,
				2);

			return;
		}

		//clamp to gpu texture resolution upper bound
		if (TEXTURE_MAX_SIZE == 0)
		{
			GLint maxSize{};
			glGetIntegerv(
				GL_MAX_TEXTURE_SIZE,
				&maxSize);

			TEXTURE_MAX_SIZE = maxSize;
		}

		string maxSizeStr = to_string(TEXTURE_MAX_SIZE);

		if (newSize.x > TEXTURE_MAX_SIZE
			|| newSize.y > TEXTURE_MAX_SIZE)
		{
			ostringstream oss{};
			oss << "failed to resize texture '" << name
				<< "' because the user-defined new size '" << sizeX << "x" << sizeY
				<< "' is too big! Size cannot be above '" << maxSizeStr << "x" << maxSizeStr << "' pixels!";

			Log::Print(
				oss.str(),
				"OPENGL_TEXTURE",
				LogType::LOG_ERROR,
				2);

			return;
		}

		if (type == TextureType::Type_Cube
			&& newSize.x != newSize.y)
		{
			ostringstream oss{};
			oss << "failed to resize texture '" << name
				<< "' because the user-defined size x '" << sizeX << "' does not match y '" << sizeY 
				<< "'! X and Y must be exactly the same for 'Type_Cube' textures.";

			Log::Print(
				oss.str(),
				"OPENGL_TEXTURE",
				LogType::LOG_ERROR,
				2);

			return;
		}

		vector<u8> resized(newSize.x * newSize.y * 4);

		stbir_pixel_layout layout = ToStbirLayout(
			format,
			name);

		switch (resizeType)
		{
		case TextureResizeType::RESIZE_SRGB:
		{
			if (!stbir_resize_uint8_srgb(
				reinterpret_cast<const unsigned char*>(pixels.data()),
				static_cast<int>(size.x),
				static_cast<int>(size.y),
				0,
				resized.data(),
				static_cast<int>(newSize.x),
				static_cast<int>(newSize.y),
				0,
				layout))
			{
				Log::Print(
					"Failed to resize texture '" + name + "' with resize type 'RESIZE_SRGB'!",
					"OPENGL_TEXTURE",
					LogType::LOG_ERROR,
					2);

				return;
			}

			break;
		}
		case TextureResizeType::RESIZE_LINEAR:
		{
			if (!stbir_resize_uint8_linear(
				reinterpret_cast<const unsigned char*>(pixels.data()),
				static_cast<int>(size.x),
				static_cast<int>(size.y),
				0,
				resized.data(),
				static_cast<int>(newSize.x),
				static_cast<int>(newSize.y),
				0,
				layout))
			{
				Log::Print(
					"Failed to resize texture '" + name + "' with resize type 'RESIZE_LINEAR'!",
					"OPENGL_TEXTURE",
					LogType::LOG_ERROR,
					2);

				return;
			}

			break;
		}
		case TextureResizeType::RESIZE_LINEAR_FLOAT:
		{
			vector<float> inputF(size.x* size.y * 4);
			vector<float> outputF(newSize.x* newSize.y * 4);

			//convert u8 to float [0,1]
			for (size_t i = 0; i < pixels.size(); ++i)
			{
				inputF[i] = pixels[i] / 255.0f;
			}

			if (!stbir_resize_float_linear(
				inputF.data(),
				static_cast<int>(size.x),
				static_cast<int>(size.y),
				0,
				outputF.data(),
				static_cast<int>(newSize.x),
				static_cast<int>(newSize.y),
				0,
				layout))
			{
				Log::Print(
					"Failed to resize texture '" + name + "' with resize type 'RESIZE_LINEAR_FLOAT'!",
					"OPENGL_TEXTURE",
					LogType::LOG_ERROR,
					2);

				return;
			}

			//convert back to u8 [0,255]
			resized.resize(newSize.x * newSize.y * 4);
			for (size_t i = 0; i < resized.size(); i++)
			{
				resized[i] = static_cast<u8>(clamp(
					outputF[i] * 255.0f, 0.0f, 255.0f));
			}

			break;
		}
		}

		pixels = move(resized);
		size = { (f32)newSize.x, (f32)newSize.y };

		string errorVal = OpenGL_Global::GetError();
		if (!errorVal.empty())
		{
			KalaWindowCore::ForceClose(
				"OpenGL texture error",
				"Failed to rescale texture '" + name + "'! Reason: " + errorVal);

			return;
		}

		HotReload();

		Log::Print(
			"Rescaled texture '" + name + "' to new scale '" + sizeX + "x" + sizeY + "'!",
			"OPENGL_TEXTURE",
			LogType::LOG_SUCCESS);
	}

	void OpenGL_Texture::HotReload()
	{
		GLenum targetType = ToGLTarget(type);
		
		glBindTexture(targetType, openGLID);

		GLFormatInfo fmt = ToGLFormat(format);

		bool compressed = 
			format == TextureFormat::Format_BC1
			|| format == TextureFormat::Format_BC3
			|| format == TextureFormat::Format_BC4
			|| format == TextureFormat::Format_BC5
			|| format == TextureFormat::Format_BC7;

		string target{};

		switch (type)
		{
		case TextureType::Type_2D:
			//allocate all mip levels up front
			glTexStorage2D(
				GL_TEXTURE_2D,
				mipMapLevels,
				fmt.internalFormat,
				static_cast<GLsizei>(size.x),
				static_cast<GLsizei>(size.y));

			//upload base level data
			if (compressed)
			{
				glCompressedTexSubImage2D(
					GL_TEXTURE_2D,
					0,
					0,
					0,
					static_cast<GLsizei>(size.x),
					static_cast<GLsizei>(size.y),
					fmt.internalFormat,
					static_cast<GLsizei>(pixels.size()),
					pixels.data());

				target = "glCompressedTexSubImage2D (Type_2D)";
			}
			else
			{
				glTexSubImage2D(
					GL_TEXTURE_2D,
					0,
					0,
					0,
					static_cast<GLsizei>(size.x),
					static_cast<GLsizei>(size.y),
					fmt.format,
					fmt.type,
					pixels.data());

				target = "glTexSubImage2D (Type_2D)";
			}
			break;

		case TextureType::Type_2DArray:
			//allocate all mip levels up front
			glTexStorage3D(
				GL_TEXTURE_2D_ARRAY,
				mipMapLevels,
				fmt.internalFormat,
				static_cast<GLsizei>(size.x),
				static_cast<GLsizei>(size.y),
				static_cast<GLsizei>(depth));

			//upload base level data
			if (compressed)
			{
				glCompressedTexSubImage3D(
					GL_TEXTURE_2D_ARRAY,
					0,
					0,
					0,
					0,
					static_cast<GLsizei>(size.x),
					static_cast<GLsizei>(size.y),
					static_cast<GLsizei>(depth),
					fmt.internalFormat,
					static_cast<GLsizei>(pixels.size()),
					pixels.data());

				target = "glCompressedTexSubImage3D (Type_2DArray)";
			}
			else
			{
				glTexSubImage3D(
					GL_TEXTURE_2D_ARRAY,
					0,
					0,
					0,
					0,
					static_cast<GLsizei>(size.x),
					static_cast<GLsizei>(size.y),
					static_cast<GLsizei>(depth),
					fmt.format,
					fmt.type,
					pixels.data());

				target = "glTexSubImage3D (Type_2DArray)";
			}
			break;

		case TextureType::Type_3D:
			//allocate all mip levels up front
			glTexStorage3D(
				GL_TEXTURE_3D,
				mipMapLevels,
				fmt.internalFormat,
				static_cast<GLsizei>(size.x),
				static_cast<GLsizei>(size.y),
				static_cast<GLsizei>(depth));

			//upload base level data
			if (compressed)
			{
				glCompressedTexSubImage3D(
					GL_TEXTURE_3D,
					0,
					0,
					0,
					0,
					static_cast<GLsizei>(size.x),
					static_cast<GLsizei>(size.y),
					static_cast<GLsizei>(depth),
					fmt.internalFormat,
					static_cast<GLsizei>(pixels.size()),
					pixels.data());

				target = "glCompressedTexSubImage3D (Type_3D)";
			}
			else
			{
				glTexSubImage3D(
					GL_TEXTURE_3D,
					0,
					0,
					0,
					0,
					static_cast<GLsizei>(size.x),
					static_cast<GLsizei>(size.y),
					static_cast<GLsizei>(depth),
					fmt.format,
					fmt.type,
					pixels.data());

				target = "glTexSubImage3D (Type_3D)";
			}
			break;

		case TextureType::Type_Cube:
			//allocate all mip levels up front
			glTexStorage2D(
				GL_TEXTURE_CUBE_MAP,
				mipMapLevels,
				fmt.internalFormat,
				static_cast<GLsizei>(size.x),
				static_cast<GLsizei>(size.y));

			for (int i = 0; i < 6; i++)
			{
				//upload base level data
				if (compressed)
				{
					glCompressedTexSubImage2D(
						GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
						0,
						0,
						0,
						static_cast<GLsizei>(size.x),
						static_cast<GLsizei>(size.y),
						fmt.internalFormat,
						static_cast<GLsizei>(cubePixels[i].size()),
						cubePixels[i].data());

					target = "glCompressedTexSubImage2D (Type_Cube)";
				}
				else
				{
					glTexSubImage2D(
						GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
						0,
						0,
						0,
						static_cast<GLsizei>(size.x),
						static_cast<GLsizei>(size.y),
						fmt.format,
						fmt.type,
						cubePixels[i].data());

					target = "glTexSubImage2D (Type_Cube)";
				}
			}
			break;
		}

		if (mipMapLevels > 1) glGenerateMipmap(targetType);

		string errorVal = OpenGL_Global::GetError();
		if (!errorVal.empty())
		{
			KalaWindowCore::ForceClose(
				"OpenGL texture error",
				"Failed to hot-reload texture '" + name + "' with " + target + "! Reason: " + errorVal);

			return;
		}

		Log::Print(
			"Hot-reloaded texture '" + name + "' with '" + target + "'!",
			"OPENGL_TEXTURE",
			LogType::LOG_SUCCESS);
	}

	OpenGL_Texture::~OpenGL_Texture()
	{
		if (openGLID != 0)
		{
			glDeleteTextures(1, &openGLID);
			openGLID = 0;
		}

		Log::Print(
			"Destroyed texture '" + GetName() + "'!",
			"OPENGL_TEXTURE",
			LogType::LOG_SUCCESS);
	}
}

OpenGL_Texture* InitTexture(
	const string& name,
	const string& filePath,
	TextureType type,
	TextureFormat formatIn,
	TextureFormat& formatOut,
	bool flipVertically,
	vec2& size,
	vector<u8>& data,
	int& nrChannels)
{
	if (name == fallbackTextureName)
	{
		Log::Print(
			"Texture name '" + name + "' is reserved for the fallback texture!",
			"OPENGL_TEXTURE",
			LogType::LOG_ERROR,
			2);

		return OpenGL_Texture::GetFallbackTexture();
	}

	TextureCheckResult result = IsValidTexture(name, filePath);

	if (result == TextureCheckResult::RESULT_INVALID) return OpenGL_Texture::GetFallbackTexture();
	else if (result == TextureCheckResult::RESULT_ALREADY_EXISTS)
	{
		for (const auto& [key, value] : createdOpenGLTextures)
		{
			if (value->GetName() == name)
			{
				Log::Print(
					"Returning existing texture '" + name + "'.",
					"OPENGL_TEXTURE",
					LogType::LOG_INFO);

				return value.get();
			}
		}

		Log::Print(
			"Returning fallback texture because existing texture was not found!",
			"OPENGL_TEXTURE",
			LogType::LOG_WARNING);

		return OpenGL_Texture::GetFallbackTexture();
	}

	Log::Print(
		"Loading texture '" + name + "'.",
		"OPENGL_TEXTURE",
		LogType::LOG_INFO);

	//
	// GET TEXTURE DATA FROM FILE
	//

	int width{};
	int height{};
	stbi_set_flip_vertically_on_load(flipVertically);

	unsigned char* newData = stbi_load(
		(filePath).c_str(),
		&width,
		&height,
		&nrChannels,
		0);

	if (!newData)
	{
		Log::Print(
			"Failed to get texture data from texture '" + filePath + "' for texture '" + name + "'!",
			"OPENGL_TEXTURE",
			LogType::LOG_ERROR,
			2);

		return OpenGL_Texture::GetFallbackTexture();
	}

	formatOut = formatIn;
	if (formatOut == TextureFormat::Format_Auto)
	{
		ostringstream oss{};
		oss << "Texture format 'Format_Auto' was used for texture '" + name + "'."
			<< " This is not recommended, consider using true format.";

		Log::Print(
			oss.str(),
			"OPENGL_TEXTURE",
			LogType::LOG_WARNING);

		if (nrChannels == 1) formatOut = TextureFormat::Format_R8;
		else if (nrChannels == 2) formatOut = TextureFormat::Format_RG8;
		else if (nrChannels == 3) formatOut = TextureFormat::Format_RGB8;
		else if (nrChannels == 4) formatOut = TextureFormat::Format_RGBA8;
		else
		{
			Log::Print(
				"Unsupported channel count '" + to_string(nrChannels) + "' for texture '" + name + "'!",
				"OPENGL_TEXTURE",
				LogType::LOG_ERROR,
				2);

			stbi_image_free(newData);

			return OpenGL_Texture::GetFallbackTexture();
		}
	}
	else
	{
		if (!IsCorrectFormat(
			formatOut,
			nrChannels))
		{
			Log::Print(
				"Texture '" + name + "' was loaded with an incorrect format! Channel count is '" + to_string(nrChannels) + "'",
				"OPENGL_TEXTURE",
				LogType::LOG_ERROR,
				2);

			stbi_image_free(newData);

			return OpenGL_Texture::GetFallbackTexture();
		}
	}

	u8 bpc = GetBytesPerChannel(formatOut);
	if (bpc == 0)
	{
		stbi_image_free(newData);

		return OpenGL_Texture::GetFallbackTexture();
	}

	size_t dataSize = static_cast<size_t>(width) * height * nrChannels * bpc;
	data.assign(newData, newData + dataSize);

	stbi_image_free(newData);

	string widthStr = to_string(width);
	string heightStr = to_string(height);

	if (width <= 0
		|| height <= 0)
	{
		ostringstream oss{};
		oss << "failed to load texture '" << name
			<< "' because the texture size '" << widthStr << "x" << heightStr
			<< "' is too small! Size cannot be '0x0' pixels or below!";

		Log::Print(
			oss.str(),
			"OPENGL_TEXTURE",
			LogType::LOG_ERROR,
			2);

		return OpenGL_Texture::GetFallbackTexture();
	}

	//clamp to gpu texture resolution upper bound
	if (TEXTURE_MAX_SIZE == 0)
	{
		GLint maxSize{};
		glGetIntegerv(
			GL_MAX_TEXTURE_SIZE,
			&maxSize);

		TEXTURE_MAX_SIZE = maxSize;
	}

	if (width > TEXTURE_MAX_SIZE
		|| height > TEXTURE_MAX_SIZE)
	{
		string maxSizeStr = to_string(TEXTURE_MAX_SIZE);

		ostringstream oss{};
		oss << "failed to load texture '" << name
			<< "' because the texture size '" << widthStr << "x" << heightStr
			<< "' is too big! Size cannot be above '" << maxSizeStr << "x" << maxSizeStr << "' pixels!";

		Log::Print(
			oss.str(),
			"OPENGL_TEXTURE",
			LogType::LOG_ERROR,
			2);

		return OpenGL_Texture::GetFallbackTexture();
	}

	if (type == TextureType::Type_Cube
		&& width != height)
	{
		ostringstream oss{};
		oss << "failed to resize texture '" << name
			<< "' because the user-defined size x '" << to_string(width) << "' does not match y '" << to_string(height)
			<< "'! X and Y must be exactly the same for 'Type_Cube' textures.";

		Log::Print(
			oss.str(),
			"OPENGL_TEXTURE",
			LogType::LOG_ERROR,
			2);

		return OpenGL_Texture::GetFallbackTexture();
	}

	//texture extension must match allowed format

	string extension = path(filePath).extension().string();
	if (!IsCompressed(formatOut)
		&& extension == ".ktx")
	{
		Log::Print(
			"Non-compressed format was used for compressed texture '" + name + "'!",
			"OPENGL_TEXTURE",
			LogType::LOG_ERROR,
			2);

		return OpenGL_Texture::GetFallbackTexture();
	}
	else if (!IsUncompressed(formatOut)
		&& (extension == ".png"
			|| extension == ".jpg"
			|| extension == ".jpeg"))
	{
		Log::Print(
			"Compressed format was used for uncompressed texture '" + name + "'!",
			"OPENGL_TEXTURE",
			LogType::LOG_ERROR,
			2);

		return OpenGL_Texture::GetFallbackTexture();
	}

	size = vec2(width, height);
	return nullptr;
}

TextureCheckResult IsValidTexture(
	const string& textureName,
	const string& texturePath)
{
	//texture name must not be empty

	if (textureName.empty())
	{
		Log::Print(
			"Cannot load a texture with no name!",
			"OPENGL_TEXTURE",
			LogType::LOG_ERROR,
			2);

		return TextureCheckResult::RESULT_INVALID;
	}

	//texture path must not be empty

	if (texturePath.empty())
	{
		Log::Print(
			"Cannot load a texture with no path!",
			"OPENGL_TEXTURE",
			LogType::LOG_ERROR,
			2);

		return TextureCheckResult::RESULT_INVALID;
	}

	//texture file must exist

	if (!exists(texturePath))
	{
		Log::Print(
			"Texture '" + textureName + "' path '" + texturePath + "' does not exist!",
			"OPENGL_TEXTURE",
			LogType::LOG_ERROR,
			2);

		return TextureCheckResult::RESULT_INVALID;
	}

	string lowerFilePath = ToLower(texturePath);

	//texture file must have an extension

	if (!path(lowerFilePath).has_extension())
	{
		Log::Print(
			"Texture '" + textureName + "' has no extension. You must use png, jpg or jpeg!",
			"OPENGL_TEXTURE",
			LogType::LOG_ERROR,
			2);

		return TextureCheckResult::RESULT_INVALID;
	}

	//texture file must have a supported extension

	if (!IsExtensionSupported(lowerFilePath))
	{
		Log::Print(
			"Texture '" + textureName + "' extension '" + path(lowerFilePath).extension().string() + "' is not supported!",
			"OPENGL_TEXTURE",
			LogType::LOG_ERROR,
			2);

		return TextureCheckResult::RESULT_INVALID;
	}

	//pass existing texture if one with the same name or path already exists

	for (const auto& [_, value] : createdOpenGLTextures)
	{
		if (value->GetName() == textureName 
			|| value->GetPath() == lowerFilePath)
		{
			Log::Print("Texture already exists: " + textureName, 
				"OPENGL_TEXTURE",
				LogType::LOG_ERROR, 
				2);
			return TextureCheckResult::RESULT_ALREADY_EXISTS;
		}
	}

	return TextureCheckResult::RESULT_OK;
}

bool IsCorrectFormat(
	TextureFormat format,
	int nrChannels)
{
	switch (nrChannels)
	{
	case 1:
		return format == TextureFormat::Format_R8
			|| format == TextureFormat::Format_R16F
			|| format == TextureFormat::Format_R32F
			|| format == TextureFormat::Format_BC4;
	case 2:
		return format == TextureFormat::Format_RG8
			|| format == TextureFormat::Format_RG16F
			|| format == TextureFormat::Format_RG32F
			|| format == TextureFormat::Format_BC5;
	case 3:
		return format == TextureFormat::Format_RGB8
			|| format == TextureFormat::Format_SRGB8
			|| format == TextureFormat::Format_BC1;
	case 4:
		return format == TextureFormat::Format_RGBA8
			|| format == TextureFormat::Format_SRGB8A8
			|| format == TextureFormat::Format_RGBA16F
			|| format == TextureFormat::Format_RGBA32F
			|| format == TextureFormat::Format_BC3
			|| format == TextureFormat::Format_BC7;
	default: return false;
	}
}

stbir_pixel_layout ToStbirLayout(
	TextureFormat format,
	const string& textureName)
{
	string title = "OpenGL Texture Error";

	switch (format)
	{
	case TextureFormat::Format_Auto:
		Log::Print(
			"Unsupported resize: Format_None/Format_Auto for texture '" + textureName + "'!",
			"OPENGL_TEXTURE",
			LogType::LOG_ERROR,
			2);

		return STBIR_RGBA;

	//single channel / depth textures
	case TextureFormat::Format_R8:
	case TextureFormat::Format_R16F:
	case TextureFormat::Format_R32F:
		return STBIR_1CHANNEL;

	//two channel
	case TextureFormat::Format_RG8:
	case TextureFormat::Format_RG16F:
	case TextureFormat::Format_RG32F:
		return STBIR_2CHANNEL;

	//three channel, stb requires explicit RGB/BGR
	case TextureFormat::Format_RGB8:
	case TextureFormat::Format_SRGB8:
		return STBIR_RGB;

	//four channel
	case TextureFormat::Format_RGBA8:
	case TextureFormat::Format_RGBA16F:
	case TextureFormat::Format_RGBA32F:
	case TextureFormat::Format_SRGB8A8:
		return STBIR_RGBA;

	//compressed formats: can’t resize directly
	case TextureFormat::Format_BC1:
	case TextureFormat::Format_BC3:
	case TextureFormat::Format_BC4:
	case TextureFormat::Format_BC5:
	case TextureFormat::Format_BC7:

		Log::Print(
			"Unsupported resize: compressed format for texture '" + textureName + "'!",
			"OPENGL_TEXTURE",
			LogType::LOG_ERROR,
			2);

		return STBIR_RGBA;

	default:
		Log::Print(
			"Unknown TextureFormat in ToStbirLayout() for texture '" + textureName + "'!",
			"OPENGL_TEXTURE",
			LogType::LOG_ERROR,
			2);

		return STBIR_RGBA;
	}
}

GLenum ToGLTarget(TextureType type)
{
	switch (type)
	{
	case TextureType::Type_2D:      return GL_TEXTURE_2D;
	case TextureType::Type_2DArray: return GL_TEXTURE_2D_ARRAY;
	case TextureType::Type_3D:      return GL_TEXTURE_3D;
	case TextureType::Type_Cube:    return GL_TEXTURE_CUBE_MAP;
	default: return GL_TEXTURE_2D;
	}
}

GLFormatInfo ToGLFormat(TextureFormat fmt)
{
	switch (fmt)
	{
	//
	// UNORM
	//

	case TextureFormat::Format_R8:       return { GL_R8,    GL_RED,  GL_UNSIGNED_BYTE };
	case TextureFormat::Format_RG8:      return { GL_RG8,   GL_RG,   GL_UNSIGNED_BYTE };
	case TextureFormat::Format_RGB8:     return { GL_RGB8,  GL_RGB,  GL_UNSIGNED_BYTE };
	case TextureFormat::Format_RGBA8:    return { GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE };

	//
	// FLOAT
	//

	case TextureFormat::Format_R16F:     return { GL_R16F,   GL_RED,  GL_HALF_FLOAT };
	case TextureFormat::Format_RG16F:    return { GL_RG16F,  GL_RG,   GL_HALF_FLOAT };
	case TextureFormat::Format_RGBA16F:  return { GL_RGBA16F,GL_RGBA, GL_HALF_FLOAT };

	case TextureFormat::Format_R32F:     return { GL_R32F,   GL_RED,  GL_FLOAT };
	case TextureFormat::Format_RG32F:    return { GL_RG32F,  GL_RG,   GL_FLOAT };
	case TextureFormat::Format_RGBA32F:  return { GL_RGBA32F,GL_RGBA, GL_FLOAT };

	//
	// COMPRESSED
	//

	case TextureFormat::Format_BC1: return { GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, GL_RGBA, GL_UNSIGNED_BYTE };
	case TextureFormat::Format_BC3: return { GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, GL_RGBA, GL_UNSIGNED_BYTE };
	case TextureFormat::Format_BC4: return { GL_COMPRESSED_RED_RGTC1, GL_RED, GL_UNSIGNED_BYTE };
	case TextureFormat::Format_BC5: return { GL_COMPRESSED_RG_RGTC2, GL_RG, GL_UNSIGNED_BYTE };
	case TextureFormat::Format_BC7: return { GL_COMPRESSED_RGBA_BPTC_UNORM, GL_RGBA, GL_UNSIGNED_BYTE };

	default: return { GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE }; //safe fallback
	}
}