//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef _WIN32

#include <windows.h>
#include <string>
#include <vector>
#include <sstream>
#include <memory>

#include "OpenGL/wglext.h"

#include "KalaHeaders/log_utils.hpp"

#include "opengl/kw_opengl.hpp"
#include "opengl/kw_opengl_functions_core.hpp"
#include "opengl/kw_opengl_functions_windows.hpp"
#include "graphics/kw_window.hpp"
#include "core/kw_core.hpp"

using KalaHeaders::KalaCore::ToVar;
using KalaHeaders::KalaCore::FromVar;
using KalaHeaders::KalaMath::vec2;
using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;

using KalaWindow::Core::KalaWindowCore;
using KalaWindow::Graphics::Window;
using KalaWindow::Graphics::WindowData;
using KalaWindow::OpenGL::OpenGL_Global;
using KalaWindow::OpenGL::OpenGLFunctions::GL_Core;
using KalaWindow::OpenGL::OpenGLFunctions::GL_Windows;
using KalaWindow::OpenGL::OpenGLFunctions::OpenGL_Functions_Core;
using KalaWindow::OpenGL::OpenGLFunctions::OpenGL_Functions_Windows;
using std::string;
using std::vector;
using std::to_string;
using std::ostringstream;
using std::dec;
using std::hex;
using std::unique_ptr;
using std::make_unique;

static bool IsCorrectVersion();
static HWND CreateDummyWindow();

static string GetErrorType(const string& errorOrigin);

namespace KalaWindow::OpenGL
{
	//
	// GLOBAL
	//

	static bool isInitialized{};
	static bool isVerboseLoggingEnabled{};

	static uintptr_t openGL32Lib{};

	void OpenGL_Global::SetVerboseLoggingState(bool newState) { isVerboseLoggingEnabled = newState; }
	bool OpenGL_Global::IsVerboseLoggingEnabled() { return isVerboseLoggingEnabled; }

	void OpenGL_Global::Initialize()
	{
		if (isInitialized)
		{
			Log::Print(
				"Cannot initialize global OpenGL more than once!",
				"OPENGL",
				LogType::LOG_ERROR);

			return;
		}

		//
		// CREATE A DUMMY CONTEXT TO LOAD WGL EXTENSIONS
		//

		HWND dummyHWND = CreateDummyWindow();
		HDC dummyDC = GetDC(dummyHWND);

		//
		// SET PIXEL FORMAT FOR DUMMY WINDOW
		//

		PIXELFORMATDESCRIPTOR pfd = {};
		pfd.nSize = sizeof(pfd);
		pfd.nVersion = 1;
		pfd.dwFlags =
			PFD_DRAW_TO_WINDOW
			| PFD_SUPPORT_OPENGL
			| PFD_DOUBLEBUFFER;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = 32;
		pfd.cDepthBits = 24;
		pfd.cStencilBits = 8;

		int pf = ChoosePixelFormat(dummyDC, &pfd);
		if (pf == 0)
		{
			KalaWindowCore::ForceClose(
				"OpenGL error",
				"ChoosePixelFormat failed during global OpenGL init!");

			return;
		}

		if (!SetPixelFormat(dummyDC, pf, &pfd))
		{
			KalaWindowCore::ForceClose(
				"OpenGL error",
				"SetPixelFormat failed during global OpenGL init!");

			return;
		}

		HGLRC dummyRC = wglCreateContext(dummyDC);
		wglMakeCurrent(dummyDC, dummyRC);

		//
		// LOAD OS AND CORE GL FUNCTIONS
		//

		OpenGL_Functions_Core::LoadAllCoreFunctions();
		OpenGL_Functions_Windows::LoadAllWindowsFunctions();

		//
		// CLEAN UP DUMMY
		//

		wglMakeCurrent(nullptr, nullptr);
		wglDeleteContext(dummyRC);
		ReleaseDC(dummyHWND, dummyDC);
		DestroyWindow(dummyHWND);

		Log::Print(
			"Initialized global OpenGL context!",
			"OPENGL",
			LogType::LOG_SUCCESS);

		isInitialized = true;
	}

	bool OpenGL_Global::IsInitialized() { return isInitialized; }

	void OpenGL_Global::SetOpenGLLibrary()
	{
		HMODULE handle = GetModuleHandleA("opengl32.dll");
		if (!handle)
		{
			KalaWindowCore::ForceClose(
				"OpenGL error",
				"Failed to get handle for 'opengl32.dll'");

			return;
		}

		openGL32Lib = FromVar(handle);
	}
	uintptr_t OpenGL_Global::GetOpenGLLibrary()
	{
		if (openGL32Lib == NULL) SetOpenGLLibrary();

		return openGL32Lib;
	}

	bool OpenGL_Global::IsExtensionSupported(const string& name)
	{
		const GL_Core* coreFunc = OpenGL_Functions_Core::GetGLCore();

		if (coreFunc->glGetIntegerv == nullptr)
		{
			KalaWindowCore::ForceClose(
				"OpenGL error",
				"OpenGL core function 'glGetIntegerv' is unassigned! Cannot call 'IsExtensionSupported'.");
				
			return false;
		}
		if (coreFunc->glGetStringi == nullptr)
		{
			KalaWindowCore::ForceClose(
				"OpenGL error",
				"OpenGL core function 'glGetStringi' is unassigned! Cannot call 'IsExtensionSupported'.");
				
			return false;
		}
		
		i32 numExtensions = 0;
		coreFunc->glGetIntegerv(
			GL_NUM_EXTENSIONS,
			&numExtensions);

		for (i32 i = 0; i < numExtensions; ++i)
		{
			const char* extName = reinterpret_cast<const char*>(
				coreFunc->glGetStringi(GL_EXTENSIONS, i));
			if (name == extName) return true;
		}

		return false;
	}
	
	void OpenGL_Global::MakeContextCurrent(
		OpenGL_Context* context,
		uintptr_t handle)
	{
		if (!context)
		{
			Log::Print(
				"Cannot set OpenGL context because the attached context doesn't exist!",
				"OPENGL",
				LogType::LOG_ERROR,
				2);

			return;
		}
		
		if (OpenGL_Context::GetRegistry().runtimeContent.empty())
		{
			Log::Print(
				"Cannot set OpenGL context because no OpenGL contexts have been created!",
				"OPENGL",
				LogType::LOG_ERROR,
				2);

			return;
		}
			
		HGLRC storedHGLRC = ToVar<HGLRC>(context->GetContext());

		if (wglGetCurrentContext() != storedHGLRC) wglMakeCurrent(
			ToVar<HDC>(handle),
			storedHGLRC);
	}
	void OpenGL_Global::MakeContextCurrent(
		uintptr_t context,
		uintptr_t handle)
	{
		if (!context)
		{
			Log::Print(
				"Cannot set OpenGL context because the attached context doesn't exist!",
				"OPENGL",
				LogType::LOG_ERROR,
				2);

			return;
		}

		if (OpenGL_Context::GetRegistry().runtimeContent.empty())
		{
			Log::Print(
				"Cannot set OpenGL context because no OpenGL contexts have been created!",
				"OPENGL",
				LogType::LOG_ERROR,
				2);

			return;
		}

		HGLRC storedHGLRC = ToVar<HGLRC>(context);

		if (wglGetCurrentContext() != storedHGLRC) wglMakeCurrent(
			ToVar<HDC>(handle),
			storedHGLRC);
	}

	bool OpenGL_Global::IsContextValid(OpenGL_Context* context)
	{
		if (!context)
		{
			Log::Print(
				"Cannot check OpenGL context validity because the attached context doesn't exist!",
				"OPENGL",
				LogType::LOG_ERROR,
				2);

			return false;
		}
		
		if (OpenGL_Context::GetRegistry().runtimeContent.empty())
		{
			Log::Print(
				"Cannot check OpenGL context validity because no OpenGL contexts have been created!",
				"OPENGL",
				LogType::LOG_ERROR,
				2);

			return false;
		}
			
		HGLRC storedHGLRC = ToVar<HGLRC>(context->GetContext());

		HGLRC current = wglGetCurrentContext();
		if (!current)
		{
			KalaWindowCore::ForceClose(
				"OpenGL error",
				"Failed to get current context with 'wglGetCurrentContext'!");

			return false;
		}

		if (current != storedHGLRC)
		{
			KalaWindowCore::ForceClose(
				"OpenGL error",
				"Current OpenGL context does not match stored context!");
					
			return false;
		}

		return true;
	}
	bool OpenGL_Global::IsContextValid(uintptr_t context)
	{
		if (!context)
		{
			Log::Print(
				"Cannot check OpenGL context validity because the attached context doesn't exist!",
				"OPENGL",
				LogType::LOG_ERROR,
				2);

			return false;
		}

		if (OpenGL_Context::GetRegistry().runtimeContent.empty())
		{
			Log::Print(
				"Cannot check OpenGL context validity because no OpenGL contexts have been created!",
				"OPENGL",
				LogType::LOG_ERROR,
				2);

			return false;
		}

		HGLRC storedHGLRC = ToVar<HGLRC>(context);

		HGLRC current = wglGetCurrentContext();
		if (!current)
		{
			KalaWindowCore::ForceClose(
				"OpenGL error",
				"Failed to get current context with 'wglGetCurrentContext'!");

			return false;
		}

		if (current != storedHGLRC)
		{
			KalaWindowCore::ForceClose(
				"OpenGL error",
				"Current OpenGL context does not match stored context!");

			return false;
		}

		return true;
	}

	string OpenGL_Global::GetError()
	{	
		const GL_Core* coreFunc = OpenGL_Functions_Core::GetGLCore();

		GLenum error{};
		string errorVal{};

		while ((error = coreFunc->glGetError()) != GL_NO_ERROR)
		{
			switch (error)
			{
			case GL_INVALID_ENUM:                  errorVal = "GL_INVALID_ENUM"; break;
			case GL_INVALID_VALUE:                 errorVal = "GL_INVALID_VALUE"; break;
			case GL_INVALID_INDEX:                 errorVal = "GL_INVALID_INDEX"; break;

			case GL_INVALID_OPERATION:             errorVal = "GL_INVALID_OPERATION"; break;
			case GL_STACK_OVERFLOW:                errorVal = "GL_STACK_OVERFLOW"; break;
			case GL_STACK_UNDERFLOW:               errorVal = "GL_STACK_UNDERFLOW"; break;
			case GL_INVALID_FRAMEBUFFER_OPERATION: errorVal = "GL_INVALID_FRAMEBUFFER_OPERATION"; break;

			case GL_OUT_OF_MEMORY:                 errorVal = "GL_OUT_OF_MEMORY"; break;

			default:                               errorVal = "Unknown error"; break;
			}
		}

		return errorVal;
	}

	//
	// CONTEXT
	//

	static KalaWindowRegistry<OpenGL_Context> registry{};

	KalaWindowRegistry<OpenGL_Context>& OpenGL_Context::GetRegistry() { return registry; }

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
		if (!OpenGL_Global::IsInitialized())
		{
			KalaWindowCore::ForceClose(
				"OpenGL error",
				"Cannot initialize OpenGL context because global OpenGL has not yet been initialized!");

			return nullptr;
		}

		Window* window = Window::GetRegistry().GetContent(windowID);

		if (!window
			|| !window->IsInitialized())
		{
			KalaWindowCore::ForceClose(
				"OpenGL error",
				"Cannot initialize OpenGL context because it's window was not found!");

			return nullptr;
		}

		const GL_Core* coreFunc = OpenGL_Functions_Core::GetGLCore();
		const GL_Windows* windowsFunc = OpenGL_Functions_Windows::GetGLWindows();

		u32 newID = KalaWindowCore::GetGlobalID() + 1;
		KalaWindowCore::SetGlobalID(newID);

		unique_ptr<OpenGL_Context> newCont = make_unique<OpenGL_Context>();
		OpenGL_Context* contPtr = newCont.get();

		Log::Print(
			"Creating OpenGL context for window '" + window->GetTitle() + "' with ID '" + to_string(newID) + "'.",
			"OPENGL",
			LogType::LOG_DEBUG);

		contPtr->ID = newID;

		const WindowData& wData = window->GetWindowData();
		HWND windowRef = ToVar<HWND>(wData.hwnd);

		HDC hdc = ToVar<HDC>(wData.hdc);

		vector<int> pixelAttribs =
		{
			WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
			WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
			WGL_DOUBLE_BUFFER_ARB, GL_TRUE
		};

		//color buffer
		switch (cBits)
		{
		case ColorBufferBits::COLOR_RGBA8:
			pixelAttribs.push_back(WGL_COLOR_BITS_ARB);
			pixelAttribs.push_back(32);
			pixelAttribs.push_back(WGL_PIXEL_TYPE_ARB);
			pixelAttribs.push_back(WGL_TYPE_RGBA_ARB);
			break;
		case ColorBufferBits::COLOR_RGB10_A2:
			pixelAttribs.push_back(WGL_COLOR_BITS_ARB);
			pixelAttribs.push_back(32);
			pixelAttribs.push_back(WGL_ALPHA_BITS_ARB);
			pixelAttribs.push_back(2);
			pixelAttribs.push_back(WGL_PIXEL_TYPE_ARB);
			pixelAttribs.push_back(WGL_TYPE_RGBA_ARB);
			break;
		}

		//depth buffer
		switch (dBits)
		{
		case DepthBufferBits::DEPTH_16:
			pixelAttribs.push_back(WGL_DEPTH_BITS_ARB);
			pixelAttribs.push_back(16);
			break;
		case DepthBufferBits::DEPTH_24:
			pixelAttribs.push_back(WGL_DEPTH_BITS_ARB);
			pixelAttribs.push_back(24);
			break;
		}

		//stencil buffer
		switch (sBits)
		{
		case StencilBufferBits::STENCIL_NONE:
			pixelAttribs.push_back(WGL_STENCIL_BITS_ARB);
			pixelAttribs.push_back(0);
			break;
		case StencilBufferBits::STENCIL_8:
			pixelAttribs.push_back(WGL_STENCIL_BITS_ARB);
			pixelAttribs.push_back(8);
			break;
		}

		//alpha channel
		switch (aChannel)
		{
		case AlphaChannel::ALPHA_NONE:
			pixelAttribs.push_back(WGL_ALPHA_BITS_ARB);
			pixelAttribs.push_back(0);
			break;
		case AlphaChannel::ALPHA_8:
			pixelAttribs.push_back(WGL_ALPHA_BITS_ARB);
			pixelAttribs.push_back(8);
			break;
		}

		//msaa
		if (msaa != MultiSampling::MSAA_1X)
		{
			pixelAttribs.push_back(WGL_SAMPLE_BUFFERS_ARB);
			pixelAttribs.push_back(1);
			pixelAttribs.push_back(WGL_SAMPLES_ARB);
			pixelAttribs.push_back(static_cast<int>(msaa));
		}

		//srgb
		if (srgb == SRGBMode::SRGB_ENABLED)
		{
			pixelAttribs.push_back(WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB);
			pixelAttribs.push_back(GL_TRUE);
		}

		//always use full hardware acceleration
		pixelAttribs.push_back(WGL_ACCELERATION_ARB);
		pixelAttribs.push_back(WGL_FULL_ACCELERATION_ARB);

		//always use exchange swap method
		pixelAttribs.push_back(WGL_SWAP_METHOD_ARB);
		pixelAttribs.push_back(WGL_SWAP_EXCHANGE_ARB);

		//null terminator
		pixelAttribs.push_back(0);

		int pfID = 0;
		UINT numFormats = 0;

		BOOL result = (windowsFunc->wglChoosePixelFormatARB(
			hdc,
			pixelAttribs.data(),
			nullptr,
			1,
			&pfID,
			&numFormats));

		bool success = result && numFormats > 0;

		if (!result)
		{
			KalaWindowCore::ForceClose(
				"OpenGL error",
				GetErrorType("wglChoosePixelFormatARB"));

			return nullptr;
		}
		else if (numFormats == 0)
		{
			KalaWindowCore::ForceClose(
				"OpenGL error",
				"wglChoosePixelFormatARB returned no matching pixel formats!");

			return nullptr;
		}

		PIXELFORMATDESCRIPTOR pfd{};
		int describeResult = DescribePixelFormat(
			hdc,
			pfID,
			sizeof(pfd),
			&pfd);

		if (describeResult == 0)
		{
			KalaWindowCore::ForceClose(
				"OpenGL error",
				"DescribePixelFormat failed!");

			return nullptr;
		}

		if (!SetPixelFormat(
			hdc,
			pfID,
			&pfd))
		{
			KalaWindowCore::ForceClose(
				"OpenGL error",
				"SetPixelFormat failed!");

			return nullptr;
		}

		int attrib_color_bits = WGL_COLOR_BITS_ARB;
		int colorBits = 0;
		windowsFunc->wglGetPixelFormatAttribivARB(hdc, pfID, 0, 1, &attrib_color_bits, &colorBits);

		int attrib_depth_bits = WGL_DEPTH_BITS_ARB;
		int depthBits = 0;
		windowsFunc->wglGetPixelFormatAttribivARB(hdc, pfID, 0, 1, &attrib_depth_bits, &depthBits);

		int attrib_stencil_bits = WGL_STENCIL_BITS_ARB;
		int stencilBits = 0;
		windowsFunc->wglGetPixelFormatAttribivARB(hdc, pfID, 0, 1, &attrib_stencil_bits, &stencilBits);

		int attrib_alpha_bits = WGL_ALPHA_BITS_ARB;
		int alphaBits = 0;
		windowsFunc->wglGetPixelFormatAttribivARB(hdc, pfID, 0, 1, &attrib_alpha_bits, &alphaBits);

		int attrib_draw = WGL_DRAW_TO_WINDOW_ARB;
		int drawToWindow = 0;
		windowsFunc->wglGetPixelFormatAttribivARB(hdc, pfID, 0, 1, &attrib_draw, &drawToWindow);

		int attrib_gl = WGL_SUPPORT_OPENGL_ARB;
		int supportGL = 0;
		windowsFunc->wglGetPixelFormatAttribivARB(hdc, pfID, 0, 1, &attrib_gl, &supportGL);

		int attrib_double_buffer = WGL_DOUBLE_BUFFER_ARB;
		int doubleBuffer = 0;
		windowsFunc->wglGetPixelFormatAttribivARB(hdc, pfID, 0, 1, &attrib_double_buffer, &doubleBuffer);

		//MSAA samples
		int attrib_MSAA_buffers = WGL_SAMPLE_BUFFERS_ARB;
		int sampleBuffer = 0;
		int attrib_MSAA_samples = WGL_SAMPLES_ARB;
		int samples = 0;
		windowsFunc->wglGetPixelFormatAttribivARB(hdc, pfID, 0, 1, &attrib_MSAA_buffers, &sampleBuffer);
		windowsFunc->wglGetPixelFormatAttribivARB(hdc, pfID, 0, 1, &attrib_MSAA_samples, &samples);
		string msaaVal = sampleBuffer == 1 ? to_string(samples) + "x" : "Disabled";

		//sRGB capable
		int attrib_srgb = WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB;
		int srgbBuffer = 0;
		windowsFunc->wglGetPixelFormatAttribivARB(hdc, pfID, 0, 1, &attrib_srgb, &srgbBuffer);

		//acceleration
		int attrib_acceleration = WGL_ACCELERATION_ARB;
		int accel = 0;
		windowsFunc->wglGetPixelFormatAttribivARB(hdc, pfID, 0, 1, &attrib_acceleration, &accel);

		//swap method
		int attrib_swap_method = WGL_SWAP_METHOD_ARB;
		int swap = 0;
		windowsFunc->wglGetPixelFormatAttribivARB(hdc, pfID, 0, 1, &attrib_swap_method, &swap);

		string accelVal = (accel == WGL_FULL_ACCELERATION_ARB)
			? "Hardware"
			: (accel == WGL_NO_ACCELERATION_ARB)
			? "Software"
			: "Generic";

		string swapVal = (swap == WGL_SWAP_EXCHANGE_ARB)
			? "Exchange"
			: (swap == WGL_SWAP_COPY_ARB)
			? "Copy"
			: (swap == WGL_SWAP_UNDEFINED_ARB)
			? "Undefined"
			: "Unknown";

		ostringstream ss{};
		ss << "Pixel Format Details:\n"
			<< "  Pixel format ID: " << pfID << "\n"
			<< "  Color bits:      " << colorBits << "\n"
			<< "  Depth bits:      " << depthBits << "\n"
			<< "  Stencil bits:    " << stencilBits << "\n"
			<< "  Alpha bits:      " << alphaBits << "\n"
			<< "  Flags:\n"
			<< "    Draw to window:    " << (drawToWindow == 1 ? "Yes" : "No") << "\n"
			<< "    Support OpenGL:    " << (supportGL == 1 ? "Yes" : "No") << "\n"
			<< "    Double buffer:     " << (doubleBuffer == 1 ? "Yes" : "No") << "\n"
			<< "    MSAA:              " << (sampleBuffer == 1 ? to_string(samples) + "x" : "Disabled") << "\n"
			<< "    SRGB capable:      " << (srgbBuffer == 1 ? "Yes" : "No") << "\n"
			<< "    Acceleration type: " << accelVal << "\n"
			<< "    Swap method:       " << swapVal;

		if (OpenGL_Global::IsVerboseLoggingEnabled())
		{
			Log::Print(
				ss.str(),
				"OPENGL",
				LogType::LOG_INFO);
		}

		int attribs[] =
		{
			WGL_CONTEXT_MAJOR_VERSION_ARB,
			3,
			WGL_CONTEXT_MINOR_VERSION_ARB,
			3,
			WGL_CONTEXT_PROFILE_MASK_ARB,
			WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
#ifdef _DEBUG
			WGL_CONTEXT_FLAGS_ARB,
			WGL_CONTEXT_DEBUG_BIT_ARB
			| WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
#endif
			0
		};

		HGLRC existing{};

		if (parentContext != 0)
		{
			OpenGL_Context* parentCont{};

			if (registry.createdContent.contains(parentContext))
			{
				parentCont = registry.createdContent[parentContext].get();
			}

			if (parentCont)
			{
				if (parentCont->GetID() == contPtr->ID)
				{
					Log::Print(
						"OperGL parent context ID cannot be same as this context ID!",
						"OPENGL",
						LogType::LOG_ERROR);
				}
				else existing = ToVar<HGLRC>(parentCont->hglrc);
			}
		}

		HGLRC newHGLRC = windowsFunc->wglCreateContextAttribsARB(
			hdc,
			existing,
			attribs);

		if (!newHGLRC)
		{
			KalaWindowCore::ForceClose(
				"OpenGL error",
				GetErrorType("wglCreateContextAttribsARB"));

			return nullptr;
		}

		contPtr->hglrc = FromVar(newHGLRC);

		wglMakeCurrent(hdc, newHGLRC);
		windowsFunc->wglSwapIntervalEXT(1); //default vsync is true

		//and finally set opengl viewport size
		vec2 clientRectSize = window->GetClientRectSize();
		coreFunc->glViewport(
			0,
			0,
			(GLsizei)clientRectSize.x,
			(GLsizei)clientRectSize.y);

		const char* glVersion  = reinterpret_cast<const char*>(coreFunc->glGetString(GL_VERSION));
		const char* glVendor   = reinterpret_cast<const char*>(coreFunc->glGetString(GL_VENDOR));
		const char* glRenderer = reinterpret_cast<const char*>(coreFunc->glGetString(GL_RENDERER));
		const char* glslVer    = reinterpret_cast<const char*>(coreFunc->glGetString(GL_SHADING_LANGUAGE_VERSION));

		string profileVal{};
		GLint profile = 0;
		coreFunc->glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &profile);

		if (profile & GL_CONTEXT_CORE_PROFILE_BIT)
		{
			profileVal = "Core profile";
		}
		else if (profile & GL_CONTEXT_COMPATIBILITY_PROFILE_BIT)
		{
			profileVal = "Compatibility profile";
		}
		else profileVal = "Unknown profile";

		GLint blockSize = 0;
		coreFunc->glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &blockSize);

		//also enable srgb if user requested it
		if (srgb == SRGBMode::SRGB_ENABLED
			&& srgbBuffer == 1)
		{
			coreFunc->glEnable(GL_FRAMEBUFFER_SRGB);
		}

		//sRGB enabled
		GLboolean srgbEnabled = GL_FALSE;
		coreFunc->glGetBooleanv(GL_FRAMEBUFFER_SRGB, &srgbEnabled);

		ostringstream ss2{};
		ss2 << "OpenGL Context Info:\n"
			<< "  Version:        " << (glVersion ? glVersion : "Unknown") << "\n"
			<< "  Vendor:         " << (glVendor ? glVendor : "Unknown") << "\n"
			<< "  Renderer:       " << (glRenderer ? glRenderer : "Unknown") << "\n"
			<< "  GLSL:           " << (glslVer ? glslVer : "Unknown") << "\n"
			<< "  Profile:        " << profileVal << "\n"
			<< "  UBO block size: " << to_string(blockSize) << "\n"
			<< "  SRGB enabled:   " << (srgbEnabled ? "Yes" : "No");

		if (OpenGL_Global::IsVerboseLoggingEnabled())
		{
			Log::Print(
				ss2.str(),
				"OPENGL",
				LogType::LOG_INFO);
		}

		contPtr->contextData = ss2.str();

		registry.AddContent(newID, std::move(newCont));
		window->SetGLID(newID);

		contPtr->windowID = window->GetID();

		contPtr->isInitialized = true;

		Log::Print(
			"Initialized OpenGL context for window '" + window->GetTitle() + "' with ID '" + to_string(newID) + "'!",
			"OPENGL",
			LogType::LOG_SUCCESS);

		return contPtr;
	}

	bool OpenGL_Context::IsInitialized() const { return isInitialized; }

	u32 OpenGL_Context::GetID() const { return ID; }
	u32 OpenGL_Context::GetWindowID() const { return windowID; }
	
	void OpenGL_Context::SwapOpenGLBuffers(uintptr_t handle)
	{
		if (!handle)
		{
			Log::Print(
				"Cannot swap OpenGL buffers because the window handle (hdc) is invalid!",
				"OPENGL",
				LogType::LOG_ERROR,
				2);

			return;
		}

		SwapBuffers(ToVar<HDC>(handle));
	}

	const string& OpenGL_Context::GetContextData() { return contextData; }

	uintptr_t OpenGL_Context::GetContext() const { return hglrc; }
	uintptr_t OpenGL_Context::GetParentContext() const { return parentHglrc; }
	
	void OpenGL_Context::SetVSyncState(VSyncState newValue)
	{		
		const GL_Windows* windowsFunc = OpenGL_Functions_Windows::GetGLWindows();

		if (!windowsFunc->wglSwapIntervalEXT)
		{
			Log::Print(
				"wglSwapIntervalEXT not supported! VSync setting ignored.",
				"OPENGL",
				LogType::LOG_ERROR,
				2);
				
			return;
		}
		
		vsyncState = newValue;

		windowsFunc->wglSwapIntervalEXT(newValue == VSyncState::VSYNC_ON
			? 1
			: 0);
	}
	VSyncState OpenGL_Context::GetVSyncState() const { return vsyncState; }

	OpenGL_Context::~OpenGL_Context()
	{
		if (!isInitialized)
		{
			Log::Print(
				"Cannot shut down OpenGL context because it is not initialized!",
				"OPENGL",
				LogType::LOG_ERROR,
				2);

			return;
		}

		Window* window = Window::GetRegistry().GetContent(windowID);

		if (!window
			|| !window->IsInitialized())
		{
			Log::Print(
				"Cannot shut down OpenGL context because its window was not found!",
				"OPENGL",
				LogType::LOG_ERROR,
				2);

			return;
		}

		Log::Print(
			"Destroying OpenGL context for window '" + window->GetTitle() + "' with ID '" + to_string(ID) + "'.",
			"OPENGL",
			LogType::LOG_DEBUG);

#ifdef _WIN32
		HGLRC storedHGLRC = ToVar<HGLRC>(hglrc);

		if (storedHGLRC != NULL)
		{
			if (wglGetCurrentContext() == storedHGLRC) wglMakeCurrent(nullptr, nullptr);
			wglDeleteContext(storedHGLRC);
		}
#elif __linux__
		//TODO: DEFINE
#endif
	}
}

bool IsCorrectVersion()
{
	const GL_Core* coreFunc = OpenGL_Functions_Core::GetGLCore();

	const char* versionStr = reinterpret_cast<const char*>(coreFunc->glGetString(GL_VERSION));
	if (!versionStr) return false;

	int major = 0;
	int minor = 0;
	if (sscanf_s(versionStr, "%d.%d", &major, &minor) != 2)
	{
		return false;
	}

	return
		(major > 3)
		|| (major == 3
		&& minor >= 3);
}

HWND CreateDummyWindow()
{
	WNDCLASSA wc = {};
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = DefWindowProcA;
	wc.hInstance = GetModuleHandleA(nullptr);
	wc.lpszClassName = "KalaWindowDummy";

	RegisterClassA(&wc);

	//create the hidden dummy window
	HWND hwnd = CreateWindowExA(
		0,
		wc.lpszClassName,
		"dummy opengl window",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 
		CW_USEDEFAULT,
		CW_USEDEFAULT, 
		CW_USEDEFAULT,
		nullptr, 
		nullptr,
		wc.hInstance,
		nullptr);

	return hwnd;
}

string GetErrorType(const string& errorOrigin)
{
	DWORD err = GetLastError();

	string message{};

	switch (err)
	{
	case ERROR_INVALID_OPERATION:    message = "ERROR_INVALID_OPERATION";      break;
	case ERROR_INVALID_PIXEL_FORMAT: message = "ERROR_INVALID_PIXEL_FORMAT";   break;
	case ERROR_INVALID_PARAMETER:    message = "ERROR_INVALID_PARAMETER";      break;
	case ERROR_OUTOFMEMORY:          message = "ERROR_OUTOFMEMORY";            break;
	case 0x2095:                     message = "ERROR_INVALID_VERSION_ARB";    break;
	case 0x2096:                     message = "ERROR_INVALID_PROFILE_ARB";    break;
	case ERROR_SUCCESS:              message = "ERROR_SUCCESS (no error set)"; break;
	default:                         message = "Unknown error";                break;
	}

	ostringstream oss{};
	oss << errorOrigin << " failed! Win32 error = "
		<< message << " (" << dec << err << " / 0x" << hex << err << dec << ")";

	return oss.str();
}

#endif //_WIN32