//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

/*
#include "KalaHeaders/core_utils.hpp"

#include "core/glm_global.hpp"
#include "graphics/texture.hpp"

namespace KalaWindow::Graphics::Vulkan
{
	class LIB_API Texture_Vulkan : public Texture
	{
	public:
		static Texture_Vulkan* LoadTexture(
			const string& name,
			const string& path,
			TextureType type,
			TextureFormat format,
			TextureUsage usage,
			vec2 size = vec2(0),
			u16 depth = 1,
			u8 mipMapLevels = 1);

		virtual void HotReload() override;

		uintptr_t GetImage() const { return image; }
		uintptr_t Getview() const { return view; }
		uintptr_t GetSampler() const { return sampler; }

		//Do not destroy manually, erase from containers.hpp instead
		~Texture_Vulkan() override;
	private:
		uintptr_t image{};   //VkImage
		uintptr_t view{};    //VkImageView
		uintptr_t sampler{}; //VkSampler
	};
}
*/