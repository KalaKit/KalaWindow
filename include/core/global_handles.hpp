//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include "KalaHeaders/core_utils.hpp"

namespace KalaWindow::Core
{
	class LIB_API GlobalHandle
	{
	public:
#ifdef _WIN32
		//Set the opengl handle
		static void SetOpenGLHandle();
		static inline uintptr_t GetOpenGLHandle()
		{
			if (openGL32Lib == NULL) SetOpenGLHandle();

			return openGL32Lib;
		}

		//Set the opengl master context for Windows (HGLRC)
		static inline void SetOpenGLWinContext(uintptr_t newValue) { hglrc = newValue; }
		static inline uintptr_t GetOpenGLWinContext() { return hglrc; }

		//Set the vulkan handle
		static void SetVulkanHandle() {};
		static inline uintptr_t GetVulkanHandle()
		{
			if (vulkanLib == NULL) SetOpenGLHandle();

			return vulkanLib;
		}

	private:
		static inline uintptr_t openGL32Lib{};
		static inline uintptr_t hglrc{};

		static inline uintptr_t vulkanLib{};
#elif __linux__
		//Set the opengl master context for Linux
		static void SetOpenGLLinuxContext(uintptr_t newValue) { glxContext = newValue; }
		static uintptr_t GetOpenGLLinuxContext() { return glxContext; }
	private:
		static inline uintptr_t glxContext{};
#endif
	};
}