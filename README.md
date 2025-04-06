# KalaWindow

[![License](https://img.shields.io/badge/license-Zlib-blue)](LICENSE.md)
![Platform](https://img.shields.io/badge/platform-Windows-brightgreen)
![Development Stage](https://img.shields.io/badge/development-Alpha-yellow)

![Logo](logo.png)

KalaWindow is a lightweight C++ 20 library for Windows that is used for rendering the window your program will be ran inside of which creates its own OpenGL 3.3 context and uses a custom OpenGL function loader system. KalaWindow also has built in input support and includes KalaCrashHandler for handy crash reports.

External libraries included in this library:

- magic_enum - enum-string operations (include\magic_enum)
- GLM - OpenGL math functions (include\GLM)

# Prerequisites (when compiling from source code)

- Visual Studio 2022 (with C++ CMake tools and Windows 10 or 11 SDK)
- Ninja and CMake 3.30.3 or newer (or extract Windows_prerequsites.7z and run setup.bat)

To compile from source code simply run 'build_windows_release.bat' or 'build_windows_debug.bat' depending on your preferences then copy and attach the dll, lib and header files with your preferred way to your program source directory.

# How to set up

```cpp
#include <string>
#include <iostream>

#include "window.hpp"
#include "enums.hpp"

using std::string;
using std::cout;

using KalaKit::KalaWindow;
using KalaKit::DebugType;
using KalaKit::PopupType;
using KalaKit::PopupAction;
using KalaKit::PopupResult;

static void YourInitializeFunction()
{
	//initialize the window first. this creates a window, creates an opengl context,
	//loads all the opengl functions, initializes the input system and initializes the crash handler
	int yourWindowHeight = 800;
	int yourWindowWidth = 600;
	string yourWindowTitle = "Your window";
	bool initialized = KalaWindow::Initialize(yourWindowTitle, yourWindowWidth, yourWindowHeight);
	if (!initialized)
	{
		cout << "Failed to initialize KalaWindow!\n";
		return;
	}
	
	//use this if you want the window to properly keep drawing while it is being resized
	KalaWindow::SetRedrawCallback(YourRedrawCallback);
}

static void YourUpdateLoop()
{
	//this function controls when the window should close,
	//as long as ShouldClose returns false the window will keep rendering
	while(!KalaWindow::ShouldClose())
	{
		//capture all input and handle the message loop
		KalaWindow::Update();
		
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
	
		//create a popup with one of the PopupAction actions
		//and one of the PopupType types shown in enums.hpp
		//that returns one of the PopupResult results based off of user input
		string popupTitle = "Your popup title";
		string popupMessage = "Your popup message";
		PopupAction popupAction = POPUP_ACTION_YES_NO;
		PopupType popupType = POPUP_TYPE_WARNING;
		PopupResult popupResult = POPUP_RESULT_YES;
		if (KalaWindow::CreatePopup(
			popupTitle,
			popupMessage,
			popupAction,
			popupType)
			== popupResult)
		{
			cout << "Pressed yes!\n";
		}
	}
}

//this function controls what is rendered while the window is being resized.
//do NOT put anything other than rendering related content inside here.
//only place this function inside the SetRedrawCallback function
static void YourRedrawCallback()
{
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

# Shader initialize functions

This shows an example of what you need to do to initialize a red triangle.

First create a triangle.vert shader.

```
#version 330 core

layout(location = 0) in vec2 aPos;

void main()
{
    gl_Position = vec4(aPos, 0.0, 1.0);
}
```

Then create a triangle.frag shader.

```
#version 330 core

out vec4 FragColor;

void main()
{
    FragColor = vec4(1.0, 0.0, 0.0, 1.0); //red
}
```

Then create a triangle.hpp header file

```cpp
#pragma once

#include <memory>

#include "shader.hpp"

namespace Graphics
{
	using std::unique_ptr;

	using KalaKit::Shader;

	class Triangle
	{
	public:
		static void Initialize();
		static void Render();
	private:
		static inline GLuint vao;
		static inline GLuint vbo;
		static inline unique_ptr<Shader> shader;
	};
}
```

Then create a triangle.cpp source file

```cpp
#include <filesystem>
#include <string>
#include <iostream>

//external
#include "opengl_loader.hpp"
#include "glm/glm.hpp"

//project
#include "triangle.hpp"

using std::filesystem::path;
using std::filesystem::current_path;
using std::string;
using std::cout;
using std::hex;
using glm::vec4;
using std::make_unique;

using KalaKit::OpenGLLoader;

namespace Graphics
{
	void Triangle::Initialize()
	{
		float vertices[] =
		{
			 0.0f,  0.5f, //top
			-0.5f, -0.5f, //bottom left
			 0.5f, -0.5f  //bottom right
		};

		OpenGLLoader::glGenVertexArraysPtr(1, &vao);
		OpenGLLoader::glGenBuffersPtr(1, &vbo);

		OpenGLLoader::glBindVertexArrayPtr(vao);

		OpenGLLoader::glBindBufferPtr(GL_ARRAY_BUFFER, vbo);
		OpenGLLoader::glBufferDataPtr(
			GL_ARRAY_BUFFER, 
			sizeof(vertices), 
			vertices,
			GL_STATIC_DRAW);

		OpenGLLoader::glEnableVertexAttribArrayPtr(0);
		OpenGLLoader::glVertexAttribPointerPtr(
			0,
			2,
			GL_FLOAT,
			GL_FALSE,
			2 * sizeof(float),
			(void*)0);

		OpenGLLoader::glBindVertexArrayPtr(0);

		//create shader
		string vert = (current_path() / "files" / "shaders" / "tri.vert").string();
		string frag = (current_path() / "files" / "shaders" / "tri.frag").string();
		
		shader = make_unique<Shader>(vert, frag);

		if (!shader->IsValid())
		{
			cout << "Error: Triangle shader failed to compile/link!\n";
		}
	}

	void Triangle::Render()
	{
		//use the compiled shader program
		shader->Use();

		shader->SetVec4("u_Color", vec4(1.0f, 0.0f, 0.0f, 1.0f));

		//bind the VAO
		OpenGLLoader::glBindVertexArrayPtr(vao);

		//draw the triangle
		OpenGLLoader::glDrawArraysPtr(GL_TRIANGLES, 0, 3);
	}
}
```

And then finally call the Initialize and Render functions in your initialize and runtime loop functions.

ALWAYS initialize KalaWindow first! Then load the hdc. And finally initialize the triangle.

ALWAYS call the triangle update function inside the kalawindow shouldclose while loop after kalawindows own update function.

```cpp
//clear the previous frames content first and set a background color
OpenGLLoader::glClearColorPtr(0.1f, 0.1f, 0.1f, 1.0f); //dark gray
OpenGLLoader::glClearPtr(GL_COLOR_BUFFER_BIT);

//then render the triangle
Triangle::Render();

//and then swap the buffers at the end
SwapBuffers(hdc);
```

---

# Runtime loop window functions

Call these functions INSIDE KalaWindow::ShouldClose and AFTER KalaWindow::Update.

These functions are used primarily for window interactions.

```cpp
#include <windows.h>

#include "window.hpp"
#include "enums.hpp"

using KalaKit::KalaWindow;
using KalaKit::WindowState;

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
#include <windows.h>
#include <initializer_list>

#include "input.hpp"
#include "enums.hpp"

using std::initializer_list;

using KalaKit::KalaInput;
using KalaKit::Key;

//the variable of Key that can be declared anywhere
Key yourKey;

//detect which key is currently held
bool isKeyDown = KalaInput::IsKeyHeld(yourKey);

//detect which key is currently held
bool isKeyPressed = KalaInput::IsKeyPressed(yourKey);

//detect if a combination of keys is pressed
//you must hold each key in order of the initializer list
//and once you press the last key the combo returns as true
static const initializer_list<Key> saveCombo
{
    Key::LeftControl,
    Key::S
};
bool isComboPressed = KalaInput::IsComboPressed(saveCombo);

//detect if either left or right mouse key was double-clicked.
//this does not need a reference to any Key
bool isDoubleClicked = KalaInput::IsMouseKeyDoubleClicked();

//detect if either left or right mouse key is held 
//and mouse is dragged in any direction.
//this does not need a reference to any Key
bool isMouseDragging = KalaInput::IsMouseDragging();

//get current mouse position relative to the client area (top-left = 0,0).
//coordinates are in pixels
POINT mousePos = KalaInput::GetMousePosition();

//set a new position for the mouse
POINT newMousePosition = { 0, 0 };
KalaInput::SetMousePosition(newMousePosition);

//get how much the cursor moved on screen (in client space) since the last frame.
//this uses absolute screen-based movement, affected by OS acceleration and DPI
POINT mouseDelta = KalaInput::GetMouseDelta();

//set a new mouse delta for the mouse
POINT newMouseDelta = { 100, 100 };
KalaInput::SetMouseDelta(newMouseDelta);

//get raw, unfiltered mouse movement from the hardware since the last frame.
//not affected by DPI, sensitivity, or OS mouse settings, ideal for game camera control
POINT rawMouseDelta = KalaInput::GetRawMouseDelta();

//set a new raw mouse delta for the mouse
POINT newRawMouseDelta = { 200, 200 };
KalaInput::SetRawMouseDelta(newRawMouseDelta);

//get how many scroll steps the mouse wheel moved since the last frame.
//positive = scroll up, negative = scroll down
int mouseWheelDelta = KalaInput::GetMouseWheelDelta();

//set a new mouse wheel delta for the mouse
int newMouseWheelDelta = 1234;
KalaInput::SetMouseWheelDelta(newMouseWheelDelta);

//returns true if cursor is not hidden
bool isMouseVisible = KalaInput::IsMouseVisible();

//allows to set the visibility state of the cursor,
//if true, then the cursor is visible
bool visibilityState = true;
KalaInput::SetMouseVisibility(visibilityState);

//returns true if cursor is locked
bool isMouseLocked = KalaInput::IsMouseLocked();

//allows to set the lock state of the cursor,
//if true, then the cursor is locked
bool lockState = true;
KalaInput::SetMouseLockState(lockState);
```
