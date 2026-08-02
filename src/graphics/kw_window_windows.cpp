//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef _WIN32

#include <windows.h>
#include <winuser.h>
#include <mmsystem.h>
#include <shobjidl.h>
#include <dwmapi.h>
//#include <atlbase.h>
//#include <atlcomcli.h>
#include <wrl/client.h>
#include <shobjidl.h>
#include <wtsapi32.h>
#include <shellapi.h>

#include <algorithm>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "core_utils.hpp"
#include "log_utils.hpp"

#include "graphics/kw_window.hpp"
#include "core/kw_core.hpp"
#include "core/kw_input.hpp"
#include "graphics/kw_window_global.hpp"
#include "graphics/kw_vulkan.hpp"

using KalaHeaders::KalaCore::ToVar;
using KalaHeaders::KalaCore::FromVar;
using KalaHeaders::KalaMath::vec2;
using KalaHeaders::KalaMath::isnear;
using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;

using KalaWindow::Core::KalaWindowCore;
using KalaWindow::Core::MAX_NAME_LENGTH;
using KalaWindow::Core::Input;
using KalaWindow::Graphics::ProcessWindow;
using KalaWindow::Graphics::VulkanContext;

using std::make_unique;
using std::to_string;
using std::unique_ptr;
using std::clamp;
using std::ostringstream;
using std::wstring;
using std::string;
using std::string_view;
using std::vector;

//KalaWindow will dynamically update window idle state
static void UpdateIdleState(ProcessWindow* window, bool& isIdle)
{
	isIdle =
		!window->IsForegroundWindow()
		|| window->IsMinimized()
		|| !window->IsVisible();
}

static void ForceClose(
    string&& action,
    string&& reason)
{
    KalaWindowCore::ForceClose(
        "KalaWindow window error",
        "Failed to " + std::move(action) + " because " + std::move(reason) + "!");
}

static wstring ToWide(string_view str);
static string ToShort(const wstring& str);

namespace KalaWindow::Graphics
{
	static KalaWindowRegistry<ProcessWindow> registry{};

	KalaWindowRegistry<ProcessWindow>& ProcessWindow::GetRegistry() { return registry; }

	ProcessWindow* ProcessWindow::Initialize(
		string&& title,
		vec2 pos,
		vec2 size,
		ProcessWindow* parentWindow,
		DpiContext context)
	{
		if (!Window_Global::IsInitialized())
		{
			Log::Print(
				"Failed to create window because global window has not been initialized!",
				"KW_WINDOW",
				LogType::LOG_ERROR,
				2);

			return nullptr;
		}

		if (title.empty()
            || title.size() > MAX_NAME_LENGTH)
		{
			Log::Print(
				"Failed to create window because its title is empty or too long!",
				"KW_WINDOW",
				LogType::LOG_ERROR,
				2);

			return nullptr;
		}

		if (size < MIN_WINDOW_SIZE)
		{
			Log::Print(
				"Failed to create window '" + title + "' because its size is too small!",
				"KW_WINDOW",
				LogType::LOG_ERROR,
				2);

			return nullptr;
		}
		if (size > MAX_WINDOW_SIZE)
		{
			Log::Print(
				"Failed to create window '" + title + "' because its size is too big!",
				"KW_WINDOW",
				LogType::LOG_ERROR,
				2);

			return nullptr;
		}

		u32 newID = KalaWindowCore::GetGlobalID() + 1;
		KalaWindowCore::SetGlobalID(newID);

		unique_ptr<ProcessWindow> newWindow = make_unique<ProcessWindow>();
		ProcessWindow* windowPtr = newWindow.get();

		HINSTANCE newHInstance = GetModuleHandle(nullptr);

		HWND newHwnd = CreateWindowExW(
			WS_EX_ACCEPTFILES | WS_EX_APPWINDOW,
			ToWide(Window_Global::GetAppID()).c_str(),
			ToWide(title).c_str(),
			WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN,
			pos.x,
			pos.y,
			size.x,
			size.y,
			nullptr,
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
				"KalaWindow window error",
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

		windowPtr->ID = newID;

		windowPtr->oldPos = pos;
		windowPtr->oldSize = size;

		//allow files to be dragged to this window
		DragAcceptFiles(newHwnd, TRUE);

		if (parentWindow)
		{
			const vector<ProcessWindow*>& content = registry.GetAllContent();

			if (find(content.begin(),
				content.end(),
				parentWindow)
				== content.end())
			{
				ForceClose(
					"create child window '" + title + "'",
                    "parent window was invalid!");
			}

			HWND parentWindowRef = ToVar<HWND>(parentWindow->GetWindowData().window);

			if (!parentWindowRef
				|| !IsWindow(parentWindowRef))
			{
				ForceClose(
					"create child window '" + title + "'",
                    "parent window '" + to_string(parentWindow->GetID()) + "' handle is invalid!");
			}

			parentWindow->childIDs.push_back(newID);
			windowPtr->parentID = parentWindow->ID;
		}

		windowPtr->BringToFocus();

		registry.AddContent(newID, std::move(newWindow));

		Log::Print(
			"Created new window '" + title + "' with ID '" + to_string(newID) + "'!",
			"KW_WINDOW",
			LogType::LOG_SUCCESS);

		return windowPtr;
	}

	u32 ProcessWindow::GetID() const { return ID; }
	u32 ProcessWindow::GetInputID() const { return inputID; }
	u32 ProcessWindow::GetGraphicsContextID() const { return graphicsContextID; }

	void ProcessWindow::Update()
	{
		if (!windowData.window
			|| !IsWindow(ToVar<HWND>(windowData.window)))
		{
			Log::Print(
				"Destroying window '" + to_string(ID) + "' because its HWND was lost or became invalid!",
				"KW_WINDOW",
				LogType::LOG_WARNING);

			Destroy();

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

    void ProcessWindow::SetDraggedFilesCallback(function<void(const vector<path>&, vec2)>&& newValue)
    {
        if (!newValue)
		{
			Log::Print(
				"Failed to assign window '" + to_string(ID) + "' dragged files callback because it was empty!",
				"KW_WINDOW",
				LogType::LOG_ERROR,
				2);

			return;
		}

		draggedFilesCallback = std::move(newValue);
    }
	const vector<path>& ProcessWindow::GetLastDraggedFiles() const { return lastDraggedFiles; };
	void ProcessWindow::ClearLastDraggedFiles() { lastDraggedFiles.clear(); };

	string ProcessWindow::GetTitle() const
	{
		if (!windowData.window)
		{
			ForceClose(
				"get window '" + to_string(ID) + "' title",
                "the window handle was invalid!");
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
	void ProcessWindow::SetTitle(string&& newTitle) const
	{
		if (!windowData.window)
		{
			ForceClose(
				"set window '" + to_string(ID) + "' title",
                "the window handle was invalid!");
		}

		if (newTitle.empty()
            || newTitle.length() > MAX_NAME_LENGTH)
		{
			Log::Print(
				"Failed to set window '" + to_string(ID) + "' title "
                "because the new title is empty or too long!",
				"KW_WINDOW",
				LogType::LOG_ERROR,
				2);

			return;
		}

		HWND window = ToVar<HWND>(windowData.window);

		wstring wideTitle = ToWide(newTitle);

		SetWindowTextW(
			window, 
			wideTitle.c_str());

		if (Window_Global::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set window '" + to_string(ID) + "' title to '" + newTitle + "'",
				"KW_WINDOW",
				LogType::LOG_VERBOSE);
		}
	}

	void ProcessWindow::BringToFocus()
	{
		if (!windowData.window)
		{
			ForceClose(
				"bring window '" + to_string(ID) + "' to focus",
                "the window handle was invalid!");
		}

        //skip if already focused
        if (IsFocused()) return;

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
					"Set window '" + to_string(ID) + "' focus through the fallback method.'",
					"KW_WINDOW",
					LogType::LOG_VERBOSE);
			}
		}

		//ensure keyboard focus
		SetFocus(window);
	}

	vec2 ProcessWindow::GetSize() const
	{
		if (!windowData.window)
		{
			ForceClose(
				"get" + to_string(ID) + "' size",
                "the window handle was invalid!");
		}

		HWND window = ToVar<HWND>(windowData.window);

		RECT rect{};
		GetClientRect(window, &rect);

		return vec2
		{
			scast<f32>(rect.right - rect.left),
			scast<f32>(rect.bottom - rect.top)
		};
	}
	void ProcessWindow::SetSize(vec2 newSize)
	{
		if (!windowData.window)
		{
			ForceClose(
				"set window '" + to_string(ID) + "' size",
                "the window handle was invalid!");
		}

		vec2 oldSize = GetSize();
		if (isnear(oldSize, newSize))
        {
			Log::Print(
				"Failed to set window '" + to_string(ID) + "' size because it is already the same size!",
				"KW_WINDOW",
				LogType::LOG_ERROR,
				2);

            return;
        }

        if (newSize > maxSize)
		{
			Log::Print(
				"Failed to set window '" + to_string(ID) + "' size because it cannot be bigger than window max size!",
				"KW_WINDOW",
				LogType::LOG_ERROR,
				2);

			return;
		}
        if (newSize < minSize)
		{
			Log::Print(
				"Failed to set window '" + to_string(ID) + "' size because it cannot be smaller than window min size!",
				"KW_WINDOW",
				LogType::LOG_ERROR,
				2);

			return;
		}

		HWND window = ToVar<HWND>(windowData.window);

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
				"Set window '" + to_string(ID) + "' size to '" + val + "'",
				"KW_WINDOW",
				LogType::LOG_VERBOSE);
		}
	}

	vec2 ProcessWindow::GetOuterSize() const
	{
		if (!windowData.window)
		{
			ForceClose(
				"get window '" + to_string(ID) + "' outer size",
                "the window handle was invalid!");
		}

		HWND window = ToVar<HWND>(windowData.window);

		RECT rect{};
		GetWindowRect(window, &rect);

		return vec2
		{
			scast<f32>(rect.right - rect.left),
			scast<f32>(rect.bottom - rect.top)
		};
	}
	void ProcessWindow::SetOuterSize(vec2 newSize)
	{
		if (!windowData.window)
		{
			ForceClose(
				"set window '" + to_string(ID) + "' outer size",
                "the window handle was invalid!");
		}

		vec2 oldSize = GetOuterSize();
		if (isnear(oldSize, newSize))
        {
			Log::Print(
				"Failed to set window '" + to_string(ID) + "' outer size because it is already the same size!",
				"KW_WINDOW",
				LogType::LOG_ERROR,
				2);

            return;
        }

        if (newSize > maxSize)
		{
			Log::Print(
				"Failed to set window '" + to_string(ID) + "' size because it cannot be bigger than window max size!",
				"KW_WINDOW",
				LogType::LOG_ERROR,
				2);

			return;
		}
        if (newSize < minSize)
		{
			Log::Print(
				"Failed to set window '" + to_string(ID) + "' size because it cannot be smaller than window min size!",
				"KW_WINDOW",
				LogType::LOG_ERROR,
				2);

			return;
		}

		HWND window = ToVar<HWND>(windowData.window);

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
				"Set window '" + to_string(ID) + "' outer size to '" + val + "'",
				"KW_WINDOW",
				LogType::LOG_VERBOSE);
		}
	}

	vec2 ProcessWindow::GetMaxSize() const { return maxSize; }
    void ProcessWindow::SetMaxSize(vec2 newSize)
    { 
        if (isnear(maxSize, newSize))
        {
			Log::Print(
				"Failed to set window '" + to_string(ID) + "' max size because it is already the same size!",
				"KW_WINDOW",
				LogType::LOG_ERROR,
				2);

            return;
        }

        if (newSize > MAX_WINDOW_SIZE)
		{
			Log::Print(
				"Failed to set window '" + to_string(ID)
                + "' max size because it is too big!",
				"KW_WINDOW",
				LogType::LOG_ERROR,
				2);

			return;
		}
        if (newSize < minSize)
		{
			Log::Print(
				"Failed to set window '" + to_string(ID)
                + "' max size because it cannot be smaller than window min size!",
				"KW_WINDOW",
				LogType::LOG_ERROR,
				2);

			return;
		}
		
		maxSize = newSize;
        if (GetSize() > newSize) SetSize(newSize);

		string val = to_string(newSize.x) + "x" + to_string(newSize.y);

		if (Window_Global::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set window '" + to_string(ID) + "' max size to '" + val + "'",
				"KW_WINDOW",
				LogType::LOG_VERBOSE);
		}
    }

	vec2 ProcessWindow::GetMinSize() const { return minSize; }
	void ProcessWindow::SetMinSize(vec2 newSize)
    { 
		if (isnear(minSize, newSize))
        {
			Log::Print(
				"Failed to set window '" + to_string(ID) + "' min size because it is already the same size!",
				"KW_WINDOW",
				LogType::LOG_ERROR,
				2);

            return;
        }

        if (newSize > maxSize)
		{
			Log::Print(
				"Failed to set window '" + to_string(ID) 
                + "' min size because it cannot be bigger than window max size!",
				"KW_WINDOW",
				LogType::LOG_ERROR,
				2);

			return;
		}
        if (newSize < MIN_WINDOW_SIZE)
		{
			Log::Print(
				"Failed to set window '" + to_string(ID) 
                + "' min size because it is too small!",
				"KW_WINDOW",
				LogType::LOG_ERROR,
				2);

			return;
		}

		minSize = newSize;
        if (GetSize() < minSize) SetSize(minSize);

		string val = to_string(newSize.x) + "x" + to_string(newSize.y);

		if (Window_Global::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set window '" + to_string(ID) + "' min size to '" + val + "'",
				"KW_WINDOW",
				LogType::LOG_VERBOSE);
		}
    }

	vec2 ProcessWindow::GetPosition()
	{
		if (!windowData.window)
		{
			ForceClose(
				"get window '" + to_string(ID) + "' position",
                "the window handle was invalid!");
		}

		HWND window = ToVar<HWND>(windowData.window);

		RECT rect{};
		if (GetWindowRect(window, &rect))
		{
			return vec2
			{ 
				scast<f32>(rect.left),
				scast<f32>(rect.top)
			};
		}

		return vec2{ 0, 0 };
	}
	void ProcessWindow::SetPosition(vec2 newPosition)
	{
		if (!windowData.window)
		{
			ForceClose(
				"set window '" + to_string(ID) + "' position",
                "the window handle was invalid!");
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
				"Set window '" + to_string(ID) + "' position to '" + val + "'",
				"KW_WINDOW",
				LogType::LOG_VERBOSE);
		}
	}

	bool ProcessWindow::IsAlwaysOnTop() const
	{
		if (!windowData.window)
		{
			ForceClose(
				"get window '" + to_string(ID) + "' always on top state",
                "the window handle was invalid!");
		}

		HWND window = ToVar<HWND>(windowData.window);

		LONG exStyle = GetWindowLong(
			window,
			GWL_EXSTYLE);

		return (exStyle & WS_EX_TOPMOST) != 0;
	}
	void ProcessWindow::SetAlwaysOnTopState(bool state)
	{
		if (!windowData.window)
		{
			ForceClose(
				"set window '" + to_string(ID) + "' always on top state",
                "the window handle was invalid!");
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
				"Set window '" + to_string(ID) + "' always on top state to '" + val + "'",
				"KW_WINDOW",
				LogType::LOG_VERBOSE);
		}
	}

	bool ProcessWindow::IsResizable() const
	{
		if (!windowData.window)
		{
			ForceClose(
				"get window '" + to_string(ID) + "' resizable state",
                "the window handle was invalid!");
		}

		HWND window = ToVar<HWND>(windowData.window);

		LONG style = GetWindowLong(
			window,
			GWL_STYLE);

		return (style &
			(WS_THICKFRAME
			| WS_MAXIMIZEBOX)) != 0;
	}
	void ProcessWindow::SetResizableState(bool state)
	{
		if (!windowData.window)
		{
			ForceClose(
				"set window '" + to_string(ID) + "' resizable state",
                "the window handle was invalid!");
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
				"Set window '" + to_string(ID) + "' resizable state to '" + val + "'",
				"KW_WINDOW",
				LogType::LOG_VERBOSE);
		}
	}

	bool ProcessWindow::IsIdle() const { return isIdle; }

	bool ProcessWindow::IsHovered() const { return isWindowHovered; }
	bool ProcessWindow::IsForegroundWindow() const
	{
		if (!windowData.window)
		{
			ForceClose(
				"get window '" + to_string(ID) + "' foreground state",
                "the window handle was invalid!");
		}

		HWND window = ToVar<HWND>(windowData.window);

		return GetForegroundWindow() == window;
	}
	bool ProcessWindow::IsFocused() const
	{
		if (!windowData.window)
		{
			ForceClose(
				"get window '" + to_string(ID) + "' focused state",
                "the window handle was invalid!");
		}

		HWND window = ToVar<HWND>(windowData.window);

		return GetFocus() == window;
	}
	bool ProcessWindow::IsFullscreen()
	{
		if (!windowData.window)
		{
			ForceClose(
				"get window '" + to_string(ID) + "' fullscreen state",
                "the window handle was invalid!");
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
			ForceClose(
				"get window '" + to_string(ID) + "' minimized state",
                "the window handle was invalid!");
		}

		HWND window = ToVar<HWND>(windowData.window);

		//IsIconic returns TRUE if the window is minimized
		return IsIconic(window);
	}
	bool ProcessWindow::IsVisible() const
	{
		if (!windowData.window)
		{
			ForceClose(
				"get window '" + to_string(ID) + "' visible state",
                "the window handle was invalid!");
		}

		HWND window = ToVar<HWND>(windowData.window);

		return IsWindowVisible(window);
	}
	bool ProcessWindow::IsResizing() const { return isResizing; }

	WindowMode ProcessWindow::GetWindowMode()
	{
		if (!windowData.window)
		{
			ForceClose(
				"get window '" + to_string(ID) + "' mode",
                "the window handle was invalid!");
		}

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
	void ProcessWindow::SetWindowMode(WindowMode mode)
	{
		if (!windowData.window)
		{
			ForceClose(
				"set window '" + to_string(ID) + "' mode",
                "the window handle was invalid!");
		}

		HWND window = ToVar<HWND>(windowData.window);

		string windowModeVal{};

		switch (mode)
		{
		case WindowMode::WINDOWMODE_WINDOWED:
		{
			windowModeVal = "windowed";

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
			windowModeVal = "borderless";

			//save current pos and size

			oldPos = GetPosition();
			oldSize = GetSize();
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
			windowModeVal = "exclusive";

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
				oldSize = GetSize();

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
					"Failed to switch to exclusive mode for window '" + to_string(ID) + "'!",
					"KW_WINDOW",
					LogType::LOG_ERROR,
					2);

				return;
			}

			break;
		}
		default: break;
		}

		ShowWindow(window, SW_SHOWNORMAL);

		if (Window_Global::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set window '" + to_string(ID) + "' mode to '" + windowModeVal + "'",
				"KW_WINDOW",
				LogType::LOG_VERBOSE);
		}
	}

	WindowState ProcessWindow::GetWindowState() const
	{
		if (!windowData.window)
		{
			ForceClose(
				"get window '" + to_string(ID) + "' state",
                "the window handle was invalid!");
		}

		HWND window = ToVar<HWND>(windowData.window);

		WINDOWPLACEMENT placement{};
		placement.length = sizeof(WINDOWPLACEMENT);

		if (!GetWindowPlacement(window, &placement))
		{
			Log::Print(
				"Failed to get window '" + to_string(ID) + "' state!",
				"KW_WINDOW",
				LogType::LOG_ERROR,
				2);

			return WindowState::WINDOW_NORMAL;
		}

		switch (placement.showCmd)
		{
		default:
		case SW_NORMAL:         return WindowState::WINDOW_NORMAL;
		case SW_SHOWNOACTIVATE: return WindowState::WINDOW_SHOWNOACTIVATE;
		case SW_SHOWMAXIMIZED:  return WindowState::WINDOW_MAXIMIZE;
		case SW_SHOWMINIMIZED:  return WindowState::WINDOW_MINIMIZE;
		case SW_HIDE:           return WindowState::WINDOW_HIDE;
		}
	}
	void ProcessWindow::SetWindowState(WindowState state)
	{
		if (!windowData.window)
		{
			ForceClose(
				"set window '" + to_string(ID) + "' state",
                "the window handle was invalid!");
		}

		HWND window = ToVar<HWND>(windowData.window);

		string windowStateVal{};

		switch (state)
		{
		case WindowState::WINDOW_NORMAL:
			windowStateVal = "normal";

			ShowWindow(window, SW_SHOWNORMAL);
			break;
		case WindowState::WINDOW_SHOWNOACTIVATE:
			windowStateVal = "unfocused visible";

			ShowWindow(window, SW_SHOWNOACTIVATE);
			break;
		case WindowState::WINDOW_MAXIMIZE:
			windowStateVal = "maximize";

			ShowWindow(window, SW_MAXIMIZE);
			break;
		case WindowState::WINDOW_MINIMIZE:
			windowStateVal = "minimize";

			ShowWindow(window, SW_MINIMIZE);
			break;
		case WindowState::WINDOW_HIDE:
			windowStateVal = "hide";

			ShowWindow(window, SW_HIDE);
			break;
		}

		if (state != WindowState::WINDOW_HIDE
			&& state != WindowState::WINDOW_MINIMIZE)
		{
			UpdateWindow(window);
		}

		if (Window_Global::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set window '" + to_string(ID) + "' state to '" + windowStateVal + "'",
				"KW_WINDOW",
				LogType::LOG_VERBOSE);
		}
	}

	const WindowData& ProcessWindow::GetWindowData() const { return windowData; }
	void ProcessWindow::SetWindowData(WindowData&& newWindowStruct)
    {
        if (!windowData.window)
        {
			Log::Print(
				"Failed to set window '" + to_string(ID) + "' data "
                "because the window handle was invalid!",
				"KW_WINDOW",
				LogType::LOG_ERROR,
				2);

			return;
        }

        windowData = std::move(newWindowStruct);
    }

	void ProcessWindow::ResizeCallback() { if (resizeCallback) resizeCallback(); }
	void ProcessWindow::SetResizeCallback(function<void()>&& newValue)
	{
		if (!newValue)
		{
			Log::Print(
				"Failed to assign window '" + to_string(ID) + "' resize callback because it was empty!",
				"KW_WINDOW",
				LogType::LOG_ERROR,
				2);

			return;
		}

		resizeCallback = std::move(newValue);
	}

	void ProcessWindow::SetShutdownCallback(function<void()>&& newValue)
	{
		if (!newValue)
		{
			Log::Print(
				"Failed to assign window '" + to_string(ID) + "' shutdown callback because it was empty!",
				"KW_WINDOW",
				LogType::LOG_ERROR,
				2);

			return;
		}

		shutdownCallback = std::move(newValue);
	}

	void ProcessWindow::Destroy()
	{
		if (registry.GetAllContent().size() == 1)
		{
			Log::Print(
                "\n======================================================================"
                "\nSHUTTING DOWN"
                "\n======================================================================\n",
                true);

			Log::Print(
				"Shutting down because the last process window is being destroyed.",
				"KW_WINDOW",
				LogType::LOG_INFO);
		}

		if (shutdownCallback) shutdownCallback();

		vector<u32> children = std::move(childIDs);

		for (const auto& w : children)
		{
			if (Window_Global::IsVerboseLoggingEnabled())
			{
				Log::Print(
					"Destroying child window '" + to_string(w) + "' of parent window '" + to_string(ID) + "'",
					"KW_WINDOW",
					LogType::LOG_VERBOSE);
			}

			ProcessWindow* pw = ProcessWindow::GetRegistry().GetContent(w);
			if (pw) pw->Destroy();
		}
		if (parentID != UINT32_MAX)
		{
			if (Window_Global::IsVerboseLoggingEnabled())
			{
				Log::Print(
					"Destroying child window '" + to_string(ID) + "' of parent window '" + to_string(parentID) + "'",
					"KW_WINDOW",
					LogType::LOG_VERBOSE);
			}

			ProcessWindow* pw = ProcessWindow::GetRegistry().GetContent(parentID);
			if (pw)
			{
				auto it = find(pw->childIDs.begin(), pw->childIDs.end(), ID);
				if (it != pw->childIDs.end()) pw->childIDs.erase(it);
			}
		}

        KalaWindowRegistry<VulkanContext>::RemoveAllWindowContent(ID);
		KalaWindowRegistry<Input>::RemoveAllWindowContent(ID);

		registry.RemoveContent(ID);
	}

	ProcessWindow::~ProcessWindow()
	{
		string title = GetTitle();

		Log::Print(
			"Destroying window '" + title + "' with ID '" + to_string(ID) + "'.",
			"KW_WINDOW",
			LogType::LOG_INFO);

		inputID = 0;
		graphicsContextID = 0;

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

			if (windowData.window)
			{
				DestroyWindow(hwnd);
				windowData.window = NULL;
			}
			windowData.hInstance = NULL;
		}

		if (registry.GetAllContent().empty())
		{	
			//TODO: figure out if this is even needed at all anywhere
			//timeEndPeriod(1);

			Log::Print(
                "\n======================================================================"
                "\nFINISHED SHUTDOWN"
                "\n======================================================================\n",
                true);

			exit(0);
		}
	}
}

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
