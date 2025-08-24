//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <array>

#include "KalaHeaders/api.hpp"
#include "KalaHeaders/core_types.hpp"

#include "core/glm_global.hpp"
#include "graphics/texture.hpp"

namespace KalaWindow::Graphics::OpenGL
{
	using std::array;

	class LIB_API OpenGL_Texture : public Texture
	{
	public:
		//Load a new texture from an external file.
		//Depth is always clamped to 1 for Type_2D and Type_Cube.
		//Depth is a power of 4 for Type_2DArray and Type_3D
		//and is clamped internally from 256 to 8192.
		//Mipmap levels are clamped internally through Texture::GetMaxMipMapLevels.
		//Returns a fallback texture if loading fails.
		static OpenGL_Texture* LoadTexture(
			const string& name,
			const string& path,
			TextureType type,
			TextureFormat format,
			TextureUsage usage,
			bool flipVertically = false,
			u16 depth = 1,
			u8 mipMapLevels = 1);


		//Uses the data from six existing textures to create a new cubemap texture
		//Mipmap levels are clamped internally through Texture::GetMaxMipMapLevels.
		//Returns a fallback texture if loading fails.
		static OpenGL_Texture* CreateCubeMapTexture(
			const string& name,
			const array<OpenGL_Texture*, 6>& textures,
			u8 mipMapLevels = 1);

		//Returns the fallback texture,
		//used when a texture fails to load through OpenGL_Texture::LoadTexture
		static OpenGL_Texture* GetFallbackTexture();

		virtual void Rescale(
			vec2 newSize,
			TextureResizeType type = TextureResizeType::RESIZE_SRGB) override;

		virtual void HotReload() override;

		u32 GetOpenGLID() const { return openGLID; }

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