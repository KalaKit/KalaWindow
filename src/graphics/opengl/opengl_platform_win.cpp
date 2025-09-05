//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef _WIN32

#include <Windows.h>
#include <string>
#include <sstream>

#include "KalaHeaders/logging.hpp"

#include "graphics/opengl/opengl.hpp"
#include "graphics/opengl/opengl_functions_core.hpp"
#include "graphics/opengl/opengl_functions_win.hpp"
#include "graphics/window.hpp"
#include "core/core.hpp"
#include "core/global_handles.hpp"

using KalaHeaders::Log;
using KalaHeaders::LogType;

using KalaWindow::Core::KalaWindowCore;
using KalaWindow::Core::GlobalHandle;
using KalaWindow::Graphics::Window;
using namespace KalaWindow::Graphics::OpenGLFunctions;

using std::string;
using std::to_string;
using std::ostringstream;
using std::dec;
using std::hex;

static bool IsCorrectVersion();
static HWND CreateDummyWindow();

static string GetErrorType(const string& errorOrigin)
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

namespace KalaWindow::Graphics::OpenGL
{
	bool OpenGL_Renderer::GlobalInitialize()
	{
		if (isInitialized
			&& GlobalHandle::GetOpenGLWinContext() != NULL)
		{
			Log::Print(
				"Cannot initialize global OpenGL more than once!",
				"OPENGL_WINDOWS",
				LogType::LOG_SUCCESS);

			return false;
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
				"OpenGL Window Error",
				"ChoosePixelFormat failed during global OpenGL init!");

			return false;
		}

		if (!SetPixelFormat(dummyDC, pf, &pfd))
		{
			KalaWindowCore::ForceClose(
				"OpenGL Window Error",
				"SetPixelFormat failed during global OpenGL init!");

			return false;
		}

		HGLRC dummyRC = wglCreateContext(dummyDC);
		wglMakeCurrent(dummyDC, dummyRC);

		//
		// LOAD WGL AND CORE EXTENSIONS
		//

		OpenGL_Functions_Core::LoadAllCoreFunctions();
		OpenGL_Functions_Windows::LoadAllWinFunctions();

		//
		// CLEAN UP DUMMY
		//

		wglMakeCurrent(nullptr, nullptr);
		wglDeleteContext(dummyRC);
		ReleaseDC(dummyHWND, dummyDC);
		DestroyWindow(dummyHWND);

		Log::Print(
			"Initialized OpenGL!",
			"OPENGL_WINDOWS",
			LogType::LOG_SUCCESS);

		isInitialized = true;

		return true;
	}

	bool OpenGL_Renderer::Initialize(
		Window* targetWindow,
		MultiSampling msaa,
		SRGBMode srgb,
		ColorBufferBits cBits,
		DepthBufferBits dBits,
		StencilBufferBits sBits,
		AlphaChannel aChannel)
	{
		if (targetWindow->GetOpenGLData().hdc != NULL)
		{
			Log::Print(
				"Cannot initialize OpenGL more than once per window!",
				"OPENGL_WINDOWS",
				LogType::LOG_ERROR,
				2);

			return false;
		}

		const WindowData& wData = targetWindow->GetWindowData();
		HWND windowRef = ToVar<HWND>(wData.hwnd);

		OpenGLData data{};

		HDC hdc = GetDC(windowRef);

		data.hdc = FromVar(hdc);

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

		BOOL result = (wglChoosePixelFormatARB(
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
				"OpenGL context error",
				GetErrorType("wglChoosePixelFormatARB"));

			return false;
		}
		else if (numFormats == 0)
		{
			KalaWindowCore::ForceClose(
				"OpenGL context error",
				"wglChoosePixelFormatARB returned no matching pixel formats!");

			return false;
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
				"OpenGL context error",
				"DescribePixelFormat failed!");

			return false;
		}

		if (!SetPixelFormat(
			hdc,
			pfID,
			&pfd))
		{
			KalaWindowCore::ForceClose(
				"OpenGL context error",
				"SetPixelFormat failed!");

			return false;
		}

		int attrib_color_bits = WGL_COLOR_BITS_ARB;
		int colorBits = 0;
		wglGetPixelFormatAttribivARB(hdc, pfID, 0, 1, &attrib_color_bits, &colorBits);

		int attrib_depth_bits = WGL_DEPTH_BITS_ARB;
		int depthBits = 0;
		wglGetPixelFormatAttribivARB(hdc, pfID, 0, 1, &attrib_depth_bits, &depthBits);

		int attrib_stencil_bits = WGL_STENCIL_BITS_ARB;
		int stencilBits = 0;
		wglGetPixelFormatAttribivARB(hdc, pfID, 0, 1, &attrib_stencil_bits, &stencilBits);

		int attrib_alpha_bits = WGL_ALPHA_BITS_ARB;
		int alphaBits = 0;
		wglGetPixelFormatAttribivARB(hdc, pfID, 0, 1, &attrib_alpha_bits, &alphaBits);

		int attrib_draw = WGL_DRAW_TO_WINDOW_ARB;
		int drawToWindow = 0;
		wglGetPixelFormatAttribivARB(hdc, pfID, 0, 1, &attrib_draw, &drawToWindow);

		int attrib_gl = WGL_SUPPORT_OPENGL_ARB;
		int supportGL = 0;
		wglGetPixelFormatAttribivARB(hdc, pfID, 0, 1, &attrib_gl, &supportGL);

		int attrib_double_buffer = WGL_DOUBLE_BUFFER_ARB;
		int doubleBuffer = 0;
		wglGetPixelFormatAttribivARB(hdc, pfID, 0, 1, &attrib_double_buffer, &doubleBuffer);

		//MSAA samples
		int attrib_MSAA_buffers = WGL_SAMPLE_BUFFERS_ARB;
		int sampleBuffer = 0;
		int attrib_MSAA_samples = WGL_SAMPLES_ARB;
		int samples = 0;
		wglGetPixelFormatAttribivARB(hdc, pfID, 0, 1, &attrib_MSAA_buffers, &sampleBuffer);
		wglGetPixelFormatAttribivARB(hdc, pfID, 0, 1, &attrib_MSAA_samples, &samples);
		string msaaVal = sampleBuffer == 1 ? to_string(samples) + "x" : "Disabled";

		//sRGB capable
		int attrib_srgb = WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB;
		int srgbBuffer = 0;
		wglGetPixelFormatAttribivARB(hdc, pfID, 0, 1, &attrib_srgb, &srgbBuffer);

		//acceleration
		int attrib_acceleration = WGL_ACCELERATION_ARB;
		int accel = 0;
		wglGetPixelFormatAttribivARB(hdc, pfID, 0, 1, &attrib_acceleration, &accel);

		//swap method
		int attrib_swap_method = WGL_SWAP_METHOD_ARB;
		int swap = 0;
		wglGetPixelFormatAttribivARB(hdc, pfID, 0, 1, &attrib_swap_method, &swap);

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
			<< "    Support OpenGL:    " << (supportGL == 1    ? "Yes" : "No") << "\n"
			<< "    Double buffer:     " << (doubleBuffer == 1 ? "Yes" : "No") << "\n"
			<< "    MSAA:              " << (sampleBuffer == 1 ? to_string(samples) + "x" : "Disabled") << "\n"
			<< "    SRGB capable:      " << (srgbBuffer == 1 ? "Yes" : "No") << "\n"
			<< "    Acceleration type: " << accelVal << "\n"
			<< "    Swap method:       " << swapVal;

		contextData = ss.str();

		Log::Print(
			contextData,
			"OPENGL_WINDOWS",
			LogType::LOG_INFO);

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

		HGLRC existing = ToVar<HGLRC>(GlobalHandle::GetOpenGLWinContext());

		HGLRC hglrc = wglCreateContextAttribsARB(
			hdc,
			existing,
			attribs);

		if (!hglrc)
		{
			KalaWindowCore::ForceClose(
				"OpenGL context error",
				GetErrorType("wglCreateContextAttribsARB"));

			return false;
		}

		if (existing == NULL) GlobalHandle::SetOpenGLWinContext(FromVar(hglrc));

		data.hglrc = FromVar(hglrc);

		wglMakeCurrent(hdc, hglrc);
		wglSwapIntervalEXT(1); //default vsync is true

		//and finally set opengl viewport size
		vec2 framebufferSize = targetWindow->GetFramebufferSize();
		glViewport(
			0,
			0,
			(GLsizei)framebufferSize.x,
			(GLsizei)framebufferSize.y);

		targetWindow->SetOpenGLData(data);

		const char* glVersion  = reinterpret_cast<const char*>(glGetString(GL_VERSION));
		const char* glVendor   = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
		const char* glRenderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
		const char* glslVer    = reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));

		string profileVal{};
		GLint profile = 0;
		glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &profile);

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
		glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &blockSize);

		//also enable srgb if user requested it
		if (srgb == SRGBMode::SRGB_ENABLED
			&& srgbBuffer == 1)
		{
			glEnable(GL_FRAMEBUFFER_SRGB);
		}

		//sRGB enabled
		GLboolean srgbEnabled = GL_FALSE;
		glGetBooleanv(GL_FRAMEBUFFER_SRGB, &srgbEnabled);

		ostringstream ss2{};
		ss2 << "OpenGL Context Info:\n"
			<< "  Version:        " << (glVersion  ? glVersion  : "Unknown") << "\n"
			<< "  Vendor:         " << (glVendor   ? glVendor   : "Unknown") << "\n"
			<< "  Renderer:       " << (glRenderer ? glRenderer : "Unknown") << "\n"
			<< "  GLSL:           " << (glslVer    ? glslVer    : "Unknown") << "\n"
			<< "  Profile:        " << profileVal << "\n"
			<< "  UBO block size: " << to_string(blockSize) << "\n"
			<< "  SRGB enabled: " << (srgbEnabled ? "Yes" : "No");

		Log::Print(
			ss2.str(),
			"OPENGL_WINDOWS",
			LogType::LOG_INFO);

		Log::Print(
			"Initialized OpenGL context for Window '" + targetWindow->GetTitle() + "'!",
			"OPENGL_WINDOWS",
			LogType::LOG_SUCCESS);

		return true;
	}

	void OpenGL_Renderer::SetVSyncState(VSyncState newVSyncState)
	{
		vsyncState = newVSyncState;

		if (wglSwapIntervalEXT)
		{
			if (newVSyncState == VSyncState::VSYNC_ON) wglSwapIntervalEXT(1);
			else                                       wglSwapIntervalEXT(0);
		}
		else
		{
			Log::Print(
				"wglSwapIntervalEXT not supported! VSync setting ignored.",
				"OPENGL",
				LogType::LOG_ERROR,
				2);
		}
	}

	void OpenGL_Renderer::SwapOpenGLBuffers(Window* targetWindow)
	{
		if (!IsInitialized())
		{
			Log::Print(
				"Cannot swap opengl buffers because OpenGL is not initialized!",
				"OPENGL_WINDOWS",
				LogType::LOG_ERROR);
			return;
		}

		const OpenGLData& oData = targetWindow->GetOpenGLData();
		HDC hdc = ToVar<HDC>(oData.hdc);
		SwapBuffers(hdc);
	}

	void OpenGL_Renderer::MakeContextCurrent(Window* window)
	{
		const OpenGLData& oData = window->GetOpenGLData();

		if (oData.hdc == 0
			|| oData.hglrc == 0)
		{
			Log::Print(
				"Cannot make OpenGL context current for window '" + window->GetTitle() + "' because its hdc or hglrc is not assigned!",
				"OPENGL_WINDOWS",
				LogType::LOG_ERROR);

			return;
		}

		HDC hdc = ToVar<HDC>(oData.hdc);
		HGLRC hglrc = ToVar<HGLRC>(oData.hglrc);

		if (wglGetCurrentContext() != hglrc) wglMakeCurrent(hdc, hglrc);
	}

	bool OpenGL_Renderer::IsContextValid(Window* targetWindow)
	{
		const OpenGLData& oData = targetWindow->GetOpenGLData();

		if (oData.hglrc == NULL)
		{
			string title = "OpenGL Error";
			string reason = "Failed to get HGLRC for window '" + targetWindow->GetTitle() + "' during 'IsContextValid' stage!";
			KalaWindowCore::ForceClose(title, reason);
			return false;
		}
		HGLRC hglrcReal = ToVar<HGLRC>(oData.hglrc);

		HGLRC current = wglGetCurrentContext();
		if (current == nullptr)
		{
			Log::Print(
				"OpenGL context is null!",
				"OPENGL_WINDOWS",
				LogType::LOG_ERROR,
				2);

			return false;
		}

		if (current != hglrcReal)
		{
			KalaWindowCore::ForceClose(
				"OpenGL Error",
				"Current OpenGL context does not match stored context!");
			return false;
		}

		return true;
	}

	void OpenGL_Renderer::Shutdown(Window* window)
	{
		if (!isInitialized)
		{
			Log::Print(
				"Failed to shut down OpenGL because it has not yet been initialized!",
				"OPENGL",
				LogType::LOG_ERROR,
				2);

			return;
		}

		if (window == nullptr)
		{
			Log::Print(
				"Failed to shut down OpenGL because its target window is invalid!",
				"OPENGL",
				LogType::LOG_ERROR,
				2);

			return;
		}

#ifdef _WIN32
		OpenGLData oData = window->GetOpenGLData();
		
		HGLRC hglrc = ToVar<HGLRC>(oData.hglrc);
		HDC hdc = ToVar<HDC>(oData.hdc);
		HWND hwnd = ToVar<HWND>(window->GetWindowData().hwnd);

		if (hglrc != NULL)
		{
			if (wglGetCurrentContext() == hglrc) wglMakeCurrent(nullptr, nullptr);
			wglDeleteContext(hglrc);
		}
		if (hdc != NULL) ReleaseDC(hwnd, hdc);
#elif __linux__
		//TODO: DEFINE
#endif
	}
}

bool IsCorrectVersion()
{
	const char* versionStr = reinterpret_cast<const char*>(glGetString(GL_VERSION));
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

#endif //_WIN32