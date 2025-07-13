//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef KALAWINDOW_SUPPORT_OPENGL

#pragma once

#include <vector>

#include "core/platform.hpp"
#include "graphics/opengl/opengl_typedefs.hpp"

namespace KalaWindow::Graphics
{
	using std::vector;

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

	class KALAWINDOW_API OpenGLLoader
	{
	public:
		/// <summary>
		/// Check whether the chosen OpenGL function is available or not.
		/// </summary>
		static bool IsFunctionAvailable(OpenGLFunction id);

		/// <summary>
		/// Skips user-chosen functions vector and loads everything available.
		/// Should not be called manually, already called inside of OpenGL::Initialize.
		/// </summary>
		static void LoadAllFunctions();

		struct OpenGLFunctionEntry
		{
			OpenGLFunction id; //Function enum value
			const char* name;  //Actual OpenGL function name
			void** target;     //Pointer to where the loaded function should be stored

			OpenGLFunctionEntry(
				OpenGLFunction i,
				const char* n,
				void** t) :
				id(i),
				name(n),
				target(t) {
			}
		};

		//geometry

		static PFNGLGENVERTEXARRAYSPROC        glGenVertexArrays;         //Create one or more VAO (Vertex array object)
		static PFNGLBINDVERTEXARRAYPROC        glBindVertexArray;         //Bind a VAO
		static PFNGLGENBUFFERSPROC             glGenBuffers;              //Create one or more VBO (Vertex buffer object)
		static PFNGLBINDBUFFERPROC             glBindBuffer;              //Bind a VBO
		static PFNGLDELETEVERTEXARRAYSPROC     glDeleteVertexArrays;      //Delete a VAO
		static PFNGLDELETEBUFFERSPROC          glDeleteBuffers;           //Delete a VBO
		static PFNGLBUFFERDATAPROC             glBufferData;              //Upload data to currently bound VBO
		static PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;//Enable a vertex attribute slot (position, color etc)
		static PFNGLVERTEXATTRIBPOINTERPROC    glVertexAttribPointer;     //Defines how to read vertex data from VBO
		static PFNGLGETVERTEXATTRIBIVPROC      glGetVertexAttribiv;       //Query an integer attribute parameter (size, type, stride, enabled, etc)
		static PFNGLGETVERTEXATTRIBPOINTERVPROC glGetVertexAttribPointerv;//Query the memory pointer for a vertex attribute
		static PFNGLDRAWARRAYSPROC             glDrawArrays;              //Draws vertices with bound VAO and shader (non-indexed)
		static PFNGLDRAWELEMENTSPROC           glDrawElements;            //Draws vertices using index data (EBO)

		//shaders

		static PFNGLCREATESHADERPROC           glCreateShader;            //Create shader object (vertex/fragment)
		static PFNGLSHADERSOURCEPROC           glShaderSource;            //Set the shader source code
		static PFNGLCOMPILESHADERPROC          glCompileShader;           //Compile the shader
		static PFNGLCREATEPROGRAMPROC          glCreateProgram;           //Create a shader program
		static PFNGLUSEPROGRAMPROC             glUseProgram;              //Use a shader program for drawing
		static PFNGLATTACHSHADERPROC           glAttachShader;            //Attach a shader to the program
		static PFNGLLINKPROGRAMPROC            glLinkProgram;             //Link the shader program
		static PFNGLDETACHSHADERPROC           glDetachShader;            //Detach a shader object
		static PFNGLDELETESHADERPROC           glDeleteShader;            //Delete a shader object
		static PFNGLGETSHADERIVPROC            glGetShaderiv;             //Get shader compile status
		static PFNGLGETSHADERINFOLOGPROC       glGetShaderInfoLog;        //Get shader compilation log
		static PFNGLGETPROGRAMIVPROC           glGetProgramiv;            //Get program link status
		static PFNGLGETPROGRAMINFOLOGPROC      glGetProgramInfoLog;       //Get program linking log
		static PFNGLGETACTIVEATTRIBPROC        glGetActiveAttrib;         //Query active vertex attribute (name, type, size)
		static PFNGLGETATTRIBLOCATIONPROC      glGetAttribLocation;       //Get location of a vertex attribute by name
		static PFNGLDELETEPROGRAMPROC          glDeleteProgram;           //Delete a shader program
		static PFNGLVALIDATEPROGRAMPROC        glValidateProgram;         //Validate the shader program
		static PFNGLISPROGRAMPROC              glIsProgram;               //Check if a given ID is a valid shader program

		//uniforms

		static PFNGLGETUNIFORMLOCATIONPROC     glGetUniformLocation;      //Get a uniform variable's location
		static PFNGLUNIFORM1IPROC              glUniform1i;               //Set int uniform
		static PFNGLUNIFORM1FPROC              glUniform1f;               //Set float uniform
		static PFNGLUNIFORM2FPROC              glUniform2f;               //Set vec2 uniform (x, y)
		static PFNGLUNIFORM2FVPROC             glUniform2fv;              //Set vec2 uniform from pointer
		static PFNGLUNIFORM3FPROC              glUniform3f;               //Set vec3 uniform (x, y, z)
		static PFNGLUNIFORM3FVPROC             glUniform3fv;              //Set vec3 uniform from pointer
		static PFNGLUNIFORM4FPROC              glUniform4f;               //Set vec4 uniform (x, y, z, w)
		static PFNGLUNIFORM4FVPROC             glUniform4fv;              //Set vec4 uniform from pointer
		static PFNGLUNIFORMMATRIX2FVPROC       glUniformMatrix2fv;        //Set mat2 uniform
		static PFNGLUNIFORMMATRIX3FVPROC       glUniformMatrix3fv;        //Set mat3 uniform
		static PFNGLUNIFORMMATRIX4FVPROC       glUniformMatrix4fv;        //Set mat4 uniform

		//textures

		static PFNGLGENTEXTURESPROC            glGenTextures;             //Create texture objects
		static PFNGLBINDTEXTUREPROC            glBindTexture;             //Bind a texture
		static PFNGLACTIVETEXTUREPROC          glActiveTexture;           //Select active texture unit
		static PFNGLTEXIMAGE2DPROC             glTexImage2D;              //Upload texture data
		static PFNGLTEXSUBIMAGE2DPROC          glTexSubImage2D;           //Upload a sub-region of texture data
		static PFNGLTEXPARAMETERIPROC          glTexParameteri;           //Set texture parameter (filtering/wrapping)
		static PFNGLGENERATEMIPMAPPROC         glGenerateMipmap;          //Generate mipmaps for the current texture
		static PFNGLDELETETEXTURESPROC         glDeleteTextures;          //Delete one or more textures

		//framebuffers and renderbuffers

		static PFNGLGENFRAMEBUFFERSPROC        glGenFramebuffers;         //Generate framebuffer object(s)
		static PFNGLBINDFRAMEBUFFERPROC        glBindFramebuffer;         //Bind a framebuffer object
		static PFNGLFRAMEBUFFERTEXTURE2DPROC   glFramebufferTexture2D;    //Attach a texture to a framebuffer
		static PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus;  //Check if framebuffer is complete
		static PFNGLGENRENDERBUFFERSPROC       glGenRenderbuffers;        //Generate renderbuffer object(s)
		static PFNGLBINDRENDERBUFFERPROC       glBindRenderbuffer;        //Bind a renderbuffer object
		static PFNGLRENDERBUFFERSTORAGEPROC    glRenderbufferStorage;     //Define storage for a renderbuffer
		static PFNGLFRAMEBUFFERRENDERBUFFERPROC glFramebufferRenderbuffer;//Attach a renderbuffer to a framebuffer

		//frame and render state

		static PFNGLVIEWPORTPROC               glViewport;                //Set the viewport area
		static PFNGLDISABLEPROC                glDisable;                 //Disable OpenGL capabilities like depth test
		static PFNGLCLEARCOLORPROC             glClearColor;              //Set background color for clearing
		static PFNGLCLEARPROC                  glClear;                   //Clear framebuffer (color, depth, etc)
		static PFNGLGETINTEGERVPROC            glGetIntegerv;             //Query integer values (like bound objects or limits)
		static PFNGLGETSTRINGPROC              glGetString;               //Get OpenGL version/vendor info as strings
		static PFNGLGETERRORPROC               glGetError;                //Get last OpenGL error code

	private:
		template <typename T>
		static T LoadOpenGLFunction(const char* name);

		static const vector<OpenGLFunctionEntry> openGLFunctionTable;
	};
}

#endif //KALAWINDOW_SUPPORT_OPENGL