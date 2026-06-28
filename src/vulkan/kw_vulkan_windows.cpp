//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef _WIN32

#include <windows.h>
#include <minwindef.h>

#include <memory>
#include <filesystem>

#define VK_USE_PLATFORM_WIN32_KHR
#include "vulkan/vulkan_core.h"
#include "vulkan/vulkan_win32.h"

#include "log_utils.hpp"
#include "core_utils.hpp"

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

using std::to_string;
using std::unique_ptr;
using std::make_unique;
using std::filesystem::path;

namespace KalaWindow::Vulkan
{
    //
	// GLOBAL
	//

	static bool isInitialized{};
	static bool isVerboseLoggingEnabled{};

    static VkInstance instance{};
	static VkDebugUtilsMessengerEXT debugMessenger{};

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

#ifdef KDEBUG
		wchar_t buffer[MAX_PATH]{};
		DWORD length = GetModuleFileNameW(
			nullptr,
			buffer,
			MAX_PATH);

		if (length > 0
			&& length < MAX_PATH)
		{	
			path exeDir = path(buffer).parent_path();

			_putenv_s("VK_LAYER_PATH", exeDir.string().c_str());
		}
		else
		{
			KalaWindowCore::ForceClose(
				"Global Vulkan error",
				"Failed to get path to executable!");
		}
#endif

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
        finalExtensions.push_back("VK_KHR_win32_surface");
#ifdef KDEBUG
        finalExtensions.push_back("VK_EXT_debug_utils");
#endif

        createInfo.enabledExtensionCount = scast<u32>(finalExtensions.size());
        createInfo.ppEnabledExtensionNames = finalExtensions.data();

		vector<const char*> finalLayers{};
#ifdef KDEBUG
		finalLayers.push_back("VK_LAYER_KHRONOS_validation");

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
			| VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		debugInfo.pfnUserCallback = DebugCallback;

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
		createInfo.enabledLayerCount = 0;
		createInfo.ppEnabledLayerNames = nullptr;
		createInfo.pNext = nullptr;
#endif

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

		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
			instance, "vkCreateDebugUtilsMessengerEXT");

		if (func) func(
			instance,
			&debugInfo,
			nullptr,
			&debugMessenger);
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

#if DEBUG
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

		if (!w)
		{
			KalaWindowCore::ForceClose(
				"Vulkan error",
				"Cannot initialize Vulkan context because it's window was not found!");

			return nullptr;
		}

		if (w->GetGraphicsContextID() != 0)
		{
			Log::Print(
				"Cannot add Vulkan context to window '" + w->GetTitle() + "' because it already has an existing context!",
				"KW_VULKAN",
				LogType::LOG_ERROR,
				2);

			return nullptr;
		}

        u32 newID = KalaWindowCore::GetGlobalID() + 1;
		KalaWindowCore::SetGlobalID(newID);

		unique_ptr<Vulkan_Context> newCont = make_unique<Vulkan_Context>();
		Vulkan_Context* contPtr = newCont.get();

		contPtr->ID = newID;

		const WindowData& wData = w->GetWindowData();

		if (!wData.window)
		{
			KalaWindowCore::ForceClose(
				"Window error",
				"Failed to initialize Vulkan context because the attached window was invalid!");
		}

        HWND hwnd = ToVar<HWND>(wData.window);
        HINSTANCE hInstance = ToVar<HINSTANCE>(wData.hInstance);

        VkWin32SurfaceCreateInfoKHR info{};
        info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        info.hwnd = hwnd;
        info.hinstance = hInstance;

        VkSurfaceKHR surface{};

        if (vkCreateWin32SurfaceKHR(
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
		w->SetGraphicsContextID(newID);

		contPtr->windowID = w->GetID();
        contPtr->surface = surface;

		Log::Print(
			"Created new Vulkan context with ID '" + to_string(newID) + "' for window '" + w->GetTitle() + "'!",
			"KW_VULKAN",
			LogType::LOG_SUCCESS);

		return contPtr;
    }

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

		//only destroy the instance if all windows are destroyed
		if (ProcessWindow::GetRegistry().runtimeContent.empty()
			&& instance != VK_NULL_HANDLE)
		{
			vkDestroyInstance(
				instance,
				nullptr);

			instance = VK_NULL_HANDLE;
		}
	}
}

#endif //_WIN32