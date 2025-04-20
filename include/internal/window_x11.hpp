//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef KALAKIT_X11

#include <X11/Xlib.h>

//kalakit
#include "window.hpp"

namespace KalaKit
{
  class Window_X11
  {
  public:
    static inline int width;
    static inline int height;
  
	static inline Display* newDisplay = nullptr;
	static inline Window newWindow = 0;
    static inline GC newGC = nullptr;
    static inline GLXContext glxContext = nullptr;
    static inline GLXFBConfig glxFBConfig = nullptr;
    static inline XVisualInfo* visualInfo = nullptr;

    static Window CreateWindow(
      Display* newDisplay,
      int width,
      int height,
      Window root, 
      XSetWindowAttributes swa);
  };
}

#endif // KALAKIT_X11