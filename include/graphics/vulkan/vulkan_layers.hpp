//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef KALAWINDOW_SUPPORT_VULKAN

#pragma once

#include <unordered_map>

namespace KalaWindow::Graphics
{
	using std::unordered_map;

	enum class VulkanLayers
	{
        // MAIN LAYER

        V_IL_khronos_validation,

        // SUB-LAYERS

        V_IL_khronos_profiles,
        V_IL_khronos_shader_object,
        V_IL_khronos_synchronization2,

        V_IL_lunarg_api_dump,
        V_IL_lunarg_crash_diagnostic,
        V_IL_lunarg_gfxreconstruct,
        V_IL_lunarg_monitor,
        V_IL_lunarg_screenshot
	};

	static const unordered_map<VulkanLayers, const char*> vulkanLayerInfo =
	{
        { VulkanLayers::V_IL_khronos_validation,       "VK_LAYER_KHRONOS_validation" },

        { VulkanLayers::V_IL_khronos_profiles,         "VK_LAYER_KHRONOS_profiles" },
        { VulkanLayers::V_IL_khronos_shader_object,    "VK_LAYER_KHRONOS_shader_object" },
        { VulkanLayers::V_IL_khronos_synchronization2, "VK_LAYER_KHRONOS_synchronization2" },

        { VulkanLayers::V_IL_lunarg_api_dump,          "VK_LAYER_LUNARG_api_dump" },
        { VulkanLayers::V_IL_lunarg_crash_diagnostic,  "VK_LAYER_LUNARG_crash_diagnostic" },
        { VulkanLayers::V_IL_lunarg_gfxreconstruct,    "VK_LAYER_LUNARG_gfxreconstruct" },
        { VulkanLayers::V_IL_lunarg_monitor,           "VK_LAYER_LUNARG_monitor" },
        { VulkanLayers::V_IL_lunarg_screenshot,        "VK_LAYER_LUNARG_screenshot" },
	};
}

#endif //KALAWINDOW_SUPPORT_VULKAN