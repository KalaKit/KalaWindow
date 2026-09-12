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
		//Does not give Backspace, Tab, Return or NewLine,
		//returns any other single key or shift/alt-affected key
        static u32 GetPressedChar();
		//Returns true if backspace key was pressed this frame
		static bool GetBackspaceState();
		//Returns true if tab key was pressed this frame
		static bool GetTabState();
		//Returns true if return key was pressed this frame
		static bool GetReturnState();
	private:
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

