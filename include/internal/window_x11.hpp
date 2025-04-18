//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef KALAKIT_X11

//kalakit
#include "window.hpp"

namespace KalaKit
{
  class Window_X11
  {
  public:
		static inline Display* newDisplay = nullptr;
		static inline Window newWindow = nullptr;
    staticc inline GC newGC = nullptr;
  }
}

#endif // KALAKIT_WINDOWS