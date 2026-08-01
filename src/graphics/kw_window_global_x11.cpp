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
#include <glib.h>
#include <libnotify/notify.h>
#include <canberra.h>

#include <string>
#include <array>

#include "core_utils.hpp"
#include "log_utils.hpp"
#include "string_utils.hpp"

#include "graphics/kw_window_global.hpp"
#include "core/kw_core.hpp"

using KalaHeaders::KalaCore::FromVar;
using KalaHeaders::KalaCore::RemoveDuplicates;

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;

using KalaHeaders::KalaString::SplitString;

using KalaWindow::Core::KalaWindowCore;
using KalaWindow::Graphics::Window_Global;

using std::string;
using std::to_string;
using std::array;
using std::error_code;

static ca_context* canberra{};

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

        Atom xdndAware = XInternAtom(
            display,
            "XdndAware",
            False);
        Atom xdndEnter = XInternAtom(
            display,
            "XdndEnter",
            False);
        Atom xdndPosition = XInternAtom(
            display,
            "XdndPosition",
            False);
        Atom xdndDrop = XInternAtom(
            display,
            "XdndDrop",
            False);
        Atom xdndStatus = XInternAtom(
            display,
            "XdndStatus",
            False);
        Atom xdndFinished = XInternAtom(
            display,
            "XdndFinished",
            False);
        Atom xdndActionCopy = XInternAtom(
            display,
            "XdndActionCopy",
            False);
        Atom xdndSelection = XInternAtom(
            display,
            "XdndSelection",
            False);
        Atom xdndTypeList = XInternAtom(
            display,
            "XdndTypeList",
            False);
        Atom textUri = XInternAtom(
            display,
            "text/uri-list",
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
        Atom atom_net_wm_window_type_normal = XInternAtom(
            display,
            "_NET_WM_WINDOW_TYPE_NORMAL",
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

        globalData.atom_xDndAware      = FromVar(xdndAware);
        globalData.atom_xDndEnter      = FromVar(xdndEnter);
        globalData.atom_xDndPosition   = FromVar(xdndPosition);
        globalData.atom_xDndDrop       = FromVar(xdndDrop);
        globalData.atom_xDndStatus     = FromVar(xdndStatus);
        globalData.atom_xDndFinished   = FromVar(xdndFinished);
        globalData.atom_xDndActionCopy = FromVar(xdndActionCopy);
        globalData.atom_xDndSelection  = FromVar(xdndSelection);
        globalData.atom_xDndTypeList   = FromVar(xdndTypeList);
        globalData.atom_textUri        = FromVar(textUri);
        
        globalData.atom_net_active_window = FromVar(atom_net_active_window);

        globalData.atom_net_wm_name = FromVar(net_wm_name);
        globalData.atom_net_wm_pid  = FromVar(net_wm_pid);

        globalData.atom_net_wm_window_type        = FromVar(atom_net_wm_window_type);
        globalData.atom_net_wm_window_type_normal = FromVar(atom_net_wm_window_type_normal);
        globalData.atom_net_wm_window_opacity     = FromVar(atom_net_wm_window_opacity);

        globalData.atom_net_wm_state              = FromVar(atom_net_wm_state);
        globalData.atom_net_wm_state_hidden       = FromVar(atom_net_wm_state_hidden);
        globalData.atom_net_wm_state_fullscreen   = FromVar(atom_net_wm_state_fullscreen);
        globalData.atom_net_wm_state_vertical     = FromVar(atom_net_wm_state_vertical);
        globalData.atom_net_wm_state_horizontal   = FromVar(atom_net_wm_state_horizontal);
        globalData.atom_net_wm_state_above        = FromVar(atom_net_wm_state_above);
        globalData.atom_net_wm_state_skip_taskbar = FromVar(atom_net_wm_state_skip_taskbar);

        globalData.atom_wm_delete    = FromVar(atom_wm_delete);

        //initialize libnotify
        notify_init("KalaWindow");

        //initialize canberra
        if (ca_context_create(&canberra) != CA_SUCCESS
            || !canberra)
        {
            KalaWindowCore::ForceClose(
                "KalaWindow global window error",
                "Failed to initialize Canberra!");
        }

        isInitialized = true;

		Log::Print(
			"Initialized global window context!",
			"KW_WINDOW_GLOBAL",
			LogType::LOG_SUCCESS);
    }

    bool Window_Global::IsInitialized() { return isInitialized; }

    const X11GlobalData& Window_Global::GetGlobalData() { return globalData; }

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

    vector<path> Window_Global::GetFiles(
        FileType type,
        vector<string>&& customTypes,
        path&& requiredRoot,
        bool multiple)
    {
        if (multiple
            && type == FileType::FILE_FOLDER)
        {
            Log::Print(
                "Multiple flag was used together with FILE_FOLDER! This will not work and will only select one folder.",
                "KW_WINDOW_GLOBAL",
                LogType::LOG_WARNING);
        }

        string cmd = "zenity --file-selection";
        if (multiple) cmd += " --multiple";
        if (!requiredRoot.empty()) cmd += " --filename='" + requiredRoot.string() + "/'";

        switch (type)
		{
		default:
		case FileType::FILE_ANY:
        case FileType::FILE_EXE:
            cmd += " --separator='|' --file-filter='All Files | *'";
            break;
        case FileType::FILE_FOLDER:
            cmd += " --directory";
            break;
        case FileType::FILE_CUSTOM:
            RemoveDuplicates(customTypes);

            if (customTypes.empty())
            {
                Log::Print(
                    "Failed to get files because FILE_CUSTOM was selected but no types were passed!",
                    "KW_WINDOW_GLOBAL",
                    LogType::LOG_ERROR,
                    2);

                return {};
            }

            string targetTypes{};
            for (const auto& t : customTypes)
            {
                if (!t.starts_with("*.")) targetTypes += "*." + t + " ";
                else targetTypes += t + " ";
            }
            targetTypes.pop_back();

            cmd += " --separator='|' --file-filter='Target Files | " + targetTypes + "'";
            break;
        }

        array<char, 4096> buf{};
        string raw{};

        FILE* pipe = popen(cmd.c_str(), "r");
        if (!pipe)
        {
            Log::Print(
                "Failed to get files because pipe for command '" + cmd + "' couldn't be opened!",
                "KW_WINDOW_GLOBAL",
                LogType::LOG_ERROR,
                2);

            return {};
        }

        while (fgets(buf.data(), scast<int>(buf.size()), pipe))
        {
            raw += buf.data();
        }
        int status = pclose(pipe);

        if (raw.empty())
        {
            if (status == 1)
            {
                Log::Print(
                    "File selection was cancelled by user.",
                    "KW_WINDOW_GLOBAL",
                    LogType::LOG_INFO);
            }
            else
            {
                Log::Print(
                    "Failed to get files! Zenity command '" + cmd + "', error code '" + to_string(status) + "'",
                    "KW_WINDOW_GLOBAL",
                    LogType::LOG_ERROR,
                    2);
            }

            return {};
        }
        if (raw.back() == '\n') raw.pop_back();

        vector<string> stringResult = SplitString(
            raw,
            "|");

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
                    || rel.native().starts_with(".."))
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
        NotifyNotification* notif = notify_notification_new(
            title.data(),
            message.data(),
            "dialog-information");

        if (!notif)
        {
            Log::Print(
                "Failed to allocate notification!",
                "KW_WINDOW_GLOBAL",
                LogType::LOG_ERROR,
                2);

            return;
        }

        GError *error = NULL;
        if (!notify_notification_show(
            notif,
            &error))
        {
            string errorMessage = "Failed to show notification! Reason: ";
            errorMessage += error 
                ? error->message
                : "Unknown error";

            Log::Print(
                errorMessage,
                "KW_WINDOW_GLOBAL",
                LogType::LOG_ERROR,
                2);

            g_error_free(error);
        }

        g_object_unref(G_OBJECT(notif));
    }

    void Window_Global::PlaySystemSound(SoundType type)
    {
        const char* eventID{};
        switch (type)
        {
        case SoundType::SOUND_OK:
            eventID = "dialog-information";
            break;
        case SoundType::SOUND_ERROR:
            eventID = "dialog-error";
            break;
        }

        ca_context_play(
            canberra,
            0,
            CA_PROP_EVENT_ID,
            eventID,
            CA_PROP_CANBERRA_CACHE_CONTROL,
            "permanent",
            NULL);
    }

    void Window_Global::Shutdown()
    {
        notify_uninit();
        if (canberra)
        {
            ca_context_destroy(canberra);
            canberra = nullptr;
        }
    }
}

#endif //__linux__