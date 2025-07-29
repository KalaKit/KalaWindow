//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>

#include "core/platform.hpp"
#include "core/log.hpp"

namespace KalaWindow::Graphics
{
	using KalaWindow::Core::Logger;
	using KalaWindow::Core::LogType;

	using std::string;

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

		//standard UNORM formats

		Format_R8,
		Format_RG8,
		Format_RGB8,
		Format_RGBA8,

		//float formats

		Format_R16F,
		Format_RG16F,
		Format_RGBA16F,

		Format_R32F,
		Format_RG32F,
		Format_RGBA32F,

		//depth formats

		Format_Depth16,
		Format_Depth24,
		Format_Depth32F,
		Format_Depth24Stencil8,
		Format_Depth32Stencil8, //Vulkan only

		//compressed formats

		Format_BC1, //DXT1 - RGB, no alpha
		Format_BC3, //DXT5 - RGBA
		Format_BC4, //Single channel (R)
		Format_BC5, //Two channel (RG)

		//High quality RGBA,
		//Vulkan only or with 'GL_ARB_texture_compression_bptc' extension on OpenGL
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

	class KALAWINDOW_API Texture
	{
	public:
		//Rebinds the texture
		virtual void HotReload() = 0;

		const string& GetName() const { return name; }
		void SetName(const string& newName)
		{
			if (newName.empty()
				|| newName.size() > 100)
			{
				Logger::Print(
					"Texture name is empty or too big! Must be between 1 and 100 characters.",
					"TEXTURE",
					LogType::LOG_ERROR,
					2);

				return;
			}
			name = newName;
		}

		const string& GetPath() const { return path; }
		void SetPath(const string& newPath)
		{
			if (newPath.empty()
				|| newPath.size() > 1000)
			{
				Logger::Print(
					"Texture path is empty or too big! Must be between 1 and 1000 characters.",
					"TEXTURE",
					LogType::LOG_ERROR,
					2);

				return;
			}
			path = newPath;
		}

		u32 GetID() const { return ID; }
		void SetID(u32 newID) { ID = newID; }

		vec2 GetSize() const { return size; }
		u16 GetDepth() const { return depth; }
		u8 GetMipMapLevels() const { return mipMapLevels; }

		u32 GetTexelCount() const
		{
			return static_cast<u32>(size.x)
				* static_cast<u32>(size.y)
				* static_cast<u32>(depth);
		}

		TextureType GetType() const { return type; }
		TextureFormat GetFormat() const { return format; }
		TextureUsage GetUsage() const { return usage; }

		virtual ~Texture() {};
	private:
		string name{};
		string path{};
		u32 ID{};

		vec2 size{};
		u16 depth = 1;
		u8 mipMapLevels = 1;

		TextureType type{};
		TextureFormat format{};
		TextureUsage usage{};
	};
}