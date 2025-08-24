//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <vector>

#include "KalaHeaders/api.hpp"
#include "KalaHeaders/core_types.hpp"
#include "KalaHeaders/logging.hpp"

#include "core/glm_global.hpp"

namespace KalaWindow::Graphics
{
	using KalaHeaders::Log;
	using KalaHeaders::LogType;

	using std::string;
	using std::vector;

	//Texture internal data type
	enum class TextureType
	{
		Type_2D,
		Type_2DArray,
		Type_3D,
		Type_Cube
	};

	//Texture shape/dimension
	enum class TextureFormat : u8
	{
		Format_None = 0,

		Format_Auto, //Auto-assign value (not recommended, can limit usefulness)

		//standard UNORM formats

		Format_R8,
		Format_RG8,
		Format_RGB8,
		Format_RGBA8,
		Format_SRGB8,
		Format_SRGB8A8,

		//float formats

		Format_R16F,
		Format_RG16F,
		Format_RGBA16F,

		Format_R32F,
		Format_RG32F,
		Format_RGBA32F,

		//depth formats

		Format_Depth24,
		Format_Depth32F,
		Format_Depth24Stencil8,
		Format_Depth32FStencil8,

		//compressed formats

		Format_BC1, //DXT1 - RGB, no alpha
		Format_BC3, //DXT5 - RGBA
		Format_BC4, //Single channel (R)
		Format_BC5, //Two channel (RG)

		//High quality RGBA,
		//Requires 'GL_ARB_texture_compression_bptc' extension on OpenGL
		Format_BC7  
	};

	//Texture use case
	enum class TextureUsage : u8
	{
		Usage_None = 0,

		Usage_Sampled,      //Shader sampling
		Usage_Storage,      //ImageStore/ImageLoad (Vulkan only)
		Usage_RenderTarget, //Color attachment
		Usage_DepthStencil, //Depth attachment
		Usage_TransferSrc,  //Copy source
		Usage_TransferDst   //Copy destination
	};

	enum class TextureResizeType
	{
		//Use for icons, UI textures.
		//Uses Mitchell filter when shrinking, cubing when enlarging.
		RESIZE_SRGB,

		//Use for normalmaps, heightmaps, scientific data.
		//Should not be used for icons or UI textures.
		//Assumes input is in linear color space (already gamma corrected)
		RESIZE_LINEAR,

		//Use for resizing HDR textures or floating-point buffers.
		//Same as linear but for float* pixel buffers.
		//Texture pixels are internally converted to float and back to u8 when successfully rescaled.
		RESIZE_LINEAR_FLOAT
	};

	class LIB_API Texture
	{
	public:
		//Rescale an imported texture with the chosen algorithm type
		virtual void Rescale(
			vec2 newSize,
			TextureResizeType type = TextureResizeType::RESIZE_SRGB) = 0;

		//Rebinds the texture
		virtual void HotReload() = 0;

		//Calculates maximum reasonable mipmap levels for this texture based on texture size
		//to prevent wasted VRAM and to avoid GL_INVALID_VALUE error
		static u8 GetMaxMipMapLevels(vec2 size, u16 depth, u8 mipmapLevels);

		const string& GetName() const { return name; }
		void SetName(const string& newName)
		{
			if (newName.empty()
				|| newName.size() > 100)
			{
				Log::Print(
					"Texture name is empty or too big! Must be between 1 and 100 characters.",
					"TEXTURE",
					LogType::LOG_ERROR,
					2);

				return;
			}
			name = newName;
		}

		const string& GetPath() const { return path; }

		u32 GetID() const { return ID; }

		vec2 GetSize() const { return size; }
		u16 GetDepth() const { return depth; }
		u8 GetMipMapLevels() const { return mipMapLevels; }

		void SetPixels(const vector<u8>& newPixels) { pixels = newPixels; }
		const vector<u8>& GetPixels() const { return pixels; }

		void SetCubePixels(const vector<vector<u8>>& newCubePixels) 
		{ 
			if (newCubePixels.size() != 6) return;

			cubePixels = newCubePixels;
		}
		const vector<vector<u8>>& GetCubePixels() const { return cubePixels; }

		u32 GetTexelCount() const
		{
			return static_cast<u32>(size.x)
				* static_cast<u32>(size.y)
				* static_cast<u32>(depth);
		}

		TextureType GetType() const { return type; }
		TextureFormat GetFormat() const { return format; }
		TextureUsage GetUsage() const { return usage; }

		//Do not destroy manually, erase from containers.hpp instead
		virtual ~Texture() {};
	protected:
		string name{};
		string path{};
		u32 ID{};

		vec2 size{};
		u16 depth = 1;
		u8 mipMapLevels = 1;
		vector<u8> pixels{};
		vector<vector<u8>> cubePixels{};

		TextureType type{};
		TextureFormat format{};
		TextureUsage usage{};
	};
}