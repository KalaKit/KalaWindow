//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#ifdef _WIN32
	#ifdef KALAWINDOW_DLL_EXPORT
		#define KALAWINDOW_API __declspec(dllexport)
	#else
		#define KALAWINDOW_API __declspec(dllimport)
	#endif
#else
	#define KALAWINDOW_API
#endif

#include <vector>

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
		/// </summary>
		static void LoadAllFunctions();

		/// <summary>
		/// Loads user-chosen OpenGL functions.
		/// </summary>
		static void LoadChosenFunctions(const vector<OpenGLFunction>& functions);

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
	private:
		template <typename T>
		static T LoadOpenGLFunction(const char* name);

		//geometry

		static PFNGLGENVERTEXARRAYSPROC        glGenVertexArraysPtr;
		static PFNGLBINDVERTEXARRAYPROC        glBindVertexArrayPtr;
		static PFNGLGENBUFFERSPROC             glGenBuffersPtr;
		static PFNGLBINDBUFFERPROC             glBindBufferPtr;
		static PFNGLBUFFERDATAPROC             glBufferDataPtr;
		static PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArrayPtr;
		static PFNGLVERTEXATTRIBPOINTERPROC    glVertexAttribPointerPtr;
		static PFNGLDRAWARRAYSPROC             glDrawArraysPtr;
		static PFNGLDRAWELEMENTSPROC           glDrawElementsPtr;

		//shaders

		static PFNGLCREATESHADERPROC           glCreateShaderPtr;
		static PFNGLSHADERSOURCEPROC           glShaderSourcePtr;
		static PFNGLCOMPILESHADERPROC          glCompileShaderPtr;
		static PFNGLCREATEPROGRAMPROC          glCreateProgramPtr;
		static PFNGLUSEPROGRAMPROC             glUseProgramPtr;
		static PFNGLATTACHSHADERPROC           glAttachShaderPtr;
		static PFNGLLINKPROGRAMPROC            glLinkProgramPtr;
		static PFNGLDELETESHADERPROC           glDeleteShaderPtr;
		static PFNGLGETSHADERIVPROC            glGetShaderivPtr;
		static PFNGLGETSHADERINFOLOGPROC       glGetShaderInfoLogPtr;
		static PFNGLGETPROGRAMIVPROC           glGetProgramivPtr;
		static PFNGLGETPROGRAMINFOLOGPROC      glGetProgramInfoLogPtr;

		//uniforms

		static PFNGLGETUNIFORMLOCATIONPROC     glGetUniformLocationPtr;
		static PFNGLUNIFORM1IPROC              glUniform1iPtr;
		static PFNGLUNIFORM1FPROC              glUniform1fPtr;
		static PFNGLUNIFORM3FPROC              glUniform3fPtr;
		static PFNGLUNIFORMMATRIX4FVPROC       glUniformMatrix4fvPtr;

		//textures

		static PFNGLGENTEXTURESPROC            glGenTexturesPtr;
		static PFNGLBINDTEXTUREPROC            glBindTexturePtr;
		static PFNGLTEXIMAGE2DPROC             glTexImage2DPtr;
		static PFNGLTEXPARAMETERIPROC          glTexParameteriPtr;
		static PFNGLGENERATEMIPMAPPROC         glGenerateMipmapPtr;

		static const vector<OpenGLFunctionEntry> openGLFunctionTable;
	};
}