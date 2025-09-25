//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>

#include "KalaHeaders/core_utils.hpp"

#include "graphics/window.hpp"

namespace KalaWindow::Graphics::OpenGL
{
	using std::string;

	enum VSyncState
	{
		VSYNC_ON, //Framerate is capped to monitor refresh rate.
		VSYNC_OFF //Framerate is uncapped, runs as fast as render loop allows, introduces tearing.
	};

	//Hardware accelerated antialiasing
	enum MultiSampling
	{
		MSAA_1X = 1, //Same as multisampling disabled
		MSAA_2X = 2,
		MSAA_4X = 4, //default
		MSAA_8X = 8,
		MSAA_16X = 16
	};
	enum SRGBMode
	{
		SRGB_ENABLED, //Enable color-correct gamma rendering (default)
		SRGB_DISABLED //Colors will look washed out when using linear-space lighting
	};
	enum ColorBufferBits
	{
		COLOR_RGBA8,   //8 bits per channel (default)
		COLOR_RGB10_A2 //10 bits color, 2 bits alpha (better color precision, sacrifices alpha quality)
	};
	enum DepthBufferBits
	{
		DEPTH_16, //16-bit integer depth (saves VRAM, bad precision over large distances)
		DEPTH_24  //24-bit integer point depth (default)
	};
	enum StencilBufferBits
	{
		STENCIL_NONE, //Disables stencil completely (default)
		STENCIL_8     //8-bit stencil
	};
	enum AlphaChannel
	{
		ALPHA_NONE, //Disables alpha channel completely, cannot have transparent meshes or textures
		ALPHA_8     //8-bit alpha channel (default)
	};

	class LIB_API OpenGL_Renderer
	{
	public:
		//
		// GENERAL OPENGL
		//

		static inline bool IsInitialized() { return isInitialized; }

		static inline VSyncState GetVSyncState() { return vsyncState; }
		static inline const string& GetContextData() { return contextData; }

		//Toggle verbose logging. If true, then usually frequently updated runtime values like
		//GL notifications will dump their logs into the console.
		static inline void SetVerboseLoggingState(bool newState) { isVerboseLoggingEnabled = newState; }
		static inline bool IsVerboseLoggingEnabled() { return isVerboseLoggingEnabled; }

		//Check if this extension is supported by the current context (OpenGL 3.3)
		static bool IsExtensionSupported(const string& name);

		//Place after any gl call to check if an issue or error has occurred within that point.
		//Loops through all errors so that all errors at that point are printed, not just the first one.
		static string GetError();

		//
		// OS-SPECIFIC
		//

		//Global one-time OpenGL 3.3 init, needs to be called before per-window OpenGL init
		static bool GlobalInitialize();

		//Per-window OpenGL context init
		static bool Initialize(
			Window* targetWindow,
			MultiSampling msaa = MultiSampling::MSAA_4X,
			SRGBMode srgb = SRGBMode::SRGB_ENABLED,
			ColorBufferBits cBits = ColorBufferBits::COLOR_RGBA8,
			DepthBufferBits dBits = DepthBufferBits::DEPTH_24,
			StencilBufferBits sBits = StencilBufferBits::STENCIL_NONE,
			AlphaChannel aChannel = AlphaChannel::ALPHA_8);

		//Allows to set vsync true or false.
		static void SetVSyncState(VSyncState vsyncState);

		//Call at the end of your render loop
		static void SwapOpenGLBuffers(Window* targetWindow);

		static void MakeContextCurrent(Window* window);
		static bool IsContextValid(Window* window);

		//Close the opengl context and clean the opengl textures and shaders.
		//This should always be called before the window that owns the opengl context is destroyed as well.
		static void Shutdown(Window* window);
	private:
		static inline bool isInitialized{};
		static inline bool isVerboseLoggingEnabled{};

		static inline string contextData{};

		//If off, then all framerate is uncapped
		static inline VSyncState vsyncState = VSyncState::VSYNC_ON;
	};
}