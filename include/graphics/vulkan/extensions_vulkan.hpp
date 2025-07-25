//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include "core/platform.hpp"
#include "graphics/window.hpp"

namespace KalaWindow::Graphics::Vulkan
{
    using KalaWindow::Graphics::Window;

    class KALAWINDOW_API Extensions_Vulkan
    {
    public:
        //Attach Vulkan to window
        static void CreateVulkanSurface(Window* window);

        static bool CreateSwapchain(Window* window);
        static void DestroySwapchain(Window* window);
    };
}