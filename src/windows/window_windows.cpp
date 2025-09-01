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
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")
#include <atlbase.h>
#include <atlcomcli.h>
#include <wtsapi32.h>
#pragma comment(lib, "Wtsapi32.lib")
#include <winrt/windows.ui.notifications.h>
#include <winrt/windows.data.xml.dom.h>
#pragma comment(lib, "runtimeobject.lib")
#include <wrl/client.h>
#include <shellapi.h>

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
#include "core/global_handles.hpp"

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
using KalaWindow::Core::GlobalHandle;

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
using std::string_view;
using std::vector;
using namespace winrt::Windows::UI::Notifications;
using namespace winrt::Windows::Data::Xml::Dom;
using Microsoft::WRL::ComPtr;

static bool checkedOSVersion = false;
constexpr u32 MIN_OS_VERSION = 10017763; //Windows 10 build 17763 (1809)
constexpr u16 MAX_TITLE_LENGTH = 512;
constexpr u8 MAX_LABEL_LENGTH = 64;

static string APP_ID{};

static bool enabledBeginPeriod = false;

//KalaWindow will dynamically update window idle state
static void UpdateIdleState(Window* window, bool& isIdle);

static HICON SetUpIcon(OpenGL_Texture* texture);

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

				if (Window::IsVerboseLoggingEnabled())
				{
					Log::Print(
						"Windows version '" + osVersion + "' build '" + buildVersion + "'",
						"WINDOW_WINDOWS",
						LogType::LOG_INFO);
				}

				checkedOSVersion = true;
			}

			wchar_t buffer[MAX_PATH]{};
			GetModuleFileNameW(
				nullptr, 
				buffer, 
				MAX_PATH);

			path exePath(buffer);

			APP_ID = exePath.stem().string();

			//Treat this process as a real app with a stable identity
			SetCurrentProcessExplicitAppUserModelID(ToWide(APP_ID).c_str());

			Log::Print(
				"Creating window '" + title + "'.",
				"WINDOW_WINDOWS",
				LogType::LOG_INFO);

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
			wc.lpszClassName = APP_ID.c_str();

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
			APP_ID.c_str(),
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

		u32 newID = ++globalID;
		unique_ptr<Window> newWindow = make_unique<Window>();
		Window* windowPtr = newWindow.get();
		
		newWindow->SetTitle(title);
		newWindow->ID = newID;
		newWindow->SetClientRectSize(size);
		newWindow->window_windows = newWindowStruct;

		newWindow->isInitialized = true;

		//set window state to user preferred version
		newWindow->SetWindowState(state);

		//allow files to be dragged to this window
		DragAcceptFiles(newHwnd, TRUE);

		createdWindows[newID] = move(newWindow);
		runtimeWindows.push_back(windowPtr);

		Log::Print(
			"Created window '" + title + "' with ID '" + to_string(newID) + "'!",
			"WINDOW_WINDOWS",
			LogType::LOG_SUCCESS);

		return windowPtr;
	}

	vector<string> Window::GetFile(
		FileType type,
		bool multiple)
	{
		HRESULT hr = CoInitializeEx(
			nullptr,
			COINIT_APARTMENTTHREADED
			| COINIT_DISABLE_OLE1DDE);

		if (FAILED(hr)
			&& hr != RPC_E_CHANGED_MODE)
		{
			Log::Print(
				"Failed to initialize COM! Reason: " + HResultToString(hr),
				"WINDOW_WINDOWS",
				LogType::LOG_ERROR,
				2);

			return {};
		}

		bool canUninit = (hr == S_OK);

		auto UnInit = [canUninit]()
			{
				if (canUninit)
				{
					Log::Print(
						"Calling CoUninitialize",
						"WINDOW_WINDOWS",
						LogType::LOG_DEBUG);

					CoUninitialize();
				}
				else
				{
					Log::Print(
						"Skipping CoUninitialize()",
						"WINDOW_WINDOWS",
						LogType::LOG_DEBUG);
				}
			};

		ComPtr<IFileOpenDialog> fileOpen{};

		hr = CoCreateInstance(
			CLSID_FileOpenDialog,
			nullptr,
			CLSCTX_ALL,
			IID_PPV_ARGS(&fileOpen));

		if (FAILED(hr))
		{
			Log::Print(
				"Failed to create file open dialog! Reason: " + HResultToString(hr),
				"WINDOW_WINDOWS",
				LogType::LOG_ERROR,
				2);

			UnInit();
			return{};
		}

		DWORD options{};
		fileOpen->GetOptions(&options);

		auto PrintError = [](
			const string& typeVal,
			HRESULT hr)
			{
				Log::Print(
					"Failed to set file filter to '" + typeVal + "' for dialog! Reason: " + HResultToString(hr),
					"WINDOW_WINDOWS",
					LogType::LOG_ERROR,
					2);
			};

		switch (type)
		{
		default:
		case FileType::FILE_ANY:
		{
			COMDLG_FILTERSPEC filter[] =
			{
				{ L"All Files (*.*)", L"*.*" }
			};

			hr = fileOpen->SetFileTypes(1, filter);
			if (FAILED(hr))
			{
				PrintError("FILE_ANY", hr);

				UnInit();
				return{};
			}

			break;
		}
		case FileType::FILE_FOLDER:
		{
			options |= FOS_PICKFOLDERS;

			break;
		}
		case FileType::FILE_EXE:
		{
			COMDLG_FILTERSPEC filter[] =
			{
				{ L"Executable Files (*.exe)", L"*.exe" }
			};

			hr = fileOpen->SetFileTypes(1, filter);
			if (FAILED(hr))
			{
				PrintError("FILE_EXE", hr);

				UnInit();
				return{};
			}

			break;
		}
		case FileType::FILE_TEXT:
		{
			COMDLG_FILTERSPEC filter[] =
			{
				{ L"Text Files (*.txt;*.ini;*.rtf;*.md)", L"*.txt;*.ini;*.rtf;*.md" }
			};

			hr = fileOpen->SetFileTypes(1, filter);
			if (FAILED(hr))
			{
				PrintError("FILE_TEXT", hr);

				UnInit();
				return{};
			}

			break;
		}
		case FileType::FILE_STRUCTURED:
		{
			COMDLG_FILTERSPEC filter[] =
			{
				{ L"Structured Files (*.json;*.xml;*.yaml;*.yml;*.toml)", L"*.json;*.xml;*.yaml;*.yml;*.toml" }
			};

			hr = fileOpen->SetFileTypes(1, filter);
			if (FAILED(hr))
			{
				PrintError("FILE_STRUCTURED", hr);

				UnInit();
				return{};
			}

			break;
		}
		case FileType::FILE_SCRIPT:
		{
			COMDLG_FILTERSPEC filter[] =
			{
				{ L"Script Files (*.lua;*.cpp;*.hpp;*.c;*.h)", L"*.lua;*.cpp;*.hpp;*.c;*.h" }
			};

			hr = fileOpen->SetFileTypes(1, filter);
			if (FAILED(hr))
			{
				PrintError("FILE_SCRIPT", hr);

				UnInit();
				return{};
			}

			break;
		}
		case FileType::FILE_ARCHIVE:
		{
			COMDLG_FILTERSPEC filter[] =
			{
				{ L"Archive Files (*.zip;*.7z;*.rar;*.kdat)", L"*.zip;*.7z;*.rar;*.kdat" }
			};

			hr = fileOpen->SetFileTypes(1, filter);
			if (FAILED(hr))
			{
				PrintError("FILE_ARCHIVE", hr);

				UnInit();
				return{};
			}

			break;
		}
		case FileType::FILE_VIDEO:
		{
			COMDLG_FILTERSPEC filter[] =
			{
				{ L"Video Files (*.mp4;*.mov;*.mkv)", L"*.mp4;*.mov;*.mkv" }
			};

			hr = fileOpen->SetFileTypes(1, filter);
			if (FAILED(hr))
			{
				PrintError("FILE_VIDEO", hr);

				UnInit();
				return{};
			}

			break;
		}
		case FileType::FILE_AUDIO:
		{
			COMDLG_FILTERSPEC filter[] =
			{
				{ L"Audio Files (*.wav;*.flac;*.mp3;*.ogg)", L"*.wav;*.flac;*.mp3;*.ogg" }
			};

			hr = fileOpen->SetFileTypes(1, filter);
			if (FAILED(hr))
			{
				PrintError("FILE_AUDIO", hr);

				UnInit();
				return{};
			}

			break;
		}
		case FileType::FILE_MODEL:
		{
			COMDLG_FILTERSPEC filter[] =
			{
				{ L"Model Files (*.fbx;*.obj;*.gltf)", L"*.fbx;*.obj;*.gltf" }
			};

			hr = fileOpen->SetFileTypes(1, filter);
			if (FAILED(hr))
			{
				PrintError("FILE_MODEL", hr);

				UnInit();
				return{};
			}

			break;
		}
		case FileType::FILE_SHADER:
		{
			COMDLG_FILTERSPEC filter[] =
			{
				{ L"Shader Files (*.vert;*.frag;*.geom)", L"*.vert;*.frag;*.geom" }
			};

			hr = fileOpen->SetFileTypes(1, filter);
			if (FAILED(hr))
			{
				PrintError("FILE_SHADER", hr);

				UnInit();
				return{};
			}

			break;
		}
		case FileType::FILE_TEXTURE:
		{
			COMDLG_FILTERSPEC filter[] =
			{
				{ L"Image Files (*.png;*.jpg;*.jpeg)", L"*.png;*.jpg;*.jpeg" }
			};

			hr = fileOpen->SetFileTypes(1, filter);
			if (FAILED(hr))
			{
				PrintError("FILE_TEXTURE", hr);

				UnInit();
				return{};
			}

			break;
		}
		}

		if (multiple) options |= FOS_ALLOWMULTISELECT;
		fileOpen->SetOptions(options);

		hr = fileOpen->Show(nullptr);

		//user cancelled, return cleanly
		if (hr == HRESULT_FROM_WIN32(ERROR_CANCELLED))
		{
			if (Window::IsVerboseLoggingEnabled())
			{
				Log::Print(
					"User cancelled file selection.",
					"WINDOW_WINDOWS",
					LogType::LOG_INFO);
			}

			UnInit();
			return{};
		}
		//other failed reason
		if (FAILED(hr))
		{
			Log::Print(
				"Failed to show file open dialog! Reason: " + HResultToString(hr),
				"WINDOW_WINDOWS",
				LogType::LOG_ERROR,
				2);

			UnInit();
			return{};
		}

		ComPtr<IShellItemArray> items{};
		hr = fileOpen->GetResults(&items);

		if (FAILED(hr))
		{
			Log::Print(
				"Failed to retrieve selected items from file dialog! Reason: " + HResultToString(hr),
				"WINDOW_WINDOWS",
				LogType::LOG_ERROR,
				2);

			UnInit();
			return{};
		}

		DWORD count{};
		items->GetCount(&count);
		vector<string> result{};

		for (DWORD i = 0; i < count; ++i)
		{
			ComPtr<IShellItem> item{};
			hr = items->GetItemAt(i, &item);

			if (FAILED(hr))
			{
				Log::Print(
					"Failed to get item at index '" + to_string(i) + "' from file dialog! Reason: " + HResultToString(hr),
					"WINDOW_WINDOWS",
					LogType::LOG_ERROR,
					2);

				continue;
			}

			PWSTR pszFilePath{};
			hr = item->GetDisplayName(
				SIGDN_FILESYSPATH,
				&pszFilePath);

			if (FAILED(hr))
			{
				Log::Print(
					"Failed to get file path for item at index '" + to_string(i) + "' from file dialog! Reason: " + HResultToString(hr),
					"WINDOW_WINDOWS",
					LogType::LOG_ERROR,
					2);

				continue;
			}

			wstring wide(pszFilePath);
			string path = ToShort(wide);
			
			result.push_back(path);

			CoTaskMemFree(pszFilePath);

			if (Window::IsVerboseLoggingEnabled())
			{
				Log::Print(
					"Selected file '" + path + "'",
					"WINDOW_WINDOWS",
					LogType::LOG_SUCCESS);
			}
		}

		UnInit();
		return result;
	}

	void Window::CreateNotification(
		const string& title,
		const string& nessage)
	{
		wstring titleW = ToWide(title);
		wstring messageW = ToWide(nessage);

		XmlDocument toastXml = ToastNotificationManager::GetTemplateContent(
			ToastTemplateType::ToastImageAndText02);

		auto textNodes = toastXml.GetElementsByTagName(L"text");
		textNodes.Item(0).AppendChild(toastXml.CreateTextNode(titleW));
		textNodes.Item(1).AppendChild(toastXml.CreateTextNode(messageW));

		ToastNotification toast(toastXml);

		ToastNotificationManager::CreateToastNotifier(ToWide(APP_ID)).Show(toast);

		if (Window::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Created notification '" + title + "'!",
				"WINDOW_WINDOWS",
				LogType::LOG_SUCCESS);
		}
	}

	void Window::SetClipboardText(const string& text)
	{
		if (!OpenClipboard(nullptr))
		{
			Log::Print(
				"Failed to open clipboard when writing text to clipboard!",
				"WINDOW_WINDOWS",
				LogType::LOG_ERROR,
				2);

			return;
		}

		EmptyClipboard();

		wstring wstr = ToWide(text);

		HGLOBAL hGlob = GlobalAlloc(
			GMEM_MOVEABLE,
			(wstr.size() + 1) * sizeof(wchar_t));

		if (hGlob == NULL)
		{
			Log::Print(
				"Failed to construct hGlobal when saving text to clipboard!",
				"WINDOW_WINDOWS",
				LogType::LOG_ERROR,
				2);

			CloseClipboard();
			return;
		}

		void* pMem = GlobalLock(hGlob);
		if (!pMem)
		{
			Log::Print(
				"Failed to lock hGlobal when saving text to clipboard!",
				"WINDOW_WINDOWS",
				LogType::LOG_ERROR,
				2);

			GlobalFree(hGlob);
			CloseClipboard();
			return;
		}

		memcpy(pMem,
			wstr.c_str(),
			(wstr.size() + 1) * sizeof(wchar_t));
		GlobalUnlock(hGlob);

		SetClipboardData(CF_UNICODETEXT, hGlob);

		CloseClipboard();

		if (Window::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Saved string to clipboard: '" + text + "'!",
				"WINDOW_WINDOWS",
				LogType::LOG_SUCCESS);
		}
	}
	string Window::GetClipboardText()
	{
		if (!OpenClipboard(nullptr))
		{
			Log::Print(
				"Failed to open clipboard when reading text from clipboard!",
				"WINDOW_WINDOWS",
				LogType::LOG_ERROR,
				2);

			return{};
		}

		if (!IsClipboardFormatAvailable(CF_UNICODETEXT))
		{
			if (Window::IsVerboseLoggingEnabled())
			{
				Log::Print(
					"Clipboard does not contain Unicode text.",
					"WINDOW_WINDOWS",
					LogType::LOG_WARNING,
					2);
			}

			CloseClipboard();
			return{};
		}

		HANDLE hData = GetClipboardData(CF_UNICODETEXT);
		if (hData == nullptr)
		{
			if (Window::IsVerboseLoggingEnabled())
			{
				Log::Print(
					"Clipboard had no data to read from.",
					"WINDOW_WINDOWS",
					LogType::LOG_WARNING);
			}

			CloseClipboard();
			return{};
		}

		wchar_t* wstr = static_cast<wchar_t*>(GlobalLock(hData));
		if (wstr == nullptr)
		{
			Log::Print(
				"Failed to lock memory handle to get text from clipboard!",
				"WINDOW_WINDOWS",
				LogType::LOG_ERROR,
				2);

			CloseClipboard();
			return{};
		}

		wstring wideVal(wstr);
		string shortVal = ToShort(wideVal);

		GlobalUnlock(hData);
		CloseClipboard();

		if (Window::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Read string to clipboard: '" + shortVal + "'!",
				"WINDOW_WINDOWS",
				LogType::LOG_SUCCESS);
		}

		return shortVal;
	}

	void Window::SetTitle(const string& newTitle) const
	{
		HWND window = ToVar<HWND>(window_windows.hwnd);

		if (newTitle.empty())
		{
			Log::Print(
				"Window title cannot be empty!",
				"WINDOW_WINDOWS",
				LogType::LOG_ERROR,
				2);

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

		if (Window::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set window title to '" + newTitle + "'",
				"WINDOW_WINDOWS",
				LogType::LOG_SUCCESS);
		}
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
				"WINDOW_WINDOWS",
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

		if (exeIcon == nullptr)
		{
			Log::Print(
				"Cannot set window '" + GetTitle() + "' icon because SetUpIcon failed!",
				"WINDOW_WINDOWS",
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

		if (Window::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set window '" + GetTitle() + "' icon to '" + tex->GetName() + "'",
				"WINDOW_WINDOWS",
				LogType::LOG_SUCCESS);
		}
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
				"WINDOW_WINDOWS",
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

		if (overlayIcon == nullptr)
		{
			Log::Print(
				"Cannot set window '" + GetTitle() + "' overlay icon because SetUpIcon failed!",
				"WINDOW_WINDOWS",
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
				"WINDOW_WINDOWS",
				LogType::LOG_ERROR,
				2);

			return;
		}

		hr = taskbar->HrInit();

		if (FAILED(hr))
		{
			Log::Print(
				"Failed to init ITaskbarList3 to set overlay icon!",
				"WINDOW_WINDOWS",
				LogType::LOG_ERROR,
				2);

			return;
		}

		taskbar->SetOverlayIcon(
			window,
			overlayIcon,
			tooltip.empty() ? nullptr : ToWide(tooltip).c_str());

		if (Window::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set window '" + GetTitle() + "' overlay icon to '" + tex->GetName() + "'",
				"WINDOW_WINDOWS",
				LogType::LOG_SUCCESS);
		}
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

		if (FAILED(hr))
		{
			Log::Print(
				"Failed to get ITaskbarList3 to clear overlay icon!",
				"WINDOW_WINDOWS",
				LogType::LOG_ERROR,
				2);

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
				"WINDOW_WINDOWS",
				LogType::LOG_ERROR,
				2);
		}

		if (Window::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set window '" + GetTitle() + "' rounding to '" + roundingVal + "'",
				"WINDOW_WINDOWS",
				LogType::LOG_SUCCESS);
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

		if (Window::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set window '" + GetTitle() + "' client rect size to '" + val + "'",
				"WINDOW_WINDOWS",
				LogType::LOG_SUCCESS);
		}
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

		string val = to_string(newSize.x) + "x" + to_string(newSize.y);

		if (Window::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set window '" + GetTitle() + "' outer size to '" + val + "'",
				"WINDOW_WINDOWS",
				LogType::LOG_SUCCESS);
		}
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

		string val = to_string(newSize.x) + "x" + to_string(newSize.y);

		if (Window::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set window '" + GetTitle() + "' framebuffer size to '" + val + "'",
				"WINDOW_WINDOWS",
				LogType::LOG_SUCCESS);
		}
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

		string val = to_string(newPosition.x) + "x" + to_string(newPosition.y);

		if (Window::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set window '" + GetTitle() + "' position to '" + val + "'",
				"WINDOW_WINDOWS",
				LogType::LOG_SUCCESS);
		}
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

		string val = state ? "true" : "false";

		if (Window::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set window '" + GetTitle() + "' always on state to '" + val + "'",
				"WINDOW_WINDOWS",
				LogType::LOG_SUCCESS);
		}
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

		string val = state ? "true" : "false";

		if (Window::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set window '" + GetTitle() + "' resizable state to '" + val + "'",
				"WINDOW_WINDOWS",
				LogType::LOG_SUCCESS);
		}
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

		string val = state ? "true" : "false";

		if (Window::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set window '" + GetTitle() + "' fullscreen state to '" + val + "'",
				"WINDOW_WINDOWS",
				LogType::LOG_SUCCESS);
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

		string val = state ? "true" : "false";

		if (Window::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set window '" + GetTitle() + "' top bar state to '" + val + "'",
				"WINDOW_WINDOWS",
				LogType::LOG_SUCCESS);
		}
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

		string val = state ? "true" : "false";

		if (Window::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set window '" + GetTitle() + "' minimize button state to '" + val + "'",
				"WINDOW_WINDOWS",
				LogType::LOG_SUCCESS);
		}
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

		string val = state ? "true" : "false";

		if (Window::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set window '" + GetTitle() + "' maximize button state to '" + val + "'",
				"WINDOW_WINDOWS",
				LogType::LOG_SUCCESS);
		}
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

		string val = state ? "true" : "false";

		if (Window::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set window '" + GetTitle() + "' close buttpn state to '" + val + "'",
				"WINDOW_WINDOWS",
				LogType::LOG_SUCCESS);
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

		string val = state ? "true" : "false";

		if (Window::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set window '" + GetTitle() + "' system menu state to '" + val + "'",
				"WINDOW_WINDOWS",
				LogType::LOG_SUCCESS);
		}
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

		string val = to_string(alpha);

		if (Window::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set window '" + GetTitle() + "' opacity to '" + val + "'",
				"WINDOW_WINDOWS",
				LogType::LOG_SUCCESS);
		}
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

		string val{};

		switch (state)
		{
		case WindowState::WINDOW_NORMAL:
			ShowWindow(hwnd, SW_SHOWNORMAL);
			val = "normal";
			break;
		case WindowState::WINDOW_MAXIMIZE:
			ShowWindow(hwnd, SW_MAXIMIZE);
			val = "maximized";
			break;
		case WindowState::WINDOW_MINIMIZE:
			ShowWindow(hwnd, SW_MINIMIZE);
			val = "minimized";
			break;
		case WindowState::WINDOW_HIDE:
			ShowWindow(hwnd, SW_HIDE);
			val = "hidden";
			break;
		case WindowState::WINDOW_SHOWNOACTIVATE:
			ShowWindow(hwnd, SW_SHOWNOACTIVATE);
			val = "unfocused visible";
			break;
		}

		UpdateWindow(hwnd);

		if (Window::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set window '" + GetTitle() + "' state to '" + val + "'",
				"WINDOW_WINDOWS",
				LogType::LOG_SUCCESS);
		}
	}
	WindowState Window::GetWindowState() const
	{
		HWND hwnd = ToVar<HWND>(window_windows.hwnd);

		WINDOWPLACEMENT placement{};
		placement.length = sizeof(WINDOWPLACEMENT);

		if (!GetWindowPlacement(hwnd, &placement))
		{
			Log::Print(
				"Failed to get window '" + GetTitle() + "' state!",
				"WINDOW_WINDOWS",
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
		HWND hwnd = ToVar<HWND>(window_windows.hwnd);

		if (state)
		{
			WTSRegisterSessionNotification(
				hwnd,
				NOTIFY_FOR_THIS_SESSION);

			shutdownBlockState = true;
		}
		else
		{
			WTSUnRegisterSessionNotification(hwnd);
			shutdownBlockState = false;
		}

		string val = state ? "true" : "false";

		if (Window::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set window '" + GetTitle() + "' shutdown block state to '" + val + "'",
				"WINDOW_WINDOWS",
				LogType::LOG_SUCCESS);
		}
	}

	void Window::FlashTaskbar(
		TaskbarFlashMode mode,
		u32 count) const
	{
		HWND hwnd = ToVar<HWND>(window_windows.hwnd);

		if (mode == TaskbarFlashMode::FLASH_TIMED
			&& count == 0)
		{
			Log::Print(
				"Failed to flash taskbar because mode was set to 'FLASH_TIMED' but no count value was assigned!",
				"WINDOW_WINDOWS",
				LogType::LOG_ERROR,
				2);

			return;
		}

		FLASHWINFO fi{};
		fi.cbSize = sizeof(fi);
		fi.hwnd = hwnd;

		string val{};
		string dur{};

		switch (mode)
		{
		case TaskbarFlashMode::FLASH_ONCE:
			fi.dwFlags = FLASHW_ALL;
			fi.uCount = 1;

			val = "once";
			dur = "1";

			break;
		case TaskbarFlashMode::FLASH_UNTIL_FOCUS:
			fi.dwFlags = FLASHW_ALL | FLASHW_TIMERNOFG;
			fi.uCount = 0; //keep flashing until focus

			val = "until focus";
			dur = "0";

			break;
		case TaskbarFlashMode::FLASH_TIMED:
			fi.dwFlags = FLASHW_ALL;
			fi.uCount = count; //flash x times

			val = "timed";
			dur = to_string(count);

			break;
		}

		fi.dwTimeout = 0;
		FlashWindowEx(&fi);

		if (Window::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Flashed taskbar icon for window '" + GetTitle() + "' with type '" + val + "' for '" + dur + "' times",
				"WINDOW_WINDOWS",
				LogType::LOG_SUCCESS);
		}
	}

	void Window::SetTaskbarProgressBarState(
		TaskbarProgressBarMode mode,
		u8 current,
		u8 max) const
	{
		HWND hwnd = ToVar<HWND>(window_windows.hwnd);

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
				"WINDOW_WINDOWS",
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
			taskbar->SetProgressState(hwnd, TBPF_NOPROGRESS);
			val = "none";
			break;
		case TaskbarProgressBarMode::PROGRESS_INDETERMINATE:
			taskbar->SetProgressState(hwnd, TBPF_INDETERMINATE);
			val = "indeterminate";
			break;
		case TaskbarProgressBarMode::PROGRESS_NORMAL:
			taskbar->SetProgressState(hwnd, TBPF_NORMAL);
			taskbar->SetProgressValue(hwnd, currentClamped, maxClamped);
			val = "normal";
			break;
		case TaskbarProgressBarMode::PROGRESS_PAUSED:
			taskbar->SetProgressState(hwnd, TBPF_PAUSED);
			taskbar->SetProgressValue(hwnd, currentClamped, maxClamped);
			val = "paused";
			break;
		case TaskbarProgressBarMode::PROGRESS_ERROR:
			taskbar->SetProgressState(hwnd, TBPF_ERROR);
			taskbar->SetProgressValue(hwnd, currentClamped, maxClamped);
			val = "error";
			break;
		}

		if (Window::IsVerboseLoggingEnabled())
		{
			ostringstream oss{};
			oss << "Set window '" + GetTitle() + "' taskbar duration type to '"
				+ val + "', current value to '" + currVal
				+ "' and max value to " + maxVal + "'";

			Log::Print(
				oss.str(),
				"WINDOW_WINDOWS",
				LogType::LOG_SUCCESS);
		}
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
		string title = GetTitle();

		WindowData win = window_windows;
		HWND winRef = ToVar<HWND>(win.hwnd);
		SetWindowState(WindowState::WINDOW_HIDE);

		//destroy menu bar if it was created
		if (MenuBar::IsInitialized(this)) MenuBar::DestroyMenuBar(this);

		OpenGLData openGLData = GetOpenGLData();

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

		if (shutdownBlockState) WTSUnRegisterSessionNotification(winRef);

		if (win.hwnd)
		{
			DestroyWindow(winRef);
			win.hwnd = NULL;
		}
		win.hInstance = NULL;

		Log::Print(
			"Destroyed window '" + title + "'!",
			"WINDOW_WINDOWS",
			LogType::LOG_SUCCESS);
	}

	//
	// MENU BAR CLASS DEFINITIONS
	//

	void MenuBar::CreateMenuBar(Window* windowRef)
	{
		HWND window = ToVar<HWND>(windowRef->GetWindowData().hwnd);
		WindowData wData = windowRef->GetWindowData();

		if (IsInitialized(windowRef))
		{
			Log::Print(
				"Failed to add menu bar to window '" + windowRef->GetTitle() + "' because the window already has one!",
				"MENU_BAR",
				LogType::LOG_ERROR,
				2);

			return;
		}

		HMENU hMenu = CreateMenu();
		wData.hMenu = FromVar(hMenu);
		windowRef->SetWindowData(wData);

		SetMenu(window, hMenu);
		DrawMenuBar(window);

		isEnabled = true;

		ostringstream oss{};
		oss << "Created new menu bar in window '" << windowRef->GetTitle() << "'!";

		Log::Print(
			oss.str(),
			"MENU_BAR",
			LogType::LOG_SUCCESS);
	}
	bool MenuBar::IsInitialized(Window* windowRef)
	{
		return (windowRef->GetWindowData().hMenu != NULL);
	}

	void MenuBar::SetMenuBarState(
		bool state,
		Window* windowRef)
	{
		HWND hwnd = ToVar<HWND>(windowRef->GetWindowData().hwnd);
		HMENU storedHMenu = ToVar<HMENU>(windowRef->GetWindowData().hMenu);

		if (!IsInitialized(windowRef))
		{
			Log::Print(
				"Failed to set menu bar state for window '" + windowRef->GetTitle() + "' because it has not yet created a menu bar!",
				"MENU_BAR",
				LogType::LOG_ERROR,
				2);

			return;
		}

		SetMenu(hwnd, state ? storedHMenu : nullptr);
		DrawMenuBar(hwnd);

		string val = state ? "true" : "false";
		isEnabled = state;

		if (MenuBar::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set window '" + windowRef->GetTitle() + "' menu bar state to '" + val + "'",
				"MENU_BAR",
				LogType::LOG_SUCCESS);
		}
	}
	bool MenuBar::IsEnabled(Window* window)
	{
		HWND hwnd = ToVar<HWND>(window->GetWindowData().hwnd);
		HMENU attached = GetMenu(hwnd);

		return 
			attached != NULL
			&& window->GetWindowData().hMenu != NULL
			&& isEnabled;
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

		if (!IsInitialized(windowRef))
		{
			ostringstream oss{};
			oss << "Failed to add " << typeName << " to window '" << windowRef->GetTitle() << "' because no menu bar was created!";

			Log::Print(
				oss.str(),
				"MENU_BAR",
				LogType::LOG_ERROR,
				2);

			return;
		}

		if (labelRef.empty())
		{
			ostringstream oss{};
			oss << "Failed to add " << typeName << " to window '" << windowRef->GetTitle() << "' because the label name is empty!";

			Log::Print(
				oss.str(),
				"MENU_BAR",
				LogType::LOG_ERROR,
				2);

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
				"MENU_BAR",
				LogType::LOG_ERROR,
				2);

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
				"MENU_BAR",
				LogType::LOG_ERROR,
				2);

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
						"MENU_BAR",
						LogType::LOG_ERROR,
						2);

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
					"MENU_BAR",
					LogType::LOG_ERROR,
					2);

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
					"MENU_BAR",
					LogType::LOG_ERROR,
					2);

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

				if (MenuBar::IsVerboseLoggingEnabled())
				{
					ostringstream oss{};
					oss << "Added " << typeName << " '" << labelRef << "' with ID '" << to_string(newID)
						<< "' under parent '" << parentName
						<< "' in window '" << windowRef->GetTitle() << "'!";

					Log::Print(
						oss.str(),
						"MENU_BAR",
						LogType::LOG_SUCCESS);
				}
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
					"MENU_BAR",
					LogType::LOG_ERROR,
					2);

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

		if (!IsInitialized(windowRef))
		{
			ostringstream oss{};
			oss << "Failed to add separator to menu label '" << labelRef << "' in window '" << windowRef->GetTitle()
				<< "' because it has no menu bar!";

			Log::Print(
				oss.str(),
				"MENU_BAR",
				LogType::LOG_ERROR,
				2);

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
							"MENU_BAR",
							LogType::LOG_ERROR,
							2);

						return;
					}

					AppendMenu(
						parentMenu,
						MF_SEPARATOR,
						0,
						nullptr);

					if (MenuBar::IsVerboseLoggingEnabled())
					{
						Log::Print(
							"Placed separator to the end of parent label '" + parentRef + "' in window '" + windowRef->GetTitle() + "'!",
							"MENU_BAR",
							LogType::LOG_SUCCESS);
					}

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
							"MENU_BAR",
							LogType::LOG_ERROR,
							2);

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

							if (MenuBar::IsVerboseLoggingEnabled())
							{
								Log::Print(
									"Placed separator after label '" + labelRef + "' in window '" + windowRef->GetTitle() + "'!",
									"MENU_BAR",
									LogType::LOG_SUCCESS);
							}

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
			"MENU_BAR",
			LogType::LOG_ERROR,
			2);
	}

	void MenuBar::DestroyMenuBar(Window* windowRef)
	{
		HWND window = ToVar<HWND>(windowRef->GetWindowData().hwnd);

		if (!IsInitialized(windowRef))
		{
			Log::Print(
				"Cannot destroy menu bar for window '" + windowRef->GetTitle() + "' because it hasn't created one!",
				"MENU_BAR",
				LogType::LOG_ERROR,
				2);

			return;
		}

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
			"MENU_BAR",
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

string HResultToString(HRESULT hr)
{
	LPWSTR buffer{};

	DWORD len = FormatMessageW(
		FORMAT_MESSAGE_ALLOCATE_BUFFER
		| FORMAT_MESSAGE_FROM_SYSTEM
		| FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr,
		static_cast<DWORD>(hr),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPWSTR>(&buffer),
		0,
		nullptr);

	string result{};

	char tmp[32]{};
	sprintf_s(tmp, "0x%08X", static_cast<unsigned int>(hr));
	string fmtHex = tmp;

	if (len
		&& buffer)
	{
		result = ToShort(buffer);
		LocalFree(buffer);

		//trim trailing CR/LF
		if (!result.empty()
			&& (result.back() == '\n'
			|| result.back() == '\r'))
		{
			result.erase(result.find_last_not_of("\r\n") + 1);
		}

		result += " (" + fmtHex + ")";
	}
	else result = fmtHex;

	return result;
}

#endif //_WIN32