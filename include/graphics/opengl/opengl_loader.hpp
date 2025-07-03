//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef KALAWINDOW_SUPPORT_OPENGL

#pragma once

#include <vector>

//kalawindow
#include "core/platform.hpp"
#include "core/enums.hpp"
#include "graphics/opengl/opengl_typedefs.hpp"

namespace KalaWindow::Graphics
{
	using std::vector;

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