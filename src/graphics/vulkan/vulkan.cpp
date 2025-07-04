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
using std::uintptr_t;

enum class ForceCloseType
{
	FC_VO, //is volk initialized
	FC_VU, //is vulkan initialized
	FC_D,  //is device assigned
	FC_GQ  //is graphics queue assigned
};

static void ForceClose(
	const string& title, 
	const string& reason);
static void ForceCloseMsg(
	ForceCloseType ct,
	const string& targetMsg);

static const char* ToString(VulkanLayers layer);
static const char* ToString(VulkanExtensions ext);
static bool IsExtensionInstance(VulkanExtensions ext);
static int RatePhysicalDevice(
	VkPhysicalDevice device,
	string& failReason);

static bool isVolkInitialized = false;
static bool isVulkanInitialized = false;

static VkInstance instance = VK_NULL_HANDLE;
static VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
static VkDevice device = VK_NULL_HANDLE;
static VkQueue graphicsQueue = VK_NULL_HANDLE;
static uint32_t graphicsQueueFamilyIndex = 0;

static uint32_t MAX_FRAMES = 0;
static uint32_t currentFrame = 0;

static vector<VulkanExtensions> delayedExt{};

static bool InitVolk()
{
	if (volkInitialize() != VK_SUCCESS)
	{
		ForceClose(
			"Vulkan initialization error",
			"Volk initialization failed!");
		return false;
	}

	return true;
}

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
		if (!isVolkInitialized
			&& !InitVolk())
		{
			ForceCloseMsg(ForceCloseType::FC_VO, "enable layer");
			return false;
		}

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
		if (!isVolkInitialized
			&& !InitVolk())
		{
			ForceCloseMsg(ForceCloseType::FC_VO, "enable extension");
			return false;
		}

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

	bool Renderer_Vulkan::Initialize(uint32_t max_frames)
	{
		if (max_frames < 2
			|| max_frames > 3)
		{
			ForceClose(
				"Vulkan error",
				"Cannot initialize Vulkan because max_frames was not set to 2 or 3!");
			return false;
		}
		MAX_FRAMES = max_frames;

		if (!isVolkInitialized
			&& !InitVolk())
		{
			ForceCloseMsg(ForceCloseType::FC_VO, "initialize Vulkan");
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

		isVulkanInitialized = true;
		LOG_SUCCESS("Initialized Vulkan!");

		return true;
	}

	void Renderer_Vulkan::CreateVulkanSurface(Window* targetWindow)
	{
		if (!isVulkanInitialized)
		{
			ForceCloseMsg(ForceCloseType::FC_VU, "create vulkan surface");
			return;
		}

		WindowStruct_Windows& winData = targetWindow->GetWindow_Windows();
		Window_VulkanData& vData = winData.vulkanData;

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

		vData.surface = reinterpret_cast<uintptr_t>(surface);
#elif __linux__
		//TODO: ADD LINUX SUPPORT
#endif
	}

	bool Renderer_Vulkan::CreateCommandPool(Window* window)
	{
		if (!isVulkanInitialized)
		{
			ForceCloseMsg(ForceCloseType::FC_VU, "create command pool");
			return false;
		}

		if (!device)
		{
			ForceCloseMsg(ForceCloseType::FC_D, "create command pool");
			return false;
		}

		WindowStruct_Windows& winData = window->GetWindow_Windows();
		Window_VulkanData& vData = winData.vulkanData;

		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.queueFamilyIndex = graphicsQueueFamilyIndex;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

		VkCommandPool realPool = VK_NULL_HANDLE;
		if (vkCreateCommandPool(
			device,
			&poolInfo,
			nullptr,
			&realPool) != VK_SUCCESS)
		{
			LOG_ERROR("Failed to create Vulkan command pool!");
			return false;
		}
		vData.commandPool = reinterpret_cast<uintptr_t>(realPool);

		return true;
	}

	bool Renderer_Vulkan::CreateCommandBuffer(Window* window)
	{
		if (!isVulkanInitialized)
		{
			ForceCloseMsg(ForceCloseType::FC_VU, "create command buffer");
			return false;
		}

		WindowStruct_Windows& winData = window->GetWindow_Windows();
		Window_VulkanData& vData = winData.vulkanData;

		vData.commandBuffers.resize(vData.images.size());

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = reinterpret_cast<VkCommandPool>(vData.commandPool);
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = static_cast<uint32_t>(vData.commandBuffers.size());

		vector<VkCommandBuffer> tempCB(allocInfo.commandBufferCount);
		if (vkAllocateCommandBuffers(
			device,
			&allocInfo,
			tempCB.data()) != VK_SUCCESS)
		{
			LOG_ERROR("Failed to allocate command buffers!");
			return false;
		}

		for (uint32_t i = 0; i < allocInfo.commandBufferCount; ++i)
		{
			vData.commandBuffers[i] = reinterpret_cast<uintptr_t>(tempCB[i]);
 		}

		return true;
	}

	bool Renderer_Vulkan::CreateSyncObjects(Window* window)
	{
		if (!isVulkanInitialized)
		{
			ForceCloseMsg(ForceCloseType::FC_VU, "create sync objects");
			return false;
		}

		if (!device)
		{
			ForceCloseMsg(ForceCloseType::FC_D, "create sync objects");
			return false;
		}

		WindowStruct_Windows& winData = window->GetWindow_Windows();
		Window_VulkanData& vData = winData.vulkanData;

		vData.imageAvailableSemaphores.resize(MAX_FRAMES);
		vData.renderFinishedSemaphores.resize(MAX_FRAMES);
		vData.inFlightFences.resize(MAX_FRAMES);

		VkSemaphoreCreateInfo semInfo{ VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
		VkFenceCreateInfo fenceInfo{ VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (uint32_t i = 0; i < MAX_FRAMES; ++i)
		{
			VkSemaphore realAvailableSemaphore = VK_NULL_HANDLE;
			VkSemaphore realFinishedSemaphore = VK_NULL_HANDLE;
			VkFence realFence = VK_NULL_HANDLE;

			if (vkCreateSemaphore(
					device,
					&semInfo,
					nullptr,
					&realAvailableSemaphore)
				!= VK_SUCCESS
				|| vkCreateSemaphore(
					device,
					&semInfo,
					nullptr,
					&realFinishedSemaphore)
				!= VK_SUCCESS
				|| vkCreateFence(
					device,
					&fenceInfo,
					nullptr,
					&realFence)
				!= VK_SUCCESS)
			{
				LOG_ERROR("Failed to create sync objects for frame " << i);

				vData.imageAvailableSemaphores[i] = reinterpret_cast<uintptr_t>(realAvailableSemaphore);
				vData.renderFinishedSemaphores[i] = reinterpret_cast<uintptr_t>(realFinishedSemaphore);
				vData.inFlightFences[i] = reinterpret_cast<uintptr_t>(realFence);
				DestroySyncObjects(window);

				return false;
			}

			vData.imageAvailableSemaphores[i] = reinterpret_cast<uintptr_t>(realAvailableSemaphore);
			vData.renderFinishedSemaphores[i] = reinterpret_cast<uintptr_t>(realFinishedSemaphore);
			vData.inFlightFences[i] = reinterpret_cast<uintptr_t>(realFence);
		}

		return true;
	}

	void Renderer_Vulkan::DestroySyncObjects(Window* window)
	{
		if (!isVulkanInitialized)
		{
			ForceCloseMsg(ForceCloseType::FC_VU, "destroy sync objects");
			return;
		}

		if (!device)
		{
			ForceCloseMsg(ForceCloseType::FC_D, "destroy sync objects");
			return;
		}

		WindowStruct_Windows& winData = window->GetWindow_Windows();
		Window_VulkanData& vData = winData.vulkanData;

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

		vData.imageAvailableSemaphores.clear();
		vData.renderFinishedSemaphores.clear();
		vData.inFlightFences.clear();
	}

	//
	// RUNTIME LOOP PHASE
	//

	FrameResult Renderer_Vulkan::BeginFrame(
		Window* window,
		uint32_t& imageIndex)
	{
		if (!isVulkanInitialized)
		{
			ForceCloseMsg(ForceCloseType::FC_VU, "begin frame");
			return FrameResult::VK_FRAME_ERROR;
		}

		if (!device)
		{
			ForceCloseMsg(ForceCloseType::FC_D, "begin frame");
			return FrameResult::VK_FRAME_ERROR;
		}

		WindowStruct_Windows& winData = window->GetWindow_Windows();
		Window_VulkanData& vData = winData.vulkanData;

		uint32_t nextImage = 0;

		VkSemaphore sem = 
			reinterpret_cast<VkSemaphore>(vData.imageAvailableSemaphores[currentFrame]);

		VkResult result = vkAcquireNextImageKHR(
			device,
			reinterpret_cast<VkSwapchainKHR>(vData.swapchain),
			UINT64_MAX,
			sem,
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
		if (!isVulkanInitialized)
		{
			ForceCloseMsg(ForceCloseType::FC_VU, "record command buffer");
			return false;
		}

		if (!device)
		{
			ForceCloseMsg(ForceCloseType::FC_D, "record command buffer");
			return false;
		}

		WindowStruct_Windows& winData = window->GetWindow_Windows();
		Window_VulkanData& vData = winData.vulkanData;

		VkFence fence = reinterpret_cast<VkFence>(vData.inFlightFences[currentFrame]);
		vkWaitForFences(
			device,
			1,
			&fence,
			VK_TRUE,
			UINT64_MAX);

		if (vkResetFences(
			device,
			1,
			&fence) != VK_SUCCESS)
		{
			LOG_ERROR("Failed to reset fence when recording command buffer!");
			return false;
		}

		VkCommandBuffer cmd = reinterpret_cast<VkCommandBuffer>(vData.commandBuffers[imageIndex]);

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
		renderPassInfo.renderPass = reinterpret_cast<VkRenderPass>(vData.renderPass);
		renderPassInfo.framebuffer = reinterpret_cast<VkFramebuffer>(vData.framebuffers[imageIndex]);
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent.width = vData.swapchainExtentWidth;
		renderPassInfo.renderArea.extent.height = vData.swapchainExtentHeight;
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
		if (!isVulkanInitialized)
		{
			ForceCloseMsg(ForceCloseType::FC_VU, "submit frame");
			return false;
		}

		if (!device)
		{
			ForceCloseMsg(ForceCloseType::FC_D, "submit frame");
			return false;
		}

		if (!graphicsQueue)
		{
			ForceCloseMsg(ForceCloseType::FC_GQ, "submit frame");
			return false;
		}

		WindowStruct_Windows& winData = window->GetWindow_Windows();
		Window_VulkanData& vData = winData.vulkanData;

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = 
		{ 
			reinterpret_cast<VkSemaphore>(vData.imageAvailableSemaphores[currentFrame]) 
		};
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		VkCommandBuffer realCB = reinterpret_cast<VkCommandBuffer>(vData.commandBuffers[imageIndex]);
		submitInfo.pCommandBuffers = &realCB;

		VkSemaphore signalSemaphores[] = 
		{ 
			reinterpret_cast<VkSemaphore>(vData.renderFinishedSemaphores[currentFrame])
		};
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		if (vkQueueSubmit(
			graphicsQueue,
			1,
			&submitInfo,
			reinterpret_cast<VkFence>(vData.inFlightFences[currentFrame])) != VK_SUCCESS)
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
		if (!isVulkanInitialized)
		{
			ForceCloseMsg(ForceCloseType::FC_VU, "present frame");
			return FrameResult::VK_FRAME_ERROR;
		}

		WindowStruct_Windows& winData = window->GetWindow_Windows();
		Window_VulkanData& vData = winData.vulkanData;

		VkSemaphore realFinishedSemaphore = 
			reinterpret_cast<VkSemaphore>(vData.renderFinishedSemaphores[currentFrame]);
		VkSwapchainKHR realSwapchain = 
			reinterpret_cast<VkSwapchainKHR>(vData.swapchain);

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &realFinishedSemaphore;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &realSwapchain;
		presentInfo.pImageIndices = &imageIndex;

		VkResult result = vkQueuePresentKHR(graphicsQueue, &presentInfo);

		currentFrame = (currentFrame + 1) % MAX_FRAMES;

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
		if (!isVulkanInitialized)
		{
			ForceCloseMsg(ForceCloseType::FC_VU, "hard reset");
			return;
		}

		if (!device)
		{
			ForceCloseMsg(ForceCloseType::FC_D, "hard reset");
			return;
		}

		vkDeviceWaitIdle(device);

		DestroySwapchain(window);
		CreateSwapchain(window);
		CreateRenderPass(window);
		CreateFramebuffers(window);
		CreateCommandBuffer(window);
	}

	void Renderer_Vulkan::SoftReset(
		Window* window,
		uint32_t imageIndex)
	{
		if (!isVulkanInitialized)
		{
			ForceCloseMsg(ForceCloseType::FC_VU, "soft reset");
			return;
		}

		if (!graphicsQueue)
		{
			ForceCloseMsg(ForceCloseType::FC_GQ, "soft reset");
			return;
		}

		WindowStruct_Windows& winData = window->GetWindow_Windows();
		Window_VulkanData& vData = winData.vulkanData;

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

		VkSemaphore realFinishedSemaphore =
			reinterpret_cast<VkSemaphore>(vData.renderFinishedSemaphores[currentFrame]);
		VkSwapchainKHR realSwapchain =
			reinterpret_cast<VkSwapchainKHR>(vData.swapchain);

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &realFinishedSemaphore;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &realSwapchain;
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
		if (!isVulkanInitialized)
		{
			ForceCloseMsg(ForceCloseType::FC_VU, "create render pass");
			return false;
		}

		if (!device)
		{
			ForceCloseMsg(ForceCloseType::FC_D, "create render pass");
			return false;
		}

		if (!graphicsQueue)
		{
			ForceCloseMsg(ForceCloseType::FC_GQ, "create render pass");
			return false;
		}

		WindowStruct_Windows& winData = window->GetWindow_Windows();
		Window_VulkanData& vData = winData.vulkanData;

		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = static_cast<VkFormat>(vData.swapchainImageFormat);
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

		VkRenderPass realRP = VK_NULL_HANDLE;

		if (vkCreateRenderPass(
			device,
			&renderPassInfo,
			nullptr,
			&realRP) != VK_SUCCESS)
		{
			LOG_ERROR("Failed to create Vulkan render pass!");
			return false;
		}

		vData.renderPass = reinterpret_cast<uintptr_t>(realRP);

		return true;
	}

	bool Renderer_Vulkan::CreateFramebuffers(Window* window)
	{
		if (!isVulkanInitialized)
		{
			ForceCloseMsg(ForceCloseType::FC_VU, "create framebuffers");
			return false;
		}

		WindowStruct_Windows& winData = window->GetWindow_Windows();
		Window_VulkanData& vData = winData.vulkanData;

		vData.framebuffers.resize(vData.imageViews.size());

		for (size_t i = 0; i < vData.imageViews.size(); ++i)
		{
			VkImageView attachments[] = { reinterpret_cast<VkImageView>(vData.imageViews[i]) };

			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = reinterpret_cast<VkRenderPass>(vData.renderPass);
			framebufferInfo.attachmentCount = 1;
			framebufferInfo.pAttachments = attachments;
			framebufferInfo.width = vData.swapchainExtentWidth;
			framebufferInfo.height = vData.swapchainExtentHeight;
			framebufferInfo.layers = 1;

			VkFramebuffer realFB = VK_NULL_HANDLE;
			if (vkCreateFramebuffer(
				device,
				&framebufferInfo,
				nullptr,
				&realFB) != VK_SUCCESS)
			{
				LOG_ERROR("Failed to create framebuffer for image " << i);
				return false;
			}

			vData.framebuffers[i] = reinterpret_cast<uintptr_t>(realFB);
		}

		return true;
	}

	//
	// REUSABLES
	//

	bool Renderer_Vulkan::CreateSwapchain(Window* window)
	{
		if (!isVulkanInitialized
			|| !instance)
		{
			ForceCloseMsg(ForceCloseType::FC_VU, "create swapchain");
			return false;
		}

		if (!device
			|| physicalDevice)
		{
			ForceCloseMsg(ForceCloseType::FC_D, "create swapchain");
			return false;
		}

		WindowStruct_Windows& winData = window->GetWindow_Windows();
		Window_VulkanData& vData = winData.vulkanData;

		//surface capabilities

#ifdef _WIN32
		WindowStruct_Windows& win = window->GetWindow_Windows();
#elif __linux__
		WindowStruct_X11& win = window->GetWindow_X11();
#endif
		VkSurfaceKHR surfacePtr = reinterpret_cast<VkSurfaceKHR>(vData.surface);

		VkSurfaceCapabilitiesKHR capabilities{};
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
			physicalDevice,
			(VkSurfaceKHR)surfacePtr,
			&capabilities);

		VkExtent2D extent = capabilities.currentExtent;
		vData.swapchainExtentWidth = extent.width;
		vData.swapchainExtentHeight = extent.height;

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
		uint32_t minImageCount = capabilities.minImageCount + 1;
		if (minImageCount > capabilities.maxImageCount) minImageCount = capabilities.maxImageCount;
		createInfo.minImageCount = minImageCount;
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

		VkSwapchainKHR realSC = VK_NULL_HANDLE;

		if (vkCreateSwapchainKHR(
			device,
			&createInfo,
			nullptr,
			&realSC) != VK_SUCCESS)
		{
			LOG_ERROR("Failed to create Vulkan swapchain!");
			return false;
		}

		vData.swapchain = reinterpret_cast<uintptr_t>(realSC);

		//get swapchain images

		uint32_t count = 0;
		vkGetSwapchainImagesKHR(
			device,
			reinterpret_cast<VkSwapchainKHR>(vData.swapchain),
			&count,
			nullptr);

		vector<VkImage> tempImages(count);
		vkGetSwapchainImagesKHR(
			device,
			reinterpret_cast<VkSwapchainKHR>(vData.swapchain),
			&count,
			tempImages.data());

		vData.images.resize(count);
		for (uint32_t i = 0; i < count; ++i)
		{
			vData.images[i] = reinterpret_cast<uintptr_t>(tempImages[i]);
		}

		//create image views

		vData.imageViews.resize(count);
		for (uint32_t i = 0; i < count; ++i)
		{
			VkImageViewCreateInfo viewInfo{};
			viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewInfo.image = reinterpret_cast<VkImage>(vData.images[i]);
			viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewInfo.format = static_cast<VkFormat>(vData.swapchainImageFormat);
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

			VkImageView realIV = VK_NULL_HANDLE;
			if (vkCreateImageView(
				device,
				&viewInfo,
				nullptr,
				&realIV) != VK_SUCCESS)
			{
				LOG_ERROR("Failed to create image view for swapchain image " << i);
				return false;
			}

			vData.imageViews[i] = reinterpret_cast<uintptr_t>(realIV);
		}

		LOG_SUCCESS("Successfully created Vulkan swapchain and released resources!");
		return true;
	}

	void Renderer_Vulkan::DestroySwapchain(Window* window)
	{
		WindowStruct_Windows& winData = window->GetWindow_Windows();
		Window_VulkanData& vData = winData.vulkanData;

		//destroy image views

		for (auto view : vData.imageViews)
		{
			if (view) vkDestroyImageView(device, reinterpret_cast<VkImageView>(view), nullptr);
		}

		//destroy framebuffers

		for (auto fb : vData.framebuffers)
		{
			if (fb) vkDestroyFramebuffer(device, reinterpret_cast<VkFramebuffer>(fb), nullptr);
		}

		//destroy render pass

		if (vData.renderPass)
		{
			vkDestroyRenderPass(device,  reinterpret_cast<VkRenderPass>(vData.renderPass), nullptr);
			vData.renderPass = NULL;
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

		vData.swapchain = NULL;
		vData.images.clear();
		vData.imageViews.clear();
		vData.framebuffers.clear();
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
				Window_VulkanData& vData = winData.vulkanData;

				//destroy runtime resources first
				DestroySwapchain(win);

				if (vData.commandPool)
				{
					vkDestroyCommandPool(
						device,
						reinterpret_cast<VkCommandPool>(vData.commandPool),
						nullptr);
					vData.commandPool = NULL;
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
void ForceCloseMsg(ForceCloseType fct, const string& targetMsg)
{
	if (fct == ForceCloseType::FC_VO)
	{
		ForceClose(
			"Vulkan error",
			"Cannot " + targetMsg + " because Volk failed to initialize!");
	}

	else if (fct == ForceCloseType::FC_VU)
	{
		ForceClose(
			"Vulkan error",
			"Cannot " + targetMsg + " because Vulkan is not initialized!");
	}

	else if (fct == ForceCloseType::FC_D)
	{
		ForceClose(
			"Vulkan error",
			"Cannot " + targetMsg + " because no valid device was assigned!");
	}

	else if (fct == ForceCloseType::FC_GQ)
	{
		ForceClose(
			"Vulkan error",
			"Cannot " + targetMsg + " because no graphics queue was assigned!");
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