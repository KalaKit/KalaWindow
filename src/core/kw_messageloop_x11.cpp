//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef __linux__

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/extensions/XI2.h>
#include <X11/extensions/XInput2.h>
#include <X11/keysym.h>
#include <X11/Xutil.h>

#include "glcorearb.h"

#include <vector>
#include <unordered_map>
#include <string>
#include <functional>

#include "core_utils.hpp"
#include "math_utils.hpp"
#include "log_utils.hpp"
#include "key_standards.hpp"

#include "core/kw_messageloop_x11.hpp"
#include "core/kw_registry.hpp"
#include "core/kw_core.hpp"
#include "core/kw_input.hpp"
#include "graphics/kw_window_global.hpp"
#include "graphics/kw_window.hpp"
#include "opengl/kw_opengl.hpp"
#include "opengl/kw_opengl_functions_core.hpp"

using KalaWindow::Core::Input;
using KalaWindow::Graphics::Window_Global;
using KalaWindow::Graphics::X11GlobalData;
using KalaWindow::Graphics::ProcessWindow;
using KalaWindow::Graphics::KalaWindowRegistry;
using KalaWindow::Graphics::WindowData;
using KalaWindow::OpenGL::OpenGL_Global;
using KalaWindow::OpenGL::OpenGLFunctions::GL_Core;
using KalaWindow::OpenGL::OpenGLFunctions::OpenGL_Functions_Core;

using KalaHeaders::KalaCore::ToVar;

using KalaHeaders::KalaMath::vec2;

using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;

using KalaHeaders::KalaKeyStandards::KeyboardButton;
using KalaHeaders::KalaKeyStandards::GetValueByKey;

using std::vector;
using std::unordered_map;
using std::string;
using std::to_string;
using std::function;

static function<void(u32)> addCharCallback{};
static function<void()> removeFromBackCallback{};
static function<void()> addTabCallback{};
static function<void()> addNewlineCallback{};

constexpr u32 DOUBLE_CLICK_TIME = 500;
static u32 lastClickTime[8]{};

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
    void MessageLoop::SetAddCharCallback(const function<void(u32)>& newCallback)
	{
		addCharCallback = newCallback;
	}
	void MessageLoop::SetRemoveFromBackCallback(const function<void()>& newCallback)
	{
		removeFromBackCallback = newCallback;
	}
	void MessageLoop::SetAddTabCallback(const function<void()>& newCallback)
	{
		addTabCallback = newCallback;
	}
	void MessageLoop::SetAddNewLineCallback(const function<void()>& newCallback)
	{
		addNewlineCallback = newCallback;
	}

    void MessageLoop::Update()
    {
        const X11GlobalData& globalData = Window_Global::GetGlobalData();

        if (!globalData.display)
        {
            KalaWindowCore::ForceClose(
                "Window error",
                "Failed to update message loop because the attached display was invalid!");
        }

        Display* display = ToVar<Display*>(globalData.display);

        while (XPending(display))
        {
            XEvent event{};
            XNextEvent(display, &event);

            DispatchEvents(event);
        }
    }

    void MessageLoop::DispatchEvents(XEvent& event)
    {
        const X11GlobalData& globalData = Window_Global::GetGlobalData();

        const vector<ProcessWindow*>& activeWindows = KalaWindowRegistry<ProcessWindow>::runtimeContent;

        Display* display = ToVar<Display*>(globalData.display);

        if (event.type == GenericEvent)
        {
            if (event.xcookie.extension == globalData.xiOpcode
                && XGetEventData(display, &event.xcookie))
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

                    for (const auto& w : activeWindows)
                    {
                        if (!w
                            || !w->IsInitialized())
                        {
                            continue;
                        }

                        u32 windowID = w->GetID();

                        vector<Input*> inputs = KalaWindowRegistry<Input>::GetAllWindowContent(windowID);
                        Input* input = inputs.empty() ? nullptr : inputs.front();

                        if (!input) continue;

                        vec2 delta = input->GetRawMouseDelta();
                        delta.x += (f32)dx;
                        delta.y += (f32)dy;
                        input->SetRawMouseDelta(delta);

                        if (Input::IsVerboseLoggingEnabled())
                        {
                            Log::Print("Raw mouse delta: " + to_string(dx) + ", " + to_string(dy));
                        }
                    }
                }

                XFreeEventData(display, &event.xcookie);
            }

            return;
        }

        Atom atom_wm_delete = ToVar<Atom>(globalData.atom_wm_delete);

        Atom atom_net_wm_state = ToVar<Atom>(globalData.atom_net_wm_state);

        Window target = event.xany.window;

        for (const auto& w : activeWindows)
        {
            if (!w
                || !w->IsInitialized())
            {
                continue;
            }

            const WindowData& wdata = w->GetWindowData();

			if (!wdata.window)
			{
				KalaWindowCore::ForceClose(
					"Window error",
					"Failed to update message loop because the attached window was invalid!");
			}

            Window window = ToVar<Window>(wdata.window);

            if (target != window) continue;

            u32 windowID = w->GetID();
            vector<Input*> inputs = KalaWindowRegistry<Input>::GetAllWindowContent(windowID);
            Input* input = inputs.empty() ? nullptr : inputs.front();

            if (!input) continue;

            XIC xic = ToVar<XIC>(wdata.xic);

            switch (event.type)
            {
                case ConfigureNotify:
                {
                    vec2 newPos = vec2(event.xconfigure.x, event.xconfigure.y);
                    vec2 newSize = vec2(event.xconfigure.width, event.xconfigure.height);

                    vec2 oldSize = w->size;

                    w->pos = newPos;
                    w->size = newSize;

                    if (w->IsResizable()
                        && oldSize != newSize)
                    {
                        if (OpenGL_Global::IsInitialized())
                        {
                            const GL_Core* coreFunc = OpenGL_Functions_Core::GetGLCore();

                            coreFunc->glViewport(
                                0,
                                0,
                                (GLsizei)newSize.x,
					            (GLsizei)newSize.y);
                        }

                        w->TriggerResize();
                        w->TriggerRedraw();
                    }

                    break;
                }

                case Expose:
                {
                    if (event.xexpose.count == 0) w->TriggerRedraw();
                    break;
                }

                case ClientMessage:
                {
                    if ((Atom)event.xclient.data.l[0] == atom_wm_delete)
                    {
                        w->CloseWindow();

                        if (ProcessWindow::GetRegistry().runtimeContent.empty())
                        {
                            KalaWindowCore::Shutdown();
                        }
                    }
                    break;
                }
                case DestroyNotify: break;

                case PropertyNotify:
                {
                    if (event.xproperty.atom == atom_net_wm_state) w->UpdateFullscreenState();
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
                    Status status{};

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
                            "INPUT",
                            LogType::LOG_INFO);
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
                            "INPUT",
                            LogType::LOG_INFO);
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
                                    "INPUT",
                                    LogType::LOG_INFO);
                            }

                            if (doubleClick)
                            {
                                input->SetMouseButtonDoubleClickState(
                                    MouseButton::M_LEFT, 
                                    true);

                                Log::Print(
                                    "Detected left mouse key double click.",
                                    "INPUT",
                                    LogType::LOG_INFO);
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
                                    "INPUT",
                                    LogType::LOG_INFO);
                            }

                            if (doubleClick)
                            {
                                input->SetMouseButtonDoubleClickState(
                                    MouseButton::M_RIGHT, 
                                    true);

                                Log::Print(
                                    "Detected right mouse key double click.",
                                    "INPUT",
                                    LogType::LOG_INFO);
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
                                    "INPUT",
                                    LogType::LOG_INFO);
                            }

                            if (doubleClick)
                            {
                                input->SetMouseButtonDoubleClickState(
                                    MouseButton::M_MIDDLE, 
                                    true);

                                Log::Print(
                                    "Detected middle mouse key double click.",
                                    "INPUT",
                                    LogType::LOG_INFO);
                            }

                            break;
                        }

                        case Button4:
                        {
                            input->SetScrollwheelDelta(+1.0f);
                            break;
                        }
                        case Button5:
                        {
                            input->SetScrollwheelDelta(-1.0f);
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
                                            "INPUT",
                                            LogType::LOG_INFO);
                                    }

                                    if (doubleClick)
                                    {
                                        input->SetMouseButtonDoubleClickState(
                                            MouseButton::M_X1, 
                                            true);

                                        Log::Print(
                                            "Detected x1 mouse key double click.",
                                            "INPUT",
                                            LogType::LOG_INFO);
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
                                            "INPUT",
                                            LogType::LOG_INFO);
                                    }

                                    if (doubleClick)
                                    {
                                        input->SetMouseButtonDoubleClickState(
                                            MouseButton::M_X2, 
                                            true);

                                        Log::Print(
                                            "Detected x2 mouse key double click.",
                                            "INPUT",
                                            LogType::LOG_INFO);
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
                                    "INPUT",
                                    LogType::LOG_INFO);
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
                                    "INPUT",
                                    LogType::LOG_INFO);
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
                                    "INPUT",
                                    LogType::LOG_INFO);
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
                                            "INPUT",
                                            LogType::LOG_INFO);
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
                                            "INPUT",
                                            LogType::LOG_INFO);
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

                        input->SetMousePosition(newPos);
                        input->SetMouseDelta(delta);

                        if (Input::IsVerboseLoggingEnabled())
                        {
                            Log::Print("Mouse delta: " + to_string(delta.x) + ", " + to_string(delta.y));
                        }
                    }

                    break;
                }
            }

            break;
        }
    }
}

#endif //__linux__