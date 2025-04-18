//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#ifdef KALAKIT_WINDOWS
#include <Windows.h>
#endif

//kalawindow
#include "platform.hpp"

namespace KalaKit
{
	enum class Key
	{
#ifdef KALAKIT_WINDOWS
		// Letters
		A = 'A', B = 'B', C = 'C', D = 'D', E = 'E',
		F = 'F', G = 'G', H = 'H', I = 'I', J = 'J',
		K = 'K', L = 'L', M = 'M', N = 'N', O = 'O',
		P = 'P', Q = 'Q', R = 'R', S = 'S', T = 'T',
		U = 'U', V = 'V', W = 'W', X = 'X', Y = 'Y', Z = 'Z',

		// Numbers
		Num0 = '0', Num1 = '1', Num2 = '2', Num3 = '3', Num4 = '4',
		Num5 = '5', Num6 = '6', Num7 = '7', Num8 = '8', Num9 = '9',

		// Symbols and punctuation (OEM)
		Semicolon = VK_OEM_1,    // ;:
		Equal = VK_OEM_PLUS,     // =+
		Comma = VK_OEM_COMMA,    // ,<
		Minus = VK_OEM_MINUS,    // -_
		Period = VK_OEM_PERIOD,  // .>
		Slash = VK_OEM_2,        // /?
		Backtick = VK_OEM_3,     // `~
		BracketLeft = VK_OEM_4,  // [{
		Backslash = VK_OEM_5,    // \|
		BracketRight = VK_OEM_6, // ]}
		Apostrophe = VK_OEM_7,   // '"
		Oem102 = VK_OEM_102,     // <> or \ (ISO)

		// Numpad
		Numpad0 = VK_NUMPAD0,
		Numpad1 = VK_NUMPAD1,
		Numpad2 = VK_NUMPAD2,
		Numpad3 = VK_NUMPAD3,
		Numpad4 = VK_NUMPAD4,
		Numpad5 = VK_NUMPAD5,
		Numpad6 = VK_NUMPAD6,
		Numpad7 = VK_NUMPAD7,
		Numpad8 = VK_NUMPAD8,
		Numpad9 = VK_NUMPAD9,
		NumpadAdd = VK_ADD,
		NumpadSubtract = VK_SUBTRACT,
		NumpadMultiply = VK_MULTIPLY,
		NumpadDivide = VK_DIVIDE,
		NumpadDecimal = VK_DECIMAL,
		NumLock = VK_NUMLOCK,

		// Function keys
		F1 = VK_F1, F2 = VK_F2, F3 = VK_F3, F4 = VK_F4,
		F5 = VK_F5, F6 = VK_F6, F7 = VK_F7, F8 = VK_F8,
		F9 = VK_F9, F10 = VK_F10, F11 = VK_F11, F12 = VK_F12,
		F13 = VK_F13, F14 = VK_F14, F15 = VK_F15, F16 = VK_F16,
		F17 = VK_F17, F18 = VK_F18, F19 = VK_F19, F20 = VK_F20,
		F21 = VK_F21, F22 = VK_F22, F23 = VK_F23, F24 = VK_F24,

		// Control / navigation
		Escape = VK_ESCAPE,
		Enter = VK_RETURN,
		Tab = VK_TAB,
		Backspace = VK_BACK,
		Insert = VK_INSERT,
		Delete = VK_DELETE,
		Home = VK_HOME,
		End = VK_END,
		PageUp = VK_PRIOR,
		PageDown = VK_NEXT,

		// Modifier keys
		LeftShift = VK_LSHIFT,
		RightShift = VK_RSHIFT,
		LeftControl = VK_LCONTROL,
		RightControl = VK_RCONTROL,
		LeftAlt = VK_LMENU,
		RightAlt = VK_RMENU,
		CapsLock = VK_CAPITAL,

		// System keys
		PrintScreen = VK_SNAPSHOT,
		ScrollLock = VK_SCROLL,
		Pause = VK_PAUSE,
		Menu = VK_APPS,

		// Arrow keys
		Up = VK_UP,
		Down = VK_DOWN,
		Left = VK_LEFT,
		Right = VK_RIGHT,

		// Spacebar
		Space = VK_SPACE,

		// Mouse buttons
		MouseLeft = VK_LBUTTON,
		MouseRight = VK_RBUTTON,
		MouseMiddle = VK_MBUTTON,
		MouseX1 = VK_XBUTTON1,
		MouseX2 = VK_XBUTTON2,

		// Extended mouse buttons
		MouseX3 = 0x100,
		MouseX4 = 0x101,
		MouseX5 = 0x102,
		MouseX6 = 0x103,
		MouseX7 = 0x104,
		MouseX8 = 0x105,
		MouseX9 = 0x106,
		MouseX10 = 0x107,

		// Media & browser keys
		MediaPlayPause = VK_MEDIA_PLAY_PAUSE,
		MediaStop = VK_MEDIA_STOP,
		MediaNextTrack = VK_MEDIA_NEXT_TRACK,
		MediaPrevTrack = VK_MEDIA_PREV_TRACK,
		VolumeUp = VK_VOLUME_UP,
		VolumeDown = VK_VOLUME_DOWN,
		VolumeMute = VK_VOLUME_MUTE,
		LaunchMail = VK_LAUNCH_MAIL,
		LaunchApp1 = VK_LAUNCH_APP1,
		LaunchApp2 = VK_LAUNCH_APP2,
		BrowserBack = VK_BROWSER_BACK,
		BrowserForward = VK_BROWSER_FORWARD,
		BrowserRefresh = VK_BROWSER_REFRESH,
		BrowserStop = VK_BROWSER_STOP,
		BrowserSearch = VK_BROWSER_SEARCH,
		BrowserFavorites = VK_BROWSER_FAVORITES,
		BrowserHome = VK_BROWSER_HOME
#endif
	};

	/// <summary>
	/// Debug message type printed to console. These are usable 
	/// only if your program is in Debug mode and most of these,
	/// except those marked with the (required ...) part requires
	/// that one of its function type is assigned somewhere in 
	/// your program code for them to actually return something.
	/// </summary>
	enum class DebugType
	{
		DEBUG_NONE,                    //Default option, assigning this does nothing

		//
		// INPUT ENUMS
		//

		DEBUG_KEY_HELD,                //Print key held updates (requires IsKeyDown)
		DEBUG_KEY_PRESSED,             //Print key pressed updates (requires WasKeyPressed)
		DEBUG_COMBO_PRESSED,           //Print combo pressed updates (requires WasComboPressed)
		DEBUG_DOUBLE_CLICKED,          //Print mouse double click updates (requires WasDoubleClicked)
		DEBUG_IS_MOUSE_DRAGGING,       //Print mouse dragging updates (requires IsMouseDragging)
		DEBUG_MOUSE_POSITION,          //Print mouse position updates (requires GetMousePosition)
		DEBUG_MOUSE_DELTA,             //Print regular mouse delta updates (requires GetMouseDelta)
		DEBUG_RAW_MOUSE_DELTA,         //Print raw mouse delta updates (requires GetRawMouseDelta)
		DEBUG_MOUSE_WHEEL_DELTA,       //Print scroll wheel updates (requires GetMouseWheelDelta)
		DEBUG_MOUSE_VISIBILITY,        //Print scroll mouse visibility updates (requires SetMouseVisibility)
		DEBUG_MOUSE_LOCK_STATE,        //Print scroll lock state updates (requires SetMouseLockState)
		DEBUG_PROCESS_MESSAGE_TEST,    //Print all processing messages user input sends

		//
		// WINDOW ENUMS
		//

		DEBUG_WINDOW_TITLE,            //Print window title change updates (requires SetWindowTitle)
		DEBUG_WINDOW_BORDERLESS_STATE, //Print window borderless state updates (requires SetWindowBorderlessState)
		DEBUG_WINDOW_HIDDEN_STATE,     //Print window hidden state updates (requires SetWindowHiddenState)
		DEBUG_WINDOW_SET_POSITION,     //Print window position updates (requires SetWindowPosition)
		DEBUG_WINDOW_SET_FULL_SIZE,    //Print window full size updates (requires SetWindowFullSize)
		DEBUG_WIŃDOW_SET_CONTENT_SIZE, //Print window content size updates (requires GetWindowContentSize)
		DEBUG_WINDOW_SET_MINMAX_SIZE,  //Print new min and max window size whenever it is updated (requires SetMinMaxSize)
		DEBUG_WINDOW_RESIZE,           //Print new resolution whenever window rescales
		DEBUG_WINDOW_REPAINT,          //Print new color whenever window repaints
		DEBUG_WINDOW_CORNER_EDGE,      //Print ID code and name of each corner and edge when cursor goes over it
		
		DEBUG_ALL                      //Print ALL debug updates
	};

	/// <summary>
	/// A state the window can be switched to from its current state.
	/// </summary>
	enum class WindowState
	{
		WINDOW_RESET,      //Reset window to default state
		WINDOW_MINIMIZED,  //Minimize window to taskbar
		WINDOW_MAXIMIZED   //Maximize window to full screen size
	};

	/// <summary>
	/// All the currently supported OpenGL functions that can be loaded by the user.
	/// </summary>
	enum class OpenGLFunction
	{
		//geometry

		OPENGL_GENVERTEXARRAYS,         //Create one or more VAO (Vertex array object)
		OPENGL_BINDVERTEXARRAY,         //Bind a VAO
		OPENGL_GENBUFFERS,              //Create one or more VBO (Vertex buffer object)
		OPENGL_BINDBUFFER,              //Bind a VBO
		OPENGL_BUFFERDATA,              //Upload data to currently bound VBO
		OPENGL_ENABLEVERTEXATTRIBARRAY, //Enable a vertex attribute slot (position, color etc)
		OPENGL_VERTEXATTRIBPOINTER,     //Defines how to read vertex data from VBO
		OPENGL_GETVERTEXATTRIBIV,       //Query an integer attribute parameter (size, type, stride, enabled, etc)
		OPENGL_GETVERTEXATTRIBPOINTERV, //Query the memory pointer for a vertex attribute
		OPENGL_DRAWARRAYS,              //Draws vertices with bound VAO and shader (non-indexed)
		OPENGL_DRAWELEMENTS,            //Draws vertices using index data (EBO)

		//shaders

		OPENGL_CREATESHADER,            //Create shader object (vertex/fragment)
		OPENGL_SHADERSOURCE,            //Set the shader source code
		OPENGL_COMPILESHADER,           //Compile the shader
		OPENGL_CREATEPROGRAM,           //Create a shader program
		OPENGL_USEPROGRAM,              //Use a shader program for drawing
		OPENGL_ATTACHSHADER,            //Attach a shader to the program
		OPENGL_LINKPROGRAM,             //Link the shader program
		OPENGL_DELETESHADER,            //Delete a shader object
		OPENGL_GETSHADERIV,             //Get shader compile status
		OPENGL_GETSHADERINFOLOG,        //Get shader compilation log
		OPENGL_GETPROGRAMIV,            //Get program link status
		OPENGL_GETPROGRAMINFOLOG,       //Get program linking log
		OPENGL_GETACTIVEATTRIB,         //Query active vertex attribute (name, type, size)
		OPENGL_GETATTRIBLOCATION,       //Get location of a vertex attribute by name
		OPENGL_DELETEPROGRAM,           //Delete a shader program
		OPENGL_VALIDATEPROGRAM,         //Validate the shader program
		OPENGL_ISPROGRAM,               //Check if a given ID is a valid shader program

		//uniforms

		OPENGL_GETUNIFORMLOCATION,      //Get a uniform variable's location
		OPENGL_UNIFORM1I,               //Set int uniform
		OPENGL_UNIFORM1F,               //Set float uniform
		OPENGL_UNIFORM2F,               //Set vec2 uniform (x, y)
		OPENGL_UNIFORM2FV,              //Set vec2 uniform from pointer
		OPENGL_UNIFORM3F,               //Set vec3 uniform (x, y, z)
		OPENGL_UNIFORM3FV,              //Set vec3 uniform from pointer
		OPENGL_UNIFORM4F,               //Set vec4 uniform (x, y, z, w)
		OPENGL_UNIFORM4FV,              //Set vec4 uniform from pointer
		OPENGL_UNIFORMMATRIX2FV,        //Set mat2 uniform
		OPENGL_UNIFORMMATRIX3FV,        //Set mat3 uniform
		OPENGL_UNIFORMMATRIX4FV,        //Set mat4 uniform

		//textures

		OPENGL_GENTEXTURES,             //Create texture objects
		OPENGL_BINDTEXTURE,             //Bind a texture
		OPENGL_TEXIMAGE2D,              //Upload texture data
		OPENGL_TEXPARAMETERI,           //Set texture parameter (filtering/wrapping)
		OPENGL_GENERATEMIPMAP,          //Generate mipmaps for the current texture
	
		//frame and render state

		OPENGL_VIEWPORT,                //Set the viewport area
		OPENGL_DISABLE,                 //Disable OpenGL capabilities like depth test
		OPENGL_CLEARCOLOR,              //Set background color for clearing
		OPENGL_CLEAR,                   //Clear framebuffer (color, depth, etc)
		OPENGL_GETINTEGERV,             //Query integer values (like bound objects or limits)
		OPENGL_GETSTRING,               //Get OpenGL version/vendor info as strings
		OPENGL_GETERROR                 //Get last OpenGL error code
	};

	//Buttons shown on the popup
	enum class PopupAction
	{
		POPUP_ACTION_OK,            // OK button only
		POPUP_ACTION_OK_CANCEL,     // OK and Cancel buttons
		POPUP_ACTION_YES_NO,        // Yes and No buttons
		POPUP_ACTION_YES_NO_CANCEL, // Yes, No, and Cancel buttons
		POPUP_ACTION_RETRY_CANCEL   // Retry and Cancel buttons
	};

	//Icon shown on the popup
	enum class PopupType
	{
		POPUP_TYPE_INFO,    // Info icon (blue 'i')
		POPUP_TYPE_WARNING, // Warning icon (yellow triangle)
		POPUP_TYPE_ERROR,   // Error icon (red X)
		POPUP_TYPE_QUESTION // Question icon (used for confirmations)
	};

	//User response from the popup
#ifdef KALAKIT_WINDOWS
	enum class PopupResult
	{
		POPUP_RESULT_NONE = 0,          // No response or unknown
		POPUP_RESULT_OK = IDOK,         // User clicked OK
		POPUP_RESULT_CANCEL = IDCANCEL, // User clicked Cancel
		POPUP_RESULT_YES = IDYES,       // User clicked Yes
		POPUP_RESULT_NO = IDNO,         // User clicked No
		POPUP_RESULT_RETRY = IDRETRY    // User clicked Retry
	};
#else
	enum class PopupResult
	{
		POPUP_RESULT_NONE = 0,          // No response or unknown
		POPUP_RESULT_OK = 1,            // User clicked OK
		POPUP_RESULT_CANCEL = 2,        // User clicked Cancel
		POPUP_RESULT_YES = 3,           // User clicked Yes
		POPUP_RESULT_NO = 4,            // User clicked No
		POPUP_RESULT_RETRY = 5          // User clicked Retry
	};
#endif
}