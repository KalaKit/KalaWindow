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
		static inline Display* newDisplay;
		static inline Window newWindow;
    static inline GC newGC;

    static inline int width;
    static inline int height;
  };
}

#endif // KALAKIT_WINDOWS