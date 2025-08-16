//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include "core/platform.hpp"
#include "graphics/texture.hpp"

namespace KalaWindow::Graphics::OpenGL
{
	class KALAWINDOW_API Texture_OpenGL : public Texture
	{
	public:
		static Texture_OpenGL* LoadTexture(
			const string& name,
			const string& path,
			TextureType type,
			TextureFormat format,
			TextureUsage usage,
			vec2 size = vec2(0),
			u16 depth = 1,
			u8 mipMapLevels = 1);

		virtual void HotReload() override;

		u32 GetOpenGLID() const { return openGLID; }
		void SetOpenGLID(u32 newID) { openGLID = newID; }

		//Note: Do not destroy manually, erase from containers.hpp instead
		~Texture_OpenGL() override;
	private:
		u32 openGLID{};
	};
}