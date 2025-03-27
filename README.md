# KalaWindow

[![License](https://img.shields.io/badge/license-Zlib-blue)](LICENSE.md)
![Platform](https://img.shields.io/badge/platform-Windows-brightgreen)
![Development Stage](https://img.shields.io/badge/development-Alpha-yellow)

![Logo](logo.png)

KalaWindow is a lightweight C++ 20 library for Windows that is used for rendering the window your program will be ran inside of. KalaWindow natively supports and uses KalaInput for all its input-related mechanics.

Note: This README file primarily focuses on KalaWindow functions and how to initialize KalaInput with it. Go to README-INPUT.md to read about all the runtime loop input functions.

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

using KalaKit::KalaWindow;      //core window system functions
using KalaKit::KalaInput;       //core input system functions
using KalaKit::Key;             //enum for all keyboard and mouse keys
using KalaKit::WindowDebugType; //enum for all window debug types
using KalaKit::InputDebugType;  //enum for all input debug types
using KalaKit::WindowState;     //enum for all window states

static void YourInitializeFunction()
{
	...
	
	//this function creates a window and displays it
	int myWindowHeight = 800;
	int myWindowWidth = 600;
	string myWindowTitle = "My window";
	KalaWindow::Initialize(myWindowTitle, myWindowWidth, myWindowHeight);

	//this is the core initialization function 
	//that is required to be called so that
	//the input system can initialize all its content
	KalaInput::Initialize();

	//you can pass one of the many debug types to this function
	//to be able to see messages of that debug type printed to your console,
	//the default DEBUG_NONE does nothing and if you dont want 
	//debug messages then you dont need to call this function
	KalaWindow::SetDebugState(WindowDebugType::DEBUG_NONE);

	//same as the above debug function, but for input debugging
	KalaInput::SetDebugState(InputDebugType::DEBUG_NONE);
	
	//you can pass bool true or false to this function,
	//it sets the window focus required state, which controls
	//whether the attached window needs to be in focus for
	//any input to be registred at all for KalaInput.
	//it defaults to true, so this function does not
	//need to be called if you want focus to always be required
	KalaInput::SetWindowFocusRequiredState(true);
	
	//set this function to false and assign a title and info
	//if you want to prevent the user from exiting your program.
	//setting this to false shows a warning popup with yes or no
	//and your title and info. if user presses yes the program can close,
	//if user presses no then the program stays open and the popup closes
	bool myExitState = false;
	string myTitle = "this shows up as the title!";
	string myInfo = "this shows up as info!";
	KalaInput::SetExitState(myHiddenState, myTitle, myInfo);
	
	//One of the most important functions - this directly controls if the
	//window should close or not, once this is true the window will close.
	bool myCloseState = false;
	KalaInput::SetShouldCloseState(myCloseState);
	...
}

static void YourUpdateLoop()
{
	//this function controls when the window should close,
	//as long as ShouldClose returns false the window will keep rendering
	while(!KalaWindow::ShouldClose())
	{
		//capture all input
		KalaInput::Update();
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

Call these functions INSIDE KalaWindow::ShouldClose.

These functions are used primarily for window input.

```cpp
//assign a title to the window
string windowTitle = "myWindowTitle";
void KalaWindow::SetWindowTitle(windowTitle);

//assign one of the many window states to the window
WindowState myWindowState = WindowState::WINDOW_RESET;
KalaWindow::SetWindowState(myWindowState);

//returns true if the window is borderless
bool isWindowBorderless = KalaWindow::IsWindowBorderless();

//set window borderless state to true or false with a bool parameter
bool myBorderlessState = true;
KalaWindow::SetWindowBorderlessState(myBorderlessState);

//returns true if the window is hidden
bool isWindowHidden = KalaWindow::IsWindowHidden();

//set window hidden state to true or false with a bool parameter
bool myHiddenState = true;
KalaWindow::SetWindowHiddenState(myHiddenState);

//returns the position of the window
POINT windowPosition = KalaWindow::GetWindowPosition;

//set window position with width and height parameter
int myWindowWidth = 1920;
int myWindowHeight = 1080;
KalaWindow::SetWindowPosition(myWindowWidth, myWindowHeight);

//returns the full size of the window (with borders and top bar)
POINT windowFullSize = KalaWindow::GetWindowFullSize;

//set window full size with width and height (with borders and top bar)
int myfullWidth = 1111;
int myFullHeight = 2222;
KalaWindow::SetWindowFullSize(myfullWidth, myFullHeight);

//returns the drawamble/client size of the window (without borders and top bar)
POINT windowContentSize = KalaWindow::GetWindowContentSize;

//set window content size with width and height (without borders and top bar)
int myContentWidth = 3333;
int myContentHeight = 4444;
KalaWindow::SetWindowContentSize(myContentWidth, myContentHeight);
```
