//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef _WIN32

#pragma once

#include <windows.h>
#include <functional>

#include "core_utils.hpp"

namespace KalaWindow::Core
{
	using std::function;

	using u32 = uint32_t;

	class LIB_API MessageLoop
	{
	public:
		static LRESULT CALLBACK WindowProcCallback(
			HWND hwnd,
			UINT msg,
			WPARAM wParam,
			LPARAM lParam);

        static void SetAddCharCallback(function<void(u32)>&& newCallback);
		static void SetRemoveFromBackCallback(function<void()>&& newCallback);
		static void SetAddTabCallback(function<void()>&& newCallback);
		static void SetAddNewLineCallback(function<void()>&& newCallback);
	};
}

#endif //_WIN32