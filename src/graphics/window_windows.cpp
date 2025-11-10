//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef _WIN32

#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#include <ShObjIdl.h>
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")
#include <atlbase.h>
#include <atlcomcli.h>
#include <wtsapi32.h>
#pragma comment(lib, "Wtsapi32.lib")
#include <shellapi.h>

#include <algorithm>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "KalaHeaders/log_utils.hpp"

#include "graphics/window.hpp"
#include "graphics/window_global.hpp"
#include "graphics/opengl/opengl.hpp"
#include "graphics/menubar.hpp"
#include "core/input.hpp"
#include "core/core.hpp"
#include "utils/registry.hpp"

using KalaHeaders::vec2;
using KalaHeaders::Log;
using KalaHeaders::LogType;

using KalaWindow::Graphics::OpenGL::OpenGL_Context;
using KalaWindow::Graphics::Window;
using KalaWindow::Graphics::MenuBar;
using KalaWindow::Core::KalaWindowCore;
using KalaWindow::Core::globalID;
using KalaWindow::Core::Input;
using KalaWindow::Utils::Registry;

using std::make_unique;
using std::move;
using std::to_string;
using std::unique_ptr;
using std::clamp;
using std::ostringstream;
using std::wstring;
using std::string;
using std::vector;

constexpr u16 MAX_TITLE_LENGTH = 512;

//KalaWindow will dynamically update window idle state
static void UpdateIdleState(Window* window, bool& isIdle);

/*
TODO: add texture support back

static HICON SetUpIcon(OpenGL_Texture* texture);
*/

static HICON exeIcon{};
static HICON overlayIcon{};

static wstring ToWide(const string& str);
static string ToShort(const wstring& str);

static string HResultToString(HRESULT hr);

namespace KalaWindow::Graphics
{
	Window* Window::Initialize(
		const string& title,
		vec2 size,
		Window* parentWindow,
		WindowState state,
		DpiContext context)
	{
		if (!Window_Global::IsInitialized())
		{
			KalaWindowCore::ForceClose(
				"Window error",
				"Failed to create window '" + title + "' because global window context has not been created!");

			return nullptr;
		}

		u32 newID = ++globalID;
		unique_ptr<Window> newWindow = make_unique<Window>();
		Window* windowPtr = newWindow.get();

		Log::Print(
			"Creating window '" + title + "' with ID '" + to_string(newID) + "'.",
			"WINDOW",
			LogType::LOG_DEBUG);

		HWND parentWindowRef{};
		if (parentWindow != nullptr)
		{
			if (find(registry.runtimeContent.begin(),
				registry.runtimeContent.end(),
				parentWindow)
				== registry.runtimeContent.end())
			{
				KalaWindowCore::ForceClose(
					"Window error",
					"Failed to create child window '" + title + "' because parent window pointer does not exist!");

				return nullptr;
			}

			parentWindowRef = ToVar<HWND>(parentWindow->GetWindowData().hwnd);

			if (!parentWindowRef)
			{
				KalaWindowCore::ForceClose(
					"Window error",
					"Failed to create child window '" + title + "' because parent window handle is invalid!");

				return nullptr;
			}
		}

		DWORD exStyle =
			WS_EX_APPWINDOW
			| WS_EX_ACCEPTFILES;

		HINSTANCE newHInstance = GetModuleHandle(nullptr);

		wstring appIDWide = ToWide(Window_Global::GetAppID());
		wstring titleWide = ToWide(title);

		HWND newHwnd = CreateWindowExW(
			exStyle,
			appIDWide.c_str(),
			titleWide.c_str(),
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			size.x,
			size.y,
			parentWindowRef,
			nullptr,
			newHInstance,
			windowPtr);

		if (!newHwnd)
		{
			DWORD errorCode = GetLastError();
			LPWSTR errorMsg = nullptr;
			FormatMessageW(
				FORMAT_MESSAGE_ALLOCATE_BUFFER
				| FORMAT_MESSAGE_FROM_SYSTEM
				| FORMAT_MESSAGE_IGNORE_INSERTS,
				nullptr,
				errorCode,
				0,
				(LPWSTR)&errorMsg,
				0,
				nullptr);

			LPCWSTR result = errorMsg != nullptr ? errorMsg : L"Unknown";

			ostringstream msg{};

			msg << "CreateWindowExW failed with error "
				<< errorCode << ": "
				<< ToShort(result);

			if (errorMsg) LocalFree(errorMsg);

			KalaWindowCore::ForceClose(
				"Window error",
				msg.str());

			return nullptr;
		}

		SetWindowLongPtr(
			newHwnd,
			GWLP_USERDATA,
			reinterpret_cast<LONG_PTR>(windowPtr));

		WindowData newWindowStruct =
		{
			.hwnd = FromVar(newHwnd),
			.hInstance = FromVar(newHInstance),
			.wndProc = FromVar((WNDPROC)GetWindowLongPtr(newHwnd, GWLP_WNDPROC))
		};

		//set window dpi aware state
		switch (context)
		{
		case DpiContext::DPI_PER_MONITOR:
			SetProcessDpiAwarenessContext(
				DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
			break;
		case DpiContext::DPI_SYSTEM_AWARE:
			SetProcessDpiAwarenessContext(
				DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);
			break;
		case DpiContext::DPI_UNAWARE:
			SetProcessDpiAwarenessContext(
				DPI_AWARENESS_CONTEXT_UNAWARE);
			break;
		}
		
		windowPtr->SetTitle(title);
		windowPtr->ID = newID;
		windowPtr->SetClientRectSize(size);
		windowPtr->window_windows = newWindowStruct;

		windowPtr->isInitialized = true;

		//set window state to user preferred version
		windowPtr->SetWindowState(state);

		//allow files to be dragged to this window
		DragAcceptFiles(newHwnd, TRUE);

		registry.AddContent(newID, move(newWindow));
		if (parentWindow)
		{
			parentWindow->AddChildWindow(windowPtr);
			windowPtr->SetParentWindow(parentWindow);
		}

		Log::Print(
			"Created window '" + title + "' with ID '" + to_string(newID) + "'!",
			"WINDOW",
			LogType::LOG_SUCCESS);

		return windowPtr;
	}

	void Window::SetTitle(const string& newTitle) const
	{
		HWND window = ToVar<HWND>(window_windows.hwnd);
		if (!window) return;

		if (newTitle.empty())
		{
			Log::Print(
				"Window title cannot be empty!",
				"WINDOW",
				LogType::LOG_ERROR,
				2);

			return;
		}

		string titleToSet = newTitle;
		if (newTitle.length() > MAX_TITLE_LENGTH)
		{
			Log::Print(
				"Window title exceeded max allowed length of '" + to_string(MAX_TITLE_LENGTH) + "'! Title has been truncated.",
				"WINDOW",
				LogType::LOG_WARNING);

			titleToSet = titleToSet.substr(0, MAX_TITLE_LENGTH);
		}

		wstring wideTitle = ToWide(titleToSet);

		SetWindowTextW(
			window, 
			wideTitle.c_str());

		if (Window_Global::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set window title to '" + newTitle + "'",
				"WINDOW",
				LogType::LOG_SUCCESS);
		}
	}
	const string& Window::GetTitle() const
	{
		static string result{};

		HWND window = ToVar<HWND>(window_windows.hwnd);
		if (!window) return result;

		int length = GetWindowTextLengthW(window);
		if (length == 0)
		{
			Log::Print(
				"Window title was empty!",
				"WINDOW",
				LogType::LOG_WARNING);

			return result;
		}

		wstring title(length + 1, L'\0');
		GetWindowTextW(window, title.data(), length + 1);

		title.resize(wcslen(title.c_str()));
		result = ToShort(title);

		return result;
	}

	void Window::SetIcon(u32 texture) const
	{
		/*
		TODO: add texture support back
		
		HWND window = ToVar<HWND>(window_windows.hwnd);
		if (!window) return;

		OpenGL_Texture* tex = OpenGL_Texture::registry.GetContent(texture);

		if (!tex)
		{
			Log::Print(
				"Cannot set window '" + GetTitle() + "' exe icon because the texture ID is invalid!",
				"WINDOW",
				LogType::LOG_ERROR,
				2);

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
				"WINDOW",
				LogType::LOG_ERROR,
				2);

			return;
		}

		if (exeIcon)
		{
			DestroyIcon(exeIcon);
			exeIcon = nullptr;
		}

		exeIcon = SetUpIcon(tex);

		if (!exeIcon)
		{
			Log::Print(
				"Cannot set window '" + GetTitle() + "' icon because SetUpIcon failed!",
				"WINDOW",
				LogType::LOG_ERROR,
				2);

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

		if (Window_Global::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set window '" + GetTitle() + "' icon to '" + tex->GetName() + "'",
				"WINDOW",
				LogType::LOG_SUCCESS);
		}
		*/
	}
	void Window::ClearIcon() const
	{
		HWND window = ToVar<HWND>(window_windows.hwnd);
		if (!window) return;

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
		/*
		TODO: add texture support back
		
		HWND window = ToVar<HWND>(window_windows.hwnd);
		if (!window) return;

		OpenGL_Texture* tex = OpenGL_Texture::registry.GetContent(texture);

		if (!tex)
		{
			Log::Print(
				"Cannot set window '" + GetTitle() + "' overlay icon because the texture ID is invalid!",
				"WINDOW",
				LogType::LOG_ERROR,
				2);

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
				"WINDOW",
				LogType::LOG_ERROR,
				2);

			return;
		}

		if (overlayIcon)
		{
			DestroyIcon(overlayIcon);
			overlayIcon = nullptr;
		}
		
		overlayIcon = SetUpIcon(tex);

		if (!overlayIcon)
		{
			Log::Print(
				"Cannot set window '" + GetTitle() + "' overlay icon because SetUpIcon failed!",
				"WINDOW",
				LogType::LOG_ERROR,
				2);

			return;
		}

		CComPtr<ITaskbarList3> taskbar{};
		HRESULT hr = (CoCreateInstance(
			CLSID_TaskbarList,
			nullptr,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(&taskbar)));

		if (FAILED(hr)
			|| !taskbar)
		{
			Log::Print(
				"Failed to create ITaskbarList3 to set overlay icon!",
				"WINDOW",
				LogType::LOG_ERROR,
				2);

			return;
		}

		hr = taskbar->HrInit();

		if (FAILED(hr))
		{
			Log::Print(
				"Failed to init ITaskbarList3 to set overlay icon!",
				"WINDOW",
				LogType::LOG_ERROR,
				2);

			return;
		}

		taskbar->SetOverlayIcon(
			window,
			overlayIcon,
			tooltip.empty() ? nullptr : ToWide(tooltip).c_str());

		if (Window_Global::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set window '" + GetTitle() + "' overlay icon to '" + tex->GetName() + "'",
				"WINDOW",
				LogType::LOG_SUCCESS);
		}
		*/
	}
	void Window::ClearTaskbarOverlayIcon() const
	{
		HWND window = ToVar<HWND>(window_windows.hwnd);
		if (!window) return;

		CComPtr<ITaskbarList3> taskbar{};
		HRESULT hr = (CoCreateInstance(
			CLSID_TaskbarList,
			nullptr,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(&taskbar)));

		if (FAILED(hr))
		{
			Log::Print(
				"Failed to get ITaskbarList3 to clear overlay icon!",
				"WINDOW",
				LogType::LOG_ERROR,
				2);

			return;
		}

		taskbar->SetOverlayIcon(
			window,
			nullptr,
			nullptr);
	}

	void Window::BringToFocus() const
	{
		if (IsFocused()) return; //skip all logic if already focused

		HWND window = ToVar<HWND>(window_windows.hwnd);
		if (!window) return;

		WindowState state = GetWindowState();
		if (IsMinimized()
			|| !IsVisible()
			|| state == WindowState::WINDOW_MINIMIZE
			|| state == WindowState::WINDOW_HIDE)
		{
			SetWindowState(WindowState::WINDOW_NORMAL);
		}

		//ask Windows nicely to foreground this window
		SetForegroundWindow(window);
		SetActiveWindow(window);

		//fallback: force Z-order change
		if (!IsFocused())
		{
			SetWindowPos(
				window,
				HWND_TOPMOST,
				0,
				0,
				0,
				0,
				SWP_NOMOVE
				| SWP_NOSIZE
				| SWP_SHOWWINDOW);

			SetWindowPos(
				window,
				HWND_NOTOPMOST,
				0,
				0,
				0,
				0,
				SWP_NOMOVE
				| SWP_NOSIZE
				| SWP_SHOWWINDOW);

			SetForegroundWindow(window);

			if (Window_Global::IsVerboseLoggingEnabled())
			{
				Log::Print(
					"Set window '" + GetTitle() + "' focus through the fallback method.'",
					"WINDOW",
					LogType::LOG_SUCCESS);
			}
		}

		//ensure keyboard focus
		SetFocus(window);
	}

	void Window::SetWindowRounding(WindowRounding roundState) const
	{
		HWND window = ToVar<HWND>(window_windows.hwnd);
		if (!window) return;

		DWM_WINDOW_CORNER_PREFERENCE pref{};

		string roundingVal{};

		switch (roundState)
		{
		case WindowRounding::ROUNDING_DEFAULT:
			pref = DWMWCP_DEFAULT;
			roundingVal = "default";
			break;
		case WindowRounding::ROUNDING_NONE:
			pref = DWMWCP_DONOTROUND;
			roundingVal = "none";
			break;
		case WindowRounding::ROUNDING_ROUND:
			pref = DWMWCP_ROUND;
			roundingVal = "round";
			break;
		case WindowRounding::ROUNDING_ROUND_SMALL:
			pref = DWMWCP_ROUNDSMALL;
			roundingVal = "small round";
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
				"WINDOW",
				LogType::LOG_ERROR,
				2);
		}

		if (Window_Global::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set window '" + GetTitle() + "' rounding to '" + roundingVal + "'",
				"WINDOW",
				LogType::LOG_SUCCESS);
		}
	}
	WindowRounding Window::GetWindowRoundingState() const
	{
		HWND window = ToVar<HWND>(window_windows.hwnd);
		if (!window) return WindowRounding::ROUNDING_DEFAULT;

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
				"WINDOW",
				LogType::LOG_ERROR,
				2);

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
		if (!window) return;

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

		string val = to_string(newSize.x) + "x" + to_string(newSize.y);

		if (Window_Global::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set window '" + GetTitle() + "' client rect size to '" + val + "'",
				"WINDOW",
				LogType::LOG_SUCCESS);
		}
	}
	vec2 Window::GetClientRectSize() const
	{
		HWND window = ToVar<HWND>(window_windows.hwnd);
		if (!window) return{};

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
		if (!window) return;

		SetWindowPos(
			window,
			nullptr,
			0,
			0,
			newSize.x,
			newSize.y,
			SWP_NOMOVE
			| SWP_NOZORDER);

		string val = to_string(newSize.x) + "x" + to_string(newSize.y);

		if (Window_Global::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set window '" + GetTitle() + "' outer size to '" + val + "'",
				"WINDOW",
				LogType::LOG_SUCCESS);
		}
	}
	vec2 Window::GetOuterSize() const
	{
		HWND window = ToVar<HWND>(window_windows.hwnd);
		if (!window) return{};

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
		if (!window) return;

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

		string val = to_string(newSize.x) + "x" + to_string(newSize.y);

		if (Window_Global::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set window '" + GetTitle() + "' framebuffer size to '" + val + "'",
				"WINDOW",
				LogType::LOG_SUCCESS);
		}
	}
	vec2 Window::GetFramebufferSize() const
	{
		HWND window = ToVar<HWND>(window_windows.hwnd);
		if (!window) return{};

		UINT dpi = GetDpiForWindow(window);
		RECT rect{};
		GetClientRect(window, &rect);

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
		if (!window) return;

		SetWindowPos(
			window,
			nullptr,
			newPosition.x,
			newPosition.y,
			0,
			0,
			SWP_NOSIZE
			| SWP_NOZORDER);

		string val = to_string(newPosition.x) + "x" + to_string(newPosition.y);

		if (Window_Global::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set window '" + GetTitle() + "' position to '" + val + "'",
				"WINDOW",
				LogType::LOG_SUCCESS);
		}
	}
	vec2 Window::GetPosition() const
	{
		HWND window = ToVar<HWND>(window_windows.hwnd);
		if (!window) return{};

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
		if (!window) return;

		SetWindowPos(
			window,
			state ? HWND_TOPMOST : HWND_NOTOPMOST,
			0,
			0,
			0,
			0,
			SWP_NOMOVE
			| SWP_NOSIZE);

		string val = state ? "true" : "false";

		if (Window_Global::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set window '" + GetTitle() + "' always on state to '" + val + "'",
				"WINDOW",
				LogType::LOG_SUCCESS);
		}
	}
	bool Window::IsAlwaysOnTop() const
	{
		HWND window = ToVar<HWND>(window_windows.hwnd);
		if (!window) return false;

		LONG exStyle = GetWindowLong(
			window,
			GWL_EXSTYLE);

		return (exStyle & WS_EX_TOPMOST) != 0;
	}

	void Window::SetResizableState(bool state) const
	{
		HWND window = ToVar<HWND>(window_windows.hwnd);
		if (!window) return;

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

		string val = state ? "true" : "false";

		if (Window_Global::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set window '" + GetTitle() + "' resizable state to '" + val + "'",
				"WINDOW",
				LogType::LOG_SUCCESS);
		}
	}
	bool Window::IsResizable() const
	{
		HWND window = ToVar<HWND>(window_windows.hwnd);
		if (!window) return false;

		LONG style = GetWindowLong(
			window,
			GWL_STYLE);

		return (style &
			(WS_THICKFRAME
			| WS_MAXIMIZEBOX)) != 0;
	}

	void Window::SetTopBarState(bool state) const
	{
		HWND window = ToVar<HWND>(window_windows.hwnd);
		if (!window) return;

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

		string val = state ? "true" : "false";

		if (Window_Global::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set window '" + GetTitle() + "' top bar state to '" + val + "'",
				"WINDOW",
				LogType::LOG_SUCCESS);
		}
	}
	bool Window::IsTopBarEnabled() const
	{
		HWND window = ToVar<HWND>(window_windows.hwnd);
		if (!window) return false;

		LONG style = GetWindowLong(
			window,
			GWL_STYLE);

		return (style & WS_CAPTION) != 0;
	}

	void Window::SetMinimizeButtonState(bool state) const
	{
		HWND window = ToVar<HWND>(window_windows.hwnd);
		if (!window) return;

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

		string val = state ? "true" : "false";

		if (Window_Global::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set window '" + GetTitle() + "' minimize button state to '" + val + "'",
				"WINDOW",
				LogType::LOG_SUCCESS);
		}
	}
	bool Window::IsMinimizeButtonEnabled() const
	{
		HWND window = ToVar<HWND>(window_windows.hwnd);
		if (!window) return false;

		LONG style = GetWindowLong(
			window,
			GWL_STYLE);

		return (style & WS_MINIMIZEBOX) != 0;
	}

	void Window::SetMaximizeButtonState(bool state) const
	{
		HWND window = ToVar<HWND>(window_windows.hwnd);
		if (!window) return;

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

		string val = state ? "true" : "false";

		if (Window_Global::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set window '" + GetTitle() + "' maximize button state to '" + val + "'",
				"WINDOW",
				LogType::LOG_SUCCESS);
		}
	}
	bool Window::IsMaximizeButtonEnabled() const
	{
		HWND window = ToVar<HWND>(window_windows.hwnd);
		if (!window) return false;

		LONG style = GetWindowLong(
			window,
			GWL_STYLE);

		return (style & WS_MAXIMIZEBOX) != 0;
	}

	void Window::SetCloseButtonState(bool state) const
	{
		HWND window = ToVar<HWND>(window_windows.hwnd);
		if (!window) return;

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

		string val = state ? "true" : "false";

		if (Window_Global::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set window '" + GetTitle() + "' close button state to '" + val + "'",
				"WINDOW",
				LogType::LOG_SUCCESS);
		}
	}
	bool Window::IsCloseButtonEnabled() const
	{
		HWND window = ToVar<HWND>(window_windows.hwnd);
		if (!window) return false;

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
		if (!window) return;

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

		string val = state ? "true" : "false";

		if (Window_Global::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set window '" + GetTitle() + "' system menu state to '" + val + "'",
				"WINDOW",
				LogType::LOG_SUCCESS);
		}
	}
	bool Window::IsSystemMenuEnabled() const
	{
		HWND window = ToVar<HWND>(window_windows.hwnd);
		if (!window) return false;

		LONG style = GetWindowLong(
			window,
			GWL_STYLE);

		return (style & WS_SYSMENU) != 0;
	}

	void Window::SetOpacity(float alpha) const
	{
		HWND window = ToVar<HWND>(window_windows.hwnd);
		if (!window) return;

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

		string val = to_string(alpha);

		if (Window_Global::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set window '" + GetTitle() + "' opacity to '" + val + "'",
				"WINDOW",
				LogType::LOG_SUCCESS);
		}
	}
	float Window::GetOpacity() const
	{
		HWND window = ToVar<HWND>(window_windows.hwnd);
		if (!window) return{};

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

	bool Window::IsForegroundWindow() const
	{
		HWND window = ToVar<HWND>(window_windows.hwnd);
		if (!window) return false;

		return GetForegroundWindow() == window;
	}

	bool Window::IsFocused() const
	{
		HWND window = ToVar<HWND>(window_windows.hwnd);
		if (!window) return false;

		return GetFocus() == window;
	}

	bool Window::IsMinimized() const
	{
		HWND window = ToVar<HWND>(window_windows.hwnd);
		if (!window) return false;

		//IsIconic returns TRUE if the window is minimized
		return IsIconic(window);
	}

	bool Window::IsVisible() const
	{
		HWND window = ToVar<HWND>(window_windows.hwnd);
		if (!window) return false;

		return IsWindowVisible(window);
	}

	void Window::SetExclusiveFullscreenState(bool state)
	{
		HWND window = ToVar<HWND>(window_windows.hwnd);
		if (!window) return;

		if (state)
		{
			//get current monitor

			HMONITOR hMonitor = MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST);
			MONITORINFOEX mi{};
			mi.cbSize = sizeof(mi);
			GetMonitorInfo(hMonitor, &mi);

			//query current desktop mode

			DEVMODE devMode{};
			devMode.dmSize = sizeof(devMode);
			EnumDisplaySettings(mi.szDevice, ENUM_CURRENT_SETTINGS, &devMode);

			//switch to exclusive fullscreen
			if (ChangeDisplaySettingsEx(
				mi.szDevice,
				&devMode,
				nullptr,
				CDS_FULLSCREEN,
				nullptr) == DISP_CHANGE_SUCCESSFUL)
			{
				LONG style = GetWindowLong(window, GWL_STYLE);
				style &= ~(
					WS_CAPTION 
					| WS_THICKFRAME 
					| WS_MINIMIZEBOX 
					| WS_MAXIMIZEBOX 
					| WS_SYSMENU);
				SetWindowLong(window, GWL_STYLE, style);

				oldPos = GetPosition();
				oldSize = GetClientRectSize();

				SetWindowPos(
					window,
					HWND_TOP,
					mi.rcMonitor.left,
					mi.rcMonitor.top,
					mi.rcMonitor.right - mi.rcMonitor.left,
					mi.rcMonitor.bottom - mi.rcMonitor.top,
					SWP_FRAMECHANGED
					| SWP_NOOWNERZORDER);

				ShowWindow(window, SW_SHOW);
			}
		}
		else
		{
			ChangeDisplaySettingsEx(
				nullptr,
				nullptr,
				nullptr,
				0,
				nullptr);
			LONG style = GetWindowLong(window, GWL_STYLE);

			style |= (
				WS_CAPTION
				| WS_THICKFRAME
				| WS_MINIMIZEBOX
				| WS_MAXIMIZEBOX
				| WS_SYSMENU);

			SetWindowLong(window, GWL_STYLE, style);

			SetWindowPos(
				window,
				HWND_NOTOPMOST,
				oldPos.x,
				oldPos.y,
				oldSize.x,
				oldSize.y,
				SWP_FRAMECHANGED
				| SWP_NOOWNERZORDER);

			ShowWindow(window, SW_SHOW);
		}

		isExclusiveFullscreen = state;
	}

	void Window::SetBorderlessFullscreenState(bool state)
	{
		HWND window = ToVar<HWND>(window_windows.hwnd);
		if (!window) return;

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

		string val = state ? "true" : "false";

		if (Window_Global::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set window '" + GetTitle() + "' fullscreen state to '" + val + "'",
				"WINDOW",
				LogType::LOG_SUCCESS);
		}
	}
	bool Window::IsBorderlessFullscreen() const
	{
		HWND window = ToVar<HWND>(window_windows.hwnd);
		if (!window) return false;

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

	void Window::SetWindowState(WindowState state) const
	{
		HWND window = ToVar<HWND>(window_windows.hwnd);
		if (!window) return;

		string val{};

		switch (state)
		{
		case WindowState::WINDOW_NORMAL:
			ShowWindow(window, SW_SHOWNORMAL);
			val = "normal";
			break;
		case WindowState::WINDOW_MAXIMIZE:
			ShowWindow(window, SW_MAXIMIZE);
			val = "maximized";
			break;
		case WindowState::WINDOW_MINIMIZE:
			ShowWindow(window, SW_MINIMIZE);
			val = "minimized";
			break;
		case WindowState::WINDOW_HIDE:
			ShowWindow(window, SW_HIDE);
			val = "hidden";
			break;
		case WindowState::WINDOW_SHOWNOACTIVATE:
			ShowWindow(window, SW_SHOWNOACTIVATE);
			val = "unfocused visible";
			break;
		}

		UpdateWindow(window);

		if (Window_Global::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set window '" + GetTitle() + "' state to '" + val + "'",
				"WINDOW",
				LogType::LOG_SUCCESS);
		}
	}
	WindowState Window::GetWindowState() const
	{
		HWND window = ToVar<HWND>(window_windows.hwnd);
		if (!window) return WindowState::WINDOW_NORMAL;

		WINDOWPLACEMENT placement{};
		placement.length = sizeof(WINDOWPLACEMENT);

		if (!GetWindowPlacement(window, &placement))
		{
			Log::Print(
				"Failed to get window '" + GetTitle() + "' state!",
				"WINDOW",
				LogType::LOG_ERROR,
				2);

			return WindowState::WINDOW_NORMAL;
		}

		switch (placement.showCmd)
		{
		case SW_SHOWMINIMIZED:
			return WindowState::WINDOW_MINIMIZE;
		case SW_SHOWMAXIMIZED:
			return WindowState::WINDOW_MAXIMIZE;
		case SW_HIDE:
			return WindowState::WINDOW_HIDE;
		case SW_SHOWNOACTIVATE:
			return WindowState::WINDOW_SHOWNOACTIVATE;
		case SW_NORMAL: //also covers sw_restore
		default:
			return WindowState::WINDOW_NORMAL;
		}
	}

	void Window::SetShutdownBlockState(bool state)
	{
		HWND window = ToVar<HWND>(window_windows.hwnd);
		if (!window) return;

		if (state)
		{
			WTSRegisterSessionNotification(
				window,
				NOTIFY_FOR_THIS_SESSION);

			shutdownBlockState = true;
		}
		else
		{
			WTSUnRegisterSessionNotification(window);
			shutdownBlockState = false;
		}

		string val = state ? "true" : "false";

		if (Window_Global::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set window '" + GetTitle() + "' shutdown block state to '" + val + "'",
				"WINDOW",
				LogType::LOG_SUCCESS);
		}
	}

	void Window::Flash(
		FlashTarget target,
		FlashType type,
		u32 count) const
	{
		HWND window = ToVar<HWND>(window_windows.hwnd);
		if (!window) return;

		string targetName = target == FlashTarget::TARGET_WINDOW
			? "window"
			: "taskbar";

		if (type == FlashType::FLASH_TIMED
			&& count == 0)
		{
			Log::Print(
				"Failed to flash " + targetName + " because type was set to 'FLASH_TIMED' but no count value was assigned!",
				"WINDOW",
				LogType::LOG_ERROR,
				2);

			return;
		}

		FLASHWINFO fi{};
		fi.cbSize = sizeof(fi);
		fi.hwnd = window;

		string val{};
		string dur{};

		switch (type)
		{
		case FlashType::FLASH_ONCE:
			fi.dwFlags = target == FlashTarget::TARGET_WINDOW
				? FLASHW_CAPTION
				: FLASHW_ALL;
			fi.uCount = 1;

			val = "once";
			dur = "1";

			break;
		case FlashType::FLASH_UNTIL_FOCUS:
			fi.dwFlags = target == FlashTarget::TARGET_WINDOW
				? FLASHW_CAPTION | FLASHW_TIMERNOFG
				: FLASHW_ALL | FLASHW_TIMERNOFG;
			fi.uCount = 0; //keep flashing until focus

			val = "until focus";
			dur = "0";

			break;
		case FlashType::FLASH_TIMED:
			fi.dwFlags = target == FlashTarget::TARGET_WINDOW
				? FLASHW_CAPTION
				: FLASHW_ALL;
			fi.uCount = count; //flash x times

			val = "timed";
			dur = to_string(count);

			break;
		}

		fi.dwTimeout = 0;
		FlashWindowEx(&fi);

		if (Window_Global::IsVerboseLoggingEnabled())
		{
			string targetMsg = target == FlashTarget::TARGET_WINDOW
				? "window '" + GetTitle() + "'"
				: "taskbar for window '" + GetTitle() + "'";

			Log::Print(
				"Flashed " + targetMsg + " with type '" + val + "' for '" + dur + "' times",
				"WINDOW",
				LogType::LOG_SUCCESS);
		}
	}

	void Window::SetTaskbarProgressBarState(
		TaskbarProgressBarMode mode,
		u8 current,
		u8 max) const
	{
		HWND window = ToVar<HWND>(window_windows.hwnd);
		if (!window) return;

		u8 maxClamped = clamp(
			max, 
			static_cast<u8>(1), 
			static_cast<u8>(100));

		u8 currentClamped = clamp(
			current, 
			static_cast<u8>(0),
			static_cast<u8>(maxClamped - 1));

		CComPtr<ITaskbarList3> taskbar{};
		HRESULT hr = CoCreateInstance(
			CLSID_TaskbarList,
			nullptr,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(&taskbar));

		if (FAILED(hr)
			|| !taskbar)
		{
			Log::Print(
				"Failed to create ITaskbarList3 to set taskbar progress bar mode!",
				"WINDOW",
				LogType::LOG_ERROR,
				2);

			return;
		}

		taskbar->HrInit();

		string val{};
		string currVal{};
		string maxVal{};

		switch (mode)
		{
		case TaskbarProgressBarMode::PROGRESS_NONE:
			taskbar->SetProgressState(window, TBPF_NOPROGRESS);
			val = "none";
			break;
		case TaskbarProgressBarMode::PROGRESS_INDETERMINATE:
			taskbar->SetProgressState(window, TBPF_INDETERMINATE);
			val = "indeterminate";
			break;
		case TaskbarProgressBarMode::PROGRESS_NORMAL:
			taskbar->SetProgressState(window, TBPF_NORMAL);
			taskbar->SetProgressValue(window, currentClamped, maxClamped);
			val = "normal";
			break;
		case TaskbarProgressBarMode::PROGRESS_PAUSED:
			taskbar->SetProgressState(window, TBPF_PAUSED);
			taskbar->SetProgressValue(window, currentClamped, maxClamped);
			val = "paused";
			break;
		case TaskbarProgressBarMode::PROGRESS_ERROR:
			taskbar->SetProgressState(window, TBPF_ERROR);
			taskbar->SetProgressValue(window, currentClamped, maxClamped);
			val = "error";
			break;
		}

		if (Window_Global::IsVerboseLoggingEnabled())
		{
			ostringstream oss{};
			oss << "Set window '" + GetTitle() + "' taskbar duration type to '"
				+ val + "', current value to '" + currVal
				+ "' and max value to " + maxVal + "'";

			Log::Print(
				oss.str(),
				"WINDOW",
				LogType::LOG_SUCCESS);
		}
	}

	void Window::Update()
	{
		if (!isInitialized)
		{
			Log::Print(
				"Cannot run window loop because window '" + GetTitle() + "' has not been initialized!",
				"WINDOW",
				LogType::LOG_ERROR,
				2);

			return;
		}

		UpdateIdleState(
			this,
			isIdle);

		MSG msg;

		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg); //translate virtual-key messages (like WM_KEYDOWN) to character messages (WM_CHAR)
			DispatchMessage(&msg);  //send the message to the window procedure
		}
	}

	void Window::CloseWindow()
	{
		RemoveAllChildWindows();

		/*
		TODO: add back text + image + camera support
		
		Registry<Text>::RemoveAllWindowContent(ID);
		Registry<Image>::RemoveAllWindowContent(ID);
		Registry<Camera>::RemoveAllWindowContent(ID);
		*/
		
		Registry<Input>::RemoveAllWindowContent(ID);
		Registry<MenuBar>::RemoveAllWindowContent(ID);
		Registry<OpenGL_Context>::RemoveAllWindowContent(ID);

		Registry<Window>::RemoveContent(ID);
	}

	Window::~Window()
	{
		string title = GetTitle();

		Log::Print(
			"Destroying window '" + title + "' with ID '" + to_string(ID) + "'.",
			"WINDOW",
			LogType::LOG_DEBUG);

		if (parentWindow)
		{
			parentWindow->RemoveChildWindow(this);
			RemoveParentWindow();
		}

		childWindows.clear();

		inputID = 0;
		glContextID = 0;
		menuBarID = 0;
		cameras.clear();
		widgets.clear();

		HWND winRef = ToVar<HWND>(window_windows.hwnd);
		SetWindowState(WindowState::WINDOW_HIDE);

		if (window_windows.wndProc) window_windows.wndProc = NULL;

		HDC hdc = GetDC(winRef);
		if (hdc)
		{
			ReleaseDC(
				ToVar<HWND>(window_windows.hwnd),
				hdc);
		}

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

		if (shutdownBlockState) WTSUnRegisterSessionNotification(winRef);

		if (window_windows.hwnd)
		{
			DestroyWindow(winRef);
			window_windows.hwnd = NULL;
		}
		window_windows.hInstance = NULL;
	}
}

void UpdateIdleState(Window* window, bool& isIdle)
{
	isIdle =
		!window->IsForegroundWindow()
		|| window->IsMinimized()
		|| !window->IsVisible();
}

/*
TODO: add texture support back

HICON SetUpIcon(OpenGL_Texture* texture)
{
	string name = texture->GetName();
	vec2 size = texture->GetSize();
	string sizeX = to_string(static_cast<int>(size.x));
	string sizeY = to_string(static_cast<int>(size.y));

	if (texture->GetSize().x < 32)
	{
		Log::Print(
			"Icon '" + name + "' size '" + sizeX + "x" + sizeY + "' is too small! Consider uploading a bigger icon.",
			"WINDOW",
			LogType::LOG_WARNING);
	}
	if (size.x > 256)
	{
		Log::Print(
			"Icon '" + name + "' size '" + sizeX + "x" + sizeY + "' is too big! Consider uploading a smaller icon.",
			"WINDOW",
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
			"WINDOW",
			LogType::LOG_ERROR,
			2);

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
*/

wstring ToWide(const string& str)
{
	if (str.empty()) return wstring();

	int size_needed = MultiByteToWideChar(
		CP_UTF8,
		0,
		str.c_str(),
		-1,
		nullptr,
		0);

	wstring wstr(size_needed - 1, 0);

	MultiByteToWideChar(
		CP_UTF8,
		0,
		str.c_str(),
		-1,
		wstr.data(),
		size_needed);

	return wstr;
}
string ToShort(const wstring& str)
{
	if (str.empty()) return{};

	int size_needed = WideCharToMultiByte(
		CP_UTF8,
		0,
		str.c_str(),
		-1,
		nullptr,
		0,
		nullptr,
		nullptr);

	string result(size_needed - 1, 0);

	WideCharToMultiByte(
		CP_UTF8,
		0,
		str.c_str(),
		-1,
		result.data(),
		size_needed,
		nullptr,
		nullptr);

	return result;
}

#endif //_WIN32