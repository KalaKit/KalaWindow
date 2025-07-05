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
        V_IL_api_dump,
        V_IL_crash_diagnostic,
        V_IL_gfxreconstruct,
        V_IL_monitor,
        V_IL_profiles,
        V_IL_screenshot,
        V_IL_shader_object,
        V_IL_synchronization2,
        V_IL_validation
	};

	static const unordered_map<VulkanLayers, const char*> vulkanLayerInfo =
	{
        { VulkanLayers::V_IL_api_dump,           "V_IL_LUNARG_api_dump" },
        { VulkanLayers::V_IL_crash_diagnostic,   "V_IL_LUNARG_crash_diagnostic" },
        { VulkanLayers::V_IL_gfxreconstruct,     "V_IL_LUNARG_gfxreconstruct" },
        { VulkanLayers::V_IL_monitor,            "V_IL_LUNARG_monitor" },
        { VulkanLayers::V_IL_profiles,           "V_IL_KHRONOS_profiles" },
        { VulkanLayers::V_IL_screenshot,         "V_IL_LUNARG_screenshot" },
        { VulkanLayers::V_IL_shader_object,      "V_IL_KHRONOS_shader_object" },
        { VulkanLayers::V_IL_synchronization2,   "V_IL_KHRONOS_synchronization2" },
        { VulkanLayers::V_IL_validation,         "V_IL_KHRONOS_validation" }
	};
}

#endif //KALAWINDOW_SUPPORT_VULKAN