//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <vector>

//kalawindow
#include "platform.hpp"
#include "enums.hpp"
#include "opengl_typedefs.hpp"

namespace KalaKit
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

		static PFNGLGENVERTEXARRAYSPROC        glGenVertexArraysPtr;         //Create one or more VAO (Vertex array object)
		static PFNGLBINDVERTEXARRAYPROC        glBindVertexArrayPtr;         //Bind a VAO
		static PFNGLGENBUFFERSPROC             glGenBuffersPtr;              //Create one or more VBO (Vertex buffer object)
		static PFNGLBINDBUFFERPROC             glBindBufferPtr;              //Bind a VBO
		static PFNGLBUFFERDATAPROC             glBufferDataPtr;              //Upload data to currently bound VBO
		static PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArrayPtr;//Enable a vertex attribute slot (position, color etc)
		static PFNGLVERTEXATTRIBPOINTERPROC    glVertexAttribPointerPtr;     //Defines how to read vertex data from VBO
		static PFNGLGETVERTEXATTRIBIVPROC      glGetVertexAttribivPtr;       //Query an integer attribute parameter (size, type, stride, enabled, etc)
		static PFNGLGETVERTEXATTRIBPOINTERVPROC glGetVertexAttribPointervPtr;//Query the memory pointer for a vertex attribute
		static PFNGLDRAWARRAYSPROC             glDrawArraysPtr;              //Draws vertices with bound VAO and shader (non-indexed)
		static PFNGLDRAWELEMENTSPROC           glDrawElementsPtr;            //Draws vertices using index data (EBO)

		//shaders

		static PFNGLCREATESHADERPROC           glCreateShaderPtr;            //Create shader object (vertex/fragment)
		static PFNGLSHADERSOURCEPROC           glShaderSourcePtr;            //Set the shader source code
		static PFNGLCOMPILESHADERPROC          glCompileShaderPtr;           //Compile the shader
		static PFNGLCREATEPROGRAMPROC          glCreateProgramPtr;           //Create a shader program
		static PFNGLUSEPROGRAMPROC             glUseProgramPtr;              //Use a shader program for drawing
		static PFNGLATTACHSHADERPROC           glAttachShaderPtr;            //Attach a shader to the program
		static PFNGLLINKPROGRAMPROC            glLinkProgramPtr;             //Link the shader program
		static PFNGLDELETESHADERPROC           glDeleteShaderPtr;            //Delete a shader object
		static PFNGLGETSHADERIVPROC            glGetShaderivPtr;             //Get shader compile status
		static PFNGLGETSHADERINFOLOGPROC       glGetShaderInfoLogPtr;        //Get shader compilation log
		static PFNGLGETPROGRAMIVPROC           glGetProgramivPtr;            //Get program link status
		static PFNGLGETPROGRAMINFOLOGPROC      glGetProgramInfoLogPtr;       //Get program linking log
		static PFNGLGETACTIVEATTRIBPROC        glGetActiveAttribPtr;         //Query active vertex attribute (name, type, size)
		static PFNGLGETATTRIBLOCATIONPROC      glGetAttribLocationPtr;       //Get location of a vertex attribute by name
		static PFNGLDELETEPROGRAMPROC          glDeleteProgramPtr;           //Delete a shader program
		static PFNGLVALIDATEPROGRAMPROC        glValidateProgramPtr;         //Validate the shader program
		static PFNGLISPROGRAMPROC              glIsProgramPtr;               //Check if a given ID is a valid shader program

		//uniforms

		static PFNGLGETUNIFORMLOCATIONPROC     glGetUniformLocationPtr;      //Get a uniform variable's location
		static PFNGLUNIFORM1IPROC              glUniform1iPtr;               //Set int uniform
		static PFNGLUNIFORM1FPROC              glUniform1fPtr;               //Set float uniform
		static PFNGLUNIFORM2FPROC              glUniform2fPtr;               //Set vec2 uniform (x, y)
		static PFNGLUNIFORM2FVPROC             glUniform2fvPtr;              //Set vec2 uniform from pointer
		static PFNGLUNIFORM3FPROC              glUniform3fPtr;               //Set vec3 uniform (x, y, z)
		static PFNGLUNIFORM3FVPROC             glUniform3fvPtr;              //Set vec3 uniform from pointer
		static PFNGLUNIFORM4FPROC              glUniform4fPtr;               //Set vec4 uniform (x, y, z, w)
		static PFNGLUNIFORM4FVPROC             glUniform4fvPtr;              //Set vec4 uniform from pointer
		static PFNGLUNIFORMMATRIX2FVPROC       glUniformMatrix2fvPtr;        //Set mat2 uniform
		static PFNGLUNIFORMMATRIX3FVPROC       glUniformMatrix3fvPtr;        //Set mat3 uniform
		static PFNGLUNIFORMMATRIX4FVPROC       glUniformMatrix4fvPtr;        //Set mat4 uniform

		//textures

		static PFNGLGENTEXTURESPROC            glGenTexturesPtr;             //Create texture objects
		static PFNGLBINDTEXTUREPROC            glBindTexturePtr;             //Bind a texture
		static PFNGLTEXIMAGE2DPROC             glTexImage2DPtr;              //Upload texture data
		static PFNGLTEXPARAMETERIPROC          glTexParameteriPtr;           //Set texture parameter (filtering/wrapping)
		static PFNGLGENERATEMIPMAPPROC         glGenerateMipmapPtr;          //Generate mipmaps for the current texture

		//frame and render state

		static PFNGLVIEWPORTPROC               glViewportPtr;                //Set the viewport area
		static PFNGLDISABLEPROC                glDisablePtr;                 //Disable OpenGL capabilities like depth test
		static PFNGLCLEARCOLORPROC             glClearColorPtr;              //Set background color for clearing
		static PFNGLCLEARPROC                  glClearPtr;                   //Clear framebuffer (color, depth, etc)
		static PFNGLGETINTEGERVPROC            glGetIntegervPtr;             //Query integer values (like bound objects or limits)
		static PFNGLGETSTRINGPROC              glGetStringPtr;               //Get OpenGL version/vendor info as strings
		static PFNGLGETERRORPROC               glGetErrorPtr;                //Get last OpenGL error code

	private:
		template <typename T>
		static T LoadOpenGLFunction(const char* name);

		static const vector<OpenGLFunctionEntry> openGLFunctionTable;
	};
}