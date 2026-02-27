//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef __linux__
#if defined(KW_USE_X11)

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <sys/wait.h>

#include <string>

#include "KalaHeaders/core_utils.hpp"
#include "KalaHeaders/log_utils.hpp"

#include "graphics/kw_window_global.hpp"
#include "core/kw_core.hpp"

using KalaHeaders::KalaCore::FromVar;

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;

using KalaWindow::Core::KalaWindowCore;

using std::string;
using std::to_string;

static int ErrorHandler(
    Display* display,
    XErrorEvent* error)
{
    char buffer[512]{};
    XGetErrorText(
        display,
        error->error_code,
        buffer,
        sizeof(buffer));

    Log::Print(
        "X11 Error: " + to_string(error->error_code) + "\n"
            + "request: " + to_string(error->request_code) + "\n"
            + "minor: " + to_string(error->minor_code) + "\n"
            + "reason: " + buffer,
        "WINDOW_GLOBAL",
        LogType::LOG_ERROR,
        2);

    return 0; //tells X to continue
}

static int IOErrorHandler(Display* display)
{
    Log::Print(
        "Fatal X11 IO error!",
        "WINDOW_GLOBAL",
        LogType::LOG_ERROR,
        2);

    return 0; //tells X to exit
}

namespace KalaWindow::Graphics
{
	static bool isInitialized{};
	static bool isVerboseLoggingEnabled{};

    static X11GlobalData globalData{};

    void Window_Global::SetVerboseLoggingState(bool newState) { isVerboseLoggingEnabled = newState; }
	bool Window_Global::IsVerboseLoggingEnabled() { return isVerboseLoggingEnabled; }

    bool Window_Global::Initialize()
    {
        if (isInitialized)
		{
			Log::Print(
				"Cannot initialize global window context because it has already been initialized!",
				"WINDOW_GLOBAL",
				LogType::LOG_ERROR,
				2);

			return false;
		}

        Display* display = XOpenDisplay(nullptr);
        if (!display)
        {
            KalaWindowCore::ForceClose(
                "X11 init error",
                "Failed to open X display, ensure X server is running.");

            return false;
        }

        XSetErrorHandler(ErrorHandler);
        XSetIOErrorHandler(IOErrorHandler);

        Window root = DefaultRootWindow(display);

        Atom utf8 = XInternAtom(
            display, 
            "UTF8_STRING", 
            False);

        Atom net_wm_name = XInternAtom(
            display, 
            "_NET_WM_NAME", 
            False);

        Atom net_wm_pid = XInternAtom(
            display,
            "_NET_WM_PID",
            False);

        Atom wmDelete = XInternAtom(
            display,
            "WM_DELETE_WINDOW",
            False);

        globalData.display = FromVar(display);
        globalData.window_root = FromVar(root);

        globalData.atom_utf8        = FromVar(utf8);
        globalData.atom_net_wm_name = FromVar(net_wm_name);
        globalData.atom_net_wm_pid  = FromVar(net_wm_pid);
        globalData.atom_wmDelete    = FromVar(wmDelete);

        isInitialized = true;

		Log::Print(
			"Initialized global window context!",
			"WINDOW_GLOBAL",
			LogType::LOG_SUCCESS);

        return true;
    }

    bool Window_Global::IsInitialized() { return isInitialized; }

    PopupResult Window_Global::CreatePopup(
		string_view title,
		string_view message,
		PopupAction action,
		PopupType type) 
    { 
        vector<string> args{};
        args.emplace_back("zenity");

        switch (type)
        {
            default:
            case PopupType::POPUP_TYPE_INFO:     args.emplace_back("--info");     break;
            case PopupType::POPUP_TYPE_WARNING:  args.emplace_back("--warning");  break;
            case PopupType::POPUP_TYPE_ERROR:    args.emplace_back("--error");    break;
            case PopupType::POPUP_TYPE_QUESTION: args.emplace_back("--question"); break;
        }

        args.emplace_back("--title=" + string(title));
        args.emplace_back("--text=" + string(message));

        switch (action)
        {
            default:
            case PopupAction::POPUP_ACTION_OK: break;

            case PopupAction::POPUP_ACTION_OK_CANCEL:
            case PopupAction::POPUP_ACTION_RETRY_CANCEL:
                args.emplace_back("--ok-cancel=OK");
                args.emplace_back("--cancel-label=Cancel");
                break;

            case PopupAction::POPUP_ACTION_YES_NO:
            case PopupAction::POPUP_ACTION_YES_NO_CANCEL:
                args.emplace_back("--ok-cancel=Yes");
                args.emplace_back("--cancel-label=No");
                break;
        }

        vector<char*> execArgs{};
        for (auto& s : args) execArgs.push_back(s.data());
        execArgs.push_back(nullptr);

        pid_t pid = fork();
        if (pid == 0)
        {
            execvp("zenity", execArgs.data());
            _exit(1);
        }

        int status{};
        waitpid(pid, &status, 0);

        if (!WIFEXITED(status)) return PopupResult::POPUP_RESULT_NONE;

        int code = WEXITSTATUS(status);

        if (code == 0)
        {
            switch (action)
            {
                case PopupAction::POPUP_ACTION_YES_NO:
                case PopupAction::POPUP_ACTION_YES_NO_CANCEL:
                    return PopupResult::POPUP_RESULT_YES;

                default:
                case PopupAction::POPUP_ACTION_RETRY_CANCEL:
                    return PopupResult::POPUP_RESULT_OK;
            }
        }
        else
        {
            switch (action)
            {
                case PopupAction::POPUP_ACTION_YES_NO:
                case PopupAction::POPUP_ACTION_YES_NO_CANCEL:
                    return PopupResult::POPUP_RESULT_NO;

                default: return PopupResult::POPUP_RESULT_CANCEL;
            }
        }
    }

    vector<string> Window_Global::GetFile(
		FileType type,
		bool multiple)
    {
        vector<string> files{};
        return files;
    }

    void Window_Global::CreateNotification(
		string_view title,
		string_view message)
    {

    }

    void Window_Global::PlaySystemSound(SoundType type)
    {

    }

    void Window_Global::SetClipboardText(string_view text)
    {

    }

    string Window_Global::GetClipboardText()
    {
        string res{};
        return res;
    }

    const X11GlobalData& Window_Global::GetGlobalData() { return globalData; }
}

#endif //KW_USE_X11
#endif //__linux__