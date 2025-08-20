//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

/*
#include <vector>

#include "KalaHeaders/api.hpp"
#include "KalaHeaders/core_types.hpp"

#include "graphics/window.hpp"
#include "graphics/vulkan/enums_vulkan.hpp"

namespace KalaWindow::Graphics::Vulkan
{
	using std::vector;

	enum class UpdateResult
	{
		RESULT_OK,    //Render loop frame ran correctly
		RESULT_ERROR, //Something bad happened, program should force close
		RESULT_RESIZE //Resize is needed, but otherwise everything is fine
	};

	enum class VSyncState
	{
		VSYNC_ON,              //Framerate is capped to monitor refresh rate.
		VSYNC_OFF,             //Framerate is uncapped, runs as fast as render loop allows, introduces tearing.
		VSYNC_TRIPLE_BUFFERING //Low latency, no screen tearing. Does not exist in OpenGL, will default to 'ON'!
	};

	class LIB_API Renderer_Vulkan
	{
	public:
		//
		// INITIALIZATION
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

		//Creates the command pool from command buffers
		static bool CreateCommandPool(Window* window);

		//Allocates a single primary command buffer
		static bool CreateCommandBuffer(Window* window);

		//Creates new semaphores and fences per window
		static bool CreateSyncObjects(Window* window);

		//Initializes core shader variables that are reused across each shader.
		//Must be called at the END of your Vulkan initialization system
		//and BEFORE CreateShader or CompileShader is called
		static bool InitializeShaderSystem(
			const VulkanData_ViewportState& vsStruct,
			const VulkanData_DynamicState& dsStruct,
			const VulkanData_MultisampleState& msStruct);

		//
		// RUNTIME LOOP
		//

		//The single point of entry for all Vulkan updates.
		//Calls the following systems internally:
		//  - BeginFrame
		//  - RecordCommandBuffer
		//  - SubmitFrame
		//  - PresentFrame
		static UpdateResult Update(Window* window);

		//
		// REUSABLES
		//

		static bool IsVulkanInitialized() { return isVulkanInitialized; }
		static uintptr_t GetInstance() { return instance; }
		static uintptr_t GetDevice() { return device; }
		static uintptr_t GetPhysicalDevice() { return physicalDevice; }

		//Checks if vsync is enabled or not.
		static VSyncState GetVSyncState();
		//Allows to set vsync true or false.
		static void SetVSyncState(
			VSyncState vsyncState,
			Window* window);

		//Stall the GPU, destroy and recreate
		//the swapchain and all its dependent resources
		//(render pass, framebuffers, semaphores/fences, and command buffers)
		static void HardReset(Window* window);

		//Creates the render pass for drawing
		static bool CreateRenderPass(Window* window);

		//Creates framebuffers from swapchain image views
		static bool CreateFramebuffers(Window* window);

		static const vector<VulkanLayers>& GetEnabledLayers() { return enabledLayers; }
		static const vector<VulkanInstanceExtensions>& GetEnabledInstanceExtensions() { return enabledInstanceExtensions; }
		static const vector<VulkanDeviceExtensions>& GetEnabledDeviceExtensions() { return enabledDeviceExtensions; }

		//
		// CLEANUP
		//

		static void DestroySyncObjects(Window* window);

		//Ensures all window-related data is cleared
		static void DestroyWindowData(Window* window);

		static void Shutdown();
	private:
		static inline bool isVulkanInitialized{};
		static inline bool isVulkanShaderSystemInitialized{};

		static inline uintptr_t instance{};
		static inline uintptr_t device{};
		static inline uintptr_t physicalDevice{};

		static inline vector<VulkanLayers> enabledLayers{};
		static inline vector<VulkanInstanceExtensions> enabledInstanceExtensions{};
		static inline vector<VulkanDeviceExtensions> enabledDeviceExtensions{};
	};
}
*/