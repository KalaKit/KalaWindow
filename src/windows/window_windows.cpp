//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef _WIN32

#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#include <ShlObj.h>
#include <shobjidl.h>
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")
#include <atlbase.h>
#include <atlcomcli.h>

#include <algorithm>
#include <functional>
#include <memory>
#include <filesystem>
#include <sstream>
#include <string>
#include <vector>

//#define VK_NO_PROTOTYPES
//#include <Volk/volk.h>
//#include <vulkan/vulkan.h>

//#include "graphics/vulkan/vulkan.hpp"
#include "graphics/opengl/opengl.hpp"
#include "graphics/texture.hpp"
#include "graphics/opengl/opengl_shader.hpp"
#include "graphics/opengl/opengl_texture.hpp"
#include "graphics/window.hpp"
#include "windows/messageloop.hpp"
#include "core/input.hpp"
#include "core/core.hpp"
#include "core/containers.hpp"

using KalaHeaders::Log;
using KalaHeaders::LogType;

//using KalaWindow::Graphics::Vulkan::Renderer_Vulkan;
using KalaWindow::Graphics::OpenGL::OpenGL_Renderer;
using KalaWindow::Graphics::OpenGL::OpenGL_Shader;
using KalaWindow::Graphics::OpenGL::OpenGL_Texture;
using KalaWindow::Graphics::TextureType;
using KalaWindow::Graphics::TextureFormat;
using KalaWindow::Graphics::Window;
using KalaWindow::Core::MessageLoop;
using KalaWindow::Core::Input;
using KalaWindow::Core::KalaWindowCore;
using KalaWindow::Core::globalID;
using KalaWindow::Core::createdWindows;
using KalaWindow::Core::runtimeWindows;
using KalaWindow::Core::createdMenuBarEvents;
using KalaWindow::Core::runtimeMenuBarEvents;
using KalaWindow::Core::createdOpenGLTextures;

using std::make_unique;
using std::move;
using std::to_string;
using std::find_if;
using std::function;
using std::exception;
using std::unique_ptr;
using std::clamp;
using std::filesystem::path;
using std::filesystem::exists;
using std::ostringstream;
using std::wstring;
using std::string;
using std::vector;

static bool checkedOSVersion = false;
constexpr u32 MIN_OS_VERSION = 10017763; //Windows 10 build 17763 (1809)
constexpr u16 MAX_TITLE_LENGTH = 512;
constexpr u8 MAX_LABEL_LENGTH = 64;

static bool enabledBeginPeriod = false;

//KalaWindow will dynamically update window idle state
static void UpdateIdleState(Window* window, bool& isIdle);

static HICON SetUpIcon(OpenGL_Texture* texture);

static HICON exeIcon{};
static HICON overlayIcon{};

static wstring ToWide(const string& str);
static string ToShort(const wstring& str);

namespace KalaWindow::Graphics
{
	Window* Window::Initialize(
		const string& title,
		vec2 size,
		Window* parentWindow)
	{
		HINSTANCE newHInstance = GetModuleHandle(nullptr);

		if (createdWindows.size() == 0)
		{
			if (!checkedOSVersion)
			{
				u32 version = KalaWindowCore::GetVersion();
				string versionStr = to_string(version);
				string osVersion = versionStr.substr(0, 2);
				string buildVersion = to_string(stoi(versionStr.substr(2)));

				if (version < MIN_OS_VERSION)
				{
					ostringstream oss{};
					oss << "Your version is Windows '" + osVersion + "' build '" << buildVersion
						<< "' but KalaWindow requires Windows '10' (1809 build '17763') or higher!";

					KalaWindowCore::ForceClose(
						"Windows version out of date",
						oss.str());

					return nullptr;
				}

				Log::Print(
					"Windows version '" + osVersion + "' build '" + buildVersion + "'",
					"WINDOW_WINDOWS",
					LogType::LOG_INFO);

				checkedOSVersion = true;
			}

			//Treat this process as a real app with a stable identity
			SetCurrentProcessExplicitAppUserModelID(L"KalaWindowClass");

			Log::Print(
				"Creating window '" + title + "'.",
				"WINDOW_WINDOWS",
				LogType::LOG_DEBUG);

#ifdef _WIN32
			if (!enabledBeginPeriod)
			{
				timeBeginPeriod(1);
				enabledBeginPeriod = true;
			}
#endif //_WIN32

			WNDCLASSA wc = {};
			wc.style =
				CS_OWNDC      //own the DC for the lifetime of this window
				| CS_DBLCLKS; //allow detecting double clicks
			wc.lpfnWndProc = reinterpret_cast<WNDPROC>(MessageLoop::WindowProcCallback());
			wc.hInstance = newHInstance;
			wc.lpszClassName = "KalaWindowClass";

			if (!RegisterClassA(&wc))
			{
				DWORD err = GetLastError();
				string message{};
				if (err == ERROR_CLASS_ALREADY_EXISTS)
				{
					message = "Window class already exists with different definition.\n";
				}
				else
				{
					message = "RegisterClassA failed with error: " + to_string(err) + "\n";
				}

				string errorTitle = "Window error";
				KalaWindowCore::ForceClose(
					errorTitle,
					message);

				return nullptr;
			}
		}

		HWND parentWindowRef{};
		if (parentWindow != nullptr)
		{
			if (find(runtimeWindows.begin(),
				runtimeWindows.end(),
				parentWindow)
				== runtimeWindows.end())
			{
				KalaWindowCore::ForceClose(
					"Window error",
					"Parent window pointer does not exist! Failed to newly create child window '" + title);

				return nullptr;
			}

			parentWindowRef = ToVar<HWND>(parentWindow->GetWindowData().hwnd);

			if (parentWindowRef == nullptr)
			{
				KalaWindowCore::ForceClose(
					"Window error",
					"Parent window handle is invalid! Failed to newly create child window '" + title);

				return nullptr;
			}
		}

		HWND newHwnd = CreateWindowExA(
			0,
			"KalaWindowClass",
			title.c_str(),
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			size.x,
			size.y,
			parentWindowRef,
			nullptr,
			newHInstance,
			nullptr);

		if (!newHwnd)
		{
			DWORD errorCode = GetLastError();
			LPSTR errorMsg = nullptr;
			FormatMessageA(
				FORMAT_MESSAGE_ALLOCATE_BUFFER
				| FORMAT_MESSAGE_FROM_SYSTEM
				| FORMAT_MESSAGE_IGNORE_INSERTS,
				nullptr,
				errorCode,
				0,
				(LPSTR)&errorMsg,
				0,
				nullptr);

			string message = "CreateWindowExA failed with error "
				+ to_string(errorCode)
				+ ": "
				+ (errorMsg ? errorMsg : "Unknown");

			if (errorMsg) LocalFree(errorMsg);

			string errorTitle = "Window error";
			KalaWindowCore::ForceClose(
				errorTitle,
				message);

			return nullptr;
		}

		WindowData newWindowStruct =
		{
			.hwnd = FromVar(newHwnd),
			.hInstance = FromVar(newHInstance),
			.wndProc = FromVar((WNDPROC)GetWindowLongPtr(newHwnd, GWLP_WNDPROC))
		};

		//set window to be DPI-aware per window
		SetProcessDpiAwarenessContext(
			DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

		u32 newID = ++globalID;
		unique_ptr<Window> newWindow = make_unique<Window>();
		Window* windowPtr = newWindow.get();
		
		newWindow->SetTitle(title);
		newWindow->ID = newID;
		newWindow->SetClientRectSize(size);
		newWindow->window_windows = newWindowStruct;

		newWindow->isInitialized = true;

		//ensure window is always shown in case setwindowstate will not be called
		ShowWindow(newHwnd, SW_SHOWNORMAL);
		UpdateWindow(newHwnd);

		createdWindows[newID] = move(newWindow);
		runtimeWindows.push_back(windowPtr);

		Log::Print(
			"Created window '" + title + "' with ID '" + to_string(newID) + "'!",
			"WINDOW_WINDOWS",
			LogType::LOG_SUCCESS);

		return windowPtr;
	}

	void Window::SetTitle(const string& newTitle) const
	{
		HWND window = ToVar<HWND>(window_windows.hwnd);

		if (newTitle.empty())
		{
			Log::Print(
				"Window title cannot be empty!",
				"WINDOW_WINDOWS",
				LogType::LOG_ERROR);

			return;
		}

		string titleToSet = newTitle;
		if (newTitle.length() > MAX_TITLE_LENGTH)
		{
			Log::Print(
				"Window title exceeded max allowed length of '" + to_string(MAX_TITLE_LENGTH) + "'! Title has been truncated.",
				"WINDOW_WINDOWS",
				LogType::LOG_WARNING);

			titleToSet = titleToSet.substr(0, MAX_TITLE_LENGTH);
		}

		wstring wideTitle = ToWide(titleToSet);

		SetWindowTextW(window, wideTitle.c_str());
	}
	string Window::GetTitle() const
	{
		HWND window = ToVar<HWND>(window_windows.hwnd);

		int length = GetWindowTextLengthW(window);
		if (length == 0)
		{
			Log::Print(
				"Window title was empty!",
				"WINDOW_WINDOWS",
				LogType::LOG_WARNING);

			return "";
		}

		wstring title(length + 1, L'\0');
		GetWindowTextW(window, title.data(), length + 1);

		title.resize(wcslen(title.c_str()));
		return ToShort(title);
	}

	void Window::SetIcon(u32 texture) const
	{
		HWND window = ToVar<HWND>(window_windows.hwnd);

		OpenGL_Texture* tex = createdOpenGLTextures[texture].get();

		if (!texture
			|| !tex)
		{
			Log::Print(
				"Cannot set window '" + GetTitle() + "' exe icon because the texture ID is invalid!",
				"WINDOW_WINDOWS",
				LogType::LOG_ERROR);

			return;
		}

		TextureFormat format = tex->GetFormat();
		if (format != TextureFormat::Format_RGBA8
			&& format != TextureFormat::Format_SRGB8A8
			&& format != TextureFormat::Format_RGBA16F
			&& format != TextureFormat::Format_RGBA32F)
		{
			Log::Print(
				"Cannot set window '" + GetTitle() + "' exe icon because unsupported texture was selected! Only 4-channel textures like 'Format_RGBA8' are allowed.",
				"WINDOW_WINDOWS",
				LogType::LOG_ERROR);

			return;
		}

		if (exeIcon)
		{
			DestroyIcon(exeIcon);
			exeIcon = nullptr;
		}

		exeIcon = SetUpIcon(tex);

		if (exeIcon == nullptr)
		{
			Log::Print(
				"Cannot set window '" + GetTitle() + "' icon because SetUpIcon failed!",
				"WINDOW_WINDOWS",
				LogType::LOG_ERROR);

			return;
		}

		//apply to window

		SendMessage(
			window,
			WM_SETICON,
			ICON_BIG, //task bar + alt tab
			(LPARAM)exeIcon);

		SendMessage(
			window,
			WM_SETICON,
			ICON_SMALL, //title bar + window border
			(LPARAM)exeIcon);
	}
	void Window::ClearIcon() const
	{
		HWND window = ToVar<HWND>(window_windows.hwnd);

		SendMessage(
			window,
			WM_SETICON,
			ICON_BIG, //task bar + alt tab
			(LPARAM)nullptr);

		SendMessage(
			window,
			WM_SETICON,
			ICON_SMALL, //title bar + window border
			(LPARAM)nullptr);
	}

	void Window::SetTaskbarOverlayIcon(
		u32 texture,
		const string& tooltip) const
	{
		HWND window = ToVar<HWND>(window_windows.hwnd);

		OpenGL_Texture* tex = createdOpenGLTextures[texture].get();

		if (!texture
			|| !tex)
		{
			Log::Print(
				"Cannot set window '" + GetTitle() + "' overlay icon because the texture ID is invalid!",
				"WINDOW_WINDOWS",
				LogType::LOG_ERROR);

			return;
		}

		TextureFormat format = tex->GetFormat();
		if (format != TextureFormat::Format_RGBA8
			&& format != TextureFormat::Format_SRGB8A8
			&& format != TextureFormat::Format_RGBA16F
			&& format != TextureFormat::Format_RGBA32F)
		{
			Log::Print(
				"Cannot set window '" + GetTitle() + "' overlay icon because unsupported texture was selected! Only 4-channel textures like 'Format_RGBA8' are allowed.",
				"WINDOW_WINDOWS",
				LogType::LOG_ERROR);

			return;
		}

		if (overlayIcon)
		{
			DestroyIcon(overlayIcon);
			overlayIcon = nullptr;
		}
		
		overlayIcon = SetUpIcon(tex);

		if (overlayIcon == nullptr)
		{
			Log::Print(
				"Cannot set window '" + GetTitle() + "' overlay icon because SetUpIcon failed!",
				"WINDOW_WINDOWS",
				LogType::LOG_ERROR);

			return;
		}

		CComPtr<ITaskbarList3> taskbar{};
		HRESULT hr = (CoCreateInstance(
			CLSID_TaskbarList,
			nullptr,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(&taskbar)));

		if (!SUCCEEDED(hr)
			|| !taskbar)
		{
			Log::Print(
				"Failed to create ITaskbarList3 to set overlay icon!",
				"WINDOW_WINDOWS",
				LogType::LOG_ERROR);

			return;
		}

		hr = taskbar->HrInit();

		if (!SUCCEEDED(hr))
		{
			Log::Print(
				"Failed to init ITaskbarList3 to set overlay icon!",
				"WINDOW_WINDOWS",
				LogType::LOG_ERROR);

			return;
		}

		taskbar->SetOverlayIcon(
			window,
			overlayIcon,
			tooltip.empty() ? nullptr : ToWide(tooltip).c_str());
	}
	void Window::ClearTaskbarOverlayIcon() const
	{
		HWND window = ToVar<HWND>(window_windows.hwnd);

		CComPtr<ITaskbarList3> taskbar{};
		HRESULT hr = (CoCreateInstance(
			CLSID_TaskbarList,
			nullptr,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(&taskbar)));

		if (!SUCCEEDED(hr))
		{
			Log::Print(
				"Failed to get ITaskbarList3 to clear overlay icon!",
				"WINDOW_WINDOWS",
				LogType::LOG_ERROR);

			return;
		}

		taskbar->SetOverlayIcon(
			window,
			nullptr,
			nullptr);
	}

	void Window::SetWindowRounding(WindowRounding roundState) const
	{
		HWND window = ToVar<HWND>(window_windows.hwnd);

		DWM_WINDOW_CORNER_PREFERENCE pref{};

		switch (roundState)
		{
		case WindowRounding::ROUNDING_DEFAULT:
			pref = DWMWCP_DEFAULT;
			break;
		case WindowRounding::ROUNDING_NONE:
			pref = DWMWCP_DONOTROUND;
			break;
		case WindowRounding::ROUNDING_ROUND:
			pref = DWMWCP_ROUND;
			break;
		case WindowRounding::ROUNDING_ROUND_SMALL:
			pref = DWMWCP_ROUNDSMALL;
			break;
		}

		HRESULT hr = DwmSetWindowAttribute(
			window,
			DWMWA_WINDOW_CORNER_PREFERENCE,
			&pref,
			sizeof(pref));

		if (FAILED(hr))
		{
			Log::Print(
				"Failed to set window rounding preference! This feature is not supported on Windows 10.",
				"WINDOW_WINDOWS",
				LogType::LOG_ERROR);
		}
	}
	WindowRounding Window::GetWindowRoundingState() const
	{
		HWND window = ToVar<HWND>(window_windows.hwnd);

		DWM_WINDOW_CORNER_PREFERENCE pref{};

		HRESULT hr = DwmGetWindowAttribute(
			window,
			DWMWA_WINDOW_CORNER_PREFERENCE,
			&pref,
			sizeof(pref));

		if (FAILED(hr))
		{
			Log::Print(
				"Failed to get window rounding preference! This feature is not supported on Windows 10.",
				"WINDOW_WINDOWS",
				LogType::LOG_ERROR);

			return WindowRounding::ROUNDING_NONE;
		}

		switch (pref)
		{
		case DWMWCP_DEFAULT:    return WindowRounding::ROUNDING_DEFAULT;
		case DWMWCP_DONOTROUND: return WindowRounding::ROUNDING_NONE;
		case DWMWCP_ROUND:      return WindowRounding::ROUNDING_ROUND;
		case DWMWCP_ROUNDSMALL: return WindowRounding::ROUNDING_ROUND_SMALL;
		}

		return WindowRounding::ROUNDING_NONE;
	}

	void Window::SetClientRectSize(vec2 newSize) const
	{
		HWND window = ToVar<HWND>(window_windows.hwnd);

		//desired client area
		RECT rect
		{
			0,
			0,
			(LONG)newSize.x,
			(LONG)newSize.y
		};

		//Adjust for borders/title/menu
		AdjustWindowRectEx(
			&rect,
			GetWindowLong(window, GWL_STYLE),
			FALSE,
			GetWindowLong(window, GWL_EXSTYLE));

		SetWindowPos(
			window,
			nullptr,
			0,
			0,
			rect.right - rect.left,
			rect.bottom - rect.top,
			SWP_NOMOVE
			| SWP_NOZORDER);
	}
	vec2 Window::GetClientRectSize() const
	{
		HWND window = ToVar<HWND>(window_windows.hwnd);

		RECT rect{};
		GetClientRect(window, &rect);

		return vec2
		{
			static_cast<float>(rect.right - rect.left),
			static_cast<float>(rect.bottom - rect.top)
		};
	}

	void Window::SetOuterSize(vec2 newSize) const
	{
		HWND window = ToVar<HWND>(window_windows.hwnd);

		SetWindowPos(
			window,
			nullptr,
			0,
			0,
			newSize.x,
			newSize.y,
			SWP_NOMOVE
			| SWP_NOZORDER);
	}
	vec2 Window::GetOuterSize() const
	{
		HWND window = ToVar<HWND>(window_windows.hwnd);

		RECT rect{};
		GetWindowRect(window, &rect);

		return vec2
		{
			static_cast<float>(rect.right - rect.left),
			static_cast<float>(rect.bottom - rect.top)
		};
	}

	void Window::SetFramebufferSize(vec2 newSize) const
	{
		HWND window = ToVar<HWND>(window_windows.hwnd);

		UINT dpi = GetDpiForWindow(window);

		//desired framebuffer area
		RECT rect
		{
			0,
			0,
			static_cast<int>(newSize.x),
			static_cast<int>(newSize.y)
		};

		//Adjust for borders/title/menu + DPI
		AdjustWindowRectExForDpi(
			&rect,
			GetWindowLong(window, GWL_STYLE),
			FALSE,
			GetWindowLong(window, GWL_EXSTYLE),
			dpi);

		SetWindowPos(
			window,
			nullptr,
			0,
			0,
			rect.right - rect.left,
			rect.bottom - rect.top,
			SWP_NOMOVE
			| SWP_NOZORDER);
	}
	vec2 Window::GetFramebufferSize() const
	{
		const WindowData& winData = window_windows;
		HWND hwnd = ToVar<HWND>(winData.hwnd);

		UINT dpi = GetDpiForWindow(hwnd);
		RECT rect{};
		GetClientRect(hwnd, &rect);

		int width = MulDiv(
			rect.right - rect.left,
			dpi,
			96);
		int height = MulDiv(
			rect.bottom - rect.top,
			dpi,
			96);

		return vec2
		{
			static_cast<float>(width),
			static_cast<float>(height)
		};
	}

	void Window::SetPosition(vec2 newPosition) const
	{
		HWND window = ToVar<HWND>(window_windows.hwnd);

		SetWindowPos(
			window,
			nullptr,
			newPosition.x,
			newPosition.y,
			0,
			0,
			SWP_NOSIZE
			| SWP_NOZORDER);
	}
	vec2 Window::GetPosition() const
	{
		HWND window = ToVar<HWND>(window_windows.hwnd);

		RECT rect{};
		if (GetWindowRect(window, &rect))
		{
			return vec2
			{ 
				static_cast<float>(rect.left),
				static_cast<float>(rect.top)
			};
		}

		return vec2{ 0, 0 };
	}

	void Window::SetAlwaysOnTopState(bool state) const
	{
		HWND window = ToVar<HWND>(window_windows.hwnd);

		SetWindowPos(
			window,
			state ? HWND_TOPMOST : HWND_NOTOPMOST,
			0,
			0,
			0,
			0,
			SWP_NOMOVE
			| SWP_NOSIZE);
	}
	bool Window::IsAlwaysOnTop() const
	{
		HWND window = ToVar<HWND>(window_windows.hwnd);

		LONG exStyle = GetWindowLong(
			window,
			GWL_EXSTYLE);

		return (exStyle & WS_EX_TOPMOST) != 0;
	}

	void Window::SetResizableState(bool state) const
	{
		HWND window = ToVar<HWND>(window_windows.hwnd);

		LONG style = GetWindowLong(window, GWL_STYLE);

		if (state)
		{
			style |= (
				WS_THICKFRAME
				| WS_MAXIMIZEBOX);
		}
		else
		{
			style &= ~(
				WS_THICKFRAME
				| WS_MAXIMIZEBOX);
		}

		SetWindowLong(
			window,
			GWL_STYLE,
			style);

		SetWindowPos(
			window,
			nullptr,
			0,
			0,
			0,
			0,
			SWP_NOMOVE
			| SWP_NOSIZE
			| SWP_NOZORDER
			| SWP_FRAMECHANGED);
	}
	bool Window::IsResizable() const
	{
		HWND window = ToVar<HWND>(window_windows.hwnd);

		LONG style = GetWindowLong(
			window,
			GWL_STYLE);

		return (style &
			(WS_THICKFRAME
				| WS_MAXIMIZEBOX)) != 0;
	}

	void Window::SetFullscreenState(bool state)
	{
		HWND window = ToVar<HWND>(window_windows.hwnd);

		if (state)
		{
			//save current pos, size and style

			oldPos = GetPosition();
			oldSize = GetClientRectSize();
			LONG style = GetWindowLong(window, GWL_STYLE);

			oldStyle = 0;
			if (style & WS_CAPTION)     oldStyle |= (1 << 0);
			if (style & WS_THICKFRAME)  oldStyle |= (1 << 1);
			if (style & WS_MINIMIZEBOX) oldStyle |= (1 << 2);
			if (style & WS_MAXIMIZEBOX) oldStyle |= (1 << 3);
			if (style & WS_SYSMENU)     oldStyle |= (1 << 4);

			//remove decorations
			style &= ~(
				WS_CAPTION
				| WS_THICKFRAME
				| WS_MINIMIZEBOX
				| WS_MAXIMIZEBOX
				| WS_SYSMENU);
			SetWindowLong(window, GWL_STYLE, style);

			//expand to monitor bounds

			HMONITOR hMonitor = MonitorFromWindow(
				window,
				MONITOR_DEFAULTTONEAREST);
			MONITORINFO mi{};
			mi.cbSize = sizeof(mi);
			GetMonitorInfo(hMonitor, &mi);

			SetWindowPos(
				window,
				HWND_TOP,
				mi.rcMonitor.left,
				mi.rcMonitor.top,
				mi.rcMonitor.right - mi.rcMonitor.left,
				mi.rcMonitor.bottom - mi.rcMonitor.top,
				SWP_FRAMECHANGED
				| SWP_NOOWNERZORDER);
		}
		else
		{
			if (oldPos == vec2()) oldPos = vec2(100.0f, 100.0f);
			if (oldSize == vec2()) oldSize = vec2(800.0f, 600.0f);
			if (oldStyle == 0) oldStyle = 0b11111; //enable all flags

			//rebuild style from saved flags

			LONG style = GetWindowLong(window, GWL_STYLE);
			style &= ~(
				WS_CAPTION
				| WS_THICKFRAME
				| WS_MINIMIZEBOX
				| WS_MAXIMIZEBOX
				| WS_SYSMENU);

			if (oldStyle & (1 << 0)) style |= WS_CAPTION;
			if (oldStyle & (1 << 1)) style |= WS_THICKFRAME;
			if (oldStyle & (1 << 2)) style |= WS_MINIMIZEBOX;
			if (oldStyle & (1 << 3)) style |= WS_MAXIMIZEBOX;
			if (oldStyle & (1 << 4)) style |= WS_SYSMENU;

			SetWindowLong(window, GWL_STYLE, style);

			SetWindowPos(
				window,
				nullptr,
				(int)oldPos.x,
				(int)oldPos.y,
				(int)oldSize.x,
				(int)oldSize.y,
				SWP_FRAMECHANGED
				| SWP_NOZORDER
				| SWP_NOOWNERZORDER);
		}
	}
	bool Window::IsFullscreen() const
	{
		HWND window = ToVar<HWND>(window_windows.hwnd);

		vec2 pos = GetPosition();
		vec2 size = GetOuterSize();

		//expand to monitor bounds

		HMONITOR hMonitor = MonitorFromWindow(
			window,
			MONITOR_DEFAULTTONEAREST);
		MONITORINFO mi{};
		mi.cbSize = sizeof(mi);
		GetMonitorInfo(hMonitor, &mi);

		bool rectMatches =
			pos.x == mi.rcMonitor.left
			&& pos.y == mi.rcMonitor.top
			&& size.x == (mi.rcMonitor.right - mi.rcMonitor.left)
			&& size.y == (mi.rcMonitor.bottom - mi.rcMonitor.top);

		LONG style = GetWindowLong(
			window,
			GWL_STYLE);
		bool undecorated = (style & (
			WS_CAPTION
			| WS_THICKFRAME
			| WS_MINIMIZEBOX
			| WS_MAXIMIZEBOX
			| WS_SYSMENU)) == 0;

		return rectMatches && undecorated;
	}

	void Window::SetTopBarState(bool state) const
	{
		HWND window = ToVar<HWND>(window_windows.hwnd);

		LONG style = GetWindowLong(window, GWL_STYLE);

		if (state) style |= (WS_CAPTION);
		else style &= ~(WS_CAPTION);

		SetWindowLong(
			window,
			GWL_STYLE,
			style);

		SetWindowPos(
			window,
			nullptr,
			0,
			0,
			0,
			0,
			SWP_NOMOVE
			| SWP_NOSIZE
			| SWP_NOZORDER
			| SWP_FRAMECHANGED);
	}
	bool Window::IsTopBarEnabled() const
	{
		HWND window = ToVar<HWND>(window_windows.hwnd);

		LONG style = GetWindowLong(
			window,
			GWL_STYLE);

		return (style & WS_CAPTION) != 0;
	}

	void Window::SetMinimizeButtonState(bool state) const
	{
		HWND window = ToVar<HWND>(window_windows.hwnd);

		LONG style = GetWindowLong(window, GWL_STYLE);

		if (state) style |= (WS_MINIMIZEBOX);
		else style &= ~(WS_MINIMIZEBOX);

		SetWindowLong(
			window,
			GWL_STYLE,
			style);

		SetWindowPos(
			window,
			nullptr,
			0,
			0,
			0,
			0,
			SWP_NOMOVE
			| SWP_NOSIZE
			| SWP_NOZORDER
			| SWP_FRAMECHANGED);
	}
	bool Window::IsMinimizeButtonEnabled() const
	{
		HWND window = ToVar<HWND>(window_windows.hwnd);

		LONG style = GetWindowLong(
			window,
			GWL_STYLE);

		return (style & WS_MINIMIZEBOX) != 0;
	}

	void Window::SetMaximizeButtonState(bool state) const
	{
		HWND window = ToVar<HWND>(window_windows.hwnd);

		LONG style = GetWindowLong(window, GWL_STYLE);

		if (state) style |= (WS_MAXIMIZEBOX);
		else style &= ~(WS_MAXIMIZEBOX);

		SetWindowLong(
			window,
			GWL_STYLE,
			style);

		SetWindowPos(
			window,
			nullptr,
			0,
			0,
			0,
			0,
			SWP_NOMOVE
			| SWP_NOSIZE
			| SWP_NOZORDER
			| SWP_FRAMECHANGED);
	}
	bool Window::IsMaximizeButtonEnabled() const
	{
		HWND window = ToVar<HWND>(window_windows.hwnd);

		LONG style = GetWindowLong(
			window,
			GWL_STYLE);

		return (style & WS_MAXIMIZEBOX) != 0;
	}

	void Window::SetCloseButtonState(bool state) const
	{
		HWND window = ToVar<HWND>(window_windows.hwnd);

		HMENU hSysMenu = GetSystemMenu(window, FALSE);
		if (!hSysMenu) return;

		if (state) GetSystemMenu(window, TRUE);
		else
		{
			RemoveMenu(
				hSysMenu,
				SC_CLOSE,
				MF_BYCOMMAND);

			DrawMenuBar(window);
		}
	}
	bool Window::IsCloseButtonEnabled() const
	{
		HWND window = ToVar<HWND>(window_windows.hwnd);

		HMENU hSysMenu = GetSystemMenu(window, FALSE);
		if (!hSysMenu) return false; //no system menu

		return (GetMenuState(
			hSysMenu,
			SC_CLOSE,
			MF_BYCOMMAND) != (UINT)-1);
	}

	void Window::SetSystemMenuState(bool state) const
	{
		HWND window = ToVar<HWND>(window_windows.hwnd);

		LONG style = GetWindowLong(window, GWL_STYLE);

		if (state) style |= (WS_SYSMENU);
		else style &= ~(WS_SYSMENU);

		SetWindowLong(
			window,
			GWL_STYLE,
			style);

		SetWindowPos(
			window,
			nullptr,
			0,
			0,
			0,
			0,
			SWP_NOMOVE
			| SWP_NOSIZE
			| SWP_NOZORDER
			| SWP_FRAMECHANGED);
	}
	bool Window::IsSystemMenuEnabled() const
	{
		HWND window = ToVar<HWND>(window_windows.hwnd);

		LONG style = GetWindowLong(
			window,
			GWL_STYLE);

		return (style & WS_SYSMENU) != 0;
	}

	void Window::SetOpacity(float alpha) const
	{
		HWND window = ToVar<HWND>(window_windows.hwnd);

		float clamped = clamp(alpha, 0.0f, 1.0f);

		BYTE bAlpha = static_cast<BYTE>(alpha * 255.0f);

		//WS_EX_LAYERED is required for opacity

		LONG exStyle = GetWindowLong(
			window,
			GWL_EXSTYLE);
		if (!(exStyle & WS_EX_LAYERED))
		{
			SetWindowLong(
				window,
				GWL_EXSTYLE,
				exStyle | WS_EX_LAYERED);
		}

		SetLayeredWindowAttributes(
			window,
			0,
			bAlpha,
			LWA_ALPHA);
	}
	float Window::GetOpacity() const
	{
		HWND window = ToVar<HWND>(window_windows.hwnd);

		BYTE bAlpha = 255;
		DWORD flags = 0;
		COLORREF crKey = 0;

		if (GetLayeredWindowAttributes(
			window,
			&crKey,
			&bAlpha,
			&flags)
			&& (flags & LWA_ALPHA))
		{
			return static_cast<float>(bAlpha) / 255;
		}

		//treat as fully opaque when not layered
		return 1.0f;
	}

	bool Window::IsFocused() const
	{
		HWND hwnd = ToVar<HWND>(window_windows.hwnd);

		return hwnd == GetForegroundWindow();
	}

	bool Window::IsMinimized() const
	{
		HWND hwnd = ToVar<HWND>(window_windows.hwnd);

		//IsIconic returns TRUE if the window is minimized
		return IsIconic(hwnd);
	}

	bool Window::IsVisible() const
	{
		HWND hwnd = ToVar<HWND>(window_windows.hwnd);

		return IsWindowVisible(hwnd);
	}

	void Window::SetWindowState(WindowState state) const
	{
		HWND hwnd = ToVar<HWND>(window_windows.hwnd);

		switch (state)
		{
		case WindowState::WINDOW_NORMAL:
			ShowWindow(hwnd, SW_SHOWNORMAL);
			break;
		case WindowState::WINDOW_MAXIMIZE:
			ShowWindow(hwnd, SW_MAXIMIZE);
			break;
		case WindowState::WINDOW_MINIMIZE:
			ShowWindow(hwnd, SW_MINIMIZE);
			break;
		case WindowState::WINDOW_HIDE:
			ShowWindow(hwnd, SW_HIDE);
			break;
		case WindowState::WINDOW_SHOWNOACTIVATE:
			ShowWindow(hwnd, SW_SHOWNOACTIVATE);
			break;
		}

		UpdateWindow(hwnd);
	}

	void Window::Update()
	{
		if (!isInitialized)
		{
			Log::Print(
				"Cannot run loop because window '" +
				GetTitle() +
				"' has not been initialized!",
				"WINDOW_WINDOWS",
				LogType::LOG_ERROR,
				2);
			return;
		}

		UpdateIdleState(
			this,
			isIdle);

		MSG msg;
		
		if (isWindowFocusRequired
			&& isIdle
			&& !PeekMessage(&msg, nullptr, 0, 0, PM_NOREMOVE)) 
		{
			WaitMessage();
		}

		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg); //translate virtual-key messages (like WM_KEYDOWN) to character messages (WM_CHAR)
			DispatchMessage(&msg);  //send the message to the window procedure
		}
	}

	Window::~Window()
	{
		WindowData win = window_windows;
		HWND winRef = ToVar<HWND>(win.hwnd);
		SetWindowState(WindowState::WINDOW_HIDE);

		//destroy menu bar if it was created
		if (MenuBar::HasMenuBar(this)) MenuBar::DestroyMenuBar(this);

		OpenGLData openGLData = GetOpenGLData();

		if (openGLData.hglrc)
		{
			wglMakeCurrent(nullptr, nullptr);
			wglDeleteContext(ToVar<HGLRC>(openGLData.hglrc));
			openGLData.hglrc = NULL;
		}
		if (win.wndProc) win.wndProc = NULL;
		if (openGLData.hdc)
		{
			ReleaseDC(
				ToVar<HWND>(win.hwnd),
				ToVar<HDC>(openGLData.hdc));
			openGLData.hdc = NULL;
		}

		//Renderer_Vulkan::DestroyWindowData(this);

		if (exeIcon)
		{
			DestroyIcon(exeIcon);
			exeIcon = nullptr;
		}
		if (overlayIcon)
		{
			DestroyIcon(overlayIcon);
			overlayIcon = nullptr;
		}

		if (win.hwnd)
		{
			DestroyWindow(winRef);
			win.hwnd = NULL;
		}
		win.hInstance = NULL;

		Log::Print(
			"Destroyed window '" + GetTitle() + "'!",
			"WINDOW_WINDOWS",
			LogType::LOG_SUCCESS);
	}

	//
	// MENU BAR CLASS DEFINITIONS
	//

	void MenuBar::CreateMenuBar(Window* windowRef)
	{
		HWND window = ToVar<HWND>(windowRef->GetWindowData().hwnd);

		if (HasMenuBar(windowRef))
		{
			Log::Print(
				"Failed to add menu bar to window '" + windowRef->GetTitle() + "' because the window already has one!",
				"WINDOW_WINDOWS",
				LogType::LOG_ERROR);

			return;
		}

		HMENU hMenu = CreateMenu();
		SetMenu(window, hMenu);
		DrawMenuBar(window);

		ostringstream oss{};
		oss << "Created new menu bar in window '" << windowRef->GetTitle() << "'!";

		Log::Print(
			oss.str(),
			"WINDOW_WINDOWS",
			LogType::LOG_SUCCESS);
	}
	bool MenuBar::HasMenuBar(Window* windowRef)
	{
		HWND window = ToVar<HWND>(windowRef->GetWindowData().hwnd);

		return (GetMenu(window) != nullptr);
	}

	void MenuBar::CallMenuBarEvent(
		Window* windowRef,
		const string& parentRef,
		const string& labelRef)
	{
		HWND window = ToVar<HWND>(windowRef->GetWindowData().hwnd);

		if (!HasMenuBar(windowRef))
		{
			ostringstream oss{};
			oss << "Failed to call menu bar event in window '" << windowRef->GetTitle() << "' because it has no menu bar!";

			Log::Print(
				oss.str(),
				"WINDOW_WINDOWS",
				LogType::LOG_ERROR);

			return;
		}

		if (runtimeMenuBarEvents.empty())
		{
			ostringstream oss{};
			oss << "Failed to call menu bar event by label '" << labelRef << "' or parent label '" << parentRef
				<< "' in window '" << windowRef->GetTitle() << "' because there are no menu bar events!";

			Log::Print(
				oss.str(),
				"WINDOW_WINDOWS",
				LogType::LOG_ERROR);

			return;
		}

		for (const auto& e : runtimeMenuBarEvents)
		{
			const string& parent = e->parentLabel;
			const string& label = e->label;
			if ((parent.empty()
				&& label == e->label)
				|| (parent == parentRef
				&& label == labelRef))
			{
				ostringstream oss{};
				oss << "Ran function attached to label '" << label
					<< "' in window '" << windowRef->GetTitle() << "'!";

				Log::Print(
					oss.str(),
					"WINDOW_WINDOWS",
					LogType::LOG_DEBUG);

				e->function();

				return;
			}
		}

		ostringstream oss{};
		oss << "Failed to call menu bar event by label '" << labelRef
			<< "' in window '" + windowRef->GetTitle() + "' because the event does not exist!";

		Log::Print(
			oss.str(),
			"WINDOW_WINDOWS",
			LogType::LOG_ERROR);
	}
	void MenuBar::CallMenuBarEvent(
		Window* windowRef,
		u32 IDRef)
	{
		HWND window = ToVar<HWND>(windowRef->GetWindowData().hwnd);

		if (!HasMenuBar(windowRef))
		{
			ostringstream oss{};
			oss << "Failed to call menu bar event in window '" << windowRef->GetTitle() << "' because it has no menu bar!";

			Log::Print(
				oss.str(),
				"WINDOW_WINDOWS",
				LogType::LOG_ERROR);

			return;
		}

		if (runtimeMenuBarEvents.empty())
		{
			ostringstream oss{};
			oss << "Failed to call menu bar event by ID '" << to_string(IDRef) 
				<< "' in window '" << windowRef->GetTitle() << "' because there are no menu bar events!";

			Log::Print(
				oss.str(),
				"WINDOW_WINDOWS",
				LogType::LOG_ERROR);

			return;
		}

		for (const auto& e : runtimeMenuBarEvents)
		{
			u32 ID = e->labelID;
			if (ID == IDRef)
			{
				ostringstream oss{};
				oss << "Ran function attached to ID '" << to_string(IDRef)
					<< "' in window '" << windowRef->GetTitle() << "'!";

				Log::Print(
					oss.str(),
					"WINDOW_WINDOWS",
					LogType::LOG_DEBUG);

				e->function();

				return;
			}
		}

		ostringstream oss{};
		oss << "Failed to call menu bar event by ID '" << to_string(IDRef)
			<< "' in window '" + windowRef->GetTitle() + "' because the event does not exist!";

		Log::Print(
			oss.str(),
			"WINDOW_WINDOWS",
			LogType::LOG_ERROR);
	}

	void MenuBar::CreateLabel(
		Window* windowRef,
		LabelType type,
		const string& parentRef,
		const string& labelRef,
		const function<void()> func)
	{
		HWND window = ToVar<HWND>(windowRef->GetWindowData().hwnd);

		string typeName = type == LabelType::LABEL_LEAF ? "leaf" : "branch";

		string parentName = parentRef;
		if (parentName.empty()) parentName = "root";

		if (!HasMenuBar(windowRef))
		{
			ostringstream oss{};
			oss << "Failed to add " << typeName << " to window '" << windowRef->GetTitle() << "' because no menu bar was created!";

			Log::Print(
				oss.str(),
				"WINDOW_WINDOWS",
				LogType::LOG_ERROR);

			return;
		}

		if (labelRef.empty())
		{
			ostringstream oss{};
			oss << "Failed to add " << typeName << " to window '" << windowRef->GetTitle() << "' because the label name is empty!";

			Log::Print(
				oss.str(),
				"WINDOW_WINDOWS",
				LogType::LOG_ERROR);

			return;
		}
		if (labelRef.length() > MAX_LABEL_LENGTH)
		{
			ostringstream oss{};
			oss << "Failed to add " << typeName << " '" << labelRef
				<< "' to window '" << windowRef->GetTitle() << "' because the label length '"
				<< labelRef.length() << "' is too long! You can only use label length up to '"
				<< to_string(MAX_LABEL_LENGTH) << "' characters long.";

			Log::Print(
				oss.str(),
				"WINDOW_WINDOWS",
				LogType::LOG_ERROR);

			return;
		}

		//leaf requires valid function
		if (type == LabelType::LABEL_LEAF
			&& func == nullptr)
		{
			ostringstream oss{};
			oss << "Failed to add leaf '" << labelRef << "' under parent '" << parentRef
				<< "' in window '" << windowRef->GetTitle() << "' because the leaf has an empty function!";

			Log::Print(
				oss.str(),
				"WINDOW_WINDOWS",
				LogType::LOG_ERROR);

			return;
		}

		//leaf cant have parent that is also a leaf
		if (type == LabelType::LABEL_LEAF
			&& !parentRef.empty())
		{
			for (const auto& e : runtimeMenuBarEvents)
			{
				if (e->label == parentRef
					&& e->labelID != 0)
				{
					ostringstream oss{};
					oss << "Failed to add leaf '" << labelRef << "' under parent '" << parentRef
						<< "' in window '" << windowRef->GetTitle() << "' because the parent is also a leaf!";

					Log::Print(
						oss.str(),
						"WINDOW_WINDOWS",
						LogType::LOG_ERROR);

					return;
				}
			}
		}

		//check if label or the parent of the label already exists or not
		for (const auto& e : runtimeMenuBarEvents)
		{
			const string& parent = e->parentLabel;
			const string& label = e->label;
			if (parent.empty()
				&& labelRef == label)
			{
				ostringstream oss{};
				oss << "Failed to add " << typeName << " '" << labelRef << "' to window '" << windowRef->GetTitle() 
					<< "' because the " << typeName << " already exists!";

				Log::Print(
					oss.str(),
					"WINDOW_WINDOWS",
					LogType::LOG_ERROR);

				return;
			}
			else if (parentRef == parent
				&& labelRef == label)
			{
				ostringstream oss{};
				oss << "Failed to add " << typeName << " '" << labelRef << "' under parent '" << parentName
					<< "' in window '" << windowRef->GetTitle()
					<< "' because the " << typeName << " and its parent already exists!";

				Log::Print(
					oss.str(),
					"WINDOW_WINDOWS",
					LogType::LOG_ERROR);

				return;
			}
		}

		HMENU hMenu = GetMenu(window);
		u32 newID = ++globalID;

		unique_ptr<MenuBarEvent> newEvent = make_unique<MenuBarEvent>();
		newEvent->parentLabel = parentRef;
		newEvent->label = labelRef;
		
		if (type == LabelType::LABEL_LEAF)
		{
			newEvent->function = func;
			newEvent->labelID = newID;
		}

		auto NewLabel = [&](HMENU parentMenu)
			{
				if (type == LabelType::LABEL_BRANCH)
				{
					HMENU thisMenu = CreatePopupMenu();
					AppendMenu(
						parentMenu,
						MF_POPUP,
						(UINT_PTR)thisMenu,
						ToWide(labelRef).c_str());

					newEvent->hMenu = FromVar(thisMenu);
				}
				else
				{
					AppendMenu(
						parentMenu,
						MF_STRING,
						newID,
						ToWide(labelRef).c_str());
				}

				ostringstream oss{};
				oss << "Added " << typeName << " '" << labelRef << "' with ID '" << to_string(newID)
					<< "' under parent '" << parentName
					<< "' in window '" << windowRef->GetTitle() << "'!";

				Log::Print(
					oss.str(),
					"WINDOW_WINDOWS",
					LogType::LOG_SUCCESS);
			};

		if (parentRef.empty()) NewLabel(hMenu);
		else
		{
			HMENU parentMenu{};

			for (const auto& value : runtimeMenuBarEvents)
			{
				if (value->label == parentRef)
				{
					parentMenu = ToVar<HMENU>(value->hMenu);
					break;
				}
			}

			if (!parentMenu)
			{
				ostringstream oss{};
				oss << "Failed to create " << typeName << " '" << labelRef << "' under parent '" << parentName
					<< "' in window '" << windowRef->GetTitle() << "' because the parent does not exist!";

				Log::Print(
					oss.str(),
					"WINDOW_WINDOWS",
					LogType::LOG_ERROR);

				return;
			}

			NewLabel(parentMenu);
		}

		DrawMenuBar(window);

		createdMenuBarEvents[newID] = move(newEvent);

		MenuBarEvent* storedEvent = createdMenuBarEvents[newID].get();
		runtimeMenuBarEvents.push_back(storedEvent);
	}

	void MenuBar::AddSeparator(
		Window* windowRef,
		const string& parentRef,
		const string& labelRef)
	{
		HWND window = ToVar<HWND>(windowRef->GetWindowData().hwnd);

		if (!HasMenuBar(windowRef))
		{
			ostringstream oss{};
			oss << "Failed to add separator to menu label '" << labelRef << "' in window '" << windowRef->GetTitle()
				<< "' because it has no menu bar!";

			Log::Print(
				oss.str(),
				"WINDOW_WINDOWS",
				LogType::LOG_ERROR);

			return;
		}

		HMENU hMenu = GetMenu(window);

		for (const auto& e : runtimeMenuBarEvents)
		{
			const string& parent = e->parentLabel;
			const string& label = e->label;
			if (label.empty())
			{
				if (parent == parentRef)
				{
					HMENU parentMenu{};

					for (const auto& value : runtimeMenuBarEvents)
					{
						if (value->label == parentRef)
						{
							parentMenu = ToVar<HMENU>(value->hMenu);
							break;
						}
					}

					if (!parentMenu)
					{
						ostringstream oss{};
						oss << "Failed to add separator at the end of parent '" << parentRef
							<< "' in window '" << windowRef->GetTitle() << "' because the parent does not exist!";

						Log::Print(
							oss.str(),
							"WINDOW_WINDOWS",
							LogType::LOG_ERROR);

						return;
					}

					AppendMenu(
						parentMenu,
						MF_SEPARATOR,
						0,
						nullptr);

					Log::Print(
						"Placed separator to the end of parent label '" + parentRef + "' in window '" + windowRef->GetTitle() + "'!",
						"WINDOW_WINDOWS",
						LogType::LOG_SUCCESS);

					DrawMenuBar(window);

					return;
				}
			}
			else
			{
				if (parent == parentRef
					&& label == labelRef)
				{
					HMENU parentMenu{};

					for (const auto& value : runtimeMenuBarEvents)
					{
						if (value->label == parentRef)
						{
							parentMenu = ToVar<HMENU>(value->hMenu);
							break;
						}
					}

					if (!parentMenu)
					{
						ostringstream oss{};
						oss << "Failed to add separator under parent '" << parentRef << "' after label '" << labelRef
							<< "' in window '" << windowRef->GetTitle() << "' because the label does not exist!";

						Log::Print(
							oss.str(),
							"WINDOW_WINDOWS",
							LogType::LOG_ERROR);

						return;
					}

					int pos = GetMenuItemCount(parentMenu);
					for (int i = 0; i < pos; ++i)
					{
						wchar_t buffer[MAX_LABEL_LENGTH + 1]{};
						GetMenuStringW(
							parentMenu,
							i,
							buffer,
							MAX_LABEL_LENGTH + 1,
							MF_BYPOSITION);

						if (ToWide(labelRef) == buffer)
						{
							InsertMenuW(
								parentMenu,
								i + 1, //insert after this item
								MF_BYPOSITION
								| MF_SEPARATOR,
								0,
								nullptr);

							Log::Print(
								"Placed separator after label '" + labelRef + "' in window '" + windowRef->GetTitle() + "'!",
								"WINDOW_WINDOWS",
								LogType::LOG_SUCCESS);

							DrawMenuBar(window);

							return;
						}
					}
				}
			}
		}

		ostringstream oss{};
		oss << "Failed to add separator at the end of parent '" << parentRef << "' or after label '" << labelRef
			<< "' in window '" + windowRef->GetTitle() + "' because parent or label does not exist!";

		Log::Print(
			oss.str(),
			"WINDOW_WINDOWS",
			LogType::LOG_ERROR);
	}

	void MenuBar::DestroyMenuBar(Window* windowRef)
	{
		HWND window = ToVar<HWND>(windowRef->GetWindowData().hwnd);

		HMENU hMenu = GetMenu(window);

		//detach the menu bar from the window first
		SetMenu(window, nullptr);
		DrawMenuBar(window);

		//and finally destroy the menu handle itself
		DestroyMenu(hMenu);

		ostringstream oss{};
		oss << "Destroyed menu bar in window '" << windowRef->GetTitle() << "'!";

		Log::Print(
			oss.str(),
			"WINDOW_WINDOWS",
			LogType::LOG_SUCCESS);
	}
}

void UpdateIdleState(Window* window, bool& isIdle)
{
	isIdle =
		!window->IsFocused()
		|| window->IsMinimized()
		|| !window->IsVisible();
}

HICON SetUpIcon(OpenGL_Texture* texture)
{
	string name = texture->GetName();
	vec2 size = texture->GetSize();
	string sizeX = to_string(static_cast<int>(size.x));
	string sizeY = to_string(static_cast<int>(size.y));

	if (texture->GetSize().x < 16)
	{
		Log::Print(
			"Icon '" + name + "' size '" + sizeX + "x" + sizeY + "' is too small! Consider uploading a bigger icon.",
			"WINDOW_WINDOWS",
			LogType::LOG_WARNING);
	}
	if (size.x > 256)
	{
		Log::Print(
			"Icon '" + name + "' size '" + sizeX + "x" + sizeY + "' is too big! Consider uploading a smaller icon.",
			"WINDOW_WINDOWS",
			LogType::LOG_WARNING);
	}

	//convert RGBA to BGRA

	const vector<u8>& pixels = texture->GetPixels();
	vector<u8> pixelsBGRA(pixels.size());

	for (size_t i = 0; i < static_cast<size_t>(size.x) * size.y; i++)
	{
		size_t idx = i * 4;
		pixelsBGRA[idx + 0] = pixels[idx + 2]; // B
		pixelsBGRA[idx + 1] = pixels[idx + 1]; // G
		pixelsBGRA[idx + 2] = pixels[idx + 0]; // R
		pixelsBGRA[idx + 3] = pixels[idx + 3]; // A
	}

	//create DIB section (bitmap)
	BITMAPV5HEADER bi{};
	bi.bV5Size = sizeof(BITMAPV5HEADER);
	bi.bV5Width = size.x;
	bi.bV5Height = -size.y; //negative - top-down
	bi.bV5Planes = 1;
	bi.bV5BitCount = 32;
	bi.bV5Compression = BI_BITFIELDS;
	bi.bV5RedMask = 0x00FF0000;
	bi.bV5GreenMask = 0x0000FF00;
	bi.bV5BlueMask = 0x000000FF;
	bi.bV5AlphaMask = 0xFF000000;

	HDC hdc = CreateCompatibleDC(nullptr);
	void* pvBits = nullptr;
	HBITMAP hBitMap = CreateDIBSection(
		hdc,
		reinterpret_cast<BITMAPINFO*>(&bi),
		DIB_RGB_COLORS,
		&pvBits,
		nullptr,
		0);
	DeleteDC(hdc);

	if (!hBitMap)
	{
		Log::Print(
			"Failed to create hBitMask for setting window '" + name + "' icon!",
			"WINDOW_WINDOWS",
			LogType::LOG_ERROR);

		return nullptr;
	}

	memcpy(
		pvBits,
		pixelsBGRA.data(),
		pixelsBGRA.size());

	//wrap into ICONINFO

	ICONINFO ii{};
	ii.fIcon = TRUE;
	ii.hbmMask = hBitMap;
	ii.hbmColor = hBitMap;

	HICON hIcon = CreateIconIndirect(&ii);

	DeleteObject(hBitMap);

	return hIcon;
}

wstring ToWide(const string& str)
{
	if (str.empty()) return wstring();

	int size_needed = MultiByteToWideChar(
		CP_UTF8,
		0,
		str.c_str(),
		(int)str.size(),
		nullptr,
		0);

	wstring wstr(size_needed, 0);

	MultiByteToWideChar(
		CP_UTF8,
		0,
		str.c_str(),
		(int)str.size(),
		&wstr[0],
		size_needed);

	return wstr;
}
string ToShort(const wstring& str)
{
	if (str.empty()) return string();

	int size_needed = WideCharToMultiByte(
		CP_UTF8,
		0,
		str.c_str(),
		(int)str.size(),
		nullptr,
		0,
		nullptr,
		nullptr);

	string result(size_needed, 0);

	WideCharToMultiByte(
		CP_UTF8,
		0,
		str.c_str(),
		(int)str.size(),
		&result[0],
		size_needed,
		nullptr,
		nullptr);

	return result;
}

#endif //_WIN32