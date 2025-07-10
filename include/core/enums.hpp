//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

//kalawindow
#include "platform.hpp"

namespace KalaWindow
{
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
		DEBUG_WINDOW_SET_CONTENT_SIZE, //Print window content size updates (requires GetWindowContentSize)
		DEBUG_WINDOW_SET_MINMAX_SIZE,  //Print new min and max window size whenever it is updated (requires SetMinMaxSize)
		DEBUG_WINDOW_RESIZE,           //Print new resolution whenever window rescales
		DEBUG_WINDOW_REPAINT,          //Print new color whenever window repaints
		DEBUG_WINDOW_CORNER_EDGE,      //Print ID code and name of each corner and edge when cursor goes over it

		DEBUG_ALL                      //Print ALL debug updates
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
		OPENGL_DELETEVERTEXARRAY,       //Delete a VAO
		OPENGL_DELETEBUFFER,            //Delete a VBO
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
		OPENGL_DETACHSHADER,            //Detach a shader object
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
		OPENGL_ACTIVETEXTURE,           //Select active texture unit
		OPENGL_TEXIMAGE2D,              //Upload texture data
		OPENGL_TEXSUBIMAGE2D,           //Upload a sub-region of texture data
		OPENGL_TEXPARAMETERI,           //Set texture parameter (filtering/wrapping)
		OPENGL_GENERATEMIPMAP,          //Generate mipmaps for the current texture
		OPENGL_DELETETEXTURES,          //Delete one or more textures

		//framebuffers and renderbuffers

		OPENGL_GENFRAMEBUFFERS,         //Generate framebuffer object(s)
		OPENGL_BINDFRAMEBUFFER,         //Bind a framebuffer object
		OPENGL_FRAMEBUFFERTEXTURE2D,    //Attach a texture to a framebuffer
		OPENGL_CHECKFRAMEBUFFERSTATUS,  //Check if framebuffer is complete
		OPENGL_GENRENDERBUFFERS,        //Generate renderbuffer object(s)
		OPENGL_BINDRENDERBUFFER,        //Bind a renderbuffer object
		OPENGL_RENDERBUFFERSTORAGE,     //Define storage for a renderbuffer
		OPENGL_FRAMEBUFFERRENDERBUFFER, //Attach a renderbuffer to a framebuffer

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
	enum class PopupResult
	{
		POPUP_RESULT_NONE,   //No response or unknown
		POPUP_RESULT_OK,     //User clicked OK
		POPUP_RESULT_CANCEL, //User clicked Cancel
		POPUP_RESULT_YES,    //User clicked Yes
		POPUP_RESULT_NO,     //User clicked No
		POPUP_RESULT_RETRY   //User clicked Retry
	};

	enum class FileType
	{
		FILE_ANY,         //Can select any file type
		FILE_ANY_VIDEO,   //Can select any common video file type
		FILE_ANY_AUDIO,   //Can select any common audio file type
		FILE_ANY_MODEL,   //Can select any common model file type (for graphics software and game development)
		FILE_ANY_TEXTURE, //Can select any common texture file type (for graphics software and game development)
		FILE_EXE,         //Can select any executable
		FILE_FOLDER       //Can select any folder
	};
}