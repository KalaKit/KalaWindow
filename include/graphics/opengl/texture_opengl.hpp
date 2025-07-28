//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include "graphics/texture.hpp"

namespace KalaWindow::Graphics::OpenGL
{
	class Texture_OpenGL : public Texture
	{
	public:
		virtual Texture* LoadTexture(
			const string& name,
			const string& path,
			TextureType type,
			TextureFormat format,
			TextureUsage usage,
			vec2 size = vec2(0),
			u16 depth = 1,
			u8 mipMapLevels = 1) override {};

		virtual void HotReload() override {};

		u32 GetID() const { return id; }

		~Texture_OpenGL() override {};
	private:
		u32 id = 0;
	};
}