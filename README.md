# KalaWindow

[![License](https://img.shields.io/badge/license-Zlib-blue)](LICENSE.md)
![Platform](https://img.shields.io/badge/platform-Windows-brightgreen)
![Development Stage](https://img.shields.io/badge/development-Alpha-yellow)

![Logo](logo.png)

KalaWindow is a lightweight C++ 20 library for Windows that is used for rendering the window your program will be ran inside of and handling all of its input. It also comes with KalaCrashHandler natively built in for handy crash reports.

# Prerequisites (when compiling from source code)

- Visual Studio 2022 (with C++ CMake tools and Windows 10 or 11 SDK)
- Ninja and CMake 3.30.3 or newer (or extract Windows_prerequsites.7z and run setup.bat)

To compile from source code simply run 'build_windows_release.bat' or 'build_windows_debug.bat' depending on your preferences then copy and attach the dll, lib and header files with your preferred way to your program source directory.

# How to set up

```cpp
#include <Windows.h> //used for all Windows input and window operations
#include <string>

#include "window.hpp"
#include "input.hpp"

using std::string;

using KalaKit::KalaWindow;  //core window and input system functions
using KalaKit::Key;         //enum for all keyboard and mouse keys
using KalaKit::DebugType;   //enum for all debug types
using KalaKit::WindowState; //enum for all window states

static void YourInitializeFunction()
{
	...
	//this function creates a window and displays it
	int yourWindowHeight = 800;
	int yourWindowWidth = 600;
	string yourWindowTitle = "Your window";
	KalaWindow::Initialize(yourWindowTitle, yourWindowWidth, yourWindowHeight);

	//this is the core initialization function 
	//that is required to be called so that
	//the window system can initialize all its content
	KalaWindow::Initialize();
	
	//make sure to also initialize the input system after it
	KalaInput::Initialize();

	//you can pass one of the many debug types to this function
	//to be able to see messages of that debug type printed to your console,
	//the default DEBUG_NONE does nothing and if you dont want 
	//debug messages then you dont need to call this function
	KalaWindow::SetDebugState(DebugType::DEBUG_NONE);
	
	//you can pass bool true or false to this function,
	//it sets the window focus required state, which controls
	//whether the attached window needs to be in focus for
	//any input to be registred at all for KalaWindow.
	//it defaults to true, so this function does not
	//need to be called if you want focus to always be required
	KalaWindow::SetWindowFocusRequiredState(true);
	
	//set this function to false and assign a title and info
	//if you want to prevent the user from exiting your program.
	//setting this to false shows a warning popup with yes or no
	//and your title and info. if user presses yes the program can close,
	//if user presses no then the program stays open and the popup closes
	bool yourExitState = false;
	string yourTitle = "this shows up as the title!";
	string yourInfo = "this shows up as info!";
	KalaWindow::SetExitState(yourExitState, yourTitle, yourInfo);
	
	//call this if you want to manually control 
	//where your update loop should end
	bool yourCloseState = false;
	KalaWindow::SetShouldCloseState(yourCloseState);
	...
}

static void YourUpdateLoop()
{
	//this function controls when the window should close,
	//as long as ShouldClose returns false the window will keep rendering
	while(!KalaWindow::ShouldClose())
	{
		//capture all input
		KalaWindow::Update();
	}
	
	...
}

int main()
{
	YourInitializeFunction();
	YourUpdateLoop();
	
	return 0;
}
```
---

# Runtime loop window functions

Call these functions INSIDE KalaWindow::ShouldClose and AFTER KalaWindow::Update.

These functions are used primarily for window interactions.

```cpp
//assign a title to the window
string windowTitle = "yourWindowTitle";
void KalaWindow::SetWindowTitle(windowTitle);

//assign one of the many window states to the window
WindowState yourWindowState = WindowState::WINDOW_RESET;
KalaWindow::SetWindowState(yourWindowState);

//returns true if the window is borderless
bool isWindowBorderless = KalaWindow::IsWindowBorderless();

//set window borderless state to true or false with a bool parameter
bool yourBorderlessState = true;
KalaWindow::SetWindowBorderlessState(yourBorderlessState);

//returns true if the window is hidden
bool isWindowHidden = KalaWindow::IsWindowHidden();

//set window hidden state to true or false with a bool parameter
bool yourHiddenState = true;
KalaWindow::SetWindowHiddenState(yourHiddenState);

//returns the position of the window
POINT windowPosition = KalaWindow::GetWindowPosition;

//set window position with width and height parameter
int yourWindowWidth = 1920;
int yourWindowHeight = 1080;
KalaWindow::SetWindowPosition(yourWindowWidth, yourWindowHeight);

//returns the full size of the window (with borders and top bar)
POINT windowFullSize = KalaWindow::GetWindowFullSize;

//set window full size with width and height (with borders and top bar)
int yourfullWidth = 1111;
int yourFullHeight = 2222;
KalaWindow::SetWindowFullSize(yourfullWidth, yourFullHeight);

//returns the drawamble/client size of the window (without borders and top bar)
POINT windowContentSize = KalaWindow::GetWindowContentSize;

//set window content size with width and height (without borders and top bar)
int yourContentWidth = 3333;
int yourContentHeight = 4444;
KalaWindow::SetWindowContentSize(yourContentWidth, yourContentHeight);

//get the maximum allowed size of the window
POINT maxWindowSize = KalaWindow::GetWindowMaxSize();

//get the minimum allowed size of the window
POINT minWindowSize = KalaWindow::GetWindowMinSize();

//set the new maximum and minimum allowed width and height
int newMaxWidth = 10000;
int newMaxHeight = 10000;
int newMinWidth = 1000;
int newMinHeight = 1000;
KalaWindow::SetMinMaxSize(
	newMaxWidth,
	newMaxHeight,
	newMinWidth,
	newMinHeight);
```

# Runtime loop input functions

Call these functions INSIDE KalaWindow::ShouldClose and AFTER KalaWindow::Update.

Pass one of any of the keys in KalaWindow Key enum as a parameter for most of these functions where Key is requested.

```cpp
//the variable of Key that can be declared anywhere
Key yourKey;

//detect which key is currently held
bool isKeyDown = KalaWindow::IsKeyHeld(yourKey);

//detect which key is currently held
bool isKeyPressed = KalaWindow::IsKeyPressed(yourKey);

//detect if a combination of keys is pressed
//you must hold each key in order of the initializer list
//and once you press the last key the combo returns as true
static const std::initializer_list<Key> saveCombo
{
    Key::LeftControl,
    Key::S
};
bool isComboPressed = KalaWindow::IsComboPressed(saveCombo);

//detect if either left or right mouse key was double-clicked.
//this does not need a reference to any Key
bool isDoubleClicked = KalaWindow::IsMouseKeyDoubleClicked();

//detect if either left or right mouse key is held 
//and mouse is dragged in any direction.
//this does not need a reference to any Key
bool isMouseDragging = KalaWindow::IsMouseDragging();

//get current mouse position relative to the client area (top-left = 0,0).
//coordinates are in pixels
POINT mousePos = KalaWindow::GetMousePosition();

//set a new position for the mouse
POINT newMousePosition = { 0, 0 };
KalaInput::SetMousePosition(newMousePosition);

//get how much the cursor moved on screen (in client space) since the last frame.
//this uses absolute screen-based movement, affected by OS acceleration and DPI
POINT mouseDelta = KalaWindow::GetMouseDelta();

//set a new mouse delta for the mouse
POINT newMouseDelta = { 100, 100 };
KalaWindow::SetMouseDelta(newMouseDelta);

//get raw, unfiltered mouse movement from the hardware since the last frame.
//not affected by DPI, sensitivity, or OS mouse settings, ideal for game camera control
POINT rawMouseDelta = KalaWindow::GetRawMouseDelta();

//set a new raw mouse delta for the mouse
POINT newRawMouseDelta = { 200, 200 };
KalaWindow::SetRawMouseDelta(newRawMouseDelta);

//get how many scroll steps the mouse wheel moved since the last frame.
//positive = scroll up, negative = scroll down
int mouseWheelDelta = KalaWindow::GetMouseWheelDelta();

//set a new mouse wheel delta for the mouse
int newMouseWheelDelta = 1234;
KalaWindow::SetMouseWheelDelta(newMouseWheelDelta);

//returns true if cursor is not hidden
bool isMouseVisible = KalaWindow::IsMouseVisible();

//allows to set the visibility state of the cursor,
//if true, then the cursor is visible
bool visibilityState = true;
KalaWindow::SetMouseVisibility(visibilityState);

//returns true if cursor is locked
bool isMouseLocked = KalaWindow::IsMouseLocked();

//allows to set the lock state of the cursor,
//if true, then the cursor is locked
bool lockState = true;
KalaWindow::SetMouseLockState(lockState);
```
