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

#include "graphics/texture.hpp"
#include "graphics/opengl/opengl.hpp"
#include "graphics/opengl/texture_opengl.hpp"
#include "graphics/opengl/opengl_functions_core.hpp"
#include "core/core.hpp"
#include "core/containers.hpp"

using KalaHeaders::Log;
using KalaHeaders::LogType;

using KalaWindow::Graphics::OpenGL::Renderer_OpenGL;
using KalaWindow::Graphics::OpenGL::Texture_OpenGL;
using KalaWindow::Graphics::Texture;
using KalaWindow::Graphics::TextureType;
using KalaWindow::Graphics::TextureFormat;
using namespace KalaWindow::Graphics::OpenGLFunctions;
using KalaWindow::Core::KalaWindowCore;
using KalaWindow::Core::globalID;
using KalaWindow::Core::createdOpenGLTextures;
using KalaWindow::Core::runtimeOpenGLTextures;

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

static u32 TEXTURE_MAX_SIZE{};

static Texture_OpenGL* fallbackTexture{};

constexpr string_view fallbackTextureName = "FallbackTexture";

//1x1 magenta texture fallback if a user-imported texture fails to load
constexpr array<u8, 16> fallbackPixels = 
{
	255, 0, 255, 255,   //magenta
	  0, 0,   0, 255,   //black
	  0, 0,   0, 255,   //black
	255, 0, 255, 255    //magenta
};

static TextureCheckResult IsValidTexture(
	const string& textureName,
	const string& texturePath);

struct GLFormatInfo {
	GLint internalFormat;
	GLenum format;
	GLenum type;
};

static void CheckError(const string& message);

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
	Texture_OpenGL* Texture_OpenGL::LoadTexture(
		const string& name,
		const string& path,
		TextureType type,
		TextureFormat format,
		TextureUsage usage,
		bool flipVertically,
		u16 depth,
		u8 mipMapLevels)
	{
		//ensure a fallback texture always exists
		if (fallbackTexture == nullptr) LoadFallbackTexture();

		if (name == fallbackTextureName)
		{
			Log::Print(
				"Texture name '" + name + "' is reserved for the fallback texture!",
				"TEXTURE_OPENGL",
				LogType::LOG_ERROR);

			return GetFallbackTexture();
		}

		if (format == TextureFormat::Format_None)
		{
			Log::Print(
				"Texture '" + name + "' format 'Format_None' is not valid! You must assign as 'Format_Auto' or something else",
				"TEXTURE_OPENGL",
				LogType::LOG_ERROR);

			return GetFallbackTexture();
		}

		if (type == TextureType::Type_Cube)
		{
			Log::Print(
				"Texture '" + name + "' type 'Type_Cube' is not valid! You must use 'Texture_OpenGL::CreateCubeMapTexture to create a cubemap texture'!",
				"TEXTURE_OPENGL",
				LogType::LOG_ERROR);

			return GetFallbackTexture();
		}

		//Format_BC7 requires GL_ARB_texture_compression_bptc on OpenGL
		if (format == TextureFormat::Format_BC7
			&& !Renderer_OpenGL::IsExtensionSupported("GL_ARB_texture_compression_bptc"))
		{
			ostringstream oss{};
			oss << "Unsupported texture format 'Format_BC7' was used for texture '" << name 
				<< "'! GL_ARB_texture_compression_bptc is required for this format"
				<< " but it is not supported on your hardware. Consider using 'Format_BC3' instead.";

			Log::Print(
				oss.str(),
				"TEXTURE_OPENGL",
				LogType::LOG_ERROR);

			return GetFallbackTexture();
		}

		TextureCheckResult result = (IsValidTexture(name, path));

		Texture_OpenGL* existingTex{};

		if (result == TextureCheckResult::RESULT_INVALID) return GetFallbackTexture();
		else if (result == TextureCheckResult::RESULT_ALREADY_EXISTS)
		{
			for (const auto& [key, value] : createdOpenGLTextures)
			{
				if (value->GetName() == name)
				{
					existingTex = value.get();
					break;
				}
			}

			if (existingTex != nullptr)
			{
				Log::Print(
					"Returning existing texture '" + name + "'.",
					"TEXTURE_OPENGL",
					LogType::LOG_DEBUG);

				return existingTex;
			}
			return GetFallbackTexture();
		}

		Log::Print(
			"Loading texture '" + name + "'.",
			"TEXTURE_OPENGL",
			LogType::LOG_DEBUG);

		//
		// GET TEXTURE DATA FROM FILE
		//

		int width{};
		int height{};
		int nrChannels{};
		stbi_set_flip_vertically_on_load(flipVertically);

		unsigned char* data = stbi_load(
			(path).c_str(),
			&width,
			&height,
			&nrChannels,
			0);

		TextureFormat resolvedFormat = format;
		if (resolvedFormat == TextureFormat::Format_Auto)
		{
			ostringstream oss{};
			oss << "Texture format 'Format_Auto' was used for texture '" + name + "'."
				<< " This is not recommended, consider using true format.";

			Log::Print(
				oss.str(),
				"TEXTURE_OPENGL",
				LogType::LOG_WARNING);

			if (nrChannels == 1) resolvedFormat = TextureFormat::Format_R8;
			else if (nrChannels == 3) resolvedFormat = TextureFormat::Format_RGB8;
			else if (nrChannels == 4) resolvedFormat = TextureFormat::Format_RGBA8;
			else
			{
				Log::Print(
					"Unsupported channel count '" + to_string(nrChannels) + "' for texture '" + name + "'!",
					"TEXTURE_OPENGL",
					LogType::LOG_ERROR);

				stbi_image_free(data);

				return GetFallbackTexture();
			}
		}

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
				"TEXTURE_OPENGL",
				LogType::LOG_ERROR);

			stbi_image_free(data);

			return GetFallbackTexture();
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
				"TEXTURE_OPENGL",
				LogType::LOG_ERROR);

			stbi_image_free(data);

			return GetFallbackTexture();
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
				"TEXTURE_OPENGL",
				LogType::LOG_ERROR);

			stbi_image_free(data);

			return GetFallbackTexture();
		}

		if (data == nullptr)
		{
			Log::Print(
				"Failed to load texture '" + path + "' because stbi data is nullptr!",
				"TEXTURE_OPENGL",
				LogType::LOG_ERROR);

			stbi_image_free(data);

			return GetFallbackTexture();
		}

		//
		// BIND TEXTURE
		//

		unsigned int newTextureID{};
		glGenTextures(1, &newTextureID);

		GLenum target = ToGLTarget(type);
		GLFormatInfo fmt = ToGLFormat(resolvedFormat);

		glBindTexture(target, newTextureID);

		bool isDepth = (
			resolvedFormat == TextureFormat::Format_Depth24
			|| resolvedFormat == TextureFormat::Format_Depth32F
			|| resolvedFormat == TextureFormat::Format_Depth24Stencil8
			|| resolvedFormat == TextureFormat::Format_Depth32FStencil8);

		// Default sampler states by format/usage:
		//
		// Depth + Usage_DepthStencil - Clamp to edge + Nearest filtering
		// Depth + non-DepthStencil   - Repeat + Linear (or trilinear if mipmaps)
		// Color + mipmaps > 1        - Repeat + Trilinear filtering
		// Color + no mipmaps         - Repeat + Bilinear filtering

		//
		// WRAPPING
		//

		//depth textures usually want clamp + border (for shadow maps)
		if (isDepth)
		{
			if (usage == TextureUsage::Usage_DepthStencil)
			{
				glTexParameteri(
					target,
					GL_TEXTURE_WRAP_S,
					GL_CLAMP_TO_EDGE);
				glTexParameteri(
					target,
					GL_TEXTURE_WRAP_T,
					GL_CLAMP_TO_EDGE);

				if (target == GL_TEXTURE_3D
					|| target == GL_TEXTURE_2D_ARRAY)
				{
					glTexParameteri(
						target,
						GL_TEXTURE_WRAP_R,
						GL_CLAMP_TO_EDGE);
				}
			}
			else
			{
				glTexParameteri(
					target,
					GL_TEXTURE_WRAP_S,
					GL_CLAMP_TO_BORDER);
				glTexParameteri(
					target,
					GL_TEXTURE_WRAP_T,
					GL_CLAMP_TO_BORDER);

				if (target == GL_TEXTURE_3D
					|| target == GL_TEXTURE_2D_ARRAY)
				{
					glTexParameteri(
						target,
						GL_TEXTURE_WRAP_R,
						GL_CLAMP_TO_BORDER);
				}

				float borderColor[4] =
				{
					1.0f,
					1.0f,
					1.0f,
					1.0f
				};

				glTexParameterfv(
					target,
					GL_TEXTURE_BORDER_COLOR,
					borderColor);
			}
		}
		//regular color/compressed - repeat by default
		else
		{
			glTexParameteri(
				target,
				GL_TEXTURE_WRAP_S,
				GL_REPEAT);
			glTexParameteri(
				target,
				GL_TEXTURE_WRAP_T,
				GL_REPEAT);

			if (target == GL_TEXTURE_3D
				|| target == GL_TEXTURE_2D_ARRAY)
			{
				glTexParameteri(
					target,
					GL_TEXTURE_WRAP_R,
					GL_REPEAT);
			}
		}

		//
		// FILTERING
		//

		GLenum filterParam1{};
		GLenum filterParam2{};

		if (isDepth
			&& usage == TextureUsage::Usage_DepthStencil)
		{
			filterParam1 = GL_NEAREST;
			filterParam2 = GL_NEAREST;
		}
		else if (mipMapLevels > 1)
		{
			filterParam1 = GL_LINEAR_MIPMAP_LINEAR;
			filterParam2 = GL_LINEAR;
		}
		else
		{
			filterParam1 = GL_LINEAR;
			filterParam2 = GL_LINEAR;
		}

		glTexParameteri(
			target,
			GL_TEXTURE_MIN_FILTER,
			filterParam1);
		glTexParameteri(
			target,
			GL_TEXTURE_MAG_FILTER,
			filterParam2);

		//
		// STORE DATA AND INIT TEXTURE
		//

		u32 newID = ++globalID;
		unique_ptr<Texture_OpenGL> newTexture = make_unique<Texture_OpenGL>();
		Texture_OpenGL* texturePtr = newTexture.get();

		newTexture->name = name;
		newTexture->ID = newID;
		newTexture->path = path;
		newTexture->size = vec2(width, height);
		newTexture->openGLID = newTextureID;
		newTexture->type = type;
		newTexture->format = resolvedFormat;
		newTexture->usage = usage;

		u16 clampedDepth = depth;
		if (type == TextureType::Type_2D
			|| type == TextureType::Type_Cube)
		{
			clampedDepth = 1;
		}
		else
		{
			clampedDepth = clamp(
				clampedDepth, 
				static_cast<u16>(256), 
				static_cast<u16>(8192));
		}
		newTexture->depth = clampedDepth;

		newTexture->mipMapLevels = GetMaxMipMapLevels(
			newTexture->size,
			clampedDepth,
			mipMapLevels);

		size_t dataSize = static_cast<size_t>(width) * height * nrChannels;
		vector<u8> pixels(data, data + dataSize);
		newTexture->pixels = pixels;

		stbi_image_free(data);

		newTexture->HotReload();

		createdOpenGLTextures[newID] = move(newTexture);
		runtimeOpenGLTextures.push_back(texturePtr);

		CheckError("loading texture '" + name + "'");

		Log::Print(
			"Loaded OpenGL texture '" + name + "' with ID '" + to_string(newID) + "'!",
			"TEXTURE_OPENGL",
			LogType::LOG_SUCCESS);

		return texturePtr;
	}

	Texture_OpenGL* Texture_OpenGL::CreateCubeMapTexture(
		const string& name,
		const array<Texture*, 6>& textures,
		u8 mipMapLevels)
	{
		vec2 baseSize = textures[0]->GetSize();
		TextureFormat baseFormat = textures[0]->GetFormat();
		TextureUsage baseUsage = textures[0]->GetUsage();

		for (const auto& tex : textures)
		{
			string thisName = tex->GetName();

			if (tex->GetType() == TextureType::Type_Cube)
			{
				Log::Print(
					"Failed to create cube map texture '" + name + "' because texture '" + thisName + "' is already a cube texture!",
					"TEXTURE_OPENGL",
					LogType::LOG_ERROR);

				return GetFallbackTexture();
			}

			vec2 size = tex->GetSize();
			string sizeX = to_string(static_cast<int>(size.x));
			string sizeY = to_string(static_cast<int>(size.y));

			if (size.x != size.y)
			{
				ostringstream oss{};
				oss << "failed to create cube map texture '" << name
					<< "' because texture '" << thisName << "' size x '" << sizeX << "' does not match y '" << sizeY
					<< "'! X and Y must be exactly the same for 'Type_Cube' textures.";

				Log::Print(
					oss.str(),
					"TEXTURE_OPENGL",
					LogType::LOG_ERROR);

				return GetFallbackTexture();
			}

			if (tex->GetSize() != baseSize)
			{
				ostringstream oss{};
				oss << "failed to create cube map texture '" << name
					<< "' because texture '" << thisName << "' size '" << sizeX << "x " << sizeY 
					<< "' does not match base size '" << to_string(baseSize.x) << "x" << to_string(baseSize.y)
					<< "'! Each subtexture size must match if you want to create a 'Type_Cube' texture.";

				Log::Print(
					oss.str(),
					"TEXTURE_OPENGL",
					LogType::LOG_ERROR);

				return GetFallbackTexture();
			}

			if (tex->GetFormat() != baseFormat)
			{
				ostringstream oss{};
				oss << "failed to create cube map texture '" << name
					<< "' because texture formats do not match!"
					<< " Each subtexture format must match if you want to create a 'Type_Cube' texture.";

				Log::Print(
					oss.str(),
					"TEXTURE_OPENGL",
					LogType::LOG_ERROR);

				return GetFallbackTexture();
			}

			if (tex->GetUsage() != baseUsage)
			{
				ostringstream oss{};
				oss << "failed to create cube map texture '" << name
					<< "' because texture usages do not match!"
					<< " Each subtexture usage must match if you want to create a 'Type_Cube' texture.";

				Log::Print(
					oss.str(),
					"TEXTURE_OPENGL",
					LogType::LOG_ERROR);

				return GetFallbackTexture();
			}

			if (tex->GetPixels().empty())
			{
				ostringstream oss{};
				oss << "failed to create cube map texture '" << name
					<< "' because texture '" << thisName << "' has no pixel data!"
					<< " Each subtexture must have pixel data to create a 'Type_Cube' texture.";

				Log::Print(
					oss.str(),
					"TEXTURE_OPENGL",
					LogType::LOG_ERROR);

				return GetFallbackTexture();
			}
		}

		//
		// BIND TEXTURE
		//

		unsigned int newTextureID{};
		glGenTextures(1, &newTextureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, newTextureID);

		bool isDepth = (
			baseFormat == TextureFormat::Format_Depth24
			|| baseFormat == TextureFormat::Format_Depth32F
			|| baseFormat == TextureFormat::Format_Depth24Stencil8
			|| baseFormat == TextureFormat::Format_Depth32FStencil8);

		// Default sampler states by format/usage:
		//
		// Depth + Usage_DepthStencil - Clamp to edge + Nearest filtering
		// Depth + non-DepthStencil   - Repeat + Linear (or trilinear if mipmaps)
		// Color + mipmaps > 1        - Repeat + Trilinear filtering
		// Color + no mipmaps         - Repeat + Bilinear filtering

		//
		// WRAPPING
		//

		//depth textures usually want clamp + border (for shadow maps)
		if (isDepth)
		{
			if (baseUsage == TextureUsage::Usage_DepthStencil)
			{
				glTexParameteri(
					GL_TEXTURE_CUBE_MAP,
					GL_TEXTURE_WRAP_S,
					GL_CLAMP_TO_EDGE);
				glTexParameteri(
					GL_TEXTURE_CUBE_MAP,
					GL_TEXTURE_WRAP_T,
					GL_CLAMP_TO_EDGE);
			}
			else
			{
				glTexParameteri(
					GL_TEXTURE_CUBE_MAP,
					GL_TEXTURE_WRAP_S,
					GL_CLAMP_TO_BORDER);
				glTexParameteri(
					GL_TEXTURE_CUBE_MAP,
					GL_TEXTURE_WRAP_T,
					GL_CLAMP_TO_BORDER);

				float borderColor[4] =
				{
					1.0f,
					1.0f,
					1.0f,
					1.0f
				};

				glTexParameterfv(
					GL_TEXTURE_CUBE_MAP,
					GL_TEXTURE_BORDER_COLOR,
					borderColor);
			}
		}
		//regular color/compressed - repeat by default
		else
		{
			glTexParameteri(
				GL_TEXTURE_CUBE_MAP,
				GL_TEXTURE_WRAP_S,
				GL_REPEAT);
			glTexParameteri(
				GL_TEXTURE_CUBE_MAP,
				GL_TEXTURE_WRAP_T,
				GL_REPEAT);
		}

		//
		// FILTERING
		//

		GLenum filterParam1{};
		GLenum filterParam2{};

		if (isDepth
			&& baseUsage == TextureUsage::Usage_DepthStencil)
		{
			filterParam1 = GL_NEAREST;
			filterParam2 = GL_NEAREST;
		}
		else if (mipMapLevels > 1)
		{
			filterParam1 = GL_LINEAR_MIPMAP_LINEAR;
			filterParam2 = GL_LINEAR;
		}
		else
		{
			filterParam1 = GL_LINEAR;
			filterParam2 = GL_LINEAR;
		}

		glTexParameteri(
			GL_TEXTURE_CUBE_MAP,
			GL_TEXTURE_MIN_FILTER,
			filterParam1);
		glTexParameteri(
			GL_TEXTURE_CUBE_MAP,
			GL_TEXTURE_MAG_FILTER,
			filterParam2);

		//
		// STORE DATA AND INIT TEXTURE
		//

		u32 newID = ++globalID;
		unique_ptr<Texture_OpenGL> newTexture = make_unique<Texture_OpenGL>();
		Texture_OpenGL* texturePtr = newTexture.get();

		texturePtr->name = name;
		texturePtr->ID = newID;
		texturePtr->openGLID = newTextureID;
		texturePtr->size = baseSize;
		texturePtr->type = TextureType::Type_Cube;
		texturePtr->format = baseFormat;
		texturePtr->usage = baseUsage;
		texturePtr->depth = 1; //depth is always 1 for cubemaps

		texturePtr->mipMapLevels = GetMaxMipMapLevels(
			baseSize,
			1, //depth
			mipMapLevels);

		for (int i = 0; i < 6; i++)
		{
			Texture_OpenGL* faceTex = static_cast<Texture_OpenGL*>(textures[i]);
			texturePtr->cubePixels[i] = faceTex->GetPixels();
		}

		newTexture->HotReload();

		createdOpenGLTextures[newID] = move(newTexture);
		runtimeOpenGLTextures.push_back(texturePtr);

		CheckError("creating cube texture '" + name + "'");

		Log::Print(
			"Created OpenGL cube texture '" + name + "' with ID '" + to_string(newID) + "'!",
			"TEXTURE_OPENGL",
			LogType::LOG_SUCCESS);

		return texturePtr;
	}

	void Texture_OpenGL::LoadFallbackTexture()
	{
		if (fallbackTexture != nullptr)
		{
			Log::Print(
				"Fallback texture has already been assigned! Do not call this function manually.",
				"TEXTURE_OPENGL",
				LogType::LOG_ERROR);

			return;
		}

		unsigned int newTextureID{};
		glGenTextures(1, &newTextureID);

		glBindTexture(GL_TEXTURE_2D, newTextureID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RGBA8,
			2,
			2,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			fallbackPixels.data()
		);

		u32 newID = ++globalID;
		unique_ptr<Texture_OpenGL> newTexture = make_unique<Texture_OpenGL>();
		Texture_OpenGL* texturePtr = newTexture.get();

		newTexture->name = string(fallbackTextureName);
		newTexture->ID = newID;
		newTexture->size = { 2.0f, 2.0f };
		newTexture->openGLID = newTextureID;
		newTexture->pixels.assign(fallbackPixels.begin(), fallbackPixels.end());
		newTexture->type = TextureType::Type_2D;
		newTexture->format = TextureFormat::Format_RGBA8;

		createdOpenGLTextures[newID] = move(newTexture);
		runtimeOpenGLTextures.push_back(texturePtr);

		fallbackTexture = texturePtr;

		CheckError("loading fallback texture '" + string(fallbackTextureName) + "'");

		Log::Print(
			"Loaded fallback OpenGL texture '" + string(fallbackTextureName) + "' with ID '" + to_string(newID) + "'.",
			"TEXTURE_OPENGL",
			LogType::LOG_SUCCESS);
	}

	Texture_OpenGL* Texture_OpenGL::GetFallbackTexture()
	{
		if (fallbackTexture == nullptr)
		{
			KalaWindowCore::ForceClose(
				"OpenGL texture error",
				"Failed to get fallback texture!");
		}

		return fallbackTexture;
	}

	void Texture_OpenGL::Rescale(
		vec2 newSize,
		TextureResizeType resizeType)
	{
		if (pixels.empty()
			|| size.x <= 0
			|| size.y <= 0)
		{
			Log::Print(
				"Failed to resize texture '" + name + "' because its pixel data is empty or size is invalid!",
				"TEXTURE_OPENGL",
				LogType::LOG_ERROR);

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
				"TEXTURE_OPENGL",
				LogType::LOG_ERROR);

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
				"TEXTURE_OPENGL",
				LogType::LOG_ERROR);

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
				"TEXTURE_OPENGL",
				LogType::LOG_ERROR);

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
					"TEXTURE_OPENGL",
					LogType::LOG_ERROR);

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
					"TEXTURE_OPENGL",
					LogType::LOG_ERROR);

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
					"TEXTURE_OPENGL",
					LogType::LOG_ERROR);

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

		CheckError("rescaling texture '" + name + "'");

		HotReload();

		Log::Print(
			"Rescaled texture '" + name + "' to new scale '" + sizeX + "x" + sizeY + "'!",
			"TEXTURE_OPENGL",
			LogType::LOG_SUCCESS);
	}

	void Texture_OpenGL::HotReload()
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

		CheckError("hot-reloading texture '" + name + "' with " + target);
		Log::Print(
			"Hot-reloaded texture '" + name + "' with '" + target + "'!",
			"TEXTURE_OPENGL",
			LogType::LOG_SUCCESS);

		if (mipMapLevels > 1) glGenerateMipmap(targetType);
	}

	Texture_OpenGL::~Texture_OpenGL()
	{
		if (openGLID != 0)
		{
			glDeleteTextures(1, &openGLID);
			openGLID = 0;
		}

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
		Log::Print(
			"Cannot load a texture with no name!",
			"TEXTURE_OPENGL",
			LogType::LOG_ERROR);

		return TextureCheckResult::RESULT_INVALID;
	}

	//texture path must not be empty

	if (texturePath.empty())
	{
		Log::Print(
			"Cannot load a texture with no path!",
			"TEXTURE_OPENGL",
			LogType::LOG_ERROR);

		return TextureCheckResult::RESULT_INVALID;
	}

	string texturePathName = path(texturePath).filename().string();

	//texture file must exist

	if (!exists(texturePath))
	{
		Log::Print(
			"Texture '" + textureName + "' path '" + texturePathName + "' does not exist!",
			"TEXTURE_OPENGL",
			LogType::LOG_ERROR);

		return TextureCheckResult::RESULT_INVALID;
	}

	//texture file must have an extension

	if (!path(texturePath).has_extension())
	{
		Log::Print(
			"Texture '" + textureName + "' has no extension. You must use png, jpg or jpeg!",
			"TEXTURE_OPENGL",
			LogType::LOG_ERROR);

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
		Log::Print(
			"Texture '" + textureName + "' has an invalid extension '" + thisExtension + "'. Only png, jpg and jpeg are allowed!",
			"TEXTURE_OPENGL",
			LogType::LOG_ERROR);

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

void CheckError(const string& message)
{
#ifdef _DEBUG
	Renderer_OpenGL::GetError(message);
#endif
}

stbir_pixel_layout ToStbirLayout(
	TextureFormat format,
	const string& textureName)
{
	string title = "OpenGL Texture Error";

	switch (format)
	{
	case TextureFormat::Format_None:
	case TextureFormat::Format_Auto:
		Log::Print(
			"Unsupported resize: Format_None/Format_Auto for texture '" + textureName + "'!",
			"TEXTURE_OPENGL",
			LogType::LOG_ERROR);

		return STBIR_RGBA;

	//single channel / depth textures
	case TextureFormat::Format_R8:
	case TextureFormat::Format_R16F:
	case TextureFormat::Format_R32F:
	case TextureFormat::Format_Depth24:
	case TextureFormat::Format_Depth32F:
	case TextureFormat::Format_Depth24Stencil8:
	case TextureFormat::Format_Depth32FStencil8:
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
			"TEXTURE_OPENGL",
			LogType::LOG_ERROR);

		return STBIR_RGBA;

	default:
		Log::Print(
			"Unknown TextureFormat in ToStbirLayout() for texture '" + textureName + "'!",
			"TEXTURE_OPENGL",
			LogType::LOG_ERROR);

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
	// DEPTH
	//

	case TextureFormat::Format_Depth24:          return { GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT };
	case TextureFormat::Format_Depth32F:         return { GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT };
	case TextureFormat::Format_Depth24Stencil8:  return { GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8 };
	case TextureFormat::Format_Depth32FStencil8: return { GL_DEPTH32F_STENCIL8, GL_DEPTH_STENCIL, GL_FLOAT_32_UNSIGNED_INT_24_8_REV };

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