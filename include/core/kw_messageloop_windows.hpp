//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef _WIN32

#pragma once

#include <Windows.h>
#include <functional>

#include "KalaHeaders/core_utils.hpp"

namespace KalaWindow::Core
{
	using std::function;

	class LIB_API MessageLoop
	{
	public:
		static LRESULT CALLBACK WindowProcCallback(
			HWND hwnd,
			UINT msg,
			WPARAM wParam,
			LPARAM lParam);

		static void SetAddCharCallback(const function<void(u32)>& newCallback);
		static void SetRemoveFromBackCallback(const function<void()>& newCallback);
		static void SetAddTabCallback(const function<void()>& newCallback);
		static void SetAddNewLineCallback(const function<void()>& newCallback);
	};
}

#endif //_WIN32