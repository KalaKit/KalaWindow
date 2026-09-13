//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include "core_utils.hpp"

#pragma once

#if defined(KWIN_ANY)
struct HWND__;
using HWND = HWND__*;

using UINT = unsigned int;
using WPARAM = uintptr_t;
using LPARAM = intptr_t;
using LRESULT = intptr_t;

#ifndef CALLBACK
#define CALLBACK __stdcall
#endif
#endif
namespace KalaWindow::Graphics
{
	class ProcessWindow;
	class Window_Global;
}

namespace KalaWindow::Core
{
	class LIB_API MessageLoop
	{
	friend class KalaWindow::Graphics::ProcessWindow;
	friend class KalaWindow::Graphics::Window_Global;
	public:
		//Returns key after being altered by modifier keys like shift or alt,
		//does not return backspace, tab or arrow keys, use their getters instead
        static u32 GetModifierChar();

		static bool GetBackspaceState();
		static bool GetTabState();

		static bool GetLeftArrowState();
		static bool GetRightArrowState();
		static bool GetUpArrowState();
		static bool GetDownArrowState();
	private:
		static void ClearKeys();

#if defined(KWIN_ANY)
		static LRESULT CALLBACK WindowProcCallback(
			HWND hwnd,
			UINT msg,
			WPARAM wParam,
			LPARAM lParam);
#else
		static void Update();
#endif
	};
}

