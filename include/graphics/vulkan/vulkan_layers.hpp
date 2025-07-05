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
        VL_api_dump,
        VL_crash_diagnostic,
        VL_gfxreconstruct,
        VL_monitor,
        VL_profiles,
        VL_screenshot,
        VL_shader_object,
        VL_synchronization2,
        VL_validation
	};

	static const unordered_map<VulkanLayers, const char*> vulkanLayerInfo =
	{
        { VulkanLayers::VL_api_dump,           "VK_LAYER_LUNARG_api_dump" },
        { VulkanLayers::VL_crash_diagnostic,   "VK_LAYER_LUNARG_crash_diagnostic" },
        { VulkanLayers::VL_gfxreconstruct,     "VK_LAYER_LUNARG_gfxreconstruct" },
        { VulkanLayers::VL_monitor,            "VK_LAYER_LUNARG_monitor" },
        { VulkanLayers::VL_profiles,           "VK_LAYER_KHRONOS_profiles" },
        { VulkanLayers::VL_screenshot,         "VK_LAYER_LUNARG_screenshot" },
        { VulkanLayers::VL_shader_object,      "VK_LAYER_KHRONOS_shader_object" },
        { VulkanLayers::VL_synchronization2,   "VK_LAYER_KHRONOS_synchronization2" },
        { VulkanLayers::VL_validation,         "VK_LAYER_KHRONOS_validation" },
	};
}

#endif //KALAWINDOW_SUPPORT_VULKAN