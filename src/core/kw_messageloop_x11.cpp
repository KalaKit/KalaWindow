//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include "core/kw_messageloop.hpp"

#if defined(KLIN_ANY)

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/extensions/XI2.h>
#include <X11/extensions/XInput2.h>
#include <X11/keysym.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

#include <vector>
#include <unordered_map>
#include <string>
#include <functional>
#include <climits>
#include <sstream>

#include "core_utils.hpp"
#include "math_utils.hpp"
#include "log_utils.hpp"
#include "key_standards.hpp"

#include "core/kw_registry.hpp"
#include "core/kw_core.hpp"
#include "core/kw_input.hpp"
#include "graphics/kw_window_global.hpp"
#include "graphics/kw_window.hpp"

using KalaHeaders::KalaCore::ToVar;
using KalaHeaders::KalaCore::FromVar;

using KalaHeaders::KalaMath::vec2;

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;

using KalaHeaders::KalaKeyStandards::KeyboardButton;
using KalaHeaders::KalaKeyStandards::GetValueByKey;

using KalaWindow::Core::Input;
using KalaWindow::Graphics::WindowState;
using KalaWindow::Graphics::Window_Global;
using KalaWindow::Graphics::X11GlobalData;
using KalaWindow::Graphics::ProcessWindow;
using KalaWindow::Graphics::WindowData;

using std::vector;
using std::unordered_map;
using std::string;
using std::to_string;
using std::function;
using std::stringstream;

static function<void(u32)> addCharCallback{};
static function<void()> removeFromBackCallback{};
static function<void()> addTabCallback{};
static function<void()> addNewlineCallback{};

static constexpr int SUCCESS_XGETWINDOWPROPERTY = 0;
static constexpr int SUCCESS_XSENDEVENT = 1;

static constexpr u32 DOUBLE_CLICK_TIME = 500;
static u32 lastClickTime[8]{};

static int XRESULT{};

static const unordered_map<KeySym, KeyboardButton> XKeyToKeyMap = {
	// Letters
	{ XK_a, KeyboardButton::K_A }, { XK_b, KeyboardButton::K_B }, { XK_c, KeyboardButton::K_C }, { XK_d, KeyboardButton::K_D },
	{ XK_e, KeyboardButton::K_E }, { XK_f, KeyboardButton::K_F }, { XK_g, KeyboardButton::K_G }, { XK_h, KeyboardButton::K_H },
	{ XK_i, KeyboardButton::K_I }, { XK_j, KeyboardButton::K_J }, { XK_k, KeyboardButton::K_K }, { XK_l, KeyboardButton::K_L },
	{ XK_m, KeyboardButton::K_M }, { XK_n, KeyboardButton::K_N }, { XK_o, KeyboardButton::K_O }, { XK_p, KeyboardButton::K_P },
	{ XK_q, KeyboardButton::K_Q }, { XK_r, KeyboardButton::K_R }, { XK_s, KeyboardButton::K_S }, { XK_t, KeyboardButton::K_T },
	{ XK_u, KeyboardButton::K_U }, { XK_v, KeyboardButton::K_V }, { XK_w, KeyboardButton::K_W }, { XK_x, KeyboardButton::K_X },
	{ XK_y, KeyboardButton::K_Y }, { XK_z, KeyboardButton::K_Z },

	// Numbers
	{ XK_0, KeyboardButton::K_0 }, { XK_1, KeyboardButton::K_1 }, { XK_2, KeyboardButton::K_2 }, { XK_3, KeyboardButton::K_3 },
	{ XK_4, KeyboardButton::K_4 }, { XK_5, KeyboardButton::K_5 }, { XK_6, KeyboardButton::K_6 }, { XK_7, KeyboardButton::K_7 },
	{ XK_8, KeyboardButton::K_8 }, { XK_9, KeyboardButton::K_9 },

	// Function Keys
	{ XK_F1, KeyboardButton::K_F1 }, { XK_F2, KeyboardButton::K_F2 }, { XK_F3, KeyboardButton::K_F3 }, { XK_F4, KeyboardButton::K_F4 },
	{ XK_F5, KeyboardButton::K_F5 }, { XK_F6, KeyboardButton::K_F6 }, { XK_F7, KeyboardButton::K_F7 }, { XK_F8, KeyboardButton::K_F8 },
	{ XK_F9, KeyboardButton::K_F9 }, { XK_F10, KeyboardButton::K_F10 }, { XK_F11, KeyboardButton::K_F11 }, { XK_F12, KeyboardButton::K_F12 },

	// Numpad
	{ XK_KP_0, KeyboardButton::K_NUM_0 }, { XK_KP_1, KeyboardButton::K_NUM_1 }, { XK_KP_2, KeyboardButton::K_NUM_2 },
	{ XK_KP_3, KeyboardButton::K_NUM_3 }, { XK_KP_4, KeyboardButton::K_NUM_4 }, { XK_KP_5, KeyboardButton::K_NUM_5 },
	{ XK_KP_6, KeyboardButton::K_NUM_6 }, { XK_KP_7, KeyboardButton::K_NUM_7 }, { XK_KP_8, KeyboardButton::K_NUM_8 },
	{ XK_KP_9, KeyboardButton::K_NUM_9 },
	{ XK_KP_Add, KeyboardButton::K_NUM_ADD }, { XK_KP_Subtract, KeyboardButton::K_NUM_SUBTRACT },
	{ XK_KP_Multiply, KeyboardButton::K_NUM_MULTIPLY }, { XK_KP_Divide, KeyboardButton::K_NUM_DIVIDE },
	{ XK_KP_Enter, KeyboardButton::K_NUM_RETURN }, { XK_Num_Lock, KeyboardButton::K_NUM_LOCK },
	{ XK_KP_Decimal, KeyboardButton::K_NUM_DECIMAL },

	// Navigation
	{ XK_Left, KeyboardButton::K_ARROW_LEFT }, { XK_Right, KeyboardButton::K_ARROW_RIGHT },
	{ XK_Up, KeyboardButton::K_ARROW_UP }, { XK_Down, KeyboardButton::K_ARROW_DOWN },
	{ XK_Home, KeyboardButton::K_HOME }, { XK_End, KeyboardButton::K_END },
	{ XK_Page_Up, KeyboardButton::K_PAGE_UP }, { XK_Page_Down, KeyboardButton::K_PAGE_DOWN },
	{ XK_Insert, KeyboardButton::K_INSERT }, { XK_Delete, KeyboardButton::K_DELETE },

	// Controls
	{ XK_Return, KeyboardButton::K_RETURN }, { XK_Escape, KeyboardButton::K_ESC },
	{ XK_BackSpace, KeyboardButton::K_BACKSPACE }, { XK_Tab, KeyboardButton::K_TAB },
	{ XK_Caps_Lock, KeyboardButton::K_CAPS_LOCK }, { XK_space, KeyboardButton::K_SPACE },

	// Modifiers
	{ XK_Shift_L, KeyboardButton::K_LEFT_SHIFT }, { XK_Shift_R, KeyboardButton::K_RIGHT_SHIFT },
	{ XK_Control_L, KeyboardButton::K_LEFT_CTRL }, { XK_Control_R, KeyboardButton::K_RIGHT_CTRL },

	{ XK_Alt_L,  KeyboardButton::K_LEFT_ALT }, { XK_Meta_L, KeyboardButton::K_LEFT_ALT },
    { XK_Alt_R,  KeyboardButton::K_RIGHT_ALT }, { XK_Meta_R, KeyboardButton::K_RIGHT_ALT },

	{ XK_Super_L, KeyboardButton::K_SUPERLEFT }, { XK_Super_R, KeyboardButton::K_SUPERRIGHT },

	// System / Special
	{ XK_Print, KeyboardButton::K_PRINT_SCREEN }, { XK_Scroll_Lock, KeyboardButton::K_SCROLL_LOCK },
	{ XK_Pause, KeyboardButton::K_PAUSE }, { XK_Menu, KeyboardButton::K_MENU }
};

static string TranslateKeySymToString(KeySym keysym)
{
	//normalize uppercase letters to lowercase
	if (keysym >= XK_A && keysym <= XK_Z) keysym += 32;

	KeyboardButton key = KeyboardButton::K_INVALID;

	auto it = XKeyToKeyMap.find(keysym);
	if (it != XKeyToKeyMap.end()) key = it->second;

	string result = GetValueByKey(scast<u32>(key)).data();

	return result == "?"
		? "Unknown"
		: result;
}

static KeyboardButton TranslateKeySym(KeySym keysym)
{
	//normalize uppercase letters to lowercase
	if (keysym >= XK_A && keysym <= XK_Z) keysym += 32;

	auto it = XKeyToKeyMap.find(keysym);
	if (it != XKeyToKeyMap.end()) return it->second;

	return KeyboardButton::K_INVALID;
}

namespace KalaWindow::Core
{
    void MessageLoop::SetAddCharCallback(function<void(u32)>&& newCallback)
	{
		addCharCallback = std::move(newCallback);
	}
	void MessageLoop::SetRemoveFromBackCallback(function<void()>&& newCallback)
	{
		removeFromBackCallback = std::move(newCallback);
	}
	void MessageLoop::SetAddTabCallback(function<void()>&& newCallback)
	{
		addTabCallback = std::move(newCallback);
	}
	void MessageLoop::SetAddNewLineCallback(function<void()>&& newCallback)
	{
		addNewlineCallback = std::move(newCallback);
	}

    void MessageLoop::Update()
    {
        const X11GlobalData& globalData = Window_Global::GetGlobalData();
        if (!globalData.display)
        {
            KalaWindowCore::ForceClose(
                "KalaWindow message loop error",
                "Failed to update message loop because the display was invalid!");
        }

        const vector<ProcessWindow*>& activeWindows = KalaWindowRegistry<ProcessWindow>::GetAllContent();

        Display* display = ToVar<Display*>(globalData.display);

        Atom atom_net_wm_state_fullscreen = ToVar<Atom>(globalData.atom_net_wm_state_fullscreen);
        Atom atom_net_wm_state_maximized_horizontal = ToVar<Atom>(globalData.atom_net_wm_state_maximized_horizontal);
        Atom atom_net_wm_state_maximized_vertical = ToVar<Atom>(globalData.atom_net_wm_state_maximized_vertical);
        Atom atom_net_wm_state_hidden = ToVar<Atom>(globalData.atom_net_wm_state_hidden);

        Atom atom_wm_delete = ToVar<Atom>(globalData.atom_wm_delete);
        Atom atom_net_wm_state = ToVar<Atom>(globalData.atom_net_wm_state);
        Atom atom_net_frame_extents = ToVar<Atom>(globalData.atom_net_frame_extents);

        Atom atom_textUri = ToVar<Atom>(globalData.atom_textUri);

        Atom atom_xDndStatus = ToVar<Atom>(globalData.atom_xDndStatus);
        Atom atom_xDndSelection = ToVar<Atom>(globalData.atom_xDndSelection);
        Atom atom_xDndFinished = ToVar<Atom>(globalData.atom_xDndFinished);
        Atom atom_xDndActionCopy = ToVar<Atom>(globalData.atom_xDndActionCopy);
        Atom atom_xDndEnter = ToVar<Atom>(globalData.atom_xDndEnter);
        Atom atom_xDndPosition = ToVar<Atom>(globalData.atom_xDndPosition);
        Atom atom_xDndDrop = ToVar<Atom>(globalData.atom_xDndDrop);

        while (XPending(display))
        {
            XEvent event{};
            XNextEvent(display, &event);

            if (event.type == GenericEvent)
            {
                if (XGetEventData(display, &event.xcookie)
                    && event.xcookie.extension == globalData.xiOpcode)
                {
                    if (event.xcookie.evtype == XI_RawMotion)
                    {
                        XIRawEvent* raw = rcast<XIRawEvent*>(event.xcookie.data);

                        f64* values = raw->raw_values;
                        int i = 0;

                        f64 dx{};
                        f64 dy{};

                        if (XIMaskIsSet(raw->valuators.mask, 0)) dx = values[i++];
                        if (XIMaskIsSet(raw->valuators.mask, 1)) dy = values[i++];

                        for (size_t i = 0; i < activeWindows.size(); i++)
                        {
                            ProcessWindow* w = activeWindows[i];
                            if (!w)
                            {
                                KalaWindowCore::ForceClose(
                                    "KalaWindow message loop error",
                                    "Failed to call process_message because active window '" + to_string(i) + "' was invalid!");
                            }

                            Input* input{};
                            string err = Input::GetRegistry().GetContent(w->inputID, input);
                            if (!err.empty())
                            {
                                KalaWindowCore::ForceClose(
                                    "Kalawindow message loop error",
                                    "Failed to process message for window '" + to_string(w->ID) + "' because input was invalid! Reason: " + err);
                            }

                            input->rawMouseDelta.x += (f32)dx;
                            input->rawMouseDelta.y += (f32)dy;

                            if (Input::IsVerboseLoggingEnabled())
                            {
                                Log::Print(
                                "Raw mouse delta: " + to_string(dx) + ", " + to_string(dy),
                                "KW_MESSAGE_LOOP",
                                LogType::LOG_VERBOSE);
                            }
                        }
                    }

                    XFreeEventData(display, &event.xcookie);
                }
            }

            Window target = event.xany.window;

            for (size_t i = 0; i < activeWindows.size(); i++)
            {
                ProcessWindow* w = activeWindows[i];
                if (!w)
                {
                    KalaWindowCore::ForceClose(
                        "KalaWindow message loop error",
                        "Failed to call process_message because active window '" + to_string(i) + "' was invalid!");
                }

                const WindowData& wdata = w->GetWindowData();

                if (!wdata.window)
                {
                    KalaWindowCore::ForceClose(
                        "KalaWindow message loop error",
                        "Failed to update message loop because the window handle was invalid!");
                }

                Window window = ToVar<Window>(wdata.window);

                if (target != window) continue;

                Input* input{};
                string err = Input::GetRegistry().GetContent(w->GetInputID(), input);
                if (!err.empty())
                {
                    KalaWindowCore::ForceClose(
                        "Kalawindow message loop error",
                        "Failed to process message for window '" + to_string(w->ID) + "' because input was invalid! Reason: " + err);
                }

                XIC xic = ToVar<XIC>(wdata.xic);

                switch (event.type)
                {
                    case ConfigureNotify:
                    {
                        const vec2 oldSize = w->size;

                        w->pos = vec2(event.xconfigure.x, event.xconfigure.y);
                        w->size = kclamp(
                            vec2(event.xconfigure.width, event.xconfigure.height),
                            w->minSize,
                            w->maxSize);

                        const bool resized = w->size != oldSize;

                        Atom actualType{};
                        int actualFormat{};
                        unsigned long nItems{}, bytesAfter{};
                        unsigned long* extents{};

                        XRESULT = XGetWindowProperty(
                            display,
                            window,
                            atom_net_frame_extents,
                            0, 4, False,
                            XA_CARDINAL,
                            &actualType, &actualFormat,
                            &nItems,
                            &bytesAfter,
                            (unsigned char**)&extents);

                        if (XRESULT != SUCCESS_XGETWINDOWPROPERTY)
                        {
                            Log::Print(
                                "Failed to update window '" + to_string(w->GetID()) 
                                + "' position and size because XGetWindowProperty failed! Result code: " + to_string(XRESULT),
                                "KW_MESSAGE_LOOP",
                                LogType::LOG_ERROR,
                                2);
                        }

                        if (extents)
                        {
                            w->outerSize = vec2(
                                w->size.x + extents[0] + extents[1],
                                w->size.y + extents[2] + extents[3]);
                            XFree(extents);
                        }

                        if (resized
                            && w->resizeCallback)
                        {
                            //Log::Print("@@@@@ ConfigureNotify requested resize callback...");

                            w->resizeCallback(false);
                            w->configureNotifyRequestedResizeCallback = true;
                        }

                        break;
                    }

                    case SelectionNotify:
                    {
                        if (event.xselection.property != None
                            && event.xselection.selection == atom_xDndSelection)
                        {
                            Atom actualType{};
                            int format{};
                            unsigned long nItems{}, bytesAfter{};
                            unsigned char* data{};

                            XRESULT = XGetWindowProperty(
                                display,
                                window,
                                atom_xDndSelection,
                                0,
                                LONG_MAX,
                                True,
                                AnyPropertyType,
                                &actualType,
                                &format,
                                &nItems,
                                &bytesAfter,
                                &data);

                            if (XRESULT != SUCCESS_XGETWINDOWPROPERTY
                                || !data
                                || format != 8)
                            {
                                if (XRESULT != SUCCESS_XGETWINDOWPROPERTY)
                                {
                                    Log::Print(
                                        "Failed to read dropped file paths because XGetWindowProperty failed! Result code: " + to_string(XRESULT),
                                        "KW_MESSAGE_LOOP",
                                        LogType::LOG_ERROR,
                                        2);
                                }
                                else if (!data)
                                {
                                    Log::Print(
                                        "Failed to read dropped file paths because the received data was invalid!",
                                        "KW_MESSAGE_LOOP",
                                        LogType::LOG_ERROR,
                                        2);
                                }
                                else
                                {
                                    Log::Print(
                                        "Failed to read dropped file paths because the format '" + to_string(format) + "' was invalid!",
                                        "KW_MESSAGE_LOOP",
                                        LogType::LOG_ERROR,
                                        2);
                                }
                            }
                            else
                            {
                                string uris(rcast<char*>(data), nItems);
                                vector<path> filePaths{};

                                stringstream ss(uris);
                                string line{};
                                while (getline(ss, line))
                                {
                                    if (line.starts_with("file://"))
                                    {
                                        string path = line.substr(7);
                                        string decoded{};
                                        for (size_t i = 0; i < path.size(); ++i)
                                        {
                                            if (path[i] == '%'
                                                && i + 2 < path.size())
                                            {
                                                char hex[3] = { path[i + 1], path[i + 2], '\0' };
                                                decoded += scast<char>(strtol(hex, nullptr, 16));
                                                i += 2;
                                            }
                                            else decoded += path[i];
                                        }
                                        filePaths.push_back(decoded);
                                    }
                                }

                                w->lastDraggedFiles = std::move(filePaths);

                                //reply to the source window where the file drag operation started from
                                Window source = ToVar<Window>(w->currentDndSource);

                                XEvent finished{};
                                finished.xclient.type = ClientMessage;
                                finished.xclient.display = display;
                                finished.xclient.window = source;
                                finished.xclient.message_type = atom_xDndFinished;
                                finished.xclient.format = 32;
                                finished.xclient.data.l[0] = window;
                                finished.xclient.data.l[1] = 1;
                                finished.xclient.data.l[2] = atom_xDndActionCopy;

                                XRESULT = XSendEvent(
                                    display,
                                    source,
                                    False,
                                    NoEventMask,
                                    &finished);

                                if (XRESULT != SUCCESS_XSENDEVENT)
                                {
                                    Log::Print(
                                        "Failed to handle SelectionNotify and xDndSelection because XSendEvent failed! "
                                        "Result code: " + to_string(XRESULT),
                                        "KW_WINDOW_GLOBAL",
                                        LogType::LOG_ERROR,
                                        2);
                                }

                                if (Window_Global::IsVerboseLoggingEnabled())
                                {
                                    for (const path& file : w->lastDraggedFiles)
                                    {
                                        Log::Print(
                                            "File '" + file.string() + "' was dragged to window '" + to_string(w->GetID()) + "'",
                                            "KW_MESSAGE_LOOP",
                                            LogType::LOG_VERBOSE);
                                    }
                                }

                                if (w->draggedFilesCallback)
                                {
                                    w->draggedFilesCallback(w->lastDraggedFiles, w->draggedFilesPos);
                                }

                                XFlush(display);
                            }

                            if (data) XFree(data);
                        }

                        break;
                    }
                    case SelectionRequest: break;
                    case Expose: break;

                    case ClientMessage:
                    {
                        if (Window_Global::IsVerboseLoggingEnabled())
                        {
                            char* name = XGetAtomName(
                                display,
                                event.xclient.message_type);

                            Log::Print(
                                "Received client message '" + 
                                to_string(event.xclient.message_type) + "' (" + (name ? name : "unknown") + ")'.", 
                                "KW_MESSAGE_LOOP",
                                LogType::LOG_VERBOSE);

                            if (name) XFree(name);
                        }

                        if ((Atom)event.xclient.data.l[0] == atom_wm_delete)
                        {
                            w->Destroy();
                            continue;
                        }

                        if (event.xclient.message_type == atom_xDndEnter)
                        {
                            w->currentDndSource = FromVar(event.xclient.data.l[0]);

                            continue;
                        }
                        if (event.xclient.message_type == atom_xDndPosition)
                        {
                            Window source = event.xclient.data.l[0];

                            i32 rootX = (i32)(event.xclient.data.l[2] >> 16);
                            i32 rootY = (i32)(event.xclient.data.l[2] & 0xFFFF);

                            i32 winX{}, winY{};

                            Window dummy{};
                            XTranslateCoordinates(
                                display,
                                DefaultRootWindow(display),
                                window,
                                rootX,
                                rootY,
                                &winX,
                                &winY,
                                &dummy);

                            w->draggedFilesPos = vec2(f32(winX), f32(winY));

                            if (Window_Global::IsVerboseLoggingEnabled())
                            {
                                Log::Print(
                                    "XDndPosition at window coords: " 
                                    + to_string(w->draggedFilesPos.x) + ", " 
                                    + to_string(w->draggedFilesPos.y),
                                    "KW_MESSAGE_LOOP",
                                    LogType::LOG_VERBOSE);
                            }

                            XEvent reply{};
                            reply.xclient.type = ClientMessage;
                            reply.xclient.display = display;
                            reply.xclient.window = source;
                            reply.xclient.message_type = atom_xDndStatus;
                            reply.xclient.format = 32;
                            reply.xclient.data.l[0] = window;
                            reply.xclient.data.l[1] = 1;                   //accept flag
                            reply.xclient.data.l[2] = 0;                   //bounding box left (0 = no rect)
                            reply.xclient.data.l[3] = 0;                   //bounding box top (0 = no rect)
                            reply.xclient.data.l[4] = atom_xDndActionCopy; //preferred action

                            XRESULT = XSendEvent(
                                display,
                                source,
                                False,
                                NoEventMask,
                                &reply);

                            if (XRESULT != SUCCESS_XSENDEVENT)
                            {
                                Log::Print(
                                    "Failed to handle ClientMessage and xDndPosition because XSendEvent failed! "
                                    "Result code: " + to_string(XRESULT),
                                    "KW_WINDOW_GLOBAL",
                                    LogType::LOG_ERROR,
                                    2);
                            }

                            XFlush(display);

                            continue;
                        }
                        if (event.xclient.message_type == atom_xDndDrop)
                        {
                            w->currentDndSource = FromVar(event.xclient.data.l[0]);

                            XConvertSelection(
                                display,
                                atom_xDndSelection,
                                atom_textUri,
                                atom_xDndSelection,
                                window,
                                CurrentTime);

                            XFlush(display);

                            continue;
                        }

                        break;
                    }
                    case DestroyNotify: break;

                    case PropertyNotify:
                    {
                        if (event.xproperty.atom == atom_net_wm_state
                            && event.xproperty.state == PropertyNewValue)
                        {
                            Atom actualType{};
                            int actualFormat{};
                            unsigned long nItems{}, bytesAfter{};
                            Atom* states{};

                            XRESULT = XGetWindowProperty(
                                display,
                                window,
                                atom_net_wm_state,
                                0,
                                1024,
                                False,
                                XA_ATOM,
                                &actualType,
                                &actualFormat,
                                &nItems,
                                &bytesAfter,
                                rcast<unsigned char**>(&states));

                            if (XRESULT != SUCCESS_XGETWINDOWPROPERTY)
                            {
                                Log::Print(
                                    "Failed to get window '" + to_string(w->GetID()) 
                                    + "' state because XGetWindowProperty failed! Result code: " + to_string(XRESULT),
                                    "KW_MESSAGE_LOOP",
                                    LogType::LOG_ERROR,
                                    2);

                                return;
                            }

                            bool fullScreen{};
                            bool maximizedHorizontal{};
                            bool maximizedVertical{};
                            bool hidden{};

                            if (states)
                            {
                                for (unsigned long i = 0; i < nItems; ++i)
                                {
                                    if (states[i] == atom_net_wm_state_fullscreen) fullScreen = true;
                                    else if (states[i] == atom_net_wm_state_maximized_horizontal) maximizedHorizontal = true;
                                    else if (states[i] == atom_net_wm_state_maximized_vertical) maximizedVertical = true;
                                    else if (states[i] == atom_net_wm_state_hidden) hidden = true;
                                }

                                XFree(states);
                            }

                            const bool isMaximized = 
                                maximizedHorizontal
                                && maximizedVertical;

                            const bool resizeStateChanged = 
                                isMaximized != w->isMaximized
                                || fullScreen != w->isFullscreen;

                            const bool wasMinimized = !w->isVisible; 

                            w->isFullscreen = fullScreen;
                            w->isMaximized = isMaximized;
                            w->isVisible = !hidden;

                            if (resizeStateChanged)
                            {
                                //Log::Print("@@@@@ started resize callback delay...");

                                w->delayedMaximizeRestoreCounterStart = true;
                                w->delayedMaximizeRestoreCounter = 0;
                            }

                            if (!wasMinimized
                                && hidden)
                            {
                                for (u32 childID : w->childIDs)
                                {
                                    ProcessWindow* child{};
                                    string err = ProcessWindow::GetRegistry().GetContent(childID, child);
                                    if (!err.empty())
                                    {
                                        KalaWindowCore::ForceClose(
                                            "KalaWindow message loop error",
                                            "Failed to minimize child window '" + to_string(childID) 
                                            + "' under parent '" + to_string(w->ID) + "'! Reason: " + err);
                                    }

                                    child->SetWindowState(WindowState::WINDOW_MINIMIZE);
                                }
                            }
                            else if (wasMinimized
                                     && !hidden)
                            {
                                for (u32 childID : w->childIDs)
                                {
                                    ProcessWindow* child{};
                                    string err = ProcessWindow::GetRegistry().GetContent(childID, child);
                                    if (!err.empty())
                                    {
                                        KalaWindowCore::ForceClose(
                                            "KalaWindow message loop error",
                                            "Failed to bring child window '" + to_string(childID) 
                                            + "' under parent '" + to_string(w->ID) + "' to focus! Reason: " + err);
                                    }

                                    Window childWindow = ToVar<Window>(child->GetWindowData().window);

                                    XMapWindow(display, childWindow);
                                    
                                    child->BringToFocus();
                                }
                            }
                        }

                        break;
                    }

                    case FocusIn:
                    {
                        w->isFocused = true;
                        if (xic) XSetICFocus(xic);

                        break;
                    }
                    case FocusOut:
                    {
                        w->isFocused = false;
                        if (xic) XUnsetICFocus(xic);

                        break;
                    }

                    case MapNotify:
                    {   
                        w->isVisible = true;

                        break;
                    }
                    case UnmapNotify:
                    {   
                        w->isVisible = false;

                        break;
                    }

                    case EnterNotify:
                    {
                        w->isWindowHovered = true;

                        break;
                    }
                    case LeaveNotify:
                    {
                        w->isWindowHovered = false;
                        
                        break;
                    }

                    case KeyPress:
                    {
                        KeySym ks{};
                        char buffer[32]{};
                        int status{};

                        int len = Xutf8LookupString(
                            xic,
                            &event.xkey,
                            buffer,
                            sizeof(buffer),
                            &ks,
                            &status);

                        KeyboardButton key = TranslateKeySym(ks);

                        if (Input::IsVerboseLoggingEnabled())
                        {
                            Log::Print(
                                "Detected keyboard key '" + TranslateKeySymToString(ks) + "' down.",
                                "KW_MESSAGE_LOOP",
                                LogType::LOG_VERBOSE);
                        }

                        if (input)
                        {
                            input->SetKeyState(
                                key, 
                                true);

                            switch (ks)
                            {
                                case XK_BackSpace:
                                    if (removeFromBackCallback) removeFromBackCallback();
                                    break;
                                case XK_Tab:
                                    if (addTabCallback) addTabCallback();
                                    break;
                                case XK_Return:
                                    if (addNewlineCallback) addNewlineCallback();
                                    break;
                            }
                        }

                        //utf16 text for typing
                        if (len > 0
                            && addCharCallback)
                        {
                            const unsigned char* ptr = (unsigned char*)buffer;

                            while (ptr < (unsigned char*)buffer + len)
                            {
                                u32 codePoint{};

                                if (*ptr < 0x80) codePoint = *ptr++;
                                else if ((*ptr & 0xE0) == 0xC0)
                                {
                                    codePoint = (*ptr++ & 0x1F) << 6;
                                    codePoint |= (*ptr++ & 0x3F);
                                }
                                else if ((*ptr & 0xF0) == 0xE0)
                                {
                                    codePoint = (*ptr++ & 0x0F) << 12;
                                    codePoint |= (*ptr++ & 0x3F) << 6;
                                    codePoint |= (*ptr++ & 0x3F);
                                }
                                else if ((*ptr & 0xF8) == 0xF0)
                                {
                                    codePoint = (*ptr++ & 0x07) << 18;
                                    codePoint |= (*ptr++ & 0x3F) << 12;
                                    codePoint |= (*ptr++ & 0x3F) << 6;
                                    codePoint |= (*ptr++ & 0x3F);
                                }

                                addCharCallback(codePoint);
                            }
                        }

                        break;
                    }
                    case KeyRelease:
                    {
                        //detect key auto-repeat
                        if (XEventsQueued(display, QueuedAfterReading))
                        {
                            XEvent next{};
                            XPeekEvent(display, &next);

                            if (next.type == KeyPress
                                && next.xkey.time == event.xkey.time
                                && next.xkey.keycode == event.xkey.keycode)
                            {
                                break;
                            }
                        }

                        KeySym ks{};
                        char buffer[8];

                        XLookupString(
                            &event.xkey,
                            buffer,
                            sizeof(buffer),
                            &ks,
                            nullptr);

                        KeyboardButton key = TranslateKeySym(ks);

                        if (Input::IsVerboseLoggingEnabled())
                        {
                            Log::Print(
                                "Detected keyboard key '" + TranslateKeySymToString(ks) + "' up.",
                                "KW_MESSAGE_LOOP",
                                LogType::LOG_VERBOSE);
                        }

                        if (input)
                        {
                            input->SetKeyState(
                                key, 
                                false);
                        }

                        break;
                    }

                    case ButtonPress:
                    {
                        if (!input) break;

                        u32 btn = event.xbutton.button;
                        u32 time = event.xbutton.time;

                        bool doubleClick{};

                        if (btn <= 7)
                        {
                            if (time - lastClickTime[btn] <= DOUBLE_CLICK_TIME) doubleClick = true;

                            lastClickTime[btn] = time;
                        }

                        switch (btn)
                        {
                            case Button1:
                            {
                                input->SetMouseButtonState(
                                    MouseButton::M_LEFT, 
                                    true);

                                if (Input::IsVerboseLoggingEnabled())
                                {
                                    Log::Print(
                                        "Detected left mouse key down.",
                                        "KW_MESSAGE_LOOP",
                                        LogType::LOG_VERBOSE);
                                }

                                if (doubleClick)
                                {
                                    input->SetMouseButtonDoubleClickState(
                                        MouseButton::M_LEFT, 
                                        true);

                                    if (Input::IsVerboseLoggingEnabled())
                                    {
                                        Log::Print(
                                            "Detected left mouse key double click.",
                                            "KW_MESSAGE_LOOP",
                                            LogType::LOG_VERBOSE);
                                    }
                                }

                                break;
                            }
                            case Button3:
                            {
                                input->SetMouseButtonState(
                                    MouseButton::M_RIGHT, 
                                    true);

                                if (Input::IsVerboseLoggingEnabled())
                                {
                                    Log::Print(
                                        "Detected right mouse key down.",
                                        "KW_MESSAGE_LOOP",
                                        LogType::LOG_VERBOSE);
                                }

                                if (doubleClick)
                                {
                                    input->SetMouseButtonDoubleClickState(
                                        MouseButton::M_RIGHT, 
                                        true);

                                    if (Input::IsVerboseLoggingEnabled())
                                    {
                                        Log::Print(
                                            "Detected right mouse key double click.",
                                            "KW_MESSAGE_LOOP",
                                            LogType::LOG_VERBOSE);
                                    }
                                }

                                break;
                            }
                            case Button2:
                            {
                                input->SetMouseButtonState(
                                    MouseButton::M_MIDDLE, 
                                    true);

                                if (Input::IsVerboseLoggingEnabled())
                                {
                                    Log::Print(
                                        "Detected middle mouse key down.",
                                        "KW_MESSAGE_LOOP",
                                        LogType::LOG_VERBOSE);
                                }

                                if (doubleClick)
                                {
                                    input->SetMouseButtonDoubleClickState(
                                        MouseButton::M_MIDDLE, 
                                        true);

                                    if (Input::IsVerboseLoggingEnabled())
                                    {
                                        Log::Print(
                                            "Detected middle mouse key double click.",
                                            "KW_MESSAGE_LOOP",
                                            LogType::LOG_VERBOSE);
                                    }
                                }

                                break;
                            }

                            case Button4:
                            {
                                input->mouseWheelDelta += 1.0f;
                                break;
                            }
                            case Button5:
                            {
                                input->mouseWheelDelta -= 1.0f;
                                break;
                            }

                            default:
                            {
                                if (btn >= 8)
                                {
                                    u32 extra = btn - 8;

                                    if (extra == 0)
                                    {
                                        input->SetMouseButtonState(
                                            MouseButton::M_X1, 
                                            true);

                                        if (Input::IsVerboseLoggingEnabled())
                                        {
                                            Log::Print(
                                                "Detected x1 mouse key down.",
                                                "KW_MESSAGE_LOOP",
                                                LogType::LOG_VERBOSE);
                                        }

                                        if (doubleClick)
                                        {
                                            input->SetMouseButtonDoubleClickState(
                                                MouseButton::M_X1, 
                                                true);

                                            if (Input::IsVerboseLoggingEnabled())
                                            {
                                                Log::Print(
                                                    "Detected x1 mouse key double click.",
                                                    "KW_MESSAGE_LOOP",
                                                    LogType::LOG_VERBOSE);
                                            }
                                        }
                                    }
                                    else if (extra == 1)
                                    {
                                        input->SetMouseButtonState(
                                            MouseButton::M_X2, 
                                            true);

                                        if (Input::IsVerboseLoggingEnabled())
                                        {
                                            Log::Print(
                                                "Detected x2 mouse key down.",
                                                "KW_MESSAGE_LOOP",
                                                LogType::LOG_VERBOSE);
                                        }

                                        if (doubleClick)
                                        {
                                            input->SetMouseButtonDoubleClickState(
                                                MouseButton::M_X2, 
                                                true);

                                            if (Input::IsVerboseLoggingEnabled())
                                            {
                                                Log::Print(
                                                    "Detected x2 mouse key double click.",
                                                    "KW_MESSAGE_LOOP",
                                                    LogType::LOG_VERBOSE);
                                            }
                                        }
                                    }
                                }
                            }
                        }

                        break;
                    }
                    case ButtonRelease:
                    {
                        if (!input) break;

                        u32 btn = event.xbutton.button;

                        switch (btn)
                        {
                            case Button1:
                            {
                                input->SetMouseButtonState(
                                    MouseButton::M_LEFT, 
                                    false);

                                if (Input::IsVerboseLoggingEnabled())
                                {
                                    Log::Print(
                                        "Detected left mouse key up.",
                                        "KW_MESSAGE_LOOP",
                                        LogType::LOG_VERBOSE);
                                }

                                break;
                            }
                            case Button3:
                            {
                                input->SetMouseButtonState(
                                    MouseButton::M_RIGHT, 
                                    false);

                                if (Input::IsVerboseLoggingEnabled())
                                {
                                    Log::Print(
                                        "Detected right mouse key up.",
                                        "KW_MESSAGE_LOOP",
                                        LogType::LOG_VERBOSE);
                                }

                                break;
                            }
                            case Button2:
                            {
                                input->SetMouseButtonState(
                                    MouseButton::M_MIDDLE, 
                                    false);

                                if (Input::IsVerboseLoggingEnabled())
                                {
                                    Log::Print(
                                        "Detected middle mouse key up.",
                                        "KW_MESSAGE_LOOP",
                                        LogType::LOG_VERBOSE);
                                }

                                break;
                            }

                            default:
                            {
                                if (btn >= 8)
                                {
                                    u32 extra = btn - 8;

                                    if (extra == 0)
                                    {
                                        input->SetMouseButtonState(
                                            MouseButton::M_X1, 
                                            false);

                                        if (Input::IsVerboseLoggingEnabled())
                                        {
                                            Log::Print(
                                                "Detected x1 mouse key up.",
                                                "KW_MESSAGE_LOOP",
                                                LogType::LOG_VERBOSE);
                                        }
                                    }
                                    else if (extra == 1)
                                    {
                                        input->SetMouseButtonState(
                                            MouseButton::M_X2, 
                                            false);

                                        if (Input::IsVerboseLoggingEnabled())
                                        {
                                            Log::Print(
                                                "Detected x2 mouse key up.",
                                                "KW_MESSAGE_LOOP",
                                                LogType::LOG_VERBOSE);
                                        }
                                    }
                                }
                            }
                        }

                        break;
                    }

                    case MotionNotify:
                    {
                        vec2 newPos =
                        {
                            f32(event.xmotion.x),
                            f32(event.xmotion.y)
                        };

                        if (input)
                        {
                            //get the old position before updating
                            vec2 oldPos = input->GetMousePosition();

                            vec2 delta =
                            {
                                newPos.x - oldPos.x,
                                newPos.y - oldPos.y
                            };

                            input->mousePos = newPos;
                            input->mouseDelta = delta;

                            if (Input::IsVerboseLoggingEnabled())
                            {
                                Log::Print(
                                    "Mouse delta: " + to_string(delta.x) + ", " + to_string(delta.y),
                                    "KW_MESSAGE_LOOP",
                                    LogType::LOG_VERBOSE);
                            }
                        }

                        break;
                    }
                }

                //bump this frame, resize next frame
                if (w->delayedMaximizeRestoreCounterStart)
                {
                    if (w->delayedMaximizeRestoreCounter == 0)
                    {
                        //Log::Print("@@@@@ delayed resize callback...");

                        w->delayedMaximizeRestoreCounter++;
                    }
                    else
                    {
                        //Log::Print("@@@@@ resize callback delay triggered resize callback...");

                        w->delayedMaximizeRestoreCounterStart = false;
                        if (w->resizeCallback) w->resizeCallback(true);

                        if (w->configureNotifyRequestedResizeCallback)
                        {
                            //Log::Print("@@@@@ ConfigureNotify triggered resize callback alongside delayed resize callback...");

                            if (w->resizeCallback) w->resizeCallback(true);
                            w->configureNotifyRequestedResizeCallback = false;
                        }
                    }
                }

                break;
            }
        }
    }
}

#endif //KLIN_ANY