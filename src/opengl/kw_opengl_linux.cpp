//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef __linux__

#include "opengl/kw_opengl.hpp"

namespace KalaWindow::OpenGL
{
	//
	// GLOBAL
	//
	
	void OpenGL_Global::Initialize(
		function<void()> os_gl_Functions,
		function<void()> core_gl_Functions)
	{
		
	}
	
	void OpenGL_Global::SetOpenGLLibrary()
	{
		
	}
	
	bool OpenGL_Global::IsExtensionSupported(const string& name)
	{
		return false;
	}
	
	string OpenGL_Global::GetError()
	{
		return "";
	}
	
	//
	// CONTEXT
	//
	
	OpenGL_Context* OpenGL_Context::Initialize(
		u32 windowID,
		u32 parentContext,
		MultiSampling msaa,
		SRGBMode srgb,
		ColorBufferBits cBits,
		DepthBufferBits dBits,
		StencilBufferBits sBits,
		AlphaChannel aChannel)
	{
		return nullptr;
	}

	OpenGL_Context::~OpenGL_Context()
	{
		
	}
}

#endif //__linux__