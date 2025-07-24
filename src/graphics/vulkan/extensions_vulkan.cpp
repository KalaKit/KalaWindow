//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

/*
#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#elif __linux__
#define VK_USE_PLATFORM_XLIB_KHR
#endif
#include <Volk/volk.h>
#ifdef _WIN32
#include <Windows.h>
#include <vulkan/vulkan_win32.h>
#endif
#include <string>

#include "graphics/vulkan/extensions_vulkan.hpp"
#include "graphics/vulkan/vulkan.hpp"
#include "graphics/window.hpp"
#include "core/log.hpp"

using KalaWindow::Graphics::ShutdownState;
using KalaWindow::Graphics::Window;
using KalaWindow::Graphics::PopupAction;
using KalaWindow::Graphics::PopupType;
using KalaWindow::Graphics::PopupResult;
using KalaWindow::Graphics::Vulkan::VSyncState;
using KalaWindow::Core::Logger;
using KalaWindow::Core::LogType;
using KalaWindow::Core::TimeFormat;
using KalaWindow::Core::DateFormat;

using std::string;
using std::to_string;

//If off, then all framerate is uncapped.
//Used in window.hpp
static VSyncState vsyncState = VSyncState::VSYNC_ON;

enum class ForceCloseType
{
	FC_VO, //is volk initialized
	FC_VU  //is vulkan initialized
};

static void ForceClose(
	const string& title,
	const string& reason,
	ShutdownState state = ShutdownState::SHUTDOWN_FAILURE);
static void ForceCloseMsg(
	ForceCloseType ct,
	const string& targetMsg);

static bool IsValidHandle(
	uintptr_t handle,
	const string& variableName,
	const string& originFunction)
{
	if (handle == 0
		|| handle == UINTPTR_MAX)
	{
		ForceClose(
			"Vulkan critical error [extensions_vulkan]",
			"[ " + originFunction + " ]"
			"\nVariable '" + variableName + "' value '" + to_string(handle) + "' is invalid!",
			ShutdownState::SHUTDOWN_CRITICAL);
		return false;
	}
	return true;
}

namespace KalaWindow::Graphics::Vulkan
{
	void Extensions_Vulkan::CreateVulkanSurface(Window* targetWindow)
	{
		if (!Renderer_Vulkan::IsVulkanInitialized())
		{
			ForceCloseMsg(ForceCloseType::FC_VU, "create vulkan surface");
			return;
		}

		WindowData& winData = targetWindow->GetWindowData();
		VulkanData_Core vData{};

#ifdef _WIN32
		WindowData& window = targetWindow->GetWindowData();
		HWND windowRef = ToVar<HWND>(window.hwnd);
		HINSTANCE windowIns = ToVar<HINSTANCE>(window.hInstance);

		VkWin32SurfaceCreateInfoKHR surfaceInfo{};
		surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		surfaceInfo.hwnd = windowRef;
		surfaceInfo.hinstance = windowIns;

		VkSurfaceKHR surface{};
		VkInstance instance = ToVar<VkInstance>(Renderer_Vulkan::GetInstance());
		if (vkCreateWin32SurfaceKHR(
			instance,
			&surfaceInfo,
			nullptr,
			&surface) != VK_SUCCESS)
		{
			ForceClose(
				"Vulkan error [extensions_vulkan]",
				"Failed to create Win32 Vulkan surface!");
		}

		vData.surface = FromVar(surface);
#elif __linux__
		//TODO: ADD LINUX SUPPORT
#endif

		targetWindow->SetVulkanCoreData(vData);
	}

	bool Extensions_Vulkan::CreateSwapchain(Window* window)
	{
		if (!Renderer_Vulkan::IsVulkanInitialized()
			|| Renderer_Vulkan::GetInstance() == 0)
		{
			ForceCloseMsg(ForceCloseType::FC_VU, "create swapchain");
			return false;
		}

		if (!IsValidHandle(
			Renderer_Vulkan::GetDevice(),
			"device",
			"CreateSwapchain"))
		{
			return false;
		}

		VkDevice dev = ToVar<VkDevice>(Renderer_Vulkan::GetDevice());
		VkPhysicalDevice pDev = ToVar<VkPhysicalDevice>(Renderer_Vulkan::GetPhysicalDevice());

		WindowData& winData = window->GetWindowData();
		VulkanData_Core& vData = window->GetVulkanCoreData();

		//surface capabilities

#ifdef _WIN32
		WindowData& win = window->GetWindowData();
#elif __linux__
		WindowData& win = window->GetWindowData();
#endif
		VkSurfaceKHR surfacePtr = ToVar<VkSurfaceKHR>(vData.surface);

		VkSurfaceCapabilitiesKHR capabilities{};
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
			pDev,
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

		VkPhysicalDevice pd = ToVar<VkPhysicalDevice>(Renderer_Vulkan::GetPhysicalDevice());
		VkSurfaceKHR surface = ToVar<VkSurfaceKHR>(vData.surface);
		uint32_t presentCount{};
		
		if (vkGetPhysicalDeviceSurfacePresentModesKHR(
			pd,
			surface,
			&presentCount,
			nullptr) != VK_SUCCESS)
		{
			Logger::Print(
				"Failed to query Vulkan present mode count!",
				"EXTENSIONS_VULKAN",
				LogType::LOG_ERROR,
				2);
			return false;
		}

		vector<VkPresentModeKHR> modes(presentCount);
		if (vkGetPhysicalDeviceSurfacePresentModesKHR(
			pd,
			surface,
			&presentCount,
			modes.data()) != VK_SUCCESS)
		{
			Logger::Print(
				"Failed to query Vulkan present modes!",
				"EXTENSIONS_VULKAN",
				LogType::LOG_ERROR,
				2);
			return false;
		}

		auto SupportsMode = [&](VkPresentModeKHR target) -> bool
		{
			for (auto mode : modes)
			{
				if (mode == target) return true;
			}
			return false;
		};

		VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
		switch (vsyncState)
		{
		case VSyncState::VSYNC_ON:
		default:
			presentMode = VK_PRESENT_MODE_FIFO_KHR;
			break;
		case VSyncState::VSYNC_OFF:
			if (SupportsMode(VK_PRESENT_MODE_IMMEDIATE_KHR))
			{
				presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
			}
			else
			{
				Logger::Print(
					"Cannot set vsync to 'OFF' because it is not supported on this device! Falling back to 'ON'.",
					"EXTENSIONS_VULKAN",
					LogType::LOG_ERROR,
					2);
			}
			break;
		case VSyncState::VSYNC_TRIPLE_BUFFERING:
			if (SupportsMode(VK_PRESENT_MODE_MAILBOX_KHR))
			{
				presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
			}
			else
			{
				Logger::Print(
					"Cannot set vsync to 'TRIPLE BUFFERING' because it is not supported on this device! Falling back to 'ON'.",
					"EXTENSIONS_VULKAN",
					LogType::LOG_ERROR,
					2);
			}
			break;
		}

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
			dev,
			&createInfo,
			nullptr,
			&realSC) != VK_SUCCESS)
		{
			Logger::Print(
				"Failed to create Vulkan swapchain!",
				"EXTENSIONS_VULKAN",
				LogType::LOG_ERROR,
				2);
			return false;
		}

		vData.swapchain = FromVar(realSC);

		//get swapchain images

		uint32_t count{};
		vkGetSwapchainImagesKHR(
			dev,
			ToVar<VkSwapchainKHR>(vData.swapchain),
			&count,
			nullptr);

		vector<VkImage> tempImages(count);
		vkGetSwapchainImagesKHR(
			dev,
			ToVar<VkSwapchainKHR>(vData.swapchain),
			&count,
			tempImages.data());

		vData.images.resize(count);
		for (uint32_t i = 0; i < count; ++i)
		{
			vData.images[i] = FromVar(tempImages[i]);
		}

		vData.imagesInFlight.assign(count, FromVar<VkFence>(VK_NULL_HANDLE));

		//create image views

		vData.imageViews.resize(count);
		for (uint32_t i = 0; i < count; ++i)
		{
			VkImageViewCreateInfo viewInfo{};
			viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewInfo.image = ToVar<VkImage>(vData.images[i]);
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
				dev,
				&viewInfo,
				nullptr,
				&realIV) != VK_SUCCESS)
			{
				Logger::Print(
					"Failed to create image view for swapchain image " + i,
					"EXTENSIONS_VULKAN",
					LogType::LOG_ERROR,
					2);
				return false;
			}

			vData.imageViews[i] = FromVar(realIV);
		}

		Logger::Print(
			"Successfully created Vulkan swapchain and released resources!",
			"EXTENSIONS_VULKAN",
			LogType::LOG_SUCCESS);
		return true;
	}

	void Extensions_Vulkan::DestroySwapchain(Window* window)
	{
		WindowData& winData = window->GetWindowData();
		VulkanData_Core& vData = window->GetVulkanCoreData();

		if (Renderer_Vulkan::GetDevice() != NULL)
		{
			VkDevice dev = ToVar<VkDevice>(Renderer_Vulkan::GetDevice());

			vkDeviceWaitIdle(dev);

			for (auto view : vData.imageViews)
			{
				if (view) vkDestroyImageView(dev, ToVar<VkImageView>(view), nullptr);
			}
			for (auto fb : vData.framebuffers)
			{
				if (fb) vkDestroyFramebuffer(dev, ToVar<VkFramebuffer>(fb), nullptr);
			}
			if (vData.renderPass)
			{
				vkDestroyRenderPass(dev, ToVar<VkRenderPass>(vData.renderPass), nullptr);
				vData.renderPass = NULL;
			}
			if (vData.swapchain)
			{
				vkDestroySwapchainKHR(
					dev,
					ToVar<VkSwapchainKHR>(vData.swapchain),
					nullptr);
			}
		}

		//reset handles
		vData.swapchain = NULL;
		vData.images.clear();
		vData.imageViews.clear();
		vData.framebuffers.clear();
	}

	//
	// EXTERNAL
	//

	VSyncState Renderer_Vulkan::GetVSyncState() { return vsyncState; }
	void Renderer_Vulkan::SetVSyncState(
		VSyncState newVSyncState,
		Window* window)
	{
		vsyncState = newVSyncState;

		Renderer_Vulkan::HardReset(window);
	}
}

static void ForceClose(
	const string& title,
	const string& reason,
	ShutdownState state)
{
	Logger::Print(
		reason,
		"EXTENSIONS_VULKAN",
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
		Window::Shutdown(ShutdownState::SHUTDOWN_FAILURE);
	}
}
void ForceCloseMsg(ForceCloseType fct, const string& targetMsg)
{
	if (fct == ForceCloseType::FC_VO)
	{
		ForceClose(
			"Vulkan error [extensions_vulkan]",
			"Cannot " + targetMsg + " because Volk failed to initialize!");
	}

	else if (fct == ForceCloseType::FC_VU)
	{
		ForceClose(
			"Vulkan error [extensions_vulkan]",
			"Cannot " + targetMsg + " because Vulkan is not initialized!");
	}
}
*/