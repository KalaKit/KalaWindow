
//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef __linux__

#include <X11/Xlib.h>
#include <memory>

#define VK_USE_PLATFORM_XLIB_KHR
#include "vulkan/vulkan_core.h"
#include "vulkan/vulkan_xlib.h"

#include "log_utils.hpp"

#include "vulkan/kw_vulkan.hpp"
#include "graphics/kw_window.hpp"
#include "graphics/kw_window_global.hpp"
#include "core/kw_core.hpp"

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;

using KalaHeaders::KalaCore::ToVar;

using KalaWindow::Core::KalaWindowCore;
using KalaWindow::Graphics::ProcessWindow;
using KalaWindow::Graphics::WindowData;
using KalaWindow::Graphics::Window_Global;
using KalaWindow::Graphics::X11GlobalData;

using std::string;
using std::to_string;
using std::unique_ptr;
using std::make_unique;

static bool isVerboseLoggingEnabled{};

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

	Log::Print(
		data->pMessage,
		logTarget,
		logType);

	return VK_FALSE;
}

namespace KalaWindow::Vulkan
{
    //
	// GLOBAL
	//

	static bool isInitialized{};

    static VkInstance instance{};
	static VkDebugUtilsMessengerEXT debugMessenger{};

	void Vulkan_Global::SetVerboseLoggingState(bool newState) { isVerboseLoggingEnabled = newState; }
	bool Vulkan_Global::IsVerboseLoggingEnabled() { return isVerboseLoggingEnabled; }

	void Vulkan_Global::Initialize(const vector<string>& extensions)
    {
		if (isInitialized)
		{
			Log::Print(
				"Cannot initialize global Vulkan more than once!",
				"KW_VULKAN",
				LogType::LOG_ERROR,
				2);

			return;
		}

		if (!Window_Global::IsInitialized())
		{
			KalaWindowCore::ForceClose(
				"Global Vulkan error",
				"Cannot initialize global Vulkan because global window manager has not been initialized!");

			return;
		}

        u32 version{};

        if (vkEnumerateInstanceVersion(&version) != VK_SUCCESS
            || version < VK_API_VERSION_1_3)
        {
			KalaWindowCore::ForceClose(
				"Global Vulkan error",
				"Vulkan 1.3 is not supported on this system!");

			return;
        }

        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;

        appInfo.pApplicationName = "KalaWindow";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);

        appInfo.pEngineName = "KalaWindow";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);

        appInfo.apiVersion = VK_API_VERSION_1_3;

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
#ifdef KDEBUG
        finalExtensions.push_back("VK_EXT_debug_utils");
#endif

        createInfo.enabledExtensionCount = finalExtensions.size();
        createInfo.ppEnabledExtensionNames = finalExtensions.data();

        if (vkCreateInstance(
            &createInfo,
            nullptr,
            &instance) != VK_SUCCESS)
        {
			KalaWindowCore::ForceClose(
				"Global Vulkan error",
				"Failed to create Vulkan instance!");

			return;
        }

#ifdef KDEBUG
		VkDebugUtilsMessengerCreateInfoEXT debugInfo{};
		debugInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		debugInfo.messageSeverity = 
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		debugInfo.messageType =
			VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT;
		debugInfo.pfnUserCallback = DebugCallback;

		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
			instance, "vkCreateDebugUtilsMessengerEXT");

		func(instance, &debugInfo, nullptr, &debugMessenger);
#endif

        Log::Print(
			"Initialized global Vulkan context!",
			"KW_VULKAN",
			LogType::LOG_SUCCESS);

		isInitialized = true;
    }

    bool Vulkan_Global::IsInitialized() { return isInitialized; }

    VkInstance Vulkan_Global::GetInstance()
    {
        if (!isInitialized)
		{
			KalaWindowCore::ForceClose(
				"Global Vulkan error",
				"Cannot get Vulkan instance because Global Vulkan has not been initialized!");
		}

        return instance;
    }

    void Vulkan_Global::Shutdown()
    {
        Vulkan_Context::GetRegistry().RemoveAllContent();

#ifdef KDEBUG
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
			instance, "vkDestroyDebugUtilsMessengerEXT");

		func(instance, debugMessenger, nullptr);
#endif

        vkDestroyInstance(instance, nullptr);

        isInitialized = false;
    }

    //
	// CONTEXT
	//

	static KalaWindowRegistry<Vulkan_Context> registry{};

	KalaWindowRegistry<Vulkan_Context>& Vulkan_Context::GetRegistry() { return registry; }

	Vulkan_Context* Vulkan_Context::Initialize(u32 windowID)
	{
		if (!Vulkan_Global::IsInitialized())
		{
			KalaWindowCore::ForceClose(
				"Vulkan error",
				"Cannot initialize Vulkan context because global Vulkan has not yet been initialized!");

			return nullptr;
		}

		ProcessWindow* w = ProcessWindow::GetRegistry().GetContent(windowID);

		if (!w
			|| !w->IsInitialized())
		{
			Log::Print(
				"Cannot initialize Vulkan context because it's window was not found!",
				"KW_VULKAN",
				LogType::LOG_ERROR,
				2);

			return nullptr;
		}

		if (w->GetContextID() != 0)
		{
			Log::Print(
				"Cannot add Vulkan context to window '" + w->GetTitle() + "' because it already has an existing context!",
				"KW_VULKAN",
				LogType::LOG_ERROR,
				2);

			return nullptr;
		}

		if (!instance)
		{
            KalaWindowCore::ForceClose(
                "Window error",
                "Failed to initialize Vulkan context because the passed Vulkan instance was invalid!");
		}

        u32 newID = KalaWindowCore::GetGlobalID() + 1;
		KalaWindowCore::SetGlobalID(newID);

		unique_ptr<Vulkan_Context> newCont = make_unique<Vulkan_Context>();
		Vulkan_Context* contPtr = newCont.get();

		contPtr->ID = newID;

        const X11GlobalData& globalData = Window_Global::GetGlobalData();
		const WindowData& windowData = w->GetWindowData();

        if (!globalData.display)
        {
            KalaWindowCore::ForceClose(
                "Window error",
                "Failed to initialize Vulkan context because the attached display was invalid!");
        }
        if (!windowData.window)
        {
            KalaWindowCore::ForceClose(
                "Window error",
                "Failed to initialize Vulkan context because the attached window was invalid!");
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
				"Vulkan error",
				"Failed to create Vulkan surface for window '" + w->GetTitle() + "'!");

			return nullptr;
        }

        registry.AddContent(newID, std::move(newCont));
		w->SetContextID(newID);

		contPtr->windowID = w->GetID();
        contPtr->surface = surface;

		contPtr->isInitialized = true;

		Log::Print(
			"Created new Vulkan context with ID '" + to_string(newID) + "' for window '" + w->GetTitle() + "'!",
			"KW_VULKAN",
			LogType::LOG_SUCCESS);

		return contPtr;
    }

    bool Vulkan_Context::IsInitialized() const { return isInitialized; }

	u32 Vulkan_Context::GetID() const { return ID; }
	u32 Vulkan_Context::GetWindowID() const { return windowID; }

    VkSurfaceKHR Vulkan_Context::GetSurface() const { return surface; }

    Vulkan_Context::~Vulkan_Context()
	{
		ProcessWindow* window = ProcessWindow::GetRegistry().GetContent(windowID);
		if (!window)
		{
			Log::Print(
				"Cannot shut down Vulkan context because its window was not found!",
				"KW_VULKAN",
				LogType::LOG_ERROR,
				2);

			return;
		}

		Log::Print(
			"Destroying Vulkan context with ID '" + to_string(ID) + "' for window '" + window->GetTitle() + "'.",
			"KW_VULKAN",
			LogType::LOG_INFO);

		vkDestroySurfaceKHR(
            instance,
            surface,
            nullptr);
	}
}

#endif //__linux__
