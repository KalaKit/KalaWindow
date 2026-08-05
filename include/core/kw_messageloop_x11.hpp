//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef __linux__

#pragma once

#include <X11/Xlib.h>

#include <functional>

#include "core_utils.hpp"

namespace KalaWindow::Graphics
{
    class ProcessWindow;
}

namespace KalaWindow::Core
{
    using std::function;

    using u32 = uint32_t;

    class LIB_API MessageLoop
    {
    friend class KalaWindow::Graphics::ProcessWindow;
    public:
        static void SetAddCharCallback(function<void(u32)>&& newCallback);
		static void SetRemoveFromBackCallback(function<void()>&& newCallback);
		static void SetAddTabCallback(function<void()>&& newCallback);
		static void SetAddNewLineCallback(function<void()>&& newCallback);
    private:
        static void Update();
    };
}

#endif //__linux__