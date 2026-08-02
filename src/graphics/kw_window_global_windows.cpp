//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef _WIN32

#include <windows.h>
#include <objbase.h>
#include <shobjidl.h>
#include <mmsystem.h>
#include <shellapi.h>

//CComPtr rewritten to work on both Windows and when compiling for Windows on Linux
template<typename T>
class CComPtr
{
	T* ptr = nullptr;
public:
	CComPtr() = default;
	explicit CComPtr(T* p) noexcept : ptr(p) {}
	~CComPtr() { if (ptr) ptr->Release(); }

	CComPtr(const CComPtr&) = delete;
	CComPtr& operator=(const CComPtr&) = delete;
	CComPtr(CComPtr&& o) noexcept : ptr(o.ptr) { o.ptr = nullptr; }
	CComPtr& operator=(CComPtr&& o) noexcept
	{
		if (this != &o)
		{
			if (ptr) ptr->Release();
			ptr = o.ptr;
			o.ptr = nullptr;
		}
		return *this;
	}

	void Reset(T* p = nullptr) noexcept
	{
		if (ptr) ptr->Release();
		ptr = p;
	}

	T** operator&()
	{
		if (ptr)
		{ 
			ptr->Release();
			ptr = nullptr;
		}
		return &ptr;
	}
	operator T*() const { return ptr; }
	T* operator->() const { return ptr; }
	T* Get() const { return ptr; }
	explicit operator bool() const { return ptr != nullptr; }
};

#include <filesystem>
#include <sstream>
#include <string>
#include <system_error>

#include "log_utils.hpp"

#include "graphics/kw_window_global.hpp"
#include "graphics/kw_window.hpp"
#include "core/kw_core.hpp"
#include "core/kw_messageloop_windows.hpp"

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;

using KalaWindow::Core::KalaWindowCore;
using KalaWindow::Core::MessageLoop;

using std::wstring;

using std::filesystem::path;
using std::ostringstream;
using std::string;
using std::string_view;
using std::to_string;
using std::error_code;

static wstring ToWide(string_view str);
static string ToShort(const wstring& str);
static string HResultToString(HRESULT hr);

constexpr u32 MIN_OS_VERSION = 10017763; //Windows 10 build 17763 (1809)

static bool enabledBeginPeriod = false;

namespace KalaWindow::Graphics
{
	static bool isInitialized{};
	static bool isVerboseLoggingEnabled{};

	static u32 version{};
	static string appID{};

	bool Window_Global::IsVerboseLoggingEnabled() { return isVerboseLoggingEnabled; }
	void Window_Global::SetVerboseLoggingState(bool newState) { isVerboseLoggingEnabled = newState; }

	void Window_Global::Initialize()
	{
		if (isInitialized)
		{
			Log::Print(
				"Failed to initialize global window context because it has already been initialized!",
				"KW_WINDOW_GLOBAL",
				LogType::LOG_ERROR,
				2);

			return;
		}

		typedef LONG (WINAPI *RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);

		RTL_OSVERSIONINFOW rovi{};
		rovi.dwOSVersionInfoSize = sizeof(rovi);

		HMODULE hMod = GetModuleHandleW(L"ntdll.dll");
		if (!hMod)
		{
			KalaWindowCore::ForceClose(
				"KalaWindow global window error",
				"Failed to check OS version because hMod was invalid!");
		}

		auto pRtlGetVersion = rcast<RtlGetVersionPtr>(
			GetProcAddress(hMod, "RtlGetVersion"));
		
		if (!pRtlGetVersion
			|| pRtlGetVersion(&rovi) != 0)
		{
			KalaWindowCore::ForceClose(
				"KalaWindow global window error",
				"Failed to check OS version because pRtlGetVersion was invalid or failed!");
		}

		u32 realVersion = rovi.dwMajorVersion * 1000000 + rovi.dwBuildNumber;

		string versionStr = to_string(realVersion);
		string osVersion = versionStr.substr(0, 2);
		string buildVersion = to_string(stoi(versionStr.substr(2)));

		if (realVersion < MIN_OS_VERSION)
		{
			ostringstream oss{};
			oss << "Your version is Windows '" + osVersion + "' build '" << buildVersion
				<< "' but KalaWindow requires Windows '10' (1809 build '17763') or higher!";

			KalaWindowCore::ForceClose(
				"KalaWindow global window error",
				oss.str());
		}

		if (isVerboseLoggingEnabled)
		{
			Log::Print(
				"Windows version '" + osVersion + "' build '" + buildVersion + "'",
				"KW_WINDOW_GLOBAL",
				LogType::LOG_VERBOSE);
		}

		path exePath = KalaWindowCore::GetExePath();
		appID = exePath.stem().string();

		//Treat this process as a real app with a stable identity
		SetCurrentProcessExplicitAppUserModelID(ToWide(appID).c_str());

		//TODO: figure out if this is even needed at all anywhere
		/*
		if (!enabledBeginPeriod)
		{
			timeBeginPeriod(1);
			enabledBeginPeriod = true;
		}
		*/

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
				"KalaWindow global window error",
				string(message));
		}

		isInitialized = true;

		Log::Print(
			"Initialized global window context!",
			"KW_WINDOW_GLOBAL",
			LogType::LOG_SUCCESS);
	}

	bool Window_Global::IsInitialized() { return isInitialized; }

	u32 Window_Global::GetVersion()
	{
		if (version == 0)
		{
			typedef LONG(WINAPI* RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);

			HMODULE hMod = GetModuleHandleW(L"ntdll.dll");
			if (!hMod)
			{
				KalaWindowCore::ForceClose(
					"KalaWindow global window error",
					"Failed to get 'ntdll.dll'");

				return 0;
			}

			auto pRtlGetVersion = (RtlGetVersionPtr)GetProcAddress(hMod, "RtlGetVersion");
			if (!pRtlGetVersion)
			{
				KalaWindowCore::ForceClose(
					"KalaWindow global window error",
					"Failed to resolve address of 'RtlGetVersion'");

				return 0;
			}

			RTL_OSVERSIONINFOW rovi = { sizeof(rovi) };
			if (pRtlGetVersion(&rovi) != 0)
			{
				KalaWindowCore::ForceClose(
					"KalaWindow global window error",
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
	}

	string_view Window_Global::GetAppID() { return appID; }

	PopupResult Window_Global::CreatePopup(
		string&& title,
		string&& message,
		PopupAction action,
		PopupType type)
	{
		int flags = 0;

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

		string finalTitle = title.empty() ? "NO TITLE" : std::move(title);
		string finalMessage = message.empty() ? "NO MESSAGE" : std::move(message);

		int result = MessageBoxW(
			nullptr,
			ToWide(finalMessage).c_str(),
			ToWide(finalTitle).c_str(),
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

	vector<path> Window_Global::GetFiles(
        FileType type,
        vector<string>&& customTypes,
		path&& requiredRoot,
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
				"KW_WINDOW_GLOBAL",
				LogType::LOG_ERROR,
				2);

			return {};
		}

		bool canUninit = (hr == S_OK);

		auto UnInit = [canUninit]()
			{
				if (canUninit)
				{
					/*
					Log::Print(
						"Calling CoUninitialize",
						"KW_WINDOW_GLOBAL",
						LogType::LOG_DEBUG);
					*/

					CoUninitialize();
				}
				else
				{
					/*
					Log::Print(
						"Skipping CoUninitialize()",
						"KW_WINDOW_GLOBAL",
						LogType::LOG_DEBUG);
					*/
				}
			};

		CComPtr<IFileOpenDialog> fileOpen{};
		hr = CoCreateInstance(
			CLSID_FileOpenDialog,
			nullptr,
			CLSCTX_ALL,
			IID_PPV_ARGS(&fileOpen));

		if (FAILED(hr))
		{
			Log::Print(
				"Failed to create file open dialog! Reason: " + HResultToString(hr),
				"KW_WINDOW_GLOBAL",
				LogType::LOG_ERROR,
				2);

			UnInit();
			return {};
		}

		DWORD options{};
		fileOpen->GetOptions(&options);

		auto PrintError = [](
			const string& typeVal,
			HRESULT hr)
			{
				Log::Print(
					"Failed to set file filter to '" + typeVal + "' for dialog! Reason: " + HResultToString(hr),
					"KW_WINDOW_GLOBAL",
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
				{ L"All Files", L"*.*" }
			};

			hr = fileOpen->SetFileTypes(1, filter);
			if (FAILED(hr))
			{
				PrintError("FILE_ANY", hr);

				UnInit();
				return {};
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
				{ L"Executables", L"*.exe" }
			};

			hr = fileOpen->SetFileTypes(1, filter);
			if (FAILED(hr))
			{
				PrintError("FILE_EXE", hr);

				UnInit();
				return {};
			}

			break;
		}
		case FileType::FILE_CUSTOM:
		{
            if (customTypes.empty())
            {
                Log::Print(
                    "Failed to get files because FILE_CUSTOM was selected but no types were passed!",
                    "KW_WINDOW_GLOBAL",
                    LogType::LOG_ERROR,
                    2);

				UnInit();
                return {};
            }

			string targetTypes{};
            for (const auto& t : customTypes)
            {
                if (!t.starts_with("*.")) targetTypes += "*." + t + ";";
                else targetTypes += t + ";";
            }
            targetTypes.pop_back();
			wstring wTargetTypes = ToWide(targetTypes);

			COMDLG_FILTERSPEC filter[] =
			{
				{ L"Target Files", wTargetTypes.c_str() }
			};

			hr = fileOpen->SetFileTypes(1, filter);
			if (FAILED(hr))
			{
				PrintError("FILE_CUSTOM", hr);

				UnInit();
				return {};
			}

			break;
		}
		}

		if (!requiredRoot.empty())
		{
			CComPtr<IShellItem> rootItem{};
			hr = SHCreateItemFromParsingName(
				requiredRoot.c_str(),
				nullptr,
				IID_PPV_ARGS(&rootItem));

			if (SUCCEEDED(hr)) fileOpen->SetDefaultFolder(rootItem);
			else
			{
				Log::Print(
					"Failed to set default folder to '" + requiredRoot.string() + "'! Reason: " + HResultToString(hr),
					"KW_WINDOW_GLOBAL",
					LogType::LOG_WARNING);
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
					"KW_WINDOW_GLOBAL",
					LogType::LOG_VERBOSE);
			}

			UnInit();
			return{};
		}
		//other failed reason
		if (FAILED(hr))
		{
			Log::Print(
				"Failed to show file open dialog! Reason: " + HResultToString(hr),
				"KW_WINDOW_GLOBAL",
				LogType::LOG_ERROR,
				2);

			UnInit();
			return{};
		}

		CComPtr<IShellItemArray> items{};
		hr = fileOpen->GetResults(&items);

		if (FAILED(hr))
		{
			Log::Print(
				"Failed to retrieve selected items from file dialog! Reason: " + HResultToString(hr),
				"KW_WINDOW_GLOBAL",
				LogType::LOG_ERROR,
				2);

			UnInit();
			return{};
		}

		DWORD count{};
		items->GetCount(&count);
		vector<string> stringResult{};

		for (DWORD i = 0; i < count; ++i)
		{
			CComPtr<IShellItem> item{};
			hr = items->GetItemAt(i, &item);

			if (FAILED(hr))
			{
				Log::Print(
					"Failed to get item at index '" + to_string(i) + "' from file dialog! Reason: " + HResultToString(hr),
					"KW_WINDOW_GLOBAL",
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
					"KW_WINDOW_GLOBAL",
					LogType::LOG_ERROR,
					2);

				continue;
			}

			wstring wide(pszFilePath);
			string path = ToShort(wide);

			stringResult.push_back(path);

			CoTaskMemFree(pszFilePath);
		}

		UnInit();

        vector<path> result{};
        result.reserve(stringResult.size());
        for (auto& s : stringResult)
        {
            if (!requiredRoot.empty())
            {
                error_code ec;
                auto rel = relative(s, requiredRoot, ec);
                if (ec 
                    || rel.empty()
                    || rel.native().starts_with(L".."))
                {
                    Log::Print(
                        "Discarded file or directory '" + s 
                        + "' because it was not within the required root!",
                        "KW_WINDOW_GLOBAL",
                        LogType::LOG_WARNING);

                    continue;
                }
            }

            result.emplace_back(std::move(s));
        }

        return result;
	}

	void Window_Global::CreateNotification(
		string&& title,
		string&& message)
	{
		wstring titleW = ToWide(title);
		wstring messageW = ToWide(message);

		NOTIFYICONDATAW nid{};
		nid.cbSize = sizeof(nid);
		nid.hWnd = nullptr;
		nid.uID = 1;
		nid.uFlags = 
			NIF_ICON
			| NIF_TIP
			| NIF_INFO;
		nid.hIcon = LoadIconW(nullptr, IDI_APPLICATION);
		nid.dwInfoFlags = NIIF_INFO;

		wcsncpy(
			nid.szInfoTitle,
			titleW.c_str(),
			_countof(nid.szInfoTitle) - 1);
		nid.szInfoTitle[_countof(nid.szInfoTitle) - 1] = L'\0';

		wcsncpy(
			nid.szInfo,
			messageW.c_str(),
			_countof(nid.szInfo) - 1);
		nid.szInfo[_countof(nid.szInfo) - 1] = L'\0';

		Shell_NotifyIconW(NIM_ADD, &nid);

		if (isVerboseLoggingEnabled)
		{
			Log::Print(
				"Created notification '" + title + "'!",
				"KW_WINDOW_GLOBAL",
				LogType::LOG_VERBOSE);
		}
	}

	void Window_Global::PlaySystemSound(SoundType type)
	{
		string soundType{};

		switch (type)
		{
		case SoundType::SOUND_OK:
			MessageBeep(MB_OK);
			soundType = "ok";
			break;
		case SoundType::SOUND_ERROR:
			MessageBeep(MB_ICONHAND);
			soundType = "error";
			break;
		}

		if (isVerboseLoggingEnabled)
		{
			Log::Print(
				"Played sound type '" + soundType + "'!",
				"KW_WINDOW_GLOBAL",
				LogType::LOG_VERBOSE);
		}
	}
}

wstring ToWide(string_view input)
{
	if (input.empty()) return wstring();

	int size_needed = MultiByteToWideChar(
		CP_UTF8,
		0,
		input.data(),
		scast<int>(input.size()),
		nullptr,
		0);

	wstring wstr(size_needed, 0);

	MultiByteToWideChar(
		CP_UTF8,
		0,
		input.data(),
		scast<int>(input.size()),
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
		str.data(),
		scast<int>(str.size()),
		nullptr,
		0,
		nullptr,
		nullptr);

	string result(size_needed, 0);

	WideCharToMultiByte(
		CP_UTF8,
		0,
		str.data(),
		scast<int>(str.size()),
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
		scast<DWORD>(hr),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		rcast<LPWSTR>(&buffer),
		0,
		nullptr);

	string result{};

	char tmp[32]{};
	sprintf_s(tmp, "0x%08X", scast<unsigned int>(hr));
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