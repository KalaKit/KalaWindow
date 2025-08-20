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
		static void InitOpenGLHandle();

		static uintptr_t GetOpenGLHandle()
		{
			if (openGL32 == NULL) InitOpenGLHandle();

			return openGL32;
		}
	private:
		static inline uintptr_t openGL32{};
	};
}