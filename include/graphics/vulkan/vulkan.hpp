//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef KALAWINDOW_SUPPORT_VULKAN

#pragma once

#include <vector>

#include "core/platform.hpp"
#include "graphics/window.hpp"

namespace KalaWindow::Graphics
{
	using std::vector;

	enum class FrameResult
	{
		VK_FRAME_OK,            //Vulkan frame is fine, proceed with rendering
		VK_FRAME_RESIZE_NEEDED, //Swapchain is out of date or suboptimal - recreate needed
		VK_FRAME_ERROR          //Unexpected error - recreate needed
	};

	enum class VulkanLayers
	{
		// --- Meta-Layer ---
		
		VL_KhronosValidation,        // VK_LAYER_KHRONOS_validation
		
		// --- Sub-layers ---
		
		VL_LunargThreading,          // VK_LAYER_LUNARG_threading
		VL_LunargParamValidation,    // VK_LAYER_LUNARG_parameter_validation
		VL_LunargObjectTracker,      // VK_LAYER_LUNARG_object_tracker
		VL_LunargCoreValidation,     // VK_LAYER_LUNARG_core_validation
		VL_LunargSwapchain,          // VK_LAYER_LUNARG_swapchain
		VL_LunargImage,              // VK_LAYER_LUNARG_image
		VL_LunargApiDump             // VK_LAYER_LUNARG_api_dump
	};

	enum class VulkanExtensions
	{
		// --- Surface Support ---
		
		VE_Surface,                    // VK_KHR_surface
		VE_Win32Surface,               // VK_KHR_win32_surface
		VE_XcbSurface,                 // VK_KHR_xcb_surface
		VE_XlibSurface,                // VK_KHR_xlib_surface
		VE_ExtHeadlessSurface,         // VK_EXT_headless_surface

		// --- Presentation & Display ---
		
		VE_KhrSwapchain,               // VK_KHR_swapchain
		VE_KhrDisplay,                 // VK_KHR_display
		VE_KhrDisplaySwapchain,        // VK_KHR_display_swapchain

		// --- Ray Tracing ---
		
		VE_KhrAccelerationStructure,   // VK_KHR_acceleration_structure
		VE_KhrRayTracingPipeline,      // VK_KHR_ray_tracing_pipeline
		VE_KhrRayQuery,                // VK_KHR_ray_query
		VE_KhrDeferredHostOperations,  // VK_KHR_deferred_host_operations
		VE_KhrBufferDeviceAddress,     // VK_KHR_buffer_device_address

		// --- Shader Features ---
		
		VE_KhrShaderAtomicInt64,           // VK_KHR_shader_atomic_int64
		VE_KhrShaderSubgroupExtendedTypes, // VK_KHR_shader_subgroup_extended_types
		VE_KhrShaderTerminateInvocation,   // VK_KHR_shader_terminate_invocation
		VE_KhrShaderClock,                 // VK_KHR_shader_clock

		// --- Debugging & Validation ---
		
		VE_DebugUtils,                 // VK_EXT_debug_utils
		VE_ExtDebugReport,             // VK_EXT_debug_report
		VE_ExtValidationFeatures,      // VK_EXT_validation_features
		VE_ExtDebugMarker,             // VK_EXT_debug_marker
		VE_ExtPipelineCreationFeedback,// VK_EXT_pipeline_creation_feedback
		VE_ExtToolingInfo,             // VK_EXT_tooling_info

		// --- Utility & Maintenance ---
		
		VE_KhrMaintenance1,            // VK_KHR_maintenance1
		VE_KhrMaintenance2,            // VK_KHR_maintenance2
		VE_KhrMaintenance3,            // VK_KHR_maintenance3
		VE_KhrBindMemory2,             // VK_KHR_bind_memory2
		VE_KhrDedicatedAllocation,     // VK_KHR_dedicated_allocation
		VE_ExtTransformFeedback        // VK_EXT_transform_feedback
	};

	class KALAWINDOW_API Renderer_Vulkan
	{
	public:
		static inline void* instancePtr;

		//
		// INITIALIZE PHASE
		//

		//Enable selected vulkan layer, must be called before initializing Vulkan
		static bool EnableLayer(VulkanLayers layer);
		//Enable selected vulkan extension, must be called before initializing Vulkan
		static bool EnableExtension(VulkanExtensions extension);

		//Initialize Vulkan and apply enabled layers and extensions
		static bool Initialize();

		//Attach Vulkan to window
		static void CreateVulkanSurface(Window* window);

		//Creates the command pool from command buffers
		static bool CreateCommandPool();

		//Allocates a single primary command buffer
		static bool CreateCommandBuffer();

		//
		// RUNTIME LOOP PHASE
		//

		//Aquires the next available image from the swapchain so the GPU can render it
		static FrameResult BeginFrame(uint32_t& imageIndex);

		//Records drawing commands into a VkCommandBuffer
		static bool RecordCommandBuffer(uint32_t imageIndex);

		//Submits your command buffer to the graphics queue
		static bool SubmitFrame(uint32_t imageIndex);

		//Presents rendered image to the screen via the swapchain
		static FrameResult PresentFrame(uint32_t imageIndex);

		//
		// REMAKE PHASE
		//

		//Creates the render pass for drawing
		static bool CreateRenderPass();

		//Creates framebuffers from swapchain image views
		static bool CreateFramebuffers();

		//
		// REUSABLES
		//

		static bool CreateSwapchain(Window* window);
		static void DestroySwapchain();

		static const vector<VulkanLayers> GetEnabledLayers() { return enabledLayers; }
		static const vector<VulkanExtensions> GetEnabledExtensions() { return enabledExtensions; }

		static void Shutdown();
	private:
		static inline vector<VulkanLayers> enabledLayers{};
		static inline vector<VulkanExtensions> enabledExtensions{};
	};
}

#endif //KALAWINDOW_SUPPORT_VULKAN