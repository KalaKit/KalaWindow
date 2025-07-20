//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

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
#include "graphics/vulkan/extensions_vulkan.hpp"
#include "graphics/vulkan/shader_vulkan.hpp"
#include "graphics/render.hpp"
#include "core/log.hpp"

using KalaWindow::Graphics::Vulkan::Renderer_Vulkan;
using KalaWindow::Graphics::Vulkan::VulkanLayers;
using KalaWindow::Graphics::Vulkan::VulkanInstanceExtensions;
using KalaWindow::Graphics::Vulkan::VulkanDeviceExtensions;
using KalaWindow::Graphics::Vulkan::Extensions_Vulkan;
using KalaWindow::Graphics::Vulkan::Shader_Vulkan;
using KalaWindow::Graphics::Window;
using KalaWindow::Graphics::Render;
using KalaWindow::Graphics::PopupAction;
using KalaWindow::Graphics::PopupType;
using KalaWindow::Graphics::PopupResult;
using KalaWindow::Graphics::Vulkan::vulkanLayerInfo;
using KalaWindow::Graphics::Vulkan::vulkanInstanceExtensionsInfo;
using KalaWindow::Graphics::Vulkan::vulkanDeviceExtensionsInfo;
using KalaWindow::Graphics::ShutdownState;
using KalaWindow::Core::Logger;
using KalaWindow::Core::LogType;
using KalaWindow::Core::TimeFormat;
using KalaWindow::Core::DateFormat;

using std::string;
using std::to_string;
using std::vector;
using std::unordered_map;
using std::pair;
using std::uintptr_t;
using std::exception;

enum class ForceCloseType
{
	FC_VO, //is volk initialized
	FC_VU  //is vulkan initialized
};

static void ForceClose(
	const string& title, 
	const string& reason,
	ShutdownState state = ShutdownState::SHUTDOWN_FAILURE);

static bool IsValidHandle(
	uintptr_t handle,
	const string& variableName,
	const string& originFunction)
{
	if (handle == 0
		|| handle == UINTPTR_MAX)
	{
		ForceClose(
			"Vulkan critical error [vulkan]",
			"[ " + originFunction + " ]"
			"\nVariable '" + variableName + "' value '" + to_string(handle) + "' is invalid!",
			ShutdownState::SHUTDOWN_CRITICAL);
		return false;
	}
	return true;
}

template<typename T>
static bool IsValidIndex(
	uint32_t index,
	const vector<T>& targetVector,
	const string& variableName,
	const string& originFunction)
{
	if (targetVector.empty())
	{
		ForceClose(
			"Vulkan critical error [vulkan]",
			"[ " + originFunction + " ]"
			"\nVector for variable '" + variableName + "' is empty!",
			ShutdownState::SHUTDOWN_CRITICAL);
		return false;
	}

	if (index == UINT32_MAX)
	{
		ForceClose(
			"Vulkan critical error [vulkan]",
			"[ " + originFunction + " ]"
			"\nVariable '" + variableName + "' value '" + to_string(index) + "' is invalid!",
			ShutdownState::SHUTDOWN_CRITICAL);
		return false;
	}
	if (index >= targetVector.size())
	{
		ForceClose(
			"Vulkan critical error [vulkan]",
			"[ " + originFunction + " ]"
			"\nVariable '" + variableName + "' value '" + to_string(index) + "' is out of range!"
			" Size was '" + to_string(targetVector.size()) + "'.",
			ShutdownState::SHUTDOWN_CRITICAL);
		return false;
	}
	return true;
}

static const char* ToString(VulkanLayers layer);
static const char* ToString(VulkanInstanceExtensions ext);
static const char* ToString(VulkanDeviceExtensions ext);
static int RatePhysicalDevice(
	VkPhysicalDevice device,
	string& failReason);

static bool isVolkInitialized = false;

static VkQueue graphicsQueue = VK_NULL_HANDLE;
static uint32_t graphicsQueueFamilyIndex = 0;

static uint32_t MAX_FRAMES = 0;
static uint32_t currentFrame = 0;

static vector<VulkanDeviceExtensions> delayedExt{};

static bool InitVolk()
{
	if (isVolkInitialized) return true;

	if (volkInitialize() != VK_SUCCESS)
	{
		ForceClose(
			"Vulkan initialization error [Vulkan]",
			"Volk initialization failed!");
		return false;
	}

	return true;
}

namespace KalaWindow::Graphics::Vulkan
{
	function<void()> Renderer_Vulkan::DrawCommands = nullptr;

	//
	// INITIALIZE PHASE
	//

	bool Renderer_Vulkan::EnableLayer(VulkanLayers layer)
	{
		const char* name = ToString(layer);

		if (!isVolkInitialized
			&& !InitVolk())
		{
			Logger::Print(
				"Cannot enable layer '" + string(name) + "' because Vulkan is not initialized!",
				"VULKAN",
				LogType::LOG_ERROR);
			return false;
		}

		if (find(
			enabledLayers.begin(),
			enabledLayers.end(),
			layer) != enabledLayers.end())
		{
			Logger::Print(
				"Can not enable layer '" + string(name) + "' because it is already enabled!",
				"VULKAN",
				LogType::LOG_ERROR,
				2);
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

				Logger::Print(
					"Enabled layer '" + string(name) + "'!",
					"VULKAN",
					LogType::LOG_SUCCESS);
				return true;
			}
		}

		Logger::Print(
			"Can not enable layer '" + string(name) + "' because it is not supported on this system!",
			"VULKAN",
			LogType::LOG_ERROR,
			2);
		return false;
	}

	bool Renderer_Vulkan::EnableInstanceExtension(VulkanInstanceExtensions ext)
	{
		const char* name = ToString(ext);

		if (!isVolkInitialized
			&& !InitVolk())
		{
			Logger::Print(
				"Cannot enable instance extension '" + string(name) + "' because Vulkan is not initialized!",
				"VULKAN",
				LogType::LOG_ERROR,
				2);
			return false;
		}

		if (find(
			enabledInstanceExtensions.begin(),
			enabledInstanceExtensions.end(),
			ext) != enabledInstanceExtensions.end())
		{
			Logger::Print(
				"Can not enable instance extension '" + string(name) + "' because it is already enabled!",
				"VULKAN",
				LogType::LOG_ERROR,
				2);
			return false;
		}

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
				enabledInstanceExtensions.push_back(ext);
				Logger::Print(
					"Enabled instance extension '" + string(name) + "'!",
					"VULKAN",
					LogType::LOG_SUCCESS);
				return true;
			}
		}

		Logger::Print(
			"Instance extension '" + string(name) + "' not supported on this system!",
			"VULKAN",
			LogType::LOG_ERROR,
			2);
		return false;
	}

	bool Renderer_Vulkan::EnableDeviceExtension(VulkanDeviceExtensions ext)
	{
		const char* name = ToString(ext);

		if (!isVolkInitialized
			&& !InitVolk())
		{
			Logger::Print(
				"Cannot enable device extension '" + string(name) + "' because Vulkan is not initialized!",
				"VULKAN",
				LogType::LOG_ERROR,
				2);
			return false;
		}

		if (find(
			enabledDeviceExtensions.begin(),
			enabledDeviceExtensions.end(),
			ext) != enabledDeviceExtensions.end())
		{
			Logger::Print(
				"Can not enable device extension '" + string(name) + "' because it is already enabled!",
				"VULKAN",
				LogType::LOG_ERROR,
				2);
			return false;
		}
		if (find(
			delayedExt.begin(),
			delayedExt.end(),
			ext) != delayedExt.end())
		{
			Logger::Print(
				"Can not enable device extension '" + string(name) + "' because it is already assigned as a delayed extension!",
				"VULKAN",
				LogType::LOG_ERROR,
				2);
			return false;
		}

		delayedExt.push_back(ext);
		Logger::Print(
			"Queued device extension '" + string(name) + "' for delayed enable.",
			"VULKAN",
			LogType::LOG_INFO);
		return true;
	}

	bool Renderer_Vulkan::Initialize(uint32_t max_frames)
	{
		if (max_frames < 2
			|| max_frames > 3)
		{
			ForceClose(
				"Vulkan initialization error [vulkan]",
				"Cannot initialize Vulkan because max_frames was not set to 2 or 3!");
			return false;
		}
		MAX_FRAMES = max_frames;

		if (!isVolkInitialized
			&& !InitVolk())
		{
			Logger::Print(
				"Cannot initialize Vulkan because Volk is not initialized!",
				"VULKAN",
				LogType::LOG_ERROR,
				2);
			return false;
		}

		vector<const char*> layerNames{};
		for (const auto& layer : enabledLayers)
		{
			const char* str = ToString(layer);
			layerNames.push_back(str);
		}

		vector<const char*> instanceExtensions{};
		for (const auto& extension : enabledInstanceExtensions)
		{
			const char* name = ToString(extension);
			instanceExtensions.push_back(name);
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

		VkInstance newInstance = VK_NULL_HANDLE;

		if (vkCreateInstance(
			&instanceCreateInfo,
			nullptr,
			&newInstance) != VK_SUCCESS)
		{
			ForceClose(
				"Vulkan initialization error [vulkan]",
				"Failed to create Vulkan instance!");
			return false;
		}

		volkLoadInstance(newInstance);

		//enumerate and pick physical device

		uint32_t gpuCount = 0;
		vkEnumeratePhysicalDevices(
			newInstance,
			&gpuCount, 
			nullptr);
		if (gpuCount == 0)
		{
			ForceClose(
				"Vulkan initialization error [vulkan]",
				"No Vulkan-compatible physical devices found!");
			return false;
		}

		instance = FromVar<VkInstance>(newInstance);

		vector<VkPhysicalDevice> devices(gpuCount);
		vkEnumeratePhysicalDevices(
			newInstance,
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
				Logger::Print(
					"Device '" +
					string(props.deviceName) +
					"' was not picked because: " 
					+ failReason,
					"VULKAN",
					LogType::LOG_WARNING);
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
				"Vulkan initialization error [vulkan]",
				"No suitable physical GPU found!");
			return false;
		}

		physicalDevice = FromVar<VkPhysicalDevice>(bestDevice);

		vector<const char*> deviceExtensions{};
		for (const auto& ext : delayedExt)
		{
			const char* name = ToString(ext);
			if (name) deviceExtensions.push_back(name);
		}

		//find graphics queue

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(
			ToVar<VkPhysicalDevice>(physicalDevice),
			&queueFamilyCount,
			nullptr);
		vector<VkQueueFamilyProperties> families(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(
			ToVar<VkPhysicalDevice>(physicalDevice),
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
				"Vulkan initialization error [vulkan]",
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

		VkDevice newDevice = VK_NULL_HANDLE;

		if (vkCreateDevice(
			ToVar<VkPhysicalDevice>(physicalDevice),
			&deviceCreateInfo,
			nullptr,
			&newDevice) != VK_SUCCESS)
		{
			ForceClose(
				"Vulkan initialization error [vulkan]",
				"Failed to create logical device!");
			return false;
		}

		volkLoadDevice(newDevice);
		vkGetDeviceQueue(
			newDevice,
			graphicsQueueFamilyIndex,
			0,
			&graphicsQueue);

		device = FromVar<VkDevice>(newDevice);

		isVulkanInitialized = true;
		Logger::Print("Initialized Vulkan!",
			"VULKAN",
			LogType::LOG_SUCCESS);

		return true;
	}

	bool Renderer_Vulkan::CreateCommandPool(Window* window)
	{
		if (!isVulkanInitialized)
		{
			Logger::Print(
				"Cannot create command pool because Vulkan is not initialized!",
				"VULKAN",
				LogType::LOG_ERROR,
				2);
			return false;
		}

		if (!IsValidHandle(
			device,
			"device",
			"CreateCommandPool"))
		{
			return false;
		}

		WindowStruct_Windows& winData = window->GetWindow_Windows();
		Window_VulkanData vData{};

		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.queueFamilyIndex = graphicsQueueFamilyIndex;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

		VkCommandPool realPool = VK_NULL_HANDLE;
		if (vkCreateCommandPool(
			ToVar<VkDevice>(device),
			&poolInfo,
			nullptr,
			&realPool) != VK_SUCCESS)
		{
			Logger::Print(
				"Failed to create Vulkan command pool!",
				"VULKAN",
				LogType::LOG_ERROR,
				2);
			return false;
		}
		vData.commandPool = FromVar<VkCommandPool>(realPool);
		window->SetVulkanStruct(vData);

		return true;
	}

	bool Renderer_Vulkan::CreateCommandBuffer(Window* window)
	{
		if (!isVulkanInitialized)
		{
			Logger::Print(
				"Cannot create command buffer because Vulkan is not initialized!",
				"VULKAN",
				LogType::LOG_ERROR,
				2);
			return false;
		}

		if (!IsValidHandle(
			device,
			"device",
			"CreateCommandBuffer"))
		{
			return false;
		}

		WindowStruct_Windows& winData = window->GetWindow_Windows();
		Window_VulkanData& vData = window->GetVulkanStruct();

		if (!IsValidIndex(
			0,
			vData.images,
			"vData images",
			"CreateCommandBuffer"))
		{
			return false;
		}

		if (!IsValidHandle(
			vData.commandPool,
			"commandPool",
			"CreateCommandBuffer"))
		{
			return false;
		}

		vData.commandBuffers.resize(MAX_FRAMES);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = ToVar<VkCommandPool>(vData.commandPool);
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = static_cast<uint32_t>(vData.commandBuffers.size());

		vector<VkCommandBuffer> tempCB(allocInfo.commandBufferCount);
		if (vkAllocateCommandBuffers(
			ToVar<VkDevice>(device),
			&allocInfo,
			tempCB.data()) != VK_SUCCESS)
		{
			Logger::Print(
				"Failed to allocate command buffers!",
				"VULKAN",
				LogType::LOG_ERROR,
				2);
			return false;
		}

		for (uint32_t i = 0; i < allocInfo.commandBufferCount; ++i)
		{
			uintptr_t handle = FromVar<VkCommandBuffer>(tempCB[i]);
			if (!IsValidHandle(
				handle, 
				"commandBuffer[" + to_string(i) + "]", 
				"CreateCommandBuffer"))
			{
				return false;
			}
			vData.commandBuffers[i] = handle;
 		}

		return true;
	}

	bool Renderer_Vulkan::CreateSyncObjects(Window* window)
	{
		if (!isVulkanInitialized)
		{
			Logger::Print(
				"Cannot create sync objects because Vulkan is not initialized!",
				"VULKAN",
				LogType::LOG_ERROR,
				2);
			return false;
		}

		if (!IsValidHandle(
			device,
			"device",
			"CreateSyncObjects"))
		{
			return false;
		}

		WindowStruct_Windows& winData = window->GetWindow_Windows();
		Window_VulkanData& vData = window->GetVulkanStruct();

		if (!IsValidIndex(
			0,
			vData.images,
			"vData images",
			"CreateSyncObjects"))
		{
			return false;
		}

		vData.imageAvailableSemaphores.resize(MAX_FRAMES);
		vData.inFlightFences.resize(MAX_FRAMES);

		size_t imgCount = vData.images.size();
		vData.renderFinishedSemaphores.resize(imgCount);

		VkSemaphoreCreateInfo semInfo{ VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
		VkFenceCreateInfo fenceInfo{ VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (uint32_t i = 0; i < MAX_FRAMES; ++i)
		{
			VkSemaphore realAvailableSemaphore = VK_NULL_HANDLE;
			VkSemaphore realFinishedSemaphore = VK_NULL_HANDLE;
			VkFence realFence = VK_NULL_HANDLE;

			if (vkCreateSemaphore(
					ToVar<VkDevice>(device),
					&semInfo,
					nullptr,
					&realAvailableSemaphore)
				!= VK_SUCCESS
				|| vkCreateFence(
					ToVar<VkDevice>(device),
					&fenceInfo,
					nullptr,
					&realFence)
				!= VK_SUCCESS)
			{
				Logger::Print(
					"Failed to create per-frame sync objects for frame " + i,
					"VULKAN",
					LogType::LOG_ERROR,
					2);
				DestroySyncObjects(window);
				return false;
			}

			vData.imageAvailableSemaphores[i] = 
				FromVar<VkSemaphore>(realAvailableSemaphore);
			vData.inFlightFences[i] = 
				FromVar<VkFence>(realFence);
		}

		for (size_t i = 0; i < imgCount; ++i)
		{
			VkSemaphore realFinishedSemaphore = VK_NULL_HANDLE;
			if (vkCreateSemaphore(
				ToVar<VkDevice>(device),
				&semInfo,
				nullptr,
				&realFinishedSemaphore) != VK_SUCCESS)
			{
				Logger::Print(
					"Failed to create render-finished semaphore for image" + i,
					"VULKAN",
					LogType::LOG_ERROR,
					2);
				DestroySyncObjects(window);
				return false;
			}
			vData.renderFinishedSemaphores[i] = 
				FromVar<VkSemaphore>(realFinishedSemaphore);
		}

		return true;
	}

	void Renderer_Vulkan::DestroySyncObjects(Window* window)
	{
		WindowStruct_Windows& winData = window->GetWindow_Windows();
		Window_VulkanData& vData = window->GetVulkanStruct();

		if (device)
		{
			VkDevice d = ToVar<VkDevice>(device);

			vkDeviceWaitIdle(d);

			for (auto s : vData.imageAvailableSemaphores)
			{
				vkDestroySemaphore(d, ToVar<VkSemaphore>(s), nullptr);
			}
			for (auto s : vData.renderFinishedSemaphores)
			{
				vkDestroySemaphore(d, ToVar<VkSemaphore>(s), nullptr);
			}
			for (auto f : vData.inFlightFences)
			{
				vkDestroyFence(d, ToVar<VkFence>(f), nullptr);
			}
		}

		vData.imageAvailableSemaphores.clear();
		vData.renderFinishedSemaphores.clear();
		vData.inFlightFences.clear();
		vData.imagesInFlight.clear();
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
			Logger::Print(
				"Cannot begin frame because Vulkan is not initialized!",
				"VULKAN",
				LogType::LOG_ERROR,
				2);
			return FrameResult::VK_FRAME_ERROR;
		}

		if (!IsValidHandle(
			device,
			"device",
			"BeginFrame"))
		{
			return FrameResult::VK_FRAME_ERROR;
		}

		VkDevice d = ToVar<VkDevice>(device);

		WindowStruct_Windows& winData = window->GetWindow_Windows();
		Window_VulkanData& vData = window->GetVulkanStruct();
		{
			VkFence fence = ToVar<VkFence>(vData.inFlightFences[currentFrame]);

			VkResult waitResult = vkWaitForFences(
				d,
				1,
				&fence,
				VK_TRUE,
				UINT64_MAX);

			vkResetFences(
				d,
				1,
				&fence);
		}

		uint32_t nextImage = UINT32_MAX;

		VkSemaphore sem = 
			ToVar<VkSemaphore>(vData.imageAvailableSemaphores[currentFrame]);

		VkResult result = vkAcquireNextImageKHR(
			d,
			ToVar<VkSwapchainKHR>(vData.swapchain),
			UINT64_MAX,
			sem,
			VK_NULL_HANDLE,
			&nextImage);

		if (result == VK_SUCCESS)
		{
			if (!IsValidIndex(
				nextImage,
				vData.images,
				"nextImage",
				"BeginFrame"))
			{
				return FrameResult::VK_FRAME_ERROR;
			}

			imageIndex = nextImage;

			VkFence inFlight = ToVar<VkFence>(vData.imagesInFlight[imageIndex]);
			if (inFlight != VK_NULL_HANDLE
				&& inFlight != ToVar<VkFence>(vData.inFlightFences[currentFrame]))
			{
				vkWaitForFences(
					d,
					1,
					&inFlight,
					VK_TRUE,
					UINT64_MAX);
			}

			vData.imagesInFlight[imageIndex] = vData.inFlightFences[currentFrame];

			return FrameResult::VK_FRAME_OK;
		}

		if (result == VK_SUBOPTIMAL_KHR
			|| result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			return FrameResult::VK_FRAME_RESIZE_NEEDED;
		}

		Logger::Print(
			"vkAquireNextImageKHR failed with error: " + result,
			"VULKAN",
			LogType::LOG_ERROR,
			2);
		return FrameResult::VK_FRAME_ERROR;
	}

	bool Renderer_Vulkan::RecordCommandBuffer(
		Window* window,
		uint32_t imageIndex)
	{
		if (!isVulkanInitialized)
		{
			Logger::Print(
				"Cannot record command buffer because Vulkan is not initialized!",
				"VULKAN",
				LogType::LOG_ERROR,
				2);
			return false;
		}

		WindowStruct_Windows& winData = window->GetWindow_Windows();
		Window_VulkanData& vData = window->GetVulkanStruct();

		VkFence fence = ToVar<VkFence>(vData.inFlightFences[currentFrame]);

		if (!IsValidIndex(
			currentFrame,
			vData.commandBuffers,
			"imageIndex",
			"RecordCommandBuffer"))
		{
			return false;
		}
		VkCommandBuffer cmd = ToVar<VkCommandBuffer>(vData.commandBuffers[currentFrame]);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		if (vkResetCommandBuffer(cmd, 0) != VK_SUCCESS)
		{
			Logger::Print(
				"Failed to reset command buffer before recording!",
				"VULKAN",
				LogType::LOG_ERROR,
				2);
			return false;
		}

		if (vkBeginCommandBuffer(cmd, &beginInfo) != VK_SUCCESS)
		{
			Logger::Print(
				"Failed to begin recording command buffer!",
				"VULKAN",
				LogType::LOG_ERROR,
				2);
			return false;
		}

		VkClearValue clearColor = { {{ 0.0f, 0.0f, 0.0f, 1.0f }} };

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = ToVar<VkRenderPass>(vData.renderPass);
		renderPassInfo.framebuffer = ToVar<VkFramebuffer>(vData.framebuffers[imageIndex]);
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent.width = vData.swapchainExtentWidth;
		renderPassInfo.renderArea.extent.height = vData.swapchainExtentHeight;
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(
			cmd,
			&renderPassInfo,
			VK_SUBPASS_CONTENTS_INLINE);

		try
		{
			if (DrawCommands)
			{
				Logger::Print(
					"User draw commands start.",
					"VULKAN",
					LogType::LOG_DEBUG);

				DrawCommands();

				Logger::Print(
					"User draw commands end.",
					"VULKAN",
					LogType::LOG_ERROR);
			}
		}
		catch (const exception& e)
		{
			Logger::Print(
				"Error during DrawCommands: " + string(e.what()),
				"VULKAN",
				LogType::LOG_ERROR,
				2);
		}

		vkCmdEndRenderPass(cmd);

		if (vkEndCommandBuffer(cmd) != VK_SUCCESS)
		{
			Logger::Print(
				"Failed to record command buffer!",
				"VULKAN",
				LogType::LOG_ERROR,
				2);
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
			Logger::Print(
				"Cannot submit frame because Vulkan is not initialized!",
				"VULKAN",
				LogType::LOG_ERROR,
				2);
			return false;
		}

		if (!IsValidHandle(
			FromVar<VkQueue>(graphicsQueue),
			"graphicsQueue",
			"SubmitFrame"))
		{
			return false;
		}

		WindowStruct_Windows& winData = window->GetWindow_Windows();
		Window_VulkanData& vData = window->GetVulkanStruct();

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = 
		{ 
			ToVar<VkSemaphore>(vData.imageAvailableSemaphores[currentFrame])
		};
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;

		VkCommandBuffer realCB = ToVar<VkCommandBuffer>(vData.commandBuffers[currentFrame]);
		submitInfo.pCommandBuffers = &realCB;

		VkSemaphore signalSemaphores[] = 
		{ 
			ToVar<VkSemaphore>(vData.renderFinishedSemaphores[imageIndex])
		};
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		if (vkQueueSubmit(
			graphicsQueue,
			1,
			&submitInfo,
			ToVar<VkFence>(vData.inFlightFences[currentFrame])) != VK_SUCCESS)
		{
			Logger::Print(
				"Failed to submit frame!",
				"VULKAN",
				LogType::LOG_ERROR,
				2);
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
			Logger::Print(
				"Cannot present frame because Vulkan is not initialized!",
				"VULKAN",
				LogType::LOG_ERROR,
				2);
			return FrameResult::VK_FRAME_ERROR;
		}

		if (!IsValidHandle(
			FromVar<VkQueue>(graphicsQueue),
			"graphicsQueue",
			"PresentFrame"))
		{
			return FrameResult::VK_FRAME_ERROR;
		}

		WindowStruct_Windows& winData = window->GetWindow_Windows();
		Window_VulkanData& vData = window->GetVulkanStruct();

		VkSemaphore realFinishedSemaphore = 
			ToVar<VkSemaphore>(vData.renderFinishedSemaphores[imageIndex]);
		VkSwapchainKHR realSwapchain = 
			ToVar<VkSwapchainKHR>(vData.swapchain);

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

		Logger::Print(
			"vkQueuePresentKHR failed with error: " + result,
			"VULKAN",
			LogType::LOG_ERROR,
			2);
		return FrameResult::VK_FRAME_ERROR;
	}

	void Renderer_Vulkan::HardReset(Window* window)
	{
		if (!isVulkanInitialized)
		{
			Logger::Print(
				"Cannot hard reset because Vulkan is not initialized!",
				"VULKAN",
				LogType::LOG_ERROR,
				2);
			return;
		}

		if (!IsValidHandle(
			device,
			"device",
			"HardReset"))
		{
			return;
		}

		WindowStruct_Windows& winData = window->GetWindow_Windows();
		Window_VulkanData& vData = window->GetVulkanStruct();

		vkDeviceWaitIdle(ToVar<VkDevice>(device));

		DestroySyncObjects(window);
		Extensions_Vulkan::DestroySwapchain(window);
		vkDestroyCommandPool(
			ToVar<VkDevice>(device),
			ToVar<VkCommandPool>(vData.commandPool),
			nullptr);
		vData.commandPool = 0;

		if (!Extensions_Vulkan::CreateSwapchain(window))
		{
			ForceClose(
				"Vulkan error [vulkan]",
				"Hard reset failed because of CreateSwapchain!");
			return;
		}

		if (!CreateRenderPass(window))
		{
			ForceClose(
				"Vulkan error [vulkan]",
				"Hard reset failed because of CreateRenderPass!");
			return;
		}
		if (!CreateFramebuffers(window))
		{
			ForceClose(
				"Vulkan error [vulkan]",
				"Hard reset failed because of CreateFramebuffers!");
			return;
		}
		if (!CreateCommandPool(window))
		{
			ForceClose(
				"Vulkan error [vulkan]",
				"Hard reset failed because of CreateCommandPool!");
			return;
		}
		if (!CreateCommandBuffer(window))
		{
			ForceClose(
				"Vulkan error [vulkan]",
				"Hard reset failed because of CreateCommandBuffer!");
			return;
		}
		if (!CreateSyncObjects(window))
		{
			ForceClose(
				"Vulkan error [vulkan]",
				"Hard reset failed because of CreateSyncObjects!");
			return;
		}
	}

	//
	// REMAKE PHASE
	//

	bool Renderer_Vulkan::CreateRenderPass(Window* window)
	{
		if (!isVulkanInitialized)
		{
			Logger::Print(
				"Cannot create render pass because Vulkan is not initialized!",
				"VULKAN",
				LogType::LOG_ERROR,
				2);
			return false;
		}

		if (!IsValidHandle(
			device,
			"device",
			"CreateRenderPass"))
		{
			return false;
		}

		if (!IsValidHandle(
			FromVar<VkQueue>(graphicsQueue),
			"graphicsQueue",
			"CreateRenderPass"))
		{
			return false;
		}

		WindowStruct_Windows& winData = window->GetWindow_Windows();
		Window_VulkanData& vData = window->GetVulkanStruct();

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
			ToVar<VkDevice>(device),
			&renderPassInfo,
			nullptr,
			&realRP) != VK_SUCCESS)
		{
			Logger::Print(
				"Failed to create Vulkan render pass!",
				"VULKAN",
				LogType::LOG_ERROR,
				2);
			return false;
		}

		vData.renderPass = FromVar<VkRenderPass>(realRP);

		return true;
	}

	bool Renderer_Vulkan::CreateFramebuffers(Window* window)
	{
		if (!isVulkanInitialized)
		{
			Logger::Print(
				"Cannot create framebuffers because Vulkan is not initialized!",
				"VULKAN",
				LogType::LOG_ERROR,
				2);
			return false;
		}

		if (!IsValidHandle(
			device,
			"device",
			"CreateFramebuffers"))
		{
			return false;
		}

		WindowStruct_Windows& winData = window->GetWindow_Windows();
		Window_VulkanData& vData = window->GetVulkanStruct();

		vData.framebuffers.resize(vData.imageViews.size());

		for (size_t i = 0; i < vData.imageViews.size(); ++i)
		{
			VkImageView attachments[] = { ToVar<VkImageView>(vData.imageViews[i]) };

			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = ToVar<VkRenderPass>(vData.renderPass);
			framebufferInfo.attachmentCount = 1;
			framebufferInfo.pAttachments = attachments;
			framebufferInfo.width = vData.swapchainExtentWidth;
			framebufferInfo.height = vData.swapchainExtentHeight;
			framebufferInfo.layers = 1;

			VkFramebuffer realFB = VK_NULL_HANDLE;
			if (vkCreateFramebuffer(
				ToVar<VkDevice>(device),
				&framebufferInfo,
				nullptr,
				&realFB) != VK_SUCCESS)
			{
				Logger::Print(
					"Failed to create framebuffer for image " + i,
					"VULKAN",
					LogType::LOG_ERROR,
					2);
				return false;
			}

			vData.framebuffers[i] = FromVar<VkFramebuffer>(realFB);
		}

		return true;
	}

	//
	// REUSABLES
	//

	void Renderer_Vulkan::DestroyWindowData(Window* window)
	{
		if (device) vkDeviceWaitIdle(ToVar<VkDevice>(device));

		WindowStruct_Windows& winData = window->GetWindow_Windows();
		Window_VulkanData& vData = window->GetVulkanStruct();

		DestroySyncObjects(window);
		Extensions_Vulkan::DestroySwapchain(window);

		if (vData.commandPool
			&& device)
		{
			vkDestroyCommandPool(
				ToVar<VkDevice>(device),
				ToVar<VkCommandPool>(vData.commandPool),
				nullptr);
			vData.commandPool = NULL;
		}

		if (vData.surface
			&& instance)
		{
			vkDestroySurfaceKHR(
				ToVar<VkInstance>(instance),
				ToVar<VkSurfaceKHR>(vData.surface),
				nullptr);
		}

		vData.commandBuffers.clear();
		vData.framebuffers.clear();
		vData.images.clear();
		vData.imageViews.clear();
	}

	void Renderer_Vulkan::Shutdown()
	{
		if (device) vkDeviceWaitIdle(ToVar<VkDevice>(device));

		for (const auto& window : Window::windows)
		{
			Window* win = window;
			DestroyWindowData(win);
		}

		Shader_Vulkan::createdShaders.clear();

		if (device)
		{
			vkDestroyDevice(ToVar<VkDevice>(device), nullptr);
			device = NULL;
		}

		if (instance)
		{
			vkDestroyInstance(ToVar<VkInstance>(instance), nullptr);
			instance = NULL;
		}

		enabledLayers.clear();
		enabledInstanceExtensions.clear();
		enabledDeviceExtensions.clear();
	}
}

static void ForceClose(
	const string& title,
	const string& reason,
	ShutdownState state)
{
	Logger::Print(
		reason,
		"VULKAN",
		LogType::LOG_ERROR,
		2,
		TimeFormat::TIME_NONE,
		DateFormat::DATE_NONE);

	Window* mainWindow = Window::windows.front();
	if (mainWindow->CreatePopup(
		title,
		reason,
		PopupAction::POPUP_ACTION_OK,
		PopupType::POPUP_TYPE_ERROR)
		== PopupResult::POPUP_RESULT_OK)
	{
		Render::Shutdown(ShutdownState::SHUTDOWN_FAILURE);
	}
}

static const char* ToString(VulkanLayers layer)
{
	auto it = vulkanLayerInfo.find(layer);
	return it != vulkanLayerInfo.end() ? it->second : nullptr;
}

static const char* ToString(VulkanInstanceExtensions ext)
{
	auto it = vulkanInstanceExtensionsInfo.find(ext);
	return it != vulkanInstanceExtensionsInfo.end() ? it->second : nullptr;
}

static const char* ToString(VulkanDeviceExtensions ext)
{
	auto it = vulkanDeviceExtensionsInfo.find(ext);
	return it != vulkanDeviceExtensionsInfo.end() ? it->second : nullptr;
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