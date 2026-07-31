//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef __linux__

#include <X11/Xlib.h>
#include <X11/extensions/XI2.h>
#include <X11/extensions/XInput2.h>
#include <X11/Xatom.h>
#include <sys/wait.h>

#include <string>

#include "core_utils.hpp"
#include "log_utils.hpp"

#include "graphics/kw_window_global.hpp"
#include "core/kw_core.hpp"

using KalaHeaders::KalaCore::FromVar;

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;

using KalaWindow::Core::KalaWindowCore;
using KalaWindow::Graphics::Window_Global;

using std::string;
using std::to_string;

static int ErrorHandler(
    Display* display,
    XErrorEvent* error)
{
    if (!display)
    {
        KalaWindowCore::ForceClose(
            "KalaWindow global window error",
            "Failed to call X11 error handler because the attached display was invalid!");
    }

    char buffer[512]{};
    XGetErrorText(
        display,
        error->error_code,
        buffer,
        sizeof(buffer));

    string source = "X11_CORE";
    int decodedCode = error->error_code;

    if (error->request_code == Window_Global::GetGlobalData().xiOpcode)
    {
        source = "XI2";
        decodedCode = error->error_code - Window_Global::GetGlobalData().xiErrorBase;
    }

    Log::Print(
        source + " Error: " + to_string(decodedCode) + "\n"
            + "request: " + to_string(error->request_code) + "\n"
            + "minor: " + to_string(error->minor_code) + "\n"
            + "reason: " + buffer,
        "KW_WINDOW_GLOBAL",
        LogType::LOG_ERROR,
        2);

    return 0; //tells X to continue
}

static int IOErrorHandler(Display* display)
{
    KalaWindowCore::ForceClose(
        "KalaWindow global window error",
        "Fatal X11 IO error was detected and the program must close!");

    return 0; //tells X to exit
}

namespace KalaWindow::Graphics
{
	static bool isInitialized{};
	static bool isVerboseLoggingEnabled{};

    static X11GlobalData globalData{};

    void Window_Global::SetVerboseLoggingState(bool newState) { isVerboseLoggingEnabled = newState; }
	bool Window_Global::IsVerboseLoggingEnabled() { return isVerboseLoggingEnabled; }

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

        Display* display = XOpenDisplay(nullptr);
        if (!display)
        {
            KalaWindowCore::ForceClose(
                "KalaWindow global window error",
                "Failed to initialize global window because the created display was invalid!");
        }

        XSetErrorHandler(ErrorHandler);
        XSetIOErrorHandler(IOErrorHandler);

        XIM xim = XOpenIM(display, nullptr, nullptr, nullptr);

        Window root = DefaultRootWindow(display);

        int event{};
        int error{};
        int opCode{};

        if (!XQueryExtension(
            display,
            "XInputExtension",
            &opCode,
            &event,
            &error))
        {
            KalaWindowCore::ForceClose(
                "KalaWindow global window error",
                "XInput event is not available!");
        }

        int major = 2;
        int minor{};

        Status status = XIQueryVersion(
            display, 
            &major, 
            &minor);

        if (status != 0)
        {
            KalaWindowCore::ForceClose(
                "KalaWindow global window error",
                "XInput2 is not available! Reason: " + to_string(status));
        }

        XIEventMask mask{};
        unsigned char maskData[(XI_LASTEVENT + 7) / 8]{};

        mask.deviceid = XIAllMasterDevices;
        mask.mask_len = sizeof(maskData);
        mask.mask = maskData;

        XISetMask(mask.mask, XI_RawMotion);

        XISelectEvents(
            display, 
            root, 
            &mask, 
            1);

        //flush now and detect errors via ErrorHandler
        XSync(display, False);

        Atom utf8 = XInternAtom(
            display, 
            "UTF8_STRING", 
            False);

        Atom cardinal = XInternAtom(
            display,
            "CARDINAL",
            False);

        Atom net_wm_name = XInternAtom(
            display, 
            "_NET_WM_NAME", 
            False);

        Atom net_wm_pid = XInternAtom(
            display,
            "_NET_WM_PID",
            False);

        Atom atom_net_active_window = XInternAtom(
            display,
            "_NET_ACTIVE_WINDOW",
            False);

        Atom atom_net_wm_window_type = XInternAtom(
            display,
            "_NET_WM_WINDOW_TYPE",
            False);
        Atom atom_net_wm_window_opacity = XInternAtom(
            display,
            "_NET_WM_WINDOW_OPACITY",
            False);

        Atom atom_net_wm_state = XInternAtom(
            display,
            "_NET_WM_STATE",
            False);
        Atom atom_net_wm_state_hidden = XInternAtom(
            display,
            "_NET_WM_STATE_HIDDEN",
            False);
        Atom atom_net_wm_state_fullscreen = XInternAtom(
            display,
           "_NET_WM_STATE_FULLSCREEN",
            False);
        Atom atom_net_wm_state_vertical = XInternAtom(
            display,
           "_NET_WM_STATE_MAXIMIZED_VERT",
            False);
        Atom atom_net_wm_state_horizontal = XInternAtom(
            display,
           "_NET_WM_STATE_MAXIMIZED_HORZ",
            False);
        Atom atom_net_wm_state_above = XInternAtom(
            display,
           "_NET_WM_STATE_ABOVE",
            False);
        Atom atom_net_wm_state_skip_taskbar = XInternAtom(
            display,
           "_NET_WM_STATE_SKIP_TASKBAR",
            False);

        Atom atom_wm_delete = XInternAtom(
            display,
            "WM_DELETE_WINDOW",
            False);

        globalData.display = FromVar(display);
        globalData.window_root = FromVar(root);

        globalData.xim = FromVar(xim);

        globalData.xiErrorBase = error;
        globalData.xiOpcode = opCode;

        globalData.atom_utf8 = FromVar(utf8);

        globalData.atom_cardinal = FromVar(cardinal);
        
        globalData.atom_net_active_window = FromVar(atom_net_active_window);

        globalData.atom_net_wm_name = FromVar(net_wm_name);
        globalData.atom_net_wm_pid  = FromVar(net_wm_pid);

        globalData.atom_net_wm_window_type    = FromVar(atom_net_wm_window_type);
        globalData.atom_net_wm_window_opacity = FromVar(atom_net_wm_window_opacity);

        globalData.atom_net_wm_state              = FromVar(atom_net_wm_state);
        globalData.atom_net_wm_state_hidden       = FromVar(atom_net_wm_state_hidden);
        globalData.atom_net_wm_state_fullscreen   = FromVar(atom_net_wm_state_fullscreen);
        globalData.atom_net_wm_state_vertical     = FromVar(atom_net_wm_state_vertical);
        globalData.atom_net_wm_state_horizontal   = FromVar(atom_net_wm_state_horizontal);
        globalData.atom_net_wm_state_above        = FromVar(atom_net_wm_state_above);
        globalData.atom_net_wm_state_skip_taskbar = FromVar(atom_net_wm_state_skip_taskbar);

        globalData.atom_wm_delete    = FromVar(atom_wm_delete);

        isInitialized = true;

		Log::Print(
			"Initialized global window context!",
			"KW_WINDOW_GLOBAL",
			LogType::LOG_SUCCESS);
    }

    bool Window_Global::IsInitialized() { return isInitialized; }

    PopupResult Window_Global::CreatePopup(
		string&& title,
		string&& message,
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

        args.emplace_back("--title=" + std::move(title));
        args.emplace_back("--text=" + std::move(message));

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
		string&& title,
		string&& message)
    {

    }

    void Window_Global::PlaySystemSound(SoundType type)
    {

    }

    void Window_Global::SetClipboardText(string&& text)
    {

    }

    string Window_Global::GetClipboardText()
    {
        string res{};
        return res;
    }

    const X11GlobalData& Window_Global::GetGlobalData() { return globalData; }
}

#endif //__linux__