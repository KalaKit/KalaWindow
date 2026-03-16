//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <vector>

#include "core_utils.hpp"

#include "core/kw_registry.hpp"

struct VkInstance_T;
using VkInstance = VkInstance_T*;

struct VkSurfaceKHR_T;
using VkSurfaceKHR = VkSurfaceKHR_T*;

namespace KalaWindow::Vulkan
{
    using std::string;
    using std::vector;

    using u32 = uint32_t;

    using KalaWindow::Core::KalaWindowRegistry;

    class Vulkan_Context;

    class LIB_API Vulkan_Global
	{
	public:
    	//Toggle verbose logging. If true, then usually frequently updated runtime values like
		//VK notifications will dump their logs into the console.
		static void SetVerboseLoggingState(bool newState);
		static bool IsVerboseLoggingEnabled();
        
        //Global one-time Vulkan 1.3 init, needs to be called before per-window Vulkan init.
        //Automatically added:
        //- VK_KHR_surface
        //- VK_KHR_win32_surface (on windows)
        //- VK_KHR_xlib_surface (on linux)
		static void Initialize(const vector<string>& extensions);
		static bool IsInitialized();

        static VkInstance GetInstance();

        //Shut down Vulkan instance and all surfaces
        static void Shutdown();
    };

    class LIB_API Vulkan_Context
	{
	public:
        static KalaWindowRegistry<Vulkan_Context>& GetRegistry();

        //Initialize a per-window Vulkan context
		static Vulkan_Context* Initialize(u32 windowID);
		bool IsInitialized() const;

        VkSurfaceKHR GetSurface() const;

		u32 GetID() const;
		u32 GetWindowID() const;

    	//Do not destroy manually, erase from registry instead
		~Vulkan_Context();
	private:
		bool isInitialized{};

		u32 ID{};
		u32 windowID{};

        VkSurfaceKHR surface{};
    };
}