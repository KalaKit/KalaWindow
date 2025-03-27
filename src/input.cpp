//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

//main log macro
#define WRITE_LOG(type, msg) std::cout << "[KALAKIT_INPUT | " << type << "] " << msg << "\n"

//log types
#if KALAWINDOW_DEBUG
	#define LOG_DEBUG(msg) WRITE_LOG("DEBUG", msg)
#else
	#define LOG_DEBUG(msg)
#endif
#define LOG_SUCCESS(msg) WRITE_LOG("SUCCESS", msg)
#define LOG_ERROR(msg) WRITE_LOG("ERROR", msg)

#include <vector>
#include <type_traits>
#include <Windows.h>
#include <TlHelp32.h>
#ifndef GET_X_LPARAM
	#define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#endif
#ifndef GET_Y_LPARAM
	#define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))
#endif

#include "input.hpp"
#include "window.hpp"
#include "magic_enum.hpp"

using std::vector;
using std::to_string;
using std::next;

namespace KalaKit
{
	void KalaInput::Initialize()
	{
		if (isInitialized)
		{
			LOG_ERROR("InputSystem is already initialized!");
			return;
		}

		if (!window)
		{
			window = GetMainWindowHandle();
			if (!window)
			{
				LOG_ERROR("No window found! Make sure to call Initialize after creating your window, or attach one manually using SetWindow.");
				return;
			}
			else
			{
				char title[256];
				GetWindowTextA(window, title, sizeof(title));
				LOG_SUCCESS("Hooked onto window : " << title);
			}
		}

		//
		// FINAL INITIALIZATION STEPS FOR RAW INPUT
		//

		proc = (WNDPROC)SetWindowLongPtr(
			window,
			GWLP_WNDPROC,
			(LONG_PTR)WindowProcCallback);

		RAWINPUTDEVICE rid{};
		rid.usUsagePage = 0x01;
		rid.usUsage = 0x02;
		rid.dwFlags = RIDEV_INPUTSINK;
		rid.hwndTarget = window;

		RegisterRawInputDevices(&rid, 1, sizeof(rid));

		isInitialized = true;
	}

	bool KalaInput::IsKeyHeld(Key key)
	{
		if (!isInitialized)
		{
			LOG_ERROR("Cannot get key down because InputSystem is not initialized!");
			return false;
		}

		bool isKeyDown = keyHeld[key];

		DebugType type = KalaWindow::GetDebugType();
		if (type == DebugType::DEBUG_ALL
			|| type == DebugType::DEBUG_KEY_HELD)
		{
			//only print if key is down
			if (isKeyDown)
			{
				string keyName = ToString(key);
				LOG_DEBUG("Key '" << keyName << "' is down");
			}
		}

		return isKeyDown;
	}

	bool KalaInput::IsKeyPressed(Key key)
	{
		if (!isInitialized)
		{
			LOG_ERROR("Cannot get key pressed because InputSystem is not initialized!");
			return false;
		}

		bool wasKeyPressed = keyPressed[key];

		DebugType type = KalaWindow::GetDebugType();
		if (type == DebugType::DEBUG_ALL
			|| type == DebugType::DEBUG_KEY_PRESSED)
		{
			//only print if key was pressed
			if (wasKeyPressed)
			{
				string keyName = ToString(key);
				LOG_DEBUG("Pressed key '" << keyName << "'.");
			}
		}

		return wasKeyPressed;
	}

	bool KalaInput::IsComboPressed(initializer_list<Key> keys)
	{
		if (!isInitialized)
		{
			LOG_ERROR("Cannot get key combo pressed because InputSystem is not initialized!");
			return false;
		}

		//cannot proceed if only one key is assigned
		if (keys.size() < 2) return false;

		auto it = keys.begin();
		for (size_t i = 0; i < keys.size() - 1; i++)
		{
			if (!IsKeyHeld(*it++)) return false;
		}

		bool wasComboPressed = IsKeyPressed(*it);

		DebugType type = KalaWindow::GetDebugType();
		if (type == DebugType::DEBUG_ALL
			|| type == DebugType::DEBUG_COMBO_PRESSED)
		{
			//only print if combo was pressed
			if (wasComboPressed)
			{
				string comboString{};

				for (auto k = keys.begin(); k != keys.end(); ++k)
				{
					comboString += ToString(*k);
					if (next(k) != keys.end())
					{
						comboString += " + ";
					}
				}

				LOG_DEBUG("Pressed combo '" << comboString << "'.");
			}
		}

		//true as soon as last key is pressed
		return wasComboPressed;
	}

	bool KalaInput::IsMouseKeyDoubleClicked()
	{
		if (!isInitialized)
		{
			LOG_ERROR("Cannot get mouse key double clicked because InputSystem is not initialized!");
			return false;
		}

		bool wasDoubleClicked = 
			keyPressed[Key::MouseLeft]
			|| keyPressed[Key::MouseRight];

		DebugType type = KalaWindow::GetDebugType();
		if (type == DebugType::DEBUG_ALL
			|| type == DebugType::DEBUG_DOUBLE_CLICKED)
		{
			//only print if double clicked
			if (wasDoubleClicked)
			{
				LOG_DEBUG("Double clicked left mouse button.");
			}
		}

		return wasDoubleClicked;
	}

	bool KalaInput::IsMouseDragging()
	{
		if (!isInitialized)
		{
			LOG_ERROR("Cannot get mouse drag state because InputSystem is not initialized!");
			return false;
		}

		bool isHoldingDragKey =
			keyHeld[Key::MouseLeft]
			|| keyHeld[Key::MouseRight];

		bool isDragging = 
			isHoldingDragKey
			&& (mouseDelta.x != 0
			|| mouseDelta.y != 0);

		DebugType type = KalaWindow::GetDebugType();
		if (type == DebugType::DEBUG_ALL
			|| type == DebugType::DEBUG_IS_MOUSE_DRAGGING)
		{
			//only print if dragging
			if (isDragging)
			{
				LOG_DEBUG("Dragging mouse while holding left mouse button.");
			}
		}

		return isDragging;
	}

	POINT KalaInput::GetMousePosition()
	{
		if (!isInitialized)
		{
			LOG_ERROR("Cannot get mouse position because InputSystem is not initialized!");
			return { 0, 0 };
		}

		DebugType type = KalaWindow::GetDebugType();
		if (type == DebugType::DEBUG_ALL
			|| type == DebugType::DEBUG_MOUSE_POSITION)
		{
			static POINT lastPos = { -9999, -9999 };

			//only print if mouse moved
			if (mousePosition.x != lastPos.x
				|| mousePosition.y != lastPos.y)
			{
				LOG_DEBUG("Mouse position: (" 
					<< mousePosition.x << ", " 
					<< mousePosition.y << ")");

				lastPos.x = mousePosition.x;
				lastPos.y = mousePosition.y;
			}
		}

		return mousePosition;
	}

	POINT KalaInput::GetMouseDelta()
	{
		if (!isInitialized)
		{
			LOG_ERROR("Cannot get mouse delta because InputSystem is not initialized!");
			return { 0, 0 };
		}

		DebugType type = KalaWindow::GetDebugType();
		if (type == DebugType::DEBUG_ALL
			|| type == DebugType::DEBUG_MOUSE_DELTA)
		{
			//only print if mouse moved
			if (mouseDelta.x != 0
				|| mouseDelta.y != 0)
			{
				LOG_DEBUG("Mouse delta: (" 
					<< mouseDelta.x << ", " 
					<< mouseDelta.y << ")");
			}
		}

		return mouseDelta;
	}

	POINT KalaInput::GetRawMouseDelta()
	{
		if (!isInitialized)
		{
			LOG_ERROR("Cannot get mouse raw delta because InputSystem is not initialized!");
			return { 0, 0 };
		}

		DebugType type = KalaWindow::GetDebugType();
		if (type == DebugType::DEBUG_ALL
			|| type == DebugType::DEBUG_RAW_MOUSE_DELTA)
		{
			//only print if mouse moved
			if (rawMouseDelta.x != 0
				|| rawMouseDelta.y != 0)
			{
				LOG_DEBUG("Raw mouse delta: (" 
					<< rawMouseDelta.x << ", " 
					<< rawMouseDelta.y << ")");
			}
		}

		return rawMouseDelta;
	}

	int KalaInput::GetMouseWheelDelta()
	{
		if (!isInitialized)
		{
			LOG_ERROR("Cannot get mouse delta because InputSystem is not initialized!");
			return 0;
		}

		DebugType type = KalaWindow::GetDebugType();
		if (type == DebugType::DEBUG_ALL
			|| type == DebugType::DEBUG_MOUSE_WHEEL_DELTA)
		{
			LOG_DEBUG("Mouse wheel delta: " << to_string(mouseWheelDelta) << "");
		}

		return mouseWheelDelta;
	}

	bool KalaInput::IsMouseVisible()
	{
		if (!isInitialized)
		{
			LOG_ERROR("Cannot get mouse visible state because InputSystem is not initialized!");
			return false;
		}

		return isMouseVisible;
	}
	void KalaInput::SetMouseVisibility(bool newMouseVisibleState)
	{
		if (!isInitialized)
		{
			LOG_ERROR("Cannot set mouse visible state because InputSystem is not initialized!");
			return;
		}

		//ignores same state
		if (isMouseVisible == newMouseVisibleState) return;

		DebugType type = KalaWindow::GetDebugType();
		if (type == DebugType::DEBUG_ALL
			|| type == DebugType::DEBUG_MOUSE_VISIBILITY)
		{
			string type = newMouseVisibleState ? "true" : "false";
			LOG_DEBUG("Mouse wheel visibility: " << type << "");
		}

		isMouseVisible = newMouseVisibleState;

		if (isMouseVisible) while (ShowCursor(TRUE) < 0); //increment until visible
		else while (ShowCursor(FALSE) >= 0);              //decrement until hidden
	}

	bool KalaInput::IsMouseLocked()
	{
		if (!isInitialized)
		{
			LOG_ERROR("Cannot get mouse locked state because InputSystem is not initialized!");
			return false;
		}

		return isMouseLocked;
	}
	void KalaInput::SetMouseLockState(bool newMouseLockState)
	{
		if (!isInitialized)
		{
			LOG_ERROR("Cannot set mouse lock state because InputSystem is not initialized!");
			return;
		}

		//ignores same state
		if (isMouseLocked == newMouseLockState) return;

		DebugType type = KalaWindow::GetDebugType();
		if (type == DebugType::DEBUG_ALL
			|| type == DebugType::DEBUG_MOUSE_LOCK_STATE)
		{
			string type = newMouseLockState ? "true" : "false";
			LOG_DEBUG("Mouse wheel lock state: " << type << "");
		}

		isMouseLocked = newMouseLockState;
		if (!isMouseLocked) ClipCursor(nullptr);
		else
		{
			LockCursorToCenter();

			//clip the cursor to the window to prevent it from leaving
			RECT rect{};
			GetClientRect(window, &rect);
			ClientToScreen(window, (POINT*)&rect.left);
			ClientToScreen(window, (POINT*)&rect.right);
			ClipCursor(&rect);
		}
	}

	void KalaInput::SetExitState(
		bool setExitAllowedState,
		const string& title,
		const string& info)
	{
		if (!isInitialized)
		{
			LOG_ERROR("Cannot set exit state because InputSystem is not initialized!");
			return;
		}

		canExit = setExitAllowedState;
		exitTitle = title;
		exitInfo = info;
	}

	bool KalaInput::ShouldClose()
	{
		if (!isInitialized)
		{
			LOG_ERROR("Cannot get should close state because InputSystem is not initialized!");
			return false;
		}

		return shouldClose;
	}
	void KalaInput::SetShouldCloseState(bool newShouldCloseState)
	{
		if (!isInitialized)
		{
			LOG_ERROR("Cannot set should close state because InputSystem is not initialized!");
			return;
		}

		DebugType type = KalaWindow::GetDebugType();
		if (type == DebugType::DEBUG_ALL
			|| type == DebugType::DEBUG_WINDOW_SHOULD_CLOSE)
		{
			string type = newShouldCloseState ? "true" : "false";
			LOG_DEBUG("Window should close state: " << type << "");
		}

		//ignore same value
		if (shouldClose == newShouldCloseState) return;

		shouldClose = newShouldCloseState;
	}

	string KalaInput::ToString(Key key)
	{
		return string(magic_enum::enum_name(key));
	}

	void KalaInput::SetWindowFocusRequiredState(bool newWindowFocusRequiredState)
	{
		if (!isInitialized)
		{
			LOG_ERROR("Cannot set window focus required state because InputSystem is not initialized!");
			return;
		}

		isWindowFocusRequired = newWindowFocusRequiredState;
	}

	void KalaInput::Update()
	{
		if (!isInitialized)
		{
			LOG_ERROR("Cannot run loop because InputSystem is not initialized!");
			return;
		}

		//prevent updates when not focused
		if (isWindowFocusRequired
			&& GetForegroundWindow() != window)
		{
			return;
		}

		MSG msg;
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg); //translate virtual-key messages (like WM_KEYDOWN) to character messages (WM_CHAR)
			DispatchMessage(&msg);  //send the message to the window procedure
		}

		//ensures cursor stays locked every frame
		if (isMouseLocked) LockCursorToCenter();

		ResetFrameInput();
	}

	void KalaInput::ResetFrameInput()
	{
		//clear "pressed" keys after each frame
		for (auto& [key, pressed] : keyPressed)
		{
			pressed = false;
		}

		if (!IsMouseDragging())
		{
			mouseDelta = { 0, 0 };
			rawMouseDelta = { 0, 0 };
			mouseWheelDelta = 0;
		}
	}

	HWND KalaInput::GetWindow()
	{
		return window;
	}
	void KalaInput::SetWindow(HWND newWindow)
	{
		window = newWindow;

		char title[256];
		GetWindowTextA(window, title, sizeof(title));
		LOG_SUCCESS("Added window manually: " << title);
	}

	BOOL CALLBACK KalaInput::EnumWindowsCallback(HWND hwnd, LPARAM lParam)
	{
		DWORD windowPID = 0;
		GetWindowThreadProcessId(hwnd, &windowPID);

		DWORD currentPID = GetCurrentProcessId();

		//checks if the window belongs to this process and is visible
		if (windowPID == currentPID
			&& IsWindowVisible(hwnd))
		{
			*reinterpret_cast<HWND*>(lParam) = hwnd;
			return false;
		}
		return true;
	}

	HWND KalaInput::GetMainWindowHandle()
	{
		HWND hwnd = nullptr;
		EnumWindows(EnumWindowsCallback, reinterpret_cast<LPARAM>(&hwnd));
		return hwnd;
	}

	LRESULT CALLBACK KalaInput::WindowProcCallback(
		HWND hwnd,
		UINT msg,
		WPARAM wParam,
		LPARAM lParam)
	{
		MSG msgObj{};
		msgObj.hwnd = hwnd;
		msgObj.message = msg;
		msgObj.wParam = wParam;
		msgObj.lParam = lParam;

		bool handled = ProcessMessage(msgObj);

		//tell windows this message is fully handled
		if (handled) return 0;

		return CallWindowProc(proc, hwnd, msg, wParam, lParam);
	}

	void KalaInput::LockCursorToCenter()
	{
		RECT rect{};
		GetClientRect(window, &rect);

		POINT center{};
		center.x = (rect.right - rect.left) / 2;
		center.y = (rect.bottom - rect.top) / 2;

		ClientToScreen(window, &center);
		SetCursorPos(center.x, center.y);
	}

	bool KalaInput::AllowExit()
	{
		int result = MessageBox(
			nullptr,
			exitInfo.c_str(),
			exitTitle.c_str(),
			MB_YESNO
			| MB_ICONWARNING);

		return result == IDYES;
	}

	bool KalaInput::ProcessMessage(const MSG& msg)
	{
		DebugType type = KalaWindow::GetDebugType();
		if (type == DebugType::DEBUG_ALL
			|| type == DebugType::DEBUG_PROCESS_MESSAGE_TEST)
		{
			if (msg.message == 0)
			{
				LOG_DEBUG("Received empty or WM_NULL message.");
			}
			else
			{
				LOG_DEBUG("Got message: " << msg.message);
			}
		}

		switch (msg.message)
		{

		//
		// KEYBOARD INPUT
		//

		case WM_KEYDOWN:
		{
			Key key = static_cast<Key>(msg.wParam);
			if (!keyHeld[key]) keyPressed[key] = true;
			keyHeld[key] = true;
			return false;
		}
		case WM_KEYUP:
		{
			Key key = static_cast<Key>(msg.wParam);
			keyPressed[key] = false;
			keyHeld[key] = false;
			return false;
		}

		//
		// MOUSE MOVE
		//

		case WM_MOUSEMOVE:
		{
			POINT newPos =
			{
				GET_X_LPARAM(msg.lParam),
				GET_Y_LPARAM(msg.lParam)
			};
			mouseDelta.x = newPos.x - mousePosition.x;
			mouseDelta.y = newPos.y - mousePosition.y;
			mousePosition = newPos;
			return false;
		}

		//
		// MOUSE WHEEL
		//

		case WM_MOUSEWHEEL:
		{
			int delta = GET_WHEEL_DELTA_WPARAM(msg.wParam);
			if (delta > 0) mouseWheelDelta += 1;
			else if (delta < 0) mouseWheelDelta -= 1;
			return false;
		}

		//
		// MOUSE DOUBLE CLICK
		//

		case WM_LBUTTONDBLCLK:
		{
			SetMouseKeyState(Key::MouseLeft, true);
			keyPressed[Key::MouseLeft] = true;
			return false;
		}
		case WM_RBUTTONDBLCLK:
		{
			SetMouseKeyState(Key::MouseRight, true);
			keyPressed[Key::MouseRight] = true;
			return false;
		}

		//
		// MOUSE BUTTONS
		//

		case WM_LBUTTONDOWN: SetMouseKeyState(Key::MouseLeft, true); return false;
		case WM_LBUTTONUP: SetMouseKeyState(Key::MouseLeft, false); return false;

		case WM_RBUTTONDOWN: SetMouseKeyState(Key::MouseRight, true); return false;
		case WM_RBUTTONUP: SetMouseKeyState(Key::MouseRight, false); return false;

		case WM_MBUTTONDOWN: SetMouseKeyState(Key::MouseMiddle, true); return false;
		case WM_MBUTTONUP: SetMouseKeyState(Key::MouseMiddle, false); return false;

		case WM_XBUTTONDOWN:
		{
			WORD button = GET_XBUTTON_WPARAM(msg.wParam);
			if (button == XBUTTON1) SetMouseKeyState(Key::MouseX1, true);
			if (button == XBUTTON2) SetMouseKeyState(Key::MouseX2, true);
			return false;
		}
		case WM_XBUTTONUP:
		{
			WORD button = GET_XBUTTON_WPARAM(msg.wParam);
			if (button == XBUTTON1) SetMouseKeyState(Key::MouseX1, false);
			if (button == XBUTTON2) SetMouseKeyState(Key::MouseX2, false);
			return false;
		}

		//
		// RAW MOUSE INPUT FOR EXTRA BUTTONS
		//

		case WM_INPUT:
		{
			UINT size = 0;
			GetRawInputData(
				(HRAWINPUT)msg.lParam,
				RID_INPUT,
				nullptr,
				&size,
				sizeof(RAWINPUTHEADER));

			vector<BYTE> buffer(size);
			if (GetRawInputData(
				(HRAWINPUT)msg.lParam,
				RID_INPUT,
				buffer.data(),
				&size,
				sizeof(RAWINPUTHEADER)) != size)
			{
				return false;
			}

			RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(buffer.data());

			if (raw->header.dwType == RIM_TYPEMOUSE)
			{
				const RAWMOUSE& mouse = raw->data.mouse;

				//add support for up to 8 extra mouse buttons
				for (int i = 0; i < 8; i++)
				{
					USHORT downFlag = RI_MOUSE_BUTTON_1_DOWN << (i * 2);
					USHORT upFlag = RI_MOUSE_BUTTON_1_UP << (i * 2);

					if (mouse.usButtonFlags & downFlag)
					{
						SetMouseKeyState(static_cast<Key>(static_cast<int>(Key::MouseX3) + i), true);
					}
					if (mouse.usButtonFlags & upFlag)
					{
						SetMouseKeyState(static_cast<Key>(static_cast<int>(Key::MouseX3) + i), false);
					}
				}

				//sets raw mouse movement
				if (mouse.usFlags == MOUSE_MOVE_RELATIVE)
				{
					rawMouseDelta.x += mouse.lLastX;
					rawMouseDelta.y += mouse.lLastY;
				}
			}

			return false;
		}

		//
		// WINDOW REDRAW
		//

		case WM_PAINT:
		{
			PAINTSTRUCT ps{};
			HDC hdc = BeginPaint(window, &ps);

			int R = 255;
			int G = 0;
			int B = 0;
			HBRUSH redBrush = CreateSolidBrush(RGB(R, G, B));
			FillRect(hdc, &ps.rcPaint, redBrush);
			DeleteObject(redBrush);

			EndPaint(window, &ps);

			if (type == DebugType::DEBUG_ALL
				|| type == DebugType::DEBUG_WINDOW_REPAINT)
			{
				LOG_DEBUG("New color: " << R << ", " << G << ", " << B);
			}

			return true;
		}

		//
		// WINDOW RESIZE
		//

		case WM_SIZE:
		{
			//trigger a redraw
			InvalidateRect(window, nullptr, TRUE);

			if (type == DebugType::DEBUG_ALL
				|| type == DebugType::DEBUG_WINDOW_RESIZE)
			{
				RECT rect{};
				GetClientRect(window, &rect);

				POINT size{};
				size.x = rect.right - rect.left;
				size.y = rect.bottom - rect.top;

				LOG_DEBUG("New resolution: " << size.x << ", " << size.y);
			}

			return false;
		}

		//
		// SHUTDOWN
		//

		//user clicked X button or pressed Alt + F4
		case WM_CLOSE:
			LOG_DEBUG("Called close message.");
			if (!canExit)
			{
				//returns true if uses chooses to exit, otherwise cancels exit
				if (!AllowExit()) return true;
			}

			//signals the while loop to exit
			shouldClose = true;

			//if user agrees to exit, then the window is destroyed
			DestroyWindow(window);
			return true;

			//window was destroyed - tell the system to exit
		case WM_DESTROY:
			LOG_DEBUG("Called destroy message.");
			PostQuitMessage(0);
			return true;

		default:
			return false;
		}

		return false;
	}
}