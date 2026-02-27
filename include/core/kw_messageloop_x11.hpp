//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#if defined(__linux__) && defined(KW_USE_X11)

#pragma once

#include "KalaHeaders/core_utils.hpp"

namespace KalaWindow::Core
{
    class LIB_API MessageLoop
    {
    public:
        //Updates the global X11 event loop once per call
        static void Update();
    };
}

#endif //__linux__ && KW_USE_X11