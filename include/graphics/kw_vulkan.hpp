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

    using KalaWindow::Core::KalaWindowRegistry;

    enum class LIB_API VulkanVersion : u8
    {
        V_1_0 = 0,
        V_1_1 = 1,
        V_1_2 = 2,
        V_1_3 = 3,
        V_1_4 = 4
    };

    class LIB_API VulkanContext
	{
    friend class ProcessWindow;
	friend struct default_delete<VulkanContext>;
	public:
        KNODISCARD
		static KalaWindowRegistry<VulkanContext>& GetRegistry();

		KNODISCARD
		static bool IsVerboseLoggingEnabled();
    	//Toggle verbose logging. If true and in Debug, then Vulkan will add its own debug messages to the console log messages.
		static void SetVerboseLoggingState(bool newState);

        //Get currently assigned Vulkan version
        KNODISCARD
		static VulkanVersion GetVulkanVersion();
        //Assign a Vulkan version, defaults to 1.4,
        //cannot be called after ProcessWindow has been initialized once or more
        static void SetVulkanVersion(VulkanVersion newVersion);

        //Get all user-provided and default Vulkan extensions
        KNODISCARD
		static const vector<string>& GetExtensions();
        //Add optional features via extensions list,
        //cannot be called after ProcessWindow has been initialized once or more.
        //Automatically added extensions required for core operation:
        //- VK_KHR_surface
        //- VK_KHR_win32_surface (on windows)
        //- VK_KHR_xlib_surface (on linux)
        //- VK_EXT_debug_utils (in debug mode)
        static void AddExtensions(vector<string>&& extensions);

        KNODISCARD
		static VkInstance GetInstance();
        
		KNODISCARD
		u32 GetID() const;
		KNODISCARD
		u32 GetWindowID() const;

        KNODISCARD
		VkSurfaceKHR GetSurface() const;

        void Destroy();
	private:
        ~VulkanContext();

        static void Initialize();
		KNODISCARD
		static bool IsInitialized();

        //Initialize a per-window Vulkan context, creates a surface
		KNODISCARD
		static VulkanContext* InitializeInstance(u32 windowID);

		u32 ID{};
		u32 windowID{};

        VkSurfaceKHR surface{};
    };
}