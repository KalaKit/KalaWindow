//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef KALAWINDOW_SUPPORT_VULKAN

#define KALAKIT_MODULE "VULKAN"

#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#elif __linux__
#define VK_USE_PLATFORM_XLIB_KHR
#endif
#define VOLK_IMPLEMENTATION
#include <Volk/volk.h>
#ifdef _WIN32
#include <Windows.h>
#elif __linux__
#include <X11/Xlib.h> 
#endif

#include <stdexcept>
#include <string>
#include <vector>
#include <unordered_map>

#include "graphics/vulkan/vulkan.hpp"
#include "graphics/render.hpp"
#include "core/enums.hpp"

using KalaWindow::Graphics::Renderer_Vulkan;
using KalaWindow::Graphics::VulkanLayers;
using KalaWindow::Graphics::VulkanExtensions;
using KalaWindow::Graphics::Window;
using KalaWindow::Graphics::Render;
using KalaWindow::PopupAction;
using KalaWindow::PopupType;
using KalaWindow::PopupResult;

using std::string;
using std::to_string;
using std::vector;
using std::unordered_map;
using std::pair;

static void ForceClose(const string& title, const string& reason);
static const char* ToString(VulkanLayers layer);
static const char* ToString(VulkanExtensions ext);
static bool IsExtensionInstance(VulkanExtensions ext);
static int RatePhysicalDevice(
	VkPhysicalDevice device,
	string& failReason);

static VkInstance instance = VK_NULL_HANDLE;
static VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
static VkDevice device = VK_NULL_HANDLE;
static VkQueue graphicsQueue = VK_NULL_HANDLE;
static uint32_t graphicsQueueFamilyIndex = 0;

static vector<VulkanExtensions> delayedExt{};

static const unordered_map<VulkanLayers, const char*> layerInfo =
{
	// --- Meta-layer ---

	{ VulkanLayers::VL_KhronosValidation,     "VK_LAYER_KHRONOS_validation" },

	// --- Sub-layers ---

	{ VulkanLayers::VL_LunargThreading,       "VK_LAYER_LUNARG_threading" },
	{ VulkanLayers::VL_LunargParamValidation, "VK_LAYER_LUNARG_parameter_validation" },
	{ VulkanLayers::VL_LunargObjectTracker,   "VK_LAYER_LUNARG_object_tracker" },
	{ VulkanLayers::VL_LunargCoreValidation,  "VK_LAYER_LUNARG_core_validation" },
	{ VulkanLayers::VL_LunargSwapchain,       "VK_LAYER_LUNARG_swapchain" },
	{ VulkanLayers::VL_LunargImage,           "VK_LAYER_LUNARG_image" },
	{ VulkanLayers::VL_LunargApiDump,         "VK_LAYER_LUNARG_api_dump" }
};

static const unordered_map<VulkanExtensions, pair<const char*, bool>> extensionInfo = 
{
	// --- Surface Support ---

	{ VulkanExtensions::VE_Surface,                  { "VK_KHR_surface", false } },
	{ VulkanExtensions::VE_Win32Surface,             { "VK_KHR_win32_surface", false } },
	{ VulkanExtensions::VE_XcbSurface,               { "VK_KHR_xcb_surface", false } },
	{ VulkanExtensions::VE_XlibSurface,              { "VK_KHR_xlib_surface", false } },
	{ VulkanExtensions::VE_ExtHeadlessSurface,       { "VK_EXT_headless_surface", false } },

	// --- Presentation & Display ---

	{ VulkanExtensions::VE_KhrSwapchain,             { "VK_KHR_swapchain", true } },
	{ VulkanExtensions::VE_KhrDisplay,               { "VK_KHR_display", true } },
	{ VulkanExtensions::VE_KhrDisplaySwapchain,      { "VK_KHR_display_swapchain", true } },

	// --- Ray Tracing ---
	{ VulkanExtensions::VE_KhrAccelerationStructure, { "VK_KHR_acceleration_structure", true } },
	{ VulkanExtensions::VE_KhrRayTracingPipeline,    { "VK_KHR_ray_tracing_pipeline", true } },
	{ VulkanExtensions::VE_KhrRayQuery,              { "VK_KHR_ray_query", true } },
	{ VulkanExtensions::VE_KhrDeferredHostOperations,{ "VK_KHR_deferred_host_operations", true } },
	{ VulkanExtensions::VE_KhrBufferDeviceAddress,   { "VK_KHR_buffer_device_address", true } },

	// --- Shader Features ---
	{ VulkanExtensions::VE_KhrShaderAtomicInt64,     { "VK_KHR_shader_atomic_int64", true } },
	{ VulkanExtensions::VE_KhrShaderSubgroupExtendedTypes, { "VK_KHR_shader_subgroup_extended_types", true } },
	{ VulkanExtensions::VE_KhrShaderTerminateInvocation,   { "VK_KHR_shader_terminate_invocation", true } },
	{ VulkanExtensions::VE_KhrShaderClock,           { "VK_KHR_shader_clock", true } },

	// --- Debugging & Validation ---
	{ VulkanExtensions::VE_DebugUtils,               { "VK_EXT_debug_utils", false } },
	{ VulkanExtensions::VE_ExtDebugReport,           { "VK_EXT_debug_report", false } },
	{ VulkanExtensions::VE_ExtValidationFeatures,    { "VK_EXT_validation_features", false } },
	{ VulkanExtensions::VE_ExtDebugMarker,           { "VK_EXT_debug_marker", true } },
	{ VulkanExtensions::VE_ExtPipelineCreationFeedback, { "VK_EXT_pipeline_creation_feedback", true } },
	{ VulkanExtensions::VE_ExtToolingInfo,           { "VK_EXT_tooling_info", false } },

	// --- Utility & Maintenance ---
	{ VulkanExtensions::VE_KhrMaintenance1,          { "VK_KHR_maintenance1", true } },
	{ VulkanExtensions::VE_KhrMaintenance2,          { "VK_KHR_maintenance2", true } },
	{ VulkanExtensions::VE_KhrMaintenance3,          { "VK_KHR_maintenance3", true } },
	{ VulkanExtensions::VE_KhrBindMemory2,           { "VK_KHR_bind_memory2", true } },
	{ VulkanExtensions::VE_KhrDedicatedAllocation,   { "VK_KHR_dedicated_allocation", true } },
	{ VulkanExtensions::VE_ExtTransformFeedback,     { "VK_EXT_transform_feedback", true } },
};

namespace KalaWindow::Graphics
{
	//
	// INITIALIZE PHASE
	//

	bool Renderer_Vulkan::EnableLayer(VulkanLayers layer)
	{
		const char* name = ToString(layer);
		if (!name)
		{
			LOG_ERROR("Can not enable layer (unknown enum value)");
			return false;
		}

		if (find(
			enabledLayers.begin(),
			enabledLayers.end(),
			layer) != enabledLayers.end())
		{
			LOG_ERROR("Can not enable layer '" << name << "' because it is already enabled!");
			return false;
		}

		uint32_t count = 0;
		vkEnumerateInstanceLayerProperties(&count, nullptr);
		vector<VkLayerProperties> layers(count);
		vkEnumerateInstanceLayerProperties(&count, layers.data());

		for (const auto& l : layers)
		{
			if (strcmp(l.layerName, name) == 0)
			{
				enabledLayers.push_back(layer);
				LOG_SUCCESS("Enabled layer '" << string(name) << "'!");
				return true;
			}
		}

		LOG_ERROR("Can not enable layer '" << name << "' because it is not supported on this system!");
		return false;
	}

	bool Renderer_Vulkan::EnableExtension(VulkanExtensions ext)
	{
		const char* name = ToString(ext);
		if (!name)
		{
			LOG_ERROR("Can not enable extension (unknown enum value)");
			return false;
		}

		if (find(
			enabledExtensions.begin(),
			enabledExtensions.end(),
			ext) != enabledExtensions.end())
		{
			LOG_ERROR("Can not enable extension '" << name << "' because it is already enabled!");
			return false;
		}
		if (find(
			delayedExt.begin(),
			delayedExt.end(),
			ext) != delayedExt.end())
		{
			LOG_ERROR("Can not enable extension '" << name << "' because it is already assigned as a delayed extension!");
			return false;
		}

		if (IsExtensionInstance(ext))
		{
			uint32_t count = 0;
			vkEnumerateInstanceExtensionProperties(
				nullptr,
				&count,
				nullptr);
			vector<VkExtensionProperties> exts(count);
			vkEnumerateInstanceExtensionProperties(
				nullptr,
				&count,
				exts.data());

			for (const auto& e : exts)
			{
				if (strcmp(e.extensionName, name) == 0)
				{
					enabledExtensions.push_back(ext);
					LOG_SUCCESS("Enabled extension '" << string(name) << "'!");
					return true;
				}
			}

			LOG_ERROR("Instance extension '" << name << "' not supported on this system!");
			return false;
		}
		else
		{
			delayedExt.push_back(ext);
			LOG_SUCCESS("Queued device extension '" << name << "' for delayed enable");
			return true;
		}
	}

	bool Renderer_Vulkan::Initialize()
	{
		if (volkInitialize() != VK_SUCCESS)
		{
			ForceClose(
				"Vulkan initialization error",
				"Volk initialization failed!");
			return false;
		}

		vector<const char*> layerNames{};
		for (const auto& layer : enabledLayers)
		{
			const char* str = ToString(layer);
			layerNames.push_back(str);
		}

		vector<const char*> instanceExtensions{};
		for (const auto& extension : enabledExtensions)
		{
			const char* name = ToString(extension);
			if (IsExtensionInstance(extension)) instanceExtensions.push_back(name);
		}

		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "KalaWindow";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "KalaWindow";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_2;

		VkInstanceCreateInfo instanceCreateInfo{};
		instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceCreateInfo.pApplicationInfo = &appInfo;
		instanceCreateInfo.enabledLayerCount = 
			static_cast<uint32_t>(layerNames.size());
		instanceCreateInfo.ppEnabledLayerNames = layerNames.data();
		instanceCreateInfo.enabledExtensionCount = 
			static_cast<uint32_t>(instanceExtensions.size());
		instanceCreateInfo.ppEnabledExtensionNames = instanceExtensions.data();

		if (vkCreateInstance(
			&instanceCreateInfo,
			nullptr,
			&instance) != VK_SUCCESS)
		{
			ForceClose(
				"Vulkan initialization error",
				"Failed to create Vulkan instance!");
			return false;
		}

		volkLoadInstance(instance);
		instancePtr = static_cast<void*>(instance);

		//enumerate and pick physical device

		uint32_t gpuCount = 0;
		vkEnumeratePhysicalDevices(
			instance, 
			&gpuCount, 
			nullptr);
		if (gpuCount == 0)
		{
			ForceClose(
				"Vulkan initialization error",
				"No Vulkan-compatible physical devices found!");
			return false;
		}

		vector<VkPhysicalDevice> devices(gpuCount);
		vkEnumeratePhysicalDevices(
			instance, 
			&gpuCount, 
			devices.data());
		
		int bestScore = -1;
		VkPhysicalDevice bestDevice = VK_NULL_HANDLE;

		for (const auto& dev : devices)
		{
			VkPhysicalDeviceProperties props{};
			vkGetPhysicalDeviceProperties(dev, &props);

			string failReason{};
			int score = RatePhysicalDevice(
				dev,
				failReason);

			if (!failReason.empty())
			{
				LOG_WARNING(
					"Device '" 
					<< string(props.deviceName) 
					<< "' was not picked because: " 
					+ failReason);
			}

			if (score > bestScore)
			{
				bestScore = score;
				bestDevice = dev;
			}
		}

		if (bestDevice == VK_NULL_HANDLE)
		{
			ForceClose(
				"Vulkan initialization error",
				"No suitable physical GPU found!");
			return false;
		}

		physicalDevice = bestDevice;

		vector<const char*> deviceExtensions{};
		for (const auto& ext : delayedExt)
		{
			const char* name = ToString(ext);
			if (name) deviceExtensions.push_back(name);
		}

		//find graphics queue

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(
			physicalDevice,
			&queueFamilyCount,
			nullptr);
		vector<VkQueueFamilyProperties> families(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(
			physicalDevice,
			&queueFamilyCount,
			families.data());

		bool found = false;
		for (uint32_t i = 0; i < queueFamilyCount; ++i)
		{
			if (families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				graphicsQueueFamilyIndex = i;
				found = true;
				break;
			}
		}

		if (!found)
		{
			ForceClose(
				"Vulkan initialization error",
				"No graphics-capable queue family found!");
			return false;
		}

		//create logical device

		float priority = 1.0f;
		VkDeviceQueueCreateInfo queueInfo{};
		queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueInfo.queueFamilyIndex = graphicsQueueFamilyIndex;
		queueInfo.queueCount = 1;
		queueInfo.pQueuePriorities = &priority;

		VkDeviceCreateInfo deviceCreateInfo{};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.queueCreateInfoCount = 1;
		deviceCreateInfo.pQueueCreateInfos = &queueInfo;
		deviceCreateInfo.enabledLayerCount = 
			static_cast<uint32_t>(layerNames.size());
		deviceCreateInfo.ppEnabledLayerNames = layerNames.data();
		deviceCreateInfo.enabledExtensionCount =
			static_cast<uint32_t>(deviceExtensions.size());
		deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

		if (vkCreateDevice(
			physicalDevice,
			&deviceCreateInfo,
			nullptr,
			&device) != VK_SUCCESS)
		{
			ForceClose(
				"Vulkan initialization error",
				"Failed to create logical device!");
			return false;
		}

		volkLoadDevice(device);
		vkGetDeviceQueue(
			device,
			graphicsQueueFamilyIndex,
			0,
			&graphicsQueue);

		LOG_SUCCESS("Initialized Vulkan!");

		return true;
	}

	void Renderer_Vulkan::CreateVulkanSurface(Window* targetWindow)
	{
#ifdef _WIN32
		WindowStruct_Windows& window = targetWindow->GetWindow_Windows();
		HWND windowRef = reinterpret_cast<HWND>(window.hwnd);
		HINSTANCE windowIns = reinterpret_cast<HINSTANCE>(window.hInstance);

		VkWin32SurfaceCreateInfoKHR surfaceInfo{};
		surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		surfaceInfo.hwnd = windowRef;
		surfaceInfo.hinstance = windowIns;

		VkSurfaceKHR surface{};
		if (vkCreateWin32SurfaceKHR(
			instance,
			&surfaceInfo,
			nullptr,
			&surface) != VK_SUCCESS)
		{
			ForceClose(
				"Vulkan error",
				"Failed to create Win32 Vulkan surface!");
		}

		vData.surface = static_cast<void*>(surface);
#elif __linux__
		//TODO: ADD LINUX SUPPORT
#endif
	}

	bool Renderer_Vulkan::CreateCommandPool(Window* window)
	{
		if (device == VK_NULL_HANDLE)
		{
			LOG_ERROR("Cannot create command pool because device is not assigned!");
			return false;
		}

		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.queueFamilyIndex = graphicsQueueFamilyIndex;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

		if (vkCreateCommandPool(
			device,
			&poolInfo,
			nullptr,
			&vData.commandPool) != VK_SUCCESS)
		{
			LOG_ERROR("Failed to create Vulkan command pool!");
			return false;
		}

		return true;
	}

	bool Renderer_Vulkan::CreateCommandBuffer(Window* window)
	{
		vData.commandBuffers.resize(vData.images.size());

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = vData.commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = static_cast<uint32_t>(vData.commandBuffers.size());

		if (vkAllocateCommandBuffers(
			device,
			&allocInfo,
			commandBuffers.data()) != VK_SUCCESS)
		{
			LOG_ERROR("Failed to allocate command buffers!");
			return false;
		}

		return true;
	}

	//
	// RUNTIME LOOP PHASE
	//

	FrameResult Renderer_Vulkan::BeginFrame(
		Window* window,
		uint32_t& imageIndex)
	{
		uint32_t nextImage = 0;
		VkResult result = vkAcquireNextImageKHR(
			device,
			vData.swapchain,
			UINT64_MAX,
			vData.imageAvailableSemaphores[nextImage],
			VK_NULL_HANDLE,
			&nextImage);

		if (result == VK_SUCCESS)
		{
			imageIndex = nextImage;

			return FrameResult::VK_FRAME_OK;
		}

		if (result == VK_SUBOPTIMAL_KHR
			|| result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			return FrameResult::VK_FRAME_RESIZE_NEEDED;
		}

		LOG_ERROR("vkAquireNextImageKHR failed with error: " << result);
		return FrameResult::VK_FRAME_ERROR;
	}

	bool Renderer_Vulkan::RecordCommandBuffer(
		Window* window,
		uint32_t imageIndex)
	{
		vkWaitForFences(
			device,
			1,
			&vData.inFlightFences[imageIndex],
			VK_TRUE,
			UINT64_MAX);
		vkResetFences(
			device,
			1,
			&vData.inFlightFences[imageIndex]);

		VkCommandBuffer cmd = vData.commandBuffers[imageIndex];

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		if (vkBeginCommandBuffer(cmd, &beginInfo) != VK_SUCCESS)
		{
			LOG_ERROR("Failed to begin recording command buffer!");
			return false;
		}

		VkClearValue clearColor = { {{ 0.0f, 0.0f, 0.0f, 1.0f }} };

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = vData.renderPass;
		renderPassInfo.framebuffer = vData.frameBuffers[imageIndex];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = vData.swapchainExtent;
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(
			cmd,
			&renderPassInfo,
			VK_SUBPASS_CONTENTS_INLINE);

		//user can insert draw commands here

		vkCmdEndRenderPass(cmd);

		if (vkEndCommandBuffer(cmd) != VK_SUCCESS)
		{
			LOG_ERROR("Failed to record command buffer!");
			return false;
		}

		return true;
	}

	bool Renderer_Vulkan::SubmitFrame(
		Window* window,
		uint32_t imageIndex)
	{
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { vData.imageAvailableSemaphores[imageIndex] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &vData.commandBuffers[imageIndex];

		VkSemaphore signalSemaphores[] = { vData.renderFinishedSemaphores[imageIndex] };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		if (vkQueueSubmit(
			graphicsQueue,
			1,
			&submitInfo,
			vData.inFlightFences[imageIndex]) != VK_SUCCESS)
		{
			LOG_ERROR("Failed to submit frame!");
			return false;
		}

		return true;
	}

	FrameResult Renderer_Vulkan::PresentFrame(
		Window* window,
		uint32_t imageIndex)
	{
		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &vData.renderFinishedSemaphores[imageIndex];
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &vData.swapchain;
		presentInfo.pImageIndices = &imageIndex;

		VkResult result = vkQueuePresentKHR(graphicsQueue, &presentInfo);

		if (result == VK_SUCCESS) return FrameResult::VK_FRAME_OK;

		if (result == VK_SUBOPTIMAL_KHR
			|| result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			return FrameResult::VK_FRAME_RESIZE_NEEDED;
		}

		LOG_ERROR("vkQueuePresentKHR failed with error: " << result);
		return FrameResult::VK_FRAME_ERROR;
	}

	void Renderer_Vulkan::HardReset(Window* window)
	{
		if (!device)
		{
			LOG_ERROR("Cannot drain pending signals because no device has been assigned!");
			return;
		}

		Window* window = Window::windows.front().get();

		vkDeviceWaitIdle(device);

		DestroySwapchain();
		CreateSwapchain(window);
		CreateRenderPass();
		CreateFramebuffers();
		CreateCommandBuffer(window);
	}

	void Renderer_Vulkan::SoftReset(
		Window* window,
		uint32_t imageIndex)
	{
		if (!graphicsQueue)
		{
			LOG_ERROR("Cannot soft-reset because graphicsQueue has not been assigned!");
			return;
		}
		if (!vData.swapchain)
		{
			LOG_ERROR("Cannot soft-reset because swapchain has not been assigned!");
			return;
		}
		if (imageIndex >= vData.renderFinishedSemaphores.size())
		{
			LOG_ERROR("Cannot soft-reset! Image index " << imageIndex << " is out of range!");
			return;
		}

		vkQueueSubmit(
			graphicsQueue,
			0,
			nullptr,
			VK_NULL_HANDLE);

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &vData.renderFinishedSemaphores[imageIndex];
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &vData.swapchain;
		presentInfo.pImageIndices = &imageIndex;
		presentInfo.pResults = nullptr;

		VkResult presentResult = vkQueuePresentKHR(graphicsQueue, &presentInfo);
		if (presentResult != VK_SUCCESS)
		{
			LOG_ERROR("Soft-reset failed! Reason: " << presentResult);
		}
	}

	//
	// REMAKE PHASE
	//

	bool Renderer_Vulkan::CreateRenderPass(Window* window)
	{
		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = vData.swapchainImageFormat;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;

		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = 1;
		renderPassInfo.pAttachments = &colorAttachment;
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		if (vkCreateRenderPass(
			device,
			&renderPassInfo,
			nullptr,
			&vData.renderPass) != VK_SUCCESS)
		{
			LOG_ERROR("Failed to create Vulkan render pass!");
			return false;
		}

		return true;
	}

	bool Renderer_Vulkan::CreateFramebuffers(Window* window)
	{
		WindowStruct_Windows& winData = window->GetWindow_Windows();
		Window_VulkanData vData = winData.vulkanData;

		vData.frameBuffers.resize(vData.imageViews.size());

		for (size_t i = 0; i < reinterpret_cast<VkImageView>(vData.imageViews).size(); ++i)
		{
			VkImageView attachments[] = { imageViews[i] };

			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = vData.renderPass;
			framebufferInfo.attachmentCount = 1;
			framebufferInfo.pAttachments = attachments;
			framebufferInfo.width = vData.swapchainExtent.width;
			framebufferInfo.height = reinterpret_cast<VkExtent2D>(swapchainExtent).height;
			framebufferInfo.layers = 1;

			if (vkCreateFramebuffer(
				device,
				&framebufferInfo,
				nullptr,
				&vData.frameBuffers[i]) != VK_SUCCESS)
			{
				LOG_ERROR("Failed to create framebuffer for image " << i);
				return false;
			}
		}

		return true;
	}

	//
	// REUSABLES
	//

	bool Renderer_Vulkan::CreateSwapchain(Window* window)
	{
		if (!instance
			|| !device
			|| !physicalDevice)
		{
			LOG_ERROR("Cannot create swapchain before Vulkan is fully initialized!");
			return false;
		}

		WindowStruct_Windows& winData = window->GetWindow_Windows();
		Window_VulkanData vData = winData.vulkanData;

		//surface capabilities

#ifdef _WIN32
		WindowStruct_Windows win = window->GetWindow_Windows();
#elif __linux__
		WindowStruct_X11 win = window->GetWindow_X11();
#endif
		VkSurfaceKHR surfacePtr = reinterpret_cast<VkSurfaceKHR>(vData.surface);

		VkSurfaceCapabilitiesKHR capabilities{};
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
			physicalDevice,
			(VkSurfaceKHR)surfacePtr,
			&capabilities);

		VkExtent2D extent = capabilities.currentExtent;
		vData.swapchainExtent = extent;

		//pick format

		VkSurfaceFormatKHR format =
		{
			VK_FORMAT_B8G8R8A8_UNORM,
			VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
		};
		vData.swapchainImageFormat = format.format;

		//present mode

		VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;

		//create swapchain

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = (VkSurfaceKHR)surfacePtr;
		createInfo.minImageCount = capabilities.minImageCount + 1;
		createInfo.imageFormat = format.format;
		createInfo.imageColorSpace = format.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.preTransform = capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		if (vkCreateSwapchainKHR(
			device,
			&createInfo,
			nullptr,
			&reinterpret_cast<VkSwapchainKHR>(vData.swapchain)) != VK_SUCCESS)
		{
			LOG_ERROR("Failed to create Vulkan swapchain!");
			return false;
		}

		//get swapchain images

		uint32_t count = 0;
		vkGetSwapchainImagesKHR(
			device,
			reinterpret_cast<VkSwapchainKHR>(vData.swapchain),
			&count,
			nullptr);
		vData.images.resize(count);
		vkGetSwapchainImagesKHR(
			device,
			reinterpret_cast<VkSwapchainKHR>(vData.swapchain),
			&count,
			reinterpret_cast<VkImage>(vData.images).data());

		//create image views

		vData.imageViews.resize(count);
		for (uint32_t i = 0; i < count; ++i)
		{
			VkImageViewCreateInfo viewInfo{};
			viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewInfo.image = reinterpret_cast<VkImage>(vData.images[i]);
			viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewInfo.format = reinterpret_cast<VkFormat>(vData.swapchainImageFormat);
			viewInfo.components =
			{
				VK_COMPONENT_SWIZZLE_IDENTITY,
				VK_COMPONENT_SWIZZLE_IDENTITY,
				VK_COMPONENT_SWIZZLE_IDENTITY,
				VK_COMPONENT_SWIZZLE_IDENTITY
			};
			viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			viewInfo.subresourceRange.baseMipLevel = 0;
			viewInfo.subresourceRange.levelCount = 1;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.layerCount = 1;

			if (vkCreateImageView(
				device,
				&viewInfo,
				nullptr,
				&vData.imageViews[i]) != VK_SUCCESS)
			{
				LOG_ERROR("Failed to create image view for swapchain image " << i);
				return false;
			}
		}

		//create semaphores and fence

		vData.imageAvailableSemaphores.resize(count);
		vData.renderFinishedSemaphores.resize(count);
		vData.inFlightFences.resize(count);

		VkSemaphoreCreateInfo semInfo{ VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
		VkFenceCreateInfo fenceInfo{ VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (uint32_t i = 0; i < count; ++i)
		{
			vkCreateSemaphore(
				device,
				&semInfo,
				nullptr,
				&reinterpret_cast<VkFence>(vData.imageAvailableSemaphores[i]));
			vkCreateSemaphore(
				device,
				&semInfo,
				nullptr,
				&reinterpret_cast<VkFence>(vData.renderFinishedSemaphores[i]));
			vkCreateFence(
				device,
				&fenceInfo,
				nullptr,
				&reinterpret_cast<VkFence>(vData.inFlightFences[i]));
		}

		LOG_SUCCESS("Successfully created Vulkan swapchain and released resources!");
		return true;
	}

	void Renderer_Vulkan::DestroySwapchain(Window* window)
	{
		WindowStruct_Windows& winData = window->GetWindow_Windows();
		Window_VulkanData vData = winData.vulkanData;

		//destroy framebuffers

		for (auto fb : vData.framebuffers)
		{
			if (fb) vkDestroyFramebuffer(device, reinterpret_cast<VkFramebuffer>(fb), nullptr);
		}
		vData.framebuffers.clear();

		//destroy image views

		for (auto view : vData.framebuffers)
		{
			if (view) vkDestroyImageView(device, reinterpret_cast<VkImageView>(view), nullptr);
		}
		vData.imageViews.clear();
		vData.images.clear();

		//destroy render pass

		if (vData.renderPass)
		{
			vkDestroyRenderPass(device,  reinterpret_cast<VkRenderPass>(vData.renderPass), nullptr);
			vData.renderPass = nullptr;
		}

		//destroy semaphores and fence

		for (auto s : vData.imageAvailableSemaphores)
		{
			vkDestroySemaphore(device, reinterpret_cast<VkSemaphore>(s), nullptr);
		}
		for (auto s : vData.renderFinishedSemaphores)
		{
			vkDestroySemaphore(device, reinterpret_cast<VkSemaphore>(s), nullptr);
		}
		for (auto f : vData.inFlightFences)
		{
			vkDestroyFence(device, reinterpret_cast<VkFence>(f), nullptr);
		}

		//destroy swapchain itself

		if (vData.swapchain)
		{
			vkDestroySwapchainKHR(
				device,
				reinterpret_cast<VkSwapchainKHR>(vData.swapchain),
				nullptr);
		}

		//reset handles

		vData.swapchain = VK_NULL_HANDLE;
		vData.imageAvailableSemaphores.clear();
		vData.renderFinishedSemaphores.clear();
		vData.inFlightFences.clear();
	}

	void Renderer_Vulkan::Shutdown()
	{
		if (device)
		{
			vkDeviceWaitIdle(device);

			for (const auto& window : Window::windows)
			{
				Window* win = window.get();
				WindowStruct_Windows& winData = win->GetWindow_Windows();
				Window_VulkanData vData = winData.vulkanData;

				//destroy runtime resources first
				DestroySwapchain(win);

				if (vData.commandPool)
				{
					vkDestroyCommandPool(
						device,
						reinterpret_cast<VkCommandPool>(vData.commandPool),
						nullptr);
					vData.commandPool = VK_NULL_HANDLE;
				}
			}

			vkDestroyDevice(device, nullptr);
			device = VK_NULL_HANDLE;
		}

		if (instance)
		{
			vkDestroyInstance(instance, nullptr);
			instance = VK_NULL_HANDLE;
		}

		enabledLayers.clear();
		enabledExtensions.clear();

		for (const auto& window : Window::windows)
		{
			Window* win = window.get();
			WindowStruct_Windows& winData = win->GetWindow_Windows();
			Window_VulkanData& vData = winData.vulkanData;

			vData.commandBuffers.clear();
			vData.framebuffers.clear();
			vData.images.clear();
			vData.imageViews.clear();
			vData.imageAvailableSemaphores.clear();
			vData.renderFinishedSemaphores.clear();
			vData.inFlightFences.clear();
		}
	}
}

static void ForceClose(const string& title, const string& reason)
{
	LOG_ERROR(reason);

	Window* mainWindow = Window::windows.front().get();
	if (mainWindow->CreatePopup(
		title,
		reason,
		PopupAction::POPUP_ACTION_OK,
		PopupType::POPUP_TYPE_ERROR)
		== PopupResult::POPUP_RESULT_OK)
	{
		Render::Shutdown();
	}
}

static const char* ToString(VulkanLayers layer)
{
	auto it = layerInfo.find(layer);
	return it != layerInfo.end() ? it->second : nullptr;
}

static const char* ToString(VulkanExtensions ext)
{
	auto it = extensionInfo.find(ext);
	return it != extensionInfo.end() ? it->second.first : nullptr;
}

static bool IsExtensionInstance(VulkanExtensions ext)
{
	auto it = extensionInfo.find(ext);
	return it != extensionInfo.end() ? !it->second.second : false;
}

static int RatePhysicalDevice(
	VkPhysicalDevice device,
	string& failReason)
{
	VkPhysicalDeviceProperties props{};
	vkGetPhysicalDeviceProperties(device, &props);

	VkPhysicalDeviceFeatures features{};
	vkGetPhysicalDeviceFeatures(device, &features);

	uint32_t queueCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(
		device, 
		&queueCount, 
		nullptr);
	vector<VkQueueFamilyProperties> queues(queueCount);
	vkGetPhysicalDeviceQueueFamilyProperties(
		device, 
		&queueCount, 
		queues.data());

	bool hasGraphicsQueue = false;
	for (const auto& q : queues)
	{
		if (q.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			hasGraphicsQueue = true;
			break;
		}
	}

	//reject device with no graphics support
	if (!hasGraphicsQueue)
	{
		failReason = "Device has no graphics-capable queue!";
		return -1;
	}

	//check required device extensions
	uint32_t extCount = 0;
	vkEnumerateDeviceExtensionProperties(
		device,
		nullptr,
		&extCount,
		nullptr);
	vector<VkExtensionProperties> supportedExtensions(extCount);
	vkEnumerateDeviceExtensionProperties(
		device,
		nullptr,
		&extCount,
		supportedExtensions.data());

	for (const auto& ext : delayedExt)
	{
		const char* name = ToString(ext);
		bool found = false;
		for (const auto& available : supportedExtensions)
		{
			if (strcmp(name, available.extensionName) == 0)
			{
				found = true;
				break;
			}
		}
		if (!found)
		{
			failReason = string("Missing required device extension: ") + name;
			return -1;
		}
	}

	int score = 0;

	//prefer discrete gpus
	if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) score += 1000;

	//bump for max texture resolution
	score += static_cast<int>(props.limits.maxImageDimension2D);

	return score;
}

#endif //KALAWINDOW_SUPPORT_VULKAN