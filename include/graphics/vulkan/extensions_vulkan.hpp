//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef KALAWINDOW_SUPPORT_VULKAN

#include "graphics/window.hpp"

namespace KalaWindow::Graphics
{
    using KalaWindow::Graphics::Window;

    class Extensions_Vulkan
    {
    public:
        //Attach Vulkan to window
        static void CreateVulkanSurface(Window* window);

        static bool CreateSwapchain(Window* window);
        static void DestroySwapchain(Window* window);
    };
}

#endif //KALAWINDOW_SUPPORT_VULKAN