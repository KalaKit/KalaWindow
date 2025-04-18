//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef KALAKIT_WAYLAND

#define KALAKIT_MODULE "OPENGL"

#include <wayland-egl.h>

//kalawindow
#include "opengl.hpp"
#include "internal/opengl_wayland.hpp"
#include "internal/window_wayland.hpp"
#include "opengl_loader.hpp"

namespace KalaKit
{
	bool OpenGL::Initialize(int width, int height)
	{
		OpenGL_Wayland::eglDisplay = eglGetDisplay((EGLNativeDisplayType)Window_Wayland::newDisplay);
        if (OpenGL_Wayland::eglDisplay == EGL_NO_DISPLAY)
        {
            LOG_ERROR("eglGetDisplay failed!");
            return false;
        }

        if (!eglInitialize(OpenGL_Wayland::eglDisplay, nullptr, nullptr))
        {
            LOG_ERROR("eglInitialize failed!");
            return false;
        }

        const EGLint configAttribs[] = 
        {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT, 
            EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
            EGL_RED_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_ALPHA_SIZE, 8,
            EGL_DEPTH_SIZE, 24,
            EGL_STENCIL_SIZE, 8,
            EGL_NONE
        };

        EGLint numConfigs{};
        if (!eglChooseConfig(
            OpenGL_Wayland::eglDisplay, 
            configAttribs, 
            &OpenGL_Wayland::eglConfig, 
            1,
            &numConfigs)
            || numConfigs == 0)
        {
            LOG_ERROR("eglChooseConfig failed!");
			return false;
        }

		if (width == 0
			|| height == 0)
		{
			LOG_ERROR("OpenGL initialization failed on Wayland because width or height was 0!");
			return false;
		}
        struct wl_egl_window* eglWindow = wl_egl_window_create(
			Window_Wayland::newSurface,
			width,
			height);
		if (!eglWindow)
		{
			LOG_ERROR("wl_egl_window_create failed!");
			return false;
		}

		OpenGL_Wayland::eglSurface = eglCreateWindowSurface(
			OpenGL_Wayland::eglDisplay,
			OpenGL_Wayland::eglConfig,
			(EGLNativeWindowType)eglWindow,
			nullptr
		);
		if (OpenGL_Wayland::eglSurface == EGL_NO_SURFACE)
		{
			LOG_ERROR("eglCreateWindowSurface failed!");
			return false;
		}

		const EGLint contextAttribs[] = 
		{
			EGL_CONTEXT_MAJOR_VERSION, 3,
			EGL_CONTEXT_MINOR_VERSION, 3,
			EGL_CONTEXT_OPENGL_PROFILE_MASK,
			EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT,
			EGL_NONE
		};

		eglBindAPI(EGL_OPENGL_API);
		OpenGL_Wayland::eglContext = eglCreateContext(
			OpenGL_Wayland::eglDisplay,
			OpenGL_Wayland::eglConfig,
			EGL_NO_CONTEXT,
			contextAttribs
		);
		if (OpenGL_Wayland::eglContext == EGL_NO_CONTEXT)
		{
			LOG_ERROR("eglCreateContext failed!");
			return false;
		}

		if (!eglMakeCurrent(
			OpenGL_Wayland::eglDisplay,
			OpenGL_Wayland::eglSurface,
			OpenGL_Wayland::eglSurface,
			OpenGL_Wayland::eglContext
		))
		{
			LOG_ERROR("eglMakeCurrent failed!");
			return false;
		}

		const GLubyte* version = glGetString(GL_VERSION);
		if (!version)
		{
			LOG_ERROR("glGetString(GL_VERSION) failed!");
			return false;
		}

		LOG_DEBUG("OpenGL version: " << version);

		OpenGLLoader::LoadAllFunctions();

		//and finally set opengl viewport size
		OpenGLLoader::glViewportPtr(0, 0, width, height);

		return true;
	}

    OPENGLCONTEXT OpenGL::GetOpenGLContext()
	{
    	return eglGetCurrentContext();
	}

	bool OpenGL::IsContextValid()
	{
		EGLContext current = eglGetCurrentContext();
		if (current == nullptr)
		{
			LOG_ERROR("Current OpenGL context is null!");
			return false;
		}

		return true;
	}
}

#endif // KALAKIT_WAYLAND