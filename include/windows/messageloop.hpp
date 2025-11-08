//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef _WIN32

#pragma once

#include <Windows.h>

#include "KalaHeaders/core_utils.hpp"

#include "ui/text.hpp"

namespace KalaWindow::Windows
{
	using KalaWindow::UI::Text;
	
	class LIB_API MessageLoop
	{
	public:
		static LRESULT CALLBACK WindowProcCallback(
			HWND hwnd,
			UINT msg,
			WPARAM wParam,
			LPARAM lParam);
			
		//The text widget that is currently active and focused
		static inline Text* activeText{};
	};
}

#endif //_WIN32