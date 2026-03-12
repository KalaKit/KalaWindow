//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef __linux__

#pragma once

#include <X11/Xlib.h>

#include <functional>

#include "core_utils.hpp"

namespace KalaWindow::Core
{
    using std::function;

    using u32 = uint32_t;

    class LIB_API MessageLoop
    {
    public:
        //Updates the global X11 event loop once per call
        static void Update();

        static void SetAddCharCallback(const function<void(u32)>& newCallback);
		static void SetRemoveFromBackCallback(const function<void()>& newCallback);
		static void SetAddTabCallback(const function<void()>& newCallback);
		static void SetAddNewLineCallback(const function<void()>& newCallback);
    private:
        static void DispatchEvents(XEvent& event);
    };
}

#endif //__linux__