//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef KALAWINDOW_SUPPORT_VULKAN

#pragma once

#include <vector>

#include "core/platform.hpp"
#include "graphics/window.hpp"
#include "graphics/vulkan/vulkan_layers.hpp"
#include "graphics/vulkan/vulkan_extensions.hpp"

namespace KalaWindow::Graphics
{
	using std::vector;

	enum class FrameResult
	{
		VK_FRAME_OK,            //Vulkan frame is fine, proceed with rendering
		VK_FRAME_RESIZE_NEEDED, //Swapchain is out of date or suboptimal - recreate needed
		VK_FRAME_ERROR          //Unexpected error - recreate needed
	};

	class KALAWINDOW_API Renderer_Vulkan
	{
	public:
		//
		// INITIALIZE PHASE
		//

		//Enable selected vulkan layer
		static bool EnableLayer(VulkanLayers layer);
		//Enable selected vulkan instance extension
		static bool EnableInstanceExtension(VulkanInstanceExtensions extension);
		//Enable selected vulkan device extension, this will be loaded after device has been selected
		static bool EnableDeviceExtension(VulkanDeviceExtensions extension);

		//Initialize Vulkan and apply enabled layers and extensions.
		//Max_frames must be 2 or 3: 
		//   2 = lower latency (fast-paced titles, VR, etc.)
		//   3 = better throughput / smoother frame pacing for most apps.
		static bool Initialize(uint32_t max_frames);

		//Attach Vulkan to window
		static void CreateVulkanSurface(Window* window);

		//Creates the command pool from command buffers
		static bool CreateCommandPool(Window* window);

		//Allocates a single primary command buffer
		static bool CreateCommandBuffer(Window* window);

		//Creates new semaphores and fences per window
		static bool CreateSyncObjects(Window* window);
		static void DestroySyncObjects(Window* window);

		//
		// RUNTIME LOOP PHASE
		//

		//Aquires the next available image from the swapchain so the GPU can render it
		static FrameResult BeginFrame(
			Window* window,
			uint32_t& imageIndex);

		//Records drawing commands into a VkCommandBuffer
		static bool RecordCommandBuffer(
			Window* window,
			uint32_t imageIndex);

		//Submits your command buffer to the graphics queue
		static bool SubmitFrame(
			Window* window,
			uint32_t imageIndex);

		//Presents rendered image to the screen via the swapchain
		static FrameResult PresentFrame(
			Window* window,
			uint32_t imageIndex);

		//Stall the GPU, destroy and recreate
		//the swapchain and all its dependent resources
		//(render pass, framebuffers, semaphores/fences, and command buffers)
		static void HardReset(Window* window);

		//Issue a zero‐work submit to consume the image
		//- acquire semaphore, then immediately present that image back to the swapchain
		//- resetting its semaphores without tearing down the swapchain
		static void SoftReset(
			Window* window,
			uint32_t imageIndex);

		//Ensures all window-related data is cleared
		static void DestroyWindowData(Window* window);

		//
		// REMAKE PHASE
		//

		//Creates the render pass for drawing
		static bool CreateRenderPass(Window* window);

		//Creates framebuffers from swapchain image views
		static bool CreateFramebuffers(Window* window);

		//
		// REUSABLES
		//

		static bool CreateSwapchain(Window* window);
		static void DestroySwapchain(Window* window);

		static const vector<VulkanLayers> GetEnabledLayers() { return enabledLayers; }
		static const vector<VulkanInstanceExtensions> GetEnabledInstanceExtensions() { return enabledInstanceExtensions; }
		static const vector<VulkanDeviceExtensions> GetEnabledDeviceExtensions() { return enabledDeviceExtensions; }

		static void Shutdown();
	private:
		static inline vector<VulkanLayers> enabledLayers{};
		static inline vector<VulkanInstanceExtensions> enabledInstanceExtensions{};
		static inline vector<VulkanDeviceExtensions> enabledDeviceExtensions{};
	};
}

#endif //KALAWINDOW_SUPPORT_VULKAN