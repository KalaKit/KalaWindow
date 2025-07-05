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
        VK_LAYER_api_dump,
        VK_LAYER_crash_diagnostic,
        VK_LAYER_gfxreconstruct,
        VK_LAYER_monitor,
        VK_LAYER_profiles,
        VK_LAYER_screenshot,
        VK_LAYER_shader_object,
        VK_LAYER_synchronization2,
        VK_LAYER_validation
	};

	static const unordered_map<VulkanLayers, const char*> vulkanLayerInfo =
	{
        { VulkanLayers::VK_LAYER_api_dump,           "VK_LAYER_LUNARG_api_dump" },
        { VulkanLayers::VK_LAYER_crash_diagnostic,   "VK_LAYER_LUNARG_crash_diagnostic" },
        { VulkanLayers::VK_LAYER_gfxreconstruct,     "VK_LAYER_LUNARG_gfxreconstruct" },
        { VulkanLayers::VK_LAYER_monitor,            "VK_LAYER_LUNARG_monitor" },
        { VulkanLayers::VK_LAYER_profiles,           "VK_LAYER_KHRONOS_profiles" },
        { VulkanLayers::VK_LAYER_screenshot,         "VK_LAYER_LUNARG_screenshot" },
        { VulkanLayers::VK_LAYER_shader_object,      "VK_LAYER_KHRONOS_shader_object" },
        { VulkanLayers::VK_LAYER_synchronization2,   "VK_LAYER_KHRONOS_synchronization2" },
        { VulkanLayers::VK_LAYER_validation,         "VK_LAYER_KHRONOS_validation" }
	};
}

#endif //KALAWINDOW_SUPPORT_VULKAN