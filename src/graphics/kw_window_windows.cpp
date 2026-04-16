//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef _WIN32

#include <windows.h>
#include <mmsystem.h>
#include <shobjidl.h>
#include <dwmapi.h>
#include <atlbase.h>
#include <atlcomcli.h>
#include <wtsapi32.h>
#include <shellapi.h>

#include <algorithm>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "log_utils.hpp"

#include "graphics/kw_window.hpp"
#include "core/kw_core.hpp"
#include "core/kw_input.hpp"
#include "graphics/kw_window_global.hpp"
#include "graphics/kw_menubar_windows.hpp"
#include "vulkan/kw_vulkan.hpp"

using KalaHeaders::KalaCore::ToVar;
using KalaHeaders::KalaCore::FromVar;
using KalaHeaders::KalaMath::vec2;
using KalaHeaders::KalaMath::isnear;
using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;

using KalaWindow::Core::KalaWindowCore;
using KalaWindow::Core::Input;
using KalaWindow::Graphics::ProcessWindow;
using KalaWindow::Graphics::MenuBar;
using KalaWindow::Vulkan::Vulkan_Context;

using std::make_unique;
using std::to_string;
using std::unique_ptr;
using std::clamp;
using std::ostringstream;
using std::wstring;
using std::string;
using std::string_view;
using std::vector;

constexpr u16 MAX_TITLE_LENGTH = 50;

//KalaWindow will dynamically update window idle state
static void UpdateIdleState(ProcessWindow* window, bool& isIdle)
{
	isIdle =
		!window->IsForegroundWindow()
		|| window->IsMinimized()
		|| !window->IsVisible();
}

/*
TODO: add texture support back

static HICON SetUpIcon(OpenGL_Texture* texture);
*/

static HICON exeIcon{};
static HICON overlayIcon{};

static wstring ToWide(string_view str);
static string ToShort(const wstring& str);

namespace KalaWindow::Graphics
{
	static KalaWindowRegistry<ProcessWindow> registry{};

	KalaWindowRegistry<ProcessWindow>& ProcessWindow::GetRegistry() { return registry; }

	ProcessWindow* ProcessWindow::Initialize(
		string_view title,
		ProcessWindow* parentWindow,
		DpiContext context)
	{
		if (!Window_Global::IsInitialized())
		{
			KalaWindowCore::ForceClose(
				"Window error",
				"Failed to create window '" + string(title) + "' because global window context has not been created!");

			return nullptr;
		}

		u32 newID = KalaWindowCore::GetGlobalID() + 1;
		KalaWindowCore::SetGlobalID(newID);

		unique_ptr<ProcessWindow> newWindow = make_unique<ProcessWindow>();
		ProcessWindow* windowPtr = newWindow.get();

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
					"Failed to create child window '" + string(title) + "' because parent window pointer does not exist!");

				return nullptr;
			}

			parentWindowRef = ToVar<HWND>(parentWindow->GetWindowData().window);

			if (!parentWindowRef)
			{
				KalaWindowCore::ForceClose(
					"Window error",
					"Failed to create child window '" + string(title) + "' because parent window handle is invalid!");

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
			800,
			800,
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
			rcast<LONG_PTR>(windowPtr));
			
		HDC newHDC = GetDC(newHwnd);

		WindowData newWindowStruct =
		{
			.window = FromVar(newHwnd),
			.handle = FromVar(newHDC),
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
		
		windowPtr->windowData = newWindowStruct;

		windowPtr->SetTitle(title);
		windowPtr->ID = newID;
		windowPtr->SetClientRectSize(800);
		windowPtr->SetPosition(800);

		windowPtr->isInitialized = true;

		windowPtr->oldPos = windowPtr->GetPosition();
		windowPtr->oldSize = windowPtr->GetClientRectSize();

		//allow files to be dragged to this window
		DragAcceptFiles(newHwnd, TRUE);

		registry.AddContent(newID, std::move(newWindow));

		Log::Print(
			"Created new window '" + string(title) + "' with ID '" + to_string(newID) + "'!",
			"KW_WINDOW",
			LogType::LOG_SUCCESS);

		return windowPtr;
	}

	bool ProcessWindow::IsInitialized() const { return isInitialized; }

	u32 ProcessWindow::GetID() const { return ID; }

	void ProcessWindow::Update()
	{
		if (!isInitialized)
		{
			Log::Print(
				"Cannot run window loop because window '" + GetTitle() + "' has not been initialized!",
				"KW_WINDOW",
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

	void ProcessWindow::SetLastDraggedFiles(const vector<string>& files) { lastDraggedFiles = files; };
	const vector<string>& ProcessWindow::GetLastDraggedFiles() const { return lastDraggedFiles; };
	void ProcessWindow::ClearLastDraggedFiles() { lastDraggedFiles.clear(); };

	void ProcessWindow::SetTitle(string_view newTitle) const
	{
		if (!windowData.window)
		{
			KalaWindowCore::ForceClose(
				"Window error",
				"Failed to set window title because the attached window was invalid!");
		}

		HWND window = ToVar<HWND>(windowData.window);

		if (newTitle.empty())
		{
			Log::Print(
				"Window title cannot be empty!",
				"KW_WINDOW",
				LogType::LOG_ERROR,
				2);

			return;
		}

		string titleToSet = string(newTitle);
		if (newTitle.length() > MAX_TITLE_LENGTH)
		{
			Log::Print(
				"Window title exceeded max allowed length of '" + to_string(MAX_TITLE_LENGTH) + "'! Title has been truncated.",
				"KW_WINDOW",
				LogType::LOG_ERROR,
                2);

            return;
		}

		wstring wideTitle = ToWide(titleToSet);

		SetWindowTextW(
			window, 
			wideTitle.c_str());

		if (Window_Global::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set window title to '" + string(newTitle) + "'",
				"KW_WINDOW",
				LogType::LOG_SUCCESS);
		}
	}
	string ProcessWindow::GetTitle() const
	{
		if (!windowData.window)
		{
			KalaWindowCore::ForceClose(
				"Window error",
				"Failed to get window title because the attached window was invalid!");
		}

		HWND window = ToVar<HWND>(windowData.window);

		int length = GetWindowTextLengthW(window);
		if (length == 0)
		{
			Log::Print(
				"Window title was empty!",
				"KW_WINDOW",
				LogType::LOG_WARNING);

			return {};
		}

		wstring title(length + 1, L'\0');
		GetWindowTextW(window, title.data(), length + 1);

		title.resize(wcslen(title.c_str()));
		string result = ToShort(title);

		return result;
	}

	void ProcessWindow::SetIcon(u32 texture) const
	{
		/*
		TODO: add texture support back

		if (!windowData.window)
		{
			KalaWindowCore::ForceClose(
				"Window error",
				"Failed to set icon because the attached window was invalid!");
		}
		
		HWND window = ToVar<HWND>(windowData.window);

		OpenGL_Texture* tex = OpenGL_Texture::registry.GetContent(texture);

		if (!tex)
		{
			Log::Print(
				"Cannot set window '" + GetTitle() + "' exe icon because the texture ID is invalid!",
				"KW_WINDOW",
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
				"KW_WINDOW",
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
				"KW_WINDOW",
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
				"KW_WINDOW",
				LogType::LOG_SUCCESS);
		}
		*/
	}
	u32 ProcessWindow::GetIcon() const { return iconID; }
	void ProcessWindow::ClearIcon() const
	{
		if (!windowData.window)
		{
			KalaWindowCore::ForceClose(
				"Window error",
				"Failed to clear icon because the attached window was invalid!");
		}

		HWND window = ToVar<HWND>(windowData.window);

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

	void ProcessWindow::SetTaskbarOverlayIcon(
		u32 texture,
		string_view tooltip) const
	{
		/*
		TODO: add texture support back

		if (!windowData.window)
		{
			KalaWindowCore::ForceClose(
				"Window error",
				"Failed to set tarkbar overlay icon because the attached window was invalid!");
		}

		HWND window = ToVar<HWND>(windowData.window);

		OpenGL_Texture* tex = OpenGL_Texture::registry.GetContent(texture);

		if (!tex)
		{
			Log::Print(
				"Cannot set window '" + GetTitle() + "' overlay icon because the texture ID is invalid!",
				"KW_WINDOW",
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
				"KW_WINDOW",
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
				"KW_WINDOW",
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
				"KW_WINDOW",
				LogType::LOG_ERROR,
				2);

			return;
		}

		hr = taskbar->HrInit();

		if (FAILED(hr))
		{
			Log::Print(
				"Failed to init ITaskbarList3 to set overlay icon!",
				"KW_WINDOW",
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
				"KW_WINDOW",
				LogType::LOG_SUCCESS);
		}
		*/
	}
	u32 ProcessWindow::GetTaskbarOverlayIcon() const { return overlayIconID; }
	void ProcessWindow::ClearTaskbarOverlayIcon() const
	{
		if (!windowData.window)
		{
			KalaWindowCore::ForceClose(
				"Window error",
				"Failed to clear taskbar overlay icon because the attached window was invalid!");
		}

		HWND window = ToVar<HWND>(windowData.window);

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
				"KW_WINDOW",
				LogType::LOG_ERROR,
				2);

			return;
		}

		taskbar->SetOverlayIcon(
			window,
			nullptr,
			nullptr);
	}

	void ProcessWindow::BringToFocus()
	{
		if (IsFocused()) return; //skip all logic if already focused

		if (!windowData.window)
		{
			KalaWindowCore::ForceClose(
				"Window error",
				"Failed to bring window to focus because the attached window was invalid!");
		}

		HWND window = ToVar<HWND>(windowData.window);

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
					"KW_WINDOW",
					LogType::LOG_SUCCESS);
			}
		}

		//ensure keyboard focus
		SetFocus(window);
	}

	void ProcessWindow::SetWindowRounding(WindowRounding roundState) const
	{
		if (!windowData.window)
		{
			KalaWindowCore::ForceClose(
				"Window error",
				"Failed to set window rounding because the attached window was invalid!");
		}

		HWND window = ToVar<HWND>(windowData.window);

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
				"KW_WINDOW",
				LogType::LOG_ERROR,
				2);
		}

		if (Window_Global::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set window '" + GetTitle() + "' rounding to '" + roundingVal + "'",
				"KW_WINDOW",
				LogType::LOG_SUCCESS);
		}
	}
	WindowRounding ProcessWindow::GetWindowRoundingState() const
	{
		if (!windowData.window)
		{
			KalaWindowCore::ForceClose(
				"Window error",
				"Failed to get window rounding state because the attached window was invalid!");
		}

		HWND window = ToVar<HWND>(windowData.window);

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
				"KW_WINDOW",
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

	void ProcessWindow::SetClientRectSize(vec2 newSize)
	{
		if (!windowData.window)
		{
			KalaWindowCore::ForceClose(
				"Window error",
				"Failed to set client rect size because the attached window was invalid!");
		}

		HWND window = ToVar<HWND>(windowData.window);

		vec2 oldSize = GetClientRectSize();
		if (isnear(oldSize, newSize)) return;

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

		TriggerResize();
		TriggerRedraw();

		string val = to_string(newSize.x) + "x" + to_string(newSize.y);

		if (Window_Global::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set window '" + GetTitle() + "' client rect size to '" + val + "'",
				"KW_WINDOW",
				LogType::LOG_SUCCESS);
		}
	}
	vec2 ProcessWindow::GetClientRectSize() const
	{
		if (!windowData.window)
		{
			KalaWindowCore::ForceClose(
				"Window error",
				"Failed to set client rect size because the attached window was invalid!");
		}

		HWND window = ToVar<HWND>(windowData.window);

		RECT rect{};
		GetClientRect(window, &rect);

		return vec2
		{
			scast<float>(rect.right - rect.left),
			scast<float>(rect.bottom - rect.top)
		};
	}

	void ProcessWindow::SetOuterSize(vec2 newSize)
	{
		if (!windowData.window)
		{
			KalaWindowCore::ForceClose(
				"Window error",
				"Failed to set outer window size because the attached window was invalid!");
		}

		HWND window = ToVar<HWND>(windowData.window);

		vec2 oldSize = GetOuterSize();
		if (isnear(oldSize, newSize)) return;

		SetWindowPos(
			window,
			nullptr,
			0,
			0,
			newSize.x,
			newSize.y,
			SWP_NOMOVE
			| SWP_NOZORDER);

		TriggerResize();
		TriggerRedraw();

		string val = to_string(newSize.x) + "x" + to_string(newSize.y);

		if (Window_Global::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set window '" + GetTitle() + "' outer size to '" + val + "'",
				"KW_WINDOW",
				LogType::LOG_SUCCESS);
		}
	}
	vec2 ProcessWindow::GetOuterSize() const
	{
		if (!windowData.window)
		{
			KalaWindowCore::ForceClose(
				"Window error",
				"Failed to get outer window size because the attached window was invalid!");
		}

		HWND window = ToVar<HWND>(windowData.window);

		RECT rect{};
		GetWindowRect(window, &rect);

		return vec2
		{
			scast<float>(rect.right - rect.left),
			scast<float>(rect.bottom - rect.top)
		};
	}

	void ProcessWindow::SetPosition(vec2 newPosition)
	{
		if (!windowData.window)
		{
			KalaWindowCore::ForceClose(
				"Window error",
				"Failed to set window position because the attached window was invalid!");
		}

		HWND window = ToVar<HWND>(windowData.window);

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
				"KW_WINDOW",
				LogType::LOG_SUCCESS);
		}
	}
	vec2 ProcessWindow::GetPosition()
	{
		if (!windowData.window)
		{
			KalaWindowCore::ForceClose(
				"Window error",
				"Failed to get window position because the attached window was invalid!");
		}

		HWND window = ToVar<HWND>(windowData.window);

		RECT rect{};
		if (GetWindowRect(window, &rect))
		{
			return vec2
			{ 
				scast<float>(rect.left),
				scast<float>(rect.top)
			};
		}

		return vec2{ 0, 0 };
	}

    void ProcessWindow::SetMaxSize(vec2 newMaxSize)
    { 
        maxSize = kclamp(newMaxSize, minSize + 1.0f, 10000.0f);

        if (GetClientRectSize() > maxSize) SetClientRectSize(maxSize);
    }
	vec2 ProcessWindow::GetMaxSize() const { return maxSize; }

	void ProcessWindow::SetMinSize(vec2 newMinSize)
    { 
        minSize = kclamp(newMinSize, 1.0f, maxSize - 1.0f);

        if (GetClientRectSize() < minSize) SetClientRectSize(minSize);
    }
	vec2 ProcessWindow::GetMinSize() const { return minSize; }

	void ProcessWindow::SetAlwaysOnTopState(bool state)
	{
		if (!windowData.window)
		{
			KalaWindowCore::ForceClose(
				"Window error",
				"Failed to set always on top state because the attached window was invalid!");
		}

		HWND window = ToVar<HWND>(windowData.window);

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
				"KW_WINDOW",
				LogType::LOG_SUCCESS);
		}
	}
	bool ProcessWindow::IsAlwaysOnTop() const
	{
		if (!windowData.window)
		{
			KalaWindowCore::ForceClose(
				"Window error",
				"Failed to get always on top state because the attached window was invalid!");
		}

		HWND window = ToVar<HWND>(windowData.window);

		LONG exStyle = GetWindowLong(
			window,
			GWL_EXSTYLE);

		return (exStyle & WS_EX_TOPMOST) != 0;
	}

	void ProcessWindow::SetResizableState(bool state)
	{
		if (!windowData.window)
		{
			KalaWindowCore::ForceClose(
				"Window error",
				"Failed to set resizable state because the attached window was invalid!");
		}

		HWND window = ToVar<HWND>(windowData.window);

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
				"KW_WINDOW",
				LogType::LOG_SUCCESS);
		}
	}
	bool ProcessWindow::IsResizable() const
	{
		if (!windowData.window)
		{
			KalaWindowCore::ForceClose(
				"Window error",
				"Failed to get resizable state because the attached window was invalid!");
		}

		HWND window = ToVar<HWND>(windowData.window);

		LONG style = GetWindowLong(
			window,
			GWL_STYLE);

		return (style &
			(WS_THICKFRAME
			| WS_MAXIMIZEBOX)) != 0;
	}

	void ProcessWindow::SetTopBarState(bool state) const
	{
		if (!windowData.window)
		{
			KalaWindowCore::ForceClose(
				"Window error",
				"Failed to set top bar state because the attached window was invalid!");
		}

		HWND window = ToVar<HWND>(windowData.window);

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
				"KW_WINDOW",
				LogType::LOG_SUCCESS);
		}
	}
	bool ProcessWindow::IsTopBarEnabled() const
	{
		if (!windowData.window)
		{
			KalaWindowCore::ForceClose(
				"Window error",
				"Failed to get window top bar state because the attached window was invalid!");
		}

		HWND window = ToVar<HWND>(windowData.window);

		LONG style = GetWindowLong(
			window,
			GWL_STYLE);

		return (style & WS_CAPTION) != 0;
	}

	void ProcessWindow::SetMinimizeButtonState(bool state) const
	{
		if (!windowData.window)
		{
			KalaWindowCore::ForceClose(
				"Window error",
				"Failed to set minimize button state because the attached window was invalid!");
		}

		HWND window = ToVar<HWND>(windowData.window);

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
				"KW_WINDOW",
				LogType::LOG_SUCCESS);
		}
	}
	bool ProcessWindow::IsMinimizeButtonEnabled() const
	{
		if (!windowData.window)
		{
			KalaWindowCore::ForceClose(
				"Window error",
				"Failed to get minimize button state because the attached window was invalid!");
		}

		HWND window = ToVar<HWND>(windowData.window);

		LONG style = GetWindowLong(
			window,
			GWL_STYLE);

		return (style & WS_MINIMIZEBOX) != 0;
	}

	void ProcessWindow::SetMaximizeButtonState(bool state) const
	{
		if (!windowData.window)
		{
			KalaWindowCore::ForceClose(
				"Window error",
				"Failed to set maximize button state because the attached window was invalid!");
		}

		HWND window = ToVar<HWND>(windowData.window);

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
				"KW_WINDOW",
				LogType::LOG_SUCCESS);
		}
	}
	bool ProcessWindow::IsMaximizeButtonEnabled() const
	{
		if (!windowData.window)
		{
			KalaWindowCore::ForceClose(
				"Window error",
				"Failed to get maximize button state because the attached window was invalid!");
		}

		HWND window = ToVar<HWND>(windowData.window);

		LONG style = GetWindowLong(
			window,
			GWL_STYLE);

		return (style & WS_MAXIMIZEBOX) != 0;
	}

	void ProcessWindow::SetCloseButtonState(bool state) const
	{
		if (!windowData.window)
		{
			KalaWindowCore::ForceClose(
				"Window error",
				"Failed to set close button state because the attached window was invalid!");
		}

		HWND window = ToVar<HWND>(windowData.window);

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
				"KW_WINDOW",
				LogType::LOG_SUCCESS);
		}
	}
	bool ProcessWindow::IsCloseButtonEnabled() const
	{
		if (!windowData.window)
		{
			KalaWindowCore::ForceClose(
				"Window error",
				"Failed to get close button state because the attached window was invalid!");
		}

		HWND window = ToVar<HWND>(windowData.window);

		HMENU hSysMenu = GetSystemMenu(window, FALSE);
		if (!hSysMenu) return false; //no system menu

		return (GetMenuState(
			hSysMenu,
			SC_CLOSE,
			MF_BYCOMMAND) != (UINT)-1);
	}

	void ProcessWindow::SetSystemMenuState(bool state) const
	{
		if (!windowData.window)
		{
			KalaWindowCore::ForceClose(
				"Window error",
				"Failed to destroy menu because the attached window was invalid!");
		}

		HWND window = ToVar<HWND>(windowData.window);

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
				"KW_WINDOW",
				LogType::LOG_SUCCESS);
		}
	}
	bool ProcessWindow::IsSystemMenuEnabled() const
	{
		if (!windowData.window)
		{
			KalaWindowCore::ForceClose(
				"Window error",
				"Failed to get system menu state because the attached window was invalid!");
		}

		HWND window = ToVar<HWND>(windowData.window);

		LONG style = GetWindowLong(
			window,
			GWL_STYLE);

		return (style & WS_SYSMENU) != 0;
	}

	void ProcessWindow::SetOpacity(float alpha) const
	{
		if (!windowData.window)
		{
			KalaWindowCore::ForceClose(
				"Window error",
				"Failed to set window opacity because the attached window was invalid!");
		}
		
		HWND window = ToVar<HWND>(windowData.window);

		float clamped = clamp(alpha, 0.0f, 1.0f);

		BYTE bAlpha = scast<BYTE>(clamped * 255.0f);

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
				"KW_WINDOW",
				LogType::LOG_SUCCESS);
		}
	}
	float ProcessWindow::GetOpacity() const
	{
		if (!windowData.window)
		{
			KalaWindowCore::ForceClose(
				"Window error",
				"Failed to get window opacity because the attached window was invalid!");
		}

		HWND window = ToVar<HWND>(windowData.window);

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
			return scast<float>(bAlpha) / 255;
		}

		//treat as fully opaque when not layered
		return 1.0f;
	}

	bool ProcessWindow::IsIdle() const { return isIdle; }

	bool ProcessWindow::IsHovered() const { return isWindowHovered; }
	bool ProcessWindow::IsForegroundWindow() const
	{
		if (!windowData.window)
		{
			KalaWindowCore::ForceClose(
				"Window error",
				"Failed to get foreground window state because the attached window was invalid!");
		}

		HWND window = ToVar<HWND>(windowData.window);

		return GetForegroundWindow() == window;
	}
	bool ProcessWindow::IsFocused() const
	{
		if (!windowData.window)
		{
			KalaWindowCore::ForceClose(
				"Window error",
				"Failed to get focused state because the attached window was invalid!");
		}

		HWND window = ToVar<HWND>(windowData.window);

		return GetFocus() == window;
	}
	bool ProcessWindow::IsFullscreen()
	{
		if (!windowData.window)
		{
			KalaWindowCore::ForceClose(
				"Window error",
				"Failed to get fullscreen state because the attached window was invalid!");
		}

		HWND window = ToVar<HWND>(windowData.window);

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
			isnear(pos.x, mi.rcMonitor.left)
			&& isnear(pos.y, mi.rcMonitor.top)
			&& isnear(size.x, (mi.rcMonitor.right - mi.rcMonitor.left))
			&& isnear(size.y, (mi.rcMonitor.bottom - mi.rcMonitor.top));

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
	bool ProcessWindow::IsMinimized() const
	{
		if (!windowData.window)
		{
			KalaWindowCore::ForceClose(
				"Window error",
				"Failed to get minimized state because the attached window was invalid!");
		}

		HWND window = ToVar<HWND>(windowData.window);

		//IsIconic returns TRUE if the window is minimized
		return IsIconic(window);
	}
	bool ProcessWindow::IsVisible() const
	{
		if (!windowData.window)
		{
			KalaWindowCore::ForceClose(
				"Window error",
				"Failed to get visible state because the attached window was invalid!");
		}

		HWND window = ToVar<HWND>(windowData.window);

		return IsWindowVisible(window);
	}

	void ProcessWindow::SetResizingState(bool newState) { isResizing = newState; }
	bool ProcessWindow::IsResizing() const { return isResizing; }

	void ProcessWindow::SetWindowMode(WindowMode mode)
	{
		if (!windowData.window)
		{
			KalaWindowCore::ForceClose(
				"Window error",
				"Failed to set window mode because the attached window was invalid!");
		}

		HWND window = ToVar<HWND>(windowData.window);

		switch (mode)
		{
		default:
		case WindowMode::WINDOWMODE_WINDOWED:
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

			break;
		}
		case WindowMode::WINDOWMODE_BORDERLESS:
		{
			//save current pos and size

			oldPos = GetPosition();
			oldSize = GetClientRectSize();
			LONG style = GetWindowLong(window, GWL_STYLE);

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

			break;
		}
		case WindowMode::WINDOWMODE_EXCLUSIVE:
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
			}
			else
			{
				Log::Print(
					"Failed to switch to exclusive mode for window '" + GetTitle() + "'!",
					"KW_WINDOW",
					LogType::LOG_ERROR,
					2);
			}

			break;
		}
		}

		ShowWindow(window, SW_SHOWNORMAL);

		TriggerResize();
		TriggerRedraw();
	}
	WindowMode ProcessWindow::GetWindowMode()
	{
		if (!windowData.window)
		{
			KalaWindowCore::ForceClose(
				"Window error",
				"Failed to get window mode because the attached window was invalid!");
		}

		HWND window = ToVar<HWND>(windowData.window);

		auto IsExclusive = [&]() -> bool
			{
				DEVMODE current{};
				current.dmSize = sizeof(current);

				if (!EnumDisplaySettings(
					nullptr,
					ENUM_CURRENT_SETTINGS,
					&current))
				{
					return false;
				}

				DEVMODE desktop{};
				desktop.dmSize = sizeof(desktop);

				if (!EnumDisplaySettings(
					nullptr,
					ENUM_REGISTRY_SETTINGS,
					&desktop))
				{
					return false;
				}

				return current.dmPelsWidth != desktop.dmPelsWidth
					|| current.dmPelsHeight != desktop.dmPelsHeight
					|| current.dmDisplayFrequency != desktop.dmDisplayFrequency;
			};

		if (IsExclusive())       return WindowMode::WINDOWMODE_EXCLUSIVE;
		else if (IsFullscreen()) return WindowMode::WINDOWMODE_BORDERLESS;
		else                     return WindowMode::WINDOWMODE_WINDOWED;
	}

	void ProcessWindow::SetWindowState(WindowState state)
	{
		if (!windowData.window)
		{
			KalaWindowCore::ForceClose(
				"Window error",
				"Failed to set window state because the attached window was invalid!");
		}

		HWND window = ToVar<HWND>(windowData.window);

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

		if (state != WindowState::WINDOW_HIDE
			&& state != WindowState::WINDOW_MINIMIZE)
		{
			UpdateWindow(window);

			TriggerResize();
			TriggerRedraw();
		}

		if (Window_Global::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set window '" + GetTitle() + "' state to '" + val + "'",
				"KW_WINDOW",
				LogType::LOG_SUCCESS);
		}
	}
	WindowState ProcessWindow::GetWindowState() const
	{
		if (!windowData.window)
		{
			KalaWindowCore::ForceClose(
				"Window error",
				"Failed to get window state because the attached window was invalid!");
		}

		HWND window = ToVar<HWND>(windowData.window);

		WINDOWPLACEMENT placement{};
		placement.length = sizeof(WINDOWPLACEMENT);

		if (!GetWindowPlacement(window, &placement))
		{
			Log::Print(
				"Failed to get window '" + GetTitle() + "' state!",
				"KW_WINDOW",
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

	void ProcessWindow::SetShutdownBlockState(bool state)
	{
		if (!windowData.window)
		{
			KalaWindowCore::ForceClose(
				"Window error",
				"Failed to set shutdown block state because the attached window was invalid!");
		}

		HWND window = ToVar<HWND>(windowData.window);

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
				"KW_WINDOW",
				LogType::LOG_SUCCESS);
		}
	}
	bool ProcessWindow::IShutdownBlockEnabled() const { return shutdownBlockState; }

	void ProcessWindow::Flash(
		FlashTarget target,
		FlashType type,
		u32 count) const
	{
		if (!windowData.window)
		{
			KalaWindowCore::ForceClose(
				"Window error",
				"Failed to call flash because the attached window was invalid!");
		}

		HWND window = ToVar<HWND>(windowData.window);

		string targetName = target == FlashTarget::TARGET_WINDOW
			? "window"
			: "taskbar";

		if (type == FlashType::FLASH_TIMED
			&& count == 0)
		{
			Log::Print(
				"Failed to flash " + targetName + " because type was set to 'FLASH_TIMED' but no count value was assigned!",
				"KW_WINDOW",
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
				"KW_WINDOW",
				LogType::LOG_SUCCESS);
		}
	}

	void ProcessWindow::SetTaskbarProgressBarState(
		TaskbarProgressBarMode mode,
		u8 current,
		u8 max) const
	{
		if (!windowData.window)
		{
			KalaWindowCore::ForceClose(
				"Window error",
				"Failed to set tarkbar progress bar state because the attached window was invalid!");
		}

		HWND window = ToVar<HWND>(windowData.window);

		u8 maxClamped = clamp(
			max, 
			scast<u8>(1), 
			scast<u8>(100));

		u8 currentClamped = clamp(
			current, 
			scast<u8>(0),
			scast<u8>(maxClamped - 1));

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
				"KW_WINDOW",
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
				"KW_WINDOW",
				LogType::LOG_SUCCESS);
		}
	}

	void ProcessWindow::TriggerResize() { if (resizeCallback) resizeCallback(); }
	void ProcessWindow::SetResizeCallback(const function<void()>& callback) { resizeCallback = callback; }

	void ProcessWindow::TriggerRedraw() { if (redrawCallback) redrawCallback(); }
	void ProcessWindow::SetRedrawCallback(const function<void()>& callback) { redrawCallback = callback; }

	void ProcessWindow::SetWindowData(const WindowData& newWindowStruct) { windowData = newWindowStruct; }
	const WindowData& ProcessWindow::GetWindowData() const { return windowData; }

	//
	// WINDOW CONTENT
	//

	u32 ProcessWindow::GetInputID() const { return inputID; }
	void ProcessWindow::SetInputID(u32 newValue) { inputID = newValue; }

	u32 ProcessWindow::GetContextID() const { return contextID; }
	void ProcessWindow::SetContextID(u32 newValue) { contextID = newValue; }

	u32 ProcessWindow::GetMenuBarID() const { return menuBarID; }
	void ProcessWindow::SetMenuBarID(u32 newValue) { menuBarID = newValue; }

	void ProcessWindow::SetShutdownCallback(function<void()> newValue) { shutdownCallback = newValue; }

	void ProcessWindow::CloseWindow()
	{
		if (shutdownCallback) shutdownCallback();
		
        KalaWindowRegistry<Vulkan_Context>::RemoveAllWindowContent(ID);

		KalaWindowRegistry<Input>::RemoveAllWindowContent(ID);
		KalaWindowRegistry<MenuBar>::RemoveAllWindowContent(ID);
		
		KalaWindowRegistry<ProcessWindow>::RemoveContent(ID);
	}

	ProcessWindow::~ProcessWindow()
	{
		string title = GetTitle();

		Log::Print(
			"Destroying window '" + title + "' with ID '" + to_string(ID) + "'.",
			"KW_WINDOW",
			LogType::LOG_INFO);

		inputID = 0;
		contextID = 0;
		menuBarID = 0;

		HWND hwnd = ToVar<HWND>(windowData.window);
		if (hwnd)
		{
			SetWindowState(WindowState::WINDOW_HIDE);

			if (windowData.wndProc) windowData.wndProc = NULL;

			if (windowData.handle)
			{
				ReleaseDC(
					hwnd,
					ToVar<HDC>(windowData.handle));
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

			if (shutdownBlockState) WTSUnRegisterSessionNotification(hwnd);

			if (windowData.window)
			{
				DestroyWindow(hwnd);
				windowData.window = NULL;
			}
			windowData.hInstance = NULL;
		}
	}
}

/*
TODO: add texture support back

HICON SetUpIcon(OpenGL_Texture* texture)
{
	string name = texture->GetName();
	vec2 size = texture->GetSize();
	string sizeX = to_string(scast<int>(size.x));
	string sizeY = to_string(scast<int>(size.y));

	if (texture->GetSize().x < 32)
	{
		Log::Print(
			"Icon '" + name + "' size '" + sizeX + "x" + sizeY + "' is too small! Consider uploading a bigger icon.",
			"KW_WINDOW",
			LogType::LOG_WARNING);
	}
	if (size.x > 256)
	{
		Log::Print(
			"Icon '" + name + "' size '" + sizeX + "x" + sizeY + "' is too big! Consider uploading a smaller icon.",
			"KW_WINDOW",
			LogType::LOG_WARNING);
	}

	//convert RGBA to BGRA

	const vector<u8>& pixels = texture->GetPixels();
	vector<u8> pixelsBGRA(pixels.size());

	for (size_t i = 0; i < scast<size_t>(size.x) * size.y; i++)
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

	HDC handle = CreateCompatibleDC(nullptr);
	void* pvBits = nullptr;
	HBITMAP hBitMap = CreateDIBSection(
		handle,
		rcast<BITMAPINFO*>(&bi),
		DIB_RGB_COLORS,
		&pvBits,
		nullptr,
		0);
	DeleteDC(handle);

	if (!hBitMap)
	{
		Log::Print(
			"Failed to create hBitMask for setting window '" + name + "' icon!",
			"KW_WINDOW",
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

wstring ToWide(string_view input)
{
	if (input.empty()) return wstring();

	int size_needed = MultiByteToWideChar(
		CP_UTF8,
		MB_ERR_INVALID_CHARS,
		input.data(),
		scast<int>(input.size()),
		nullptr,
		0);

	if (size_needed <= 0) return {};

	wstring wstr(size_needed, 0);

	if (MultiByteToWideChar(
		CP_UTF8,
		MB_ERR_INVALID_CHARS,
		input.data(),
		scast<int>(input.size()),
		wstr.data(),
		size_needed) <= 0)
	{
		return {};
	}

	return wstr;
}
string ToShort(const wstring& str)
{
	if (str.empty()) return{};

	int size_needed = WideCharToMultiByte(
		CP_UTF8,
		0,
		str.data(),
		scast<int>(str.size()),
		nullptr,
		0,
		nullptr,
		nullptr);

	if (size_needed <= 0) return {};

	string result(size_needed, 0);

	if (WideCharToMultiByte(
		CP_UTF8,
		0,
		str.data(),
		scast<int>(str.size()),
		result.data(),
		size_needed,
		nullptr,
		nullptr) <= 0)
	{
		return {};
	}

	return result;
}

#endif //_WIN32
