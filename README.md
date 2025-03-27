# KalaWindow

[![License](https://img.shields.io/badge/license-Zlib-blue)](LICENSE.md)
![Platform](https://img.shields.io/badge/platform-Windows-brightgreen)
![Development Stage](https://img.shields.io/badge/development-Alpha-yellow)

![Logo](logo.png)

KalaWindow is a lightweight C++ 20 library for Windows that is used for rendering the window your program will be ran inside of. KalaWindow natively supports and uses KalaInput for all its input-related mechanics.

# Prerequisites (when compiling from source code)

- Visual Studio 2022 (with C++ CMake tools and Windows 10 or 11 SDK)
- Ninja and CMake 3.30.3 or newer (or extract Windows_prerequsites.7z and run setup.bat)

To compile from source code simply run 'build_windows_release.bat' or 'build_windows_debug.bat' depending on your preferences then copy and attach the dll, lib and header files with your preferred way to your program source directory.

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
