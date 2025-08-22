//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include "KalaHeaders/api.hpp"

namespace KalaWindow::Core
{
	class LIB_API GlobalHandle
	{
	public:
		//Set the opengl handle
		static void SetOpenGLHandle();
		static uintptr_t GetOpenGLHandle()
		{
			if (openGL32Lib == NULL) SetOpenGLHandle();

			return openGL32Lib;
		}

		//Set the vulkan handle
		static void SetVulkanHandle() {};
		static uintptr_t GetVulkanHandle()
		{
			if (vulkanLib == NULL) SetOpenGLHandle();

			return vulkanLib;
		}
	private:
		static inline uintptr_t openGL32Lib{};
		static inline uintptr_t vulkanLib{};
	};
}