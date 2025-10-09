//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef _WIN32

#include <windows.h>
#include <objbase.h>
#pragma comment(lib, "Ole32.lib")
#include <ShObjIdl.h>
#include <wrl/client.h>
#include <winrt/windows.ui.notifications.h>
#include <winrt/windows.data.xml.dom.h>
#pragma comment(lib, "runtimeobject.lib")
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

#include <filesystem>

#include "KalaHeaders/log_utils.hpp"

#include "graphics/window_global.hpp"
#include "graphics/window.hpp"
#include "core/core.hpp"
#include "windows/messageloop.hpp"

using namespace KalaHeaders;

using KalaWindow::Core::KalaWindowCore;
using KalaWindow::Windows::MessageLoop;

using std::to_string;
using std::wstring;
using Microsoft::WRL::ComPtr;
using namespace winrt::Windows::UI::Notifications;
using namespace winrt::Windows::Data::Xml::Dom;
using std::filesystem::path;

static wstring ToWide(const string& str);
static string ToShort(const wstring& str);
static string HResultToString(HRESULT hr);

static bool checkedOSVersion = false;
constexpr u32 MIN_OS_VERSION = 10017763; //Windows 10 build 17763 (1809)

static bool enabledBeginPeriod = false;

namespace KalaWindow::Graphics
{
	void Window_Global::Initialize()
	{
		if (isInitialized)
		{
			Log::Print(
				"Cannot initialize global window context because it has already been initialized!",
				"WINDOW_GLOBAL",
				LogType::LOG_ERROR,
				2);

			return;
		}

		version = GetVersion();
		string versionStr = to_string(version);
		string osVersion = versionStr.substr(0, 2);
		string buildVersion = to_string(stoi(versionStr.substr(2)));

		if (version < MIN_OS_VERSION)
		{
			ostringstream oss{};
			oss << "Your version is Windows '" + osVersion + "' build '" << buildVersion
				<< "' but KalaWindow requires Windows '10' (1809 build '17763') or higher!";

			KalaWindowCore::ForceClose(
				"Global window error",
				oss.str());

			return;
		}

		if (isVerboseLoggingEnabled)
		{
			Log::Print(
				"Windows version '" + osVersion + "' build '" + buildVersion + "'",
				"WINDOW_GLOBAL",
				LogType::LOG_INFO);
		}

		wchar_t buffer[MAX_PATH]{};
		GetModuleFileNameW(
			nullptr,
			buffer,
			MAX_PATH);

		path exePath(buffer);

		appID = exePath.stem().string();

		//Treat this process as a real app with a stable identity
		SetCurrentProcessExplicitAppUserModelID(ToWide(appID).c_str());

		if (!enabledBeginPeriod)
		{
			timeBeginPeriod(1);
			enabledBeginPeriod = true;
		}

		wstring appIDWide = ToWide(appID);

		WNDCLASSW wc = {};
		wc.style =
			CS_OWNDC      //own the DC for the lifetime of this window
			| CS_DBLCLKS; //allow detecting double clicks
		wc.lpfnWndProc = MessageLoop::WindowProcCallback;
		wc.hInstance = GetModuleHandle(nullptr);
		wc.lpszClassName = appIDWide.c_str();

		if (!RegisterClassW(&wc))
		{
			DWORD err = GetLastError();
			string message{};
			if (err == ERROR_CLASS_ALREADY_EXISTS)
			{
				message = "Window class already exists with different definition.\n";
			}
			else
			{
				message = "RegisterClassW failed with error: " + to_string(err) + "\n";
			}

			KalaWindowCore::ForceClose(
				"Global window error",
				message);

			return;
		}

		isInitialized = true;

		Log::Print(
			"Initialized global window context!",
			"WINDOW_GLOBAL",
			LogType::LOG_SUCCESS);
	}

	u32 Window_Global::GetVersion()
	{
#ifdef WIN32
		if (version == 0)
		{
			typedef LONG(WINAPI* RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);

			HMODULE hMod = GetModuleHandleW(L"ntdll.dll");
			if (!hMod)
			{
				KalaWindowCore::ForceClose(
					"Global window error",
					"Failed to get 'ntdll.dll'");

				return 0;
			}

			auto pRtlGetVersion = (RtlGetVersionPtr)GetProcAddress(hMod, "RtlGetVersion");
			if (!pRtlGetVersion)
			{
				KalaWindowCore::ForceClose(
					"Global window error",
					"Failed to resolve address of 'RtlGetVersion'");

				return 0;
			}

			RTL_OSVERSIONINFOW rovi = { sizeof(rovi) };
			if (pRtlGetVersion(&rovi) != 0)
			{
				KalaWindowCore::ForceClose(
					"Global window error",
					"Call to 'RtlGetVersion' failed");

				return 0;
			}

			u32 major = rovi.dwMajorVersion;
			u32 build = rovi.dwBuildNumber;

			//Windows 11 reports as 10.0  but build >= 22000
			if (major == 10
				&& build >= 22000)
			{
				major = 11;
			}

			version = major * 1000000 + build;
			return version;
		}

		return version;
#elif __linux__
		return 0;
#endif
		return 0;
	}

	PopupResult Window_Global::CreatePopup(
		const string& title,
		const string& message,
		PopupAction action,
		PopupType type)
	{
		int flags = 0;

#ifdef _WIN32
		switch (action)
		{
		case PopupAction::POPUP_ACTION_OK:            flags |= MB_OK; break;
		case PopupAction::POPUP_ACTION_OK_CANCEL:     flags |= MB_OKCANCEL; break;
		case PopupAction::POPUP_ACTION_YES_NO:        flags |= MB_YESNO; break;
		case PopupAction::POPUP_ACTION_YES_NO_CANCEL: flags |= MB_YESNOCANCEL; break;
		case PopupAction::POPUP_ACTION_RETRY_CANCEL:  flags |= MB_RETRYCANCEL; break;
		default:                                      flags |= MB_OK; break;
		}

		switch (type)
		{
		case PopupType::POPUP_TYPE_INFO:     flags |= MB_ICONINFORMATION; break;
		case PopupType::POPUP_TYPE_WARNING:  flags |= MB_ICONWARNING; break;
		case PopupType::POPUP_TYPE_ERROR:    flags |= MB_ICONERROR; break;
		case PopupType::POPUP_TYPE_QUESTION: flags |= MB_ICONQUESTION; break;
		default:                             flags |= MB_ICONINFORMATION; break;
		}
#else
		//TODO: ADD LINUX EQUIVALENT
#endif

		wstring messageWide = ToWide(message);
		wstring titleWide = ToWide(title);

		int result = MessageBoxW(
			nullptr,
			messageWide.c_str(),
			titleWide.c_str(),
			flags);

		switch (result)
		{
		case IDOK:     return PopupResult::POPUP_RESULT_OK;
		case IDCANCEL: return PopupResult::POPUP_RESULT_CANCEL;
		case IDYES:    return PopupResult::POPUP_RESULT_YES;
		case IDNO:     return PopupResult::POPUP_RESULT_NO;
		case IDRETRY:  return PopupResult::POPUP_RESULT_RETRY;
		default:       return PopupResult::POPUP_RESULT_NONE;
		}
	}

	vector<string> Window_Global::GetFile(
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
				"WINDOW_GLOBAL",
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
						"WINDOW_GLOBAL",
						LogType::LOG_DEBUG);

					CoUninitialize();
				}
				else
				{
					Log::Print(
						"Skipping CoUninitialize()",
						"WINDOW_GLOBAL",
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
				"WINDOW_GLOBAL",
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
					"WINDOW_GLOBAL",
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
			if (isVerboseLoggingEnabled)
			{
				Log::Print(
					"User cancelled file selection.",
					"WINDOW_GLOBAL",
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
				"WINDOW_GLOBAL",
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
				"WINDOW_GLOBAL",
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
					"WINDOW_GLOBAL",
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
					"WINDOW_GLOBAL",
					LogType::LOG_ERROR,
					2);

				continue;
			}

			wstring wide(pszFilePath);
			string path = ToShort(wide);

			result.push_back(path);

			CoTaskMemFree(pszFilePath);

			if (isVerboseLoggingEnabled)
			{
				Log::Print(
					"Selected file '" + path + "'",
					"WINDOW_GLOBAL",
					LogType::LOG_SUCCESS);
			}
		}

		UnInit();
		return result;
	}

	void Window_Global::CreateNotification(
		const string& title,
		const string& message)
	{
		wstring titleW = ToWide(title);
		wstring messageW = ToWide(message);

		XmlDocument toastXml = ToastNotificationManager::GetTemplateContent(
			ToastTemplateType::ToastImageAndText02);

		auto textNodes = toastXml.GetElementsByTagName(L"text");
		textNodes.Item(0).AppendChild(toastXml.CreateTextNode(titleW));
		textNodes.Item(1).AppendChild(toastXml.CreateTextNode(messageW));

		ToastNotification toast(toastXml);

		ToastNotificationManager::CreateToastNotifier(ToWide(appID)).Show(toast);

		if (isVerboseLoggingEnabled)
		{
			Log::Print(
				"Created notification '" + title + "'!",
				"WINDOW_GLOBAL",
				LogType::LOG_SUCCESS);
		}
	}

	void Window_Global::SetClipboardText(const string& text)
	{
		if (!OpenClipboard(nullptr))
		{
			Log::Print(
				"Failed to open clipboard when writing text to clipboard!",
				"WINDOW_GLOBAL",
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
				"WINDOW_GLOBAL",
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
				"WINDOW_GLOBAL",
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

		if (isVerboseLoggingEnabled)
		{
			Log::Print(
				"Saved string to clipboard: '" + text + "'!",
				"WINDOW_GLOBAL",
				LogType::LOG_SUCCESS);
		}
	}
	string Window_Global::GetClipboardText()
	{
		if (!OpenClipboard(nullptr))
		{
			Log::Print(
				"Failed to open clipboard when reading text from clipboard!",
				"WINDOW_GLOBAL",
				LogType::LOG_ERROR,
				2);

			return{};
		}

		if (!IsClipboardFormatAvailable(CF_UNICODETEXT))
		{
			if (isVerboseLoggingEnabled)
			{
				Log::Print(
					"Clipboard does not contain Unicode text.",
					"WINDOW_GLOBAL",
					LogType::LOG_WARNING,
					2);
			}

			CloseClipboard();
			return{};
		}

		HANDLE hData = GetClipboardData(CF_UNICODETEXT);
		if (!hData)
		{
			if (isVerboseLoggingEnabled)
			{
				Log::Print(
					"Clipboard had no data to read from.",
					"WINDOW_GLOBAL",
					LogType::LOG_WARNING);
			}

			CloseClipboard();
			return{};
		}

		wchar_t* wstr = static_cast<wchar_t*>(GlobalLock(hData));
		if (!wstr)
		{
			Log::Print(
				"Failed to lock memory handle to get text from clipboard!",
				"WINDOW_GLOBAL",
				LogType::LOG_ERROR,
				2);

			CloseClipboard();
			return{};
		}

		wstring wideVal(wstr);
		string shortVal = ToShort(wideVal);

		GlobalUnlock(hData);
		CloseClipboard();

		if (isVerboseLoggingEnabled)
		{
			Log::Print(
				"Read string to clipboard: '" + shortVal + "'!",
				"WINDOW_GLOBAL",
				LogType::LOG_SUCCESS);
		}

		return shortVal;
	}
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