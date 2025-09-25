//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <array>

#include "KalaHeaders/core_utils.hpp"

#include "core/glm_global.hpp"
#include "graphics/texture.hpp"

namespace KalaWindow::Graphics::OpenGL
{
	using std::array;

	class LIB_API OpenGL_Texture : public Texture
	{
	public:
		//Load a new texture from an external file.
		//Depth is always clamped to 1 for Type_2D,
		//it is a power of 4 for Type_3D and is clamped internally from 256 to 8192.
		//Mipmap levels are clamped internally through Texture::GetMaxMipMapLevels.
		//Returns a fallback texture if loading fails.
		//Only supports Type_2D and Type_3D.
		static OpenGL_Texture* LoadTexture(
			const string& name,
			const string& path,
			TextureType type,
			TextureFormat format,
			bool flipVertically = false,
			u16 depth = 1,
			u8 mipMapLevels = 1);

		//Uses the data from six provided texture files to create a new cubemap texture.
		//Mipmap levels are clamped internally through Texture::GetMaxMipMapLevels.
		//Returns a fallback texture if loading fails. Depth is always 6 for cube map textures.
		//Size for each texture must match and be 1:1 aspect ratio.
		//Only supports Type_Cube
		static OpenGL_Texture* LoadCubeMapTexture(
			const string& name,
			const array<string, 6>& texturePaths,
			TextureFormat format,
			bool flipVertically = false,
			u8 mipMapLevels = 1);

		//Uses the data from all the provided texture files to create a new 2D array texture.
		//Mipmap levels are clamped internally through Texture::GetMaxMipMapLevels.
		//Returns a fallback texture if loading fails. Depth is always size of texturePaths vector.
		//Size for each texture must match and be 1:1 aspect ratio.
		//Only supports Type_2DArray
		static OpenGL_Texture* Load2DArrayTexture(
			const string& name,
			const vector<string>& texturePaths,
			TextureFormat format,
			bool flipVertically = false,
			u8 mipMapLevels = 1);

		//Returns the fallback texture,
		//used when a texture fails to load through OpenGL_Texture::LoadTexture
		static OpenGL_Texture* GetFallbackTexture();

		virtual void Rescale(
			vec2 newSize,
			TextureResizeType type = TextureResizeType::RESIZE_SRGB) override;

		virtual void HotReload() override;

		inline u32 GetOpenGLID() const { return openGLID; }

		//Do not destroy manually, erase from containers.hpp instead
		~OpenGL_Texture() override;
	private:
		//Called once internally when any texture is created,
		//used when a texture fails to load through OpenGL_Texture::LoadTexture.
		//Do not call manually, this has no effect for regular texture functionality.
		static void LoadFallbackTexture();

		u32 openGLID{};
	};
}