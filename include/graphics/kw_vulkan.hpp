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

namespace KalaWindow::Graphics
{
    using std::string;
    using std::vector;
    using std::default_delete;

    using u32 = uint32_t;

    using KalaWindow::Core::KalaWindowRegistry;

    class LIB_API VulkanContext
	{
    friend class ProcessWindow;
	friend struct default_delete<VulkanContext>;
	public:
        static KalaWindowRegistry<VulkanContext>& GetRegistry();

		static bool IsVerboseLoggingEnabled();
    	//Toggle verbose logging. If true and in Debug, then Vulkan will add its own debug messages to the console log messages.
		static void SetVerboseLoggingState(bool newState);

        //Global one-time Vulkan 1.4 instance init,
        //needs to be called before per-window Vulkan init.
        //Add app name what to call this Vulkan global context and crash Handler.
        //Add optional features via extensions list.
        //Automatically added extensions required for core operation:
        //- VK_KHR_surface
        //- VK_KHR_win32_surface (on windows)
        //- VK_KHR_xlib_surface (on linux)
		static void Initialize(
            string&& appName,
            vector<string>&& extensions = {});
		static bool IsInitialized();

        static VkInstance GetInstance();
        
		u32 GetID() const;
		u32 GetWindowID() const;

        VkSurfaceKHR GetSurface() const;

        void Destroy();
	private:
        ~VulkanContext();

        //Initialize a per-window Vulkan context, creates a surface
		static VulkanContext* InitializeInstance(u32 windowID);

		u32 ID{};
		u32 windowID{};

        VkSurfaceKHR surface{};
    };
}