//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <vector>

#include "KalaHeaders/core_utils.hpp"
#include "KalaHeaders/log_utils.hpp"

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

		//compressed formats
		//BC1-BC7 support will not be added to OpenGL on KalaWindow

		Format_BC1, //DXT1 - RGB, no alpha
		Format_BC3, //DXT5 - RGBA
		Format_BC4, //Single channel (R)
		Format_BC5, //Two channel (RG)

		//High quality RGBA,
		Format_BC7  
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

		inline const string& GetName() const { return name; }
		inline void SetName(const string& newName)
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

		inline const string& GetPath() const { return filePath; }

		inline u32 GetID() const { return ID; }

		inline vec2 GetSize() const { return size; }
		inline u16 GetDepth() const { return depth; }
		inline u8 GetMipMapLevels() const { return mipMapLevels; }

		inline void SetPixels(const vector<u8>& newPixels) { pixels = newPixels; }
		inline const vector<u8>& GetPixels() const { return pixels; }

		inline void SetCubePixels(const vector<vector<u8>>& newCubePixels)
		{ 
			if (newCubePixels.size() != 6) return;

			cubePixels = newCubePixels;
		}
		inline const vector<vector<u8>>& GetCubePixels() const { return cubePixels; }

		inline void SetLayerPixels(const vector<vector<u8>>& newLayerPixels)
		{
			layerPixels = newLayerPixels;
		}
		inline const vector<vector<u8>>& GetLayerPixels() const { return layerPixels; }

		inline u32 GetTexelCount() const
		{
			return static_cast<u32>(size.x)
				* static_cast<u32>(size.y)
				* static_cast<u32>(depth);
		}

		inline TextureType GetType() const { return type; }
		inline TextureFormat GetFormat() const { return format; }

		//Do not destroy manually, erase from containers.hpp instead
		virtual ~Texture() {};
	protected:
		string name{};
		string filePath{};
		u32 ID{};

		vec2 size{};
		u16 depth = 1;
		u8 mipMapLevels = 1;
		vector<u8> pixels{};              //2D/3D
		vector<vector<u8>> cubePixels{};  //Cubemap
		vector<vector<u8>> layerPixels{}; //2D array

		TextureType type{};
		TextureFormat format{};
	};
}