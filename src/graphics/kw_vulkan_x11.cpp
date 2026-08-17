
//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef __linux__

#include <X11/Xlib.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>

#include <memory>
#include <filesystem>

#define VK_USE_PLATFORM_XLIB_KHR
#include "vulkan/vulkan_core.h"
#include "vulkan/vulkan_xlib.h"

#include "log_utils.hpp"

#include "graphics/kw_vulkan.hpp"
#include "graphics/kw_window.hpp"
#include "graphics/kw_window_global.hpp"
#include "core/kw_core.hpp"
#include "core/kw_crash.hpp"

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;

using KalaHeaders::KalaCore::ToVar;

using KalaWindow::Core::KalaWindowCore;
using KalaWindow::Core::CrashHandler;
using KalaWindow::Graphics::ProcessWindow;
using KalaWindow::Graphics::WindowData;
using KalaWindow::Graphics::Window_Global;
using KalaWindow::Graphics::X11GlobalData;

using std::string;
using std::string_view;
using std::to_string;
using std::unique_ptr;
using std::make_unique;
using std::filesystem::path;

static bool isInitialized{};
static bool isVerboseLoggingEnabled{};

static VkInstance instance{};
static VkDebugUtilsMessengerEXT debugMessenger{};

static bool ContainsCrashWorthyError(string_view message)
{
	//required parameter was VK_NULL_HANDLE
	if (message.find("parameter") != string::npos
		&& message.find("VK_NULL_HANDLE") != string::npos
		&& message.find("must be a valid") != string::npos)
	{
		return true;
	}

	//accessing destroyed/freed objects - guaranteed device loss soon
	if (message.find("that has been destroyed") != string::npos
		|| message.find("that has been freed") != string::npos)
	{
		return true;
	}

	//synchronization violations that corrupt GPU state
	if (message.find("was submitted before") != string::npos
		&& message.find("had been signaled") != string::npos)
	{
		return true;
	}

	//invalid image/buffer layout transitions - memory corruption
	if (message.find("cannot transition layouts") != string::npos)
	{
		return true;
	}

	return false;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT severity,
	VkDebugUtilsMessageTypeFlagsEXT type,
	const VkDebugUtilsMessengerCallbackDataEXT* data,
	void* userData)
{
	if ((severity == VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
		|| (type & VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT))
		&& !isVerboseLoggingEnabled)
	{
		return VK_FALSE;
	}

	LogType logType{};
	switch (severity)
	{
	case VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
	{
		logType = LogType::LOG_VERBOSE;
		break;
	}
	case VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
	{
		logType = LogType::LOG_INFO;
		break;
	}
	case VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
	{
		logType = LogType::LOG_WARNING;
		break;
	}
	case VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
	{
		logType = LogType::LOG_ERROR;
		break;
	}
	default: break;
	}

	string logTarget = "KW_VULKAN_LOG_";
	if (type & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT)                logTarget += "GENERAL";
	if (type & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT)             logTarget += "VALIDATION";
	if (type & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)            logTarget += "PERFORMANCE";
	if (type & VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT) logTarget += "DEVICE_ADDRESS";

	if (severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT
		&& ContainsCrashWorthyError(data->pMessage))
	{
		KalaWindowCore::ForceClose(
			"KalaWindow Vulkan error",
			data->pMessage);
	}

	Log::Print(
		data->pMessage,
		logTarget,
		logType);

	return VK_FALSE;
}

namespace KalaWindow::Graphics
{
    //
	// GLOBAL
	//

	static KalaWindowRegistry<VulkanContext> registry{};

	KalaWindowRegistry<VulkanContext>& VulkanContext::GetRegistry() { return registry; }

	bool VulkanContext::IsVerboseLoggingEnabled() { return isVerboseLoggingEnabled; }
	void VulkanContext::SetVerboseLoggingState(bool newState) { isVerboseLoggingEnabled = newState; }

    VkInstance VulkanContext::GetInstance()
    {
        if (!isInitialized)
		{
			Log::Print(
				"Failed to get Vulkan instance because global Vulkan has not been initialized!",
				"KW_VULKAN",
				LogType::LOG_ERROR,
				2);
		}

        return instance;
    }

	void VulkanContext::Initialize(
		string&& appName,
		vector<string>&& extensions)
    {
		CrashHandler::Initialize(string(appName));

        u32 version{};

        if (vkEnumerateInstanceVersion(&version) != VK_SUCCESS
            || version < VK_API_VERSION_1_4)
        {
			KalaWindowCore::ForceClose(
				"KalaWindow Vulkan error",
				"Vulkan 1.4 is not supported on this system!");

			return;
        }

        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;

        appInfo.pApplicationName = appName.c_str();
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);

        appInfo.pEngineName = appName.c_str();
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_4;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        vector<const char*> finalExtensions{};
        finalExtensions.reserve(extensions.size() + 3);
        for (const auto& e : extensions)
        {
            finalExtensions.push_back(e.c_str());
        }

        finalExtensions.push_back("VK_KHR_surface");
        finalExtensions.push_back("VK_KHR_xlib_surface");
        finalExtensions.push_back("VK_EXT_debug_utils");

        createInfo.enabledExtensionCount = scast<u32>(finalExtensions.size());
        createInfo.ppEnabledExtensionNames = finalExtensions.data();

		VkDebugUtilsMessengerCreateInfoEXT debugInfo{};
		debugInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		debugInfo.pfnUserCallback = DebugCallback;

		vector<const char*> finalLayers{};
		
#ifdef KDEBUG
		finalLayers.push_back("VK_LAYER_KHRONOS_validation");

		debugInfo.messageSeverity = 
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		debugInfo.messageType =
			VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

		static const vector<VkValidationFeatureEnableEXT> enabledFeatures =
		{
			VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT,
			VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT,
			VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_EXT,
			VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT
		};

		VkValidationFeaturesEXT validationFeatures{};
		validationFeatures.sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
		validationFeatures.enabledValidationFeatureCount = scast<u32>(enabledFeatures.size());
		validationFeatures.pEnabledValidationFeatures = enabledFeatures.data();

		validationFeatures.pNext = &debugInfo;
		createInfo.pNext = &validationFeatures;

		createInfo.enabledLayerCount = scast<u32>(finalLayers.size());
		createInfo.ppEnabledLayerNames = finalLayers.data();
#else
		debugInfo.messageSeverity = 
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		debugInfo.messageType =
			VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

		createInfo.enabledLayerCount = 0;
		createInfo.ppEnabledLayerNames = nullptr;
		createInfo.pNext = &debugInfo;
#endif

        if (vkCreateInstance(
            &createInfo,
            nullptr,
            &instance) != VK_SUCCESS)
        {
			KalaWindowCore::ForceClose(
				"KalaWindow Vulkan error",
				"Failed to create global Vulkan because vkCreateInstance failed!");

			return;
        }

		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
			instance, "vkCreateDebugUtilsMessengerEXT");

		if (func) func(
			instance,
			&debugInfo,
			nullptr,
			&debugMessenger);

        Log::Print(
			"Initialized global Vulkan context!",
			"KW_VULKAN",
			LogType::LOG_SUCCESS);

		isInitialized = true;
    }
    bool VulkanContext::IsInitialized() { return isInitialized; }

	VulkanContext* VulkanContext::InitializeInstance(u32 windowID)
	{
		ProcessWindow* w = ProcessWindow::GetRegistry().GetContent(windowID);
		if (!w)
		{
			Log::Print(
				"Failed to initialize Vulkan context because it's window was invalid!",
				"KW_VULKAN",
				LogType::LOG_ERROR,
				2);

			return nullptr;
		}

        const X11GlobalData& globalData = Window_Global::GetGlobalData();
		const WindowData& windowData = w->GetWindowData();

        if (!globalData.display
			|| !windowData.window)
        {
            Log::Print(
				"Failed to initialize Vulkan context "
				"because the display or window handle for window '" + to_string(windowID) + "' was invalid!",
				"KW_VULKAN",
				LogType::LOG_ERROR,
				2);

			return nullptr;
        }

		Display* display = ToVar<Display*>(globalData.display);
		Window window = ToVar<Window>(windowData.window);

		VkXlibSurfaceCreateInfoKHR info{};
		info.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
		info.dpy = display;
		info.window = window;

        VkSurfaceKHR surface{};

        if (vkCreateXlibSurfaceKHR(
            instance,
            &info,
            nullptr,
            &surface) != VK_SUCCESS)
        {
			KalaWindowCore::ForceClose(
				"KalaWindow Vulkan error",
				"Failed to create Vulkan surface for window '" + to_string(w->GetID()) + "'!");

			return nullptr;
        }

        u32 newID = KalaWindowCore::GetGlobalID() + 1;
		KalaWindowCore::SetGlobalID(newID);

		unique_ptr<VulkanContext> newCont = make_unique<VulkanContext>();
		VulkanContext* contPtr = newCont.get();

		contPtr->ID = newID;
		contPtr->windowID = windowID;

		w->graphicsContextID = newID;
        contPtr->surface = surface;

        registry.AddContent(newID, std::move(newCont));

		Log::Print(
			"Created new Vulkan context '" + to_string(newID) + "' for window '" + to_string(w->GetID()) + "'!",
			"KW_VULKAN",
			LogType::LOG_SUCCESS);

		return contPtr;
    }

	u32 VulkanContext::GetID() const { return ID; }
	u32 VulkanContext::GetWindowID() const { return windowID; }

    VkSurfaceKHR VulkanContext::GetSurface() const { return surface; }

	void VulkanContext::Destroy() { registry.RemoveContent(ID); }

    VulkanContext::~VulkanContext()
	{
		ProcessWindow* window = ProcessWindow::GetRegistry().GetContent(windowID);
		if (!window)
		{
			KalaWindowCore::ForceClose(
				"KalaWindow Vulkan error",
				"Failed to destroy Vulkan context '" + to_string(ID) 
				+ "' because its window '" + to_string(windowID) + "' was invalid!");
		}

		Log::Print(
			"Destroying Vulkan context '" + to_string(ID) + "' for window '" + to_string(windowID) + "'.",
			"KW_VULKAN",
			LogType::LOG_INFO);

		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
			instance, "vkDestroyDebugUtilsMessengerEXT");

		if (func != nullptr
			&& debugMessenger != VK_NULL_HANDLE)
		{
			func(instance, debugMessenger, nullptr);
			debugMessenger = VK_NULL_HANDLE;
		}

		if (surface != VK_NULL_HANDLE)
		{
			vkDestroySurfaceKHR(
				instance,
				surface,
				nullptr);

			surface = VK_NULL_HANDLE;
		}

		if (registry.GetAllContent().empty())
		{
			Log::Print(
				"Destroying global Vulkan because all contexts were destroyed.",
				"KW_VULKAN",
				LogType::LOG_INFO);

			vkDestroyInstance(instance, nullptr);

			isInitialized = false;
		}
	}
}

#endif //__linux__
