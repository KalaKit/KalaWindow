//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef _WIN32

#pragma once

#include <windows.h>
#include <functional>

#include "core_utils.hpp"

namespace KalaWindow::Graphics
{
	class ProcessWindow;
	class Window_Global;
}

namespace KalaWindow::Core
{
	using std::function;

	using u32 = uint32_t;

	class LIB_API MessageLoop
	{
	friend class KalaWindow::Graphics::ProcessWindow;
	friend class KalaWindow::Graphics::Window_Global;
	public:
        static void SetAddCharCallback(function<void(u32)>&& newCallback);
		static void SetRemoveFromBackCallback(function<void()>&& newCallback);
		static void SetAddTabCallback(function<void()>&& newCallback);
		static void SetAddNewLineCallback(function<void()>&& newCallback);
	private:
		static LRESULT CALLBACK WindowProcCallback(
			HWND hwnd,
			UINT msg,
			WPARAM wParam,
			LPARAM lParam);
	};
}

#endif //_WIN32