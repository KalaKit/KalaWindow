//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <functional>

#include "KalaHeaders/core_utils.hpp"

#include "core/kw_registry.hpp"

namespace KalaWindow::OpenGL
{
	using std::string;
	using std::function;

	using u32 = uint32_t;

	using KalaWindow::Core::KalaWindowRegistry;
	
	enum class VSyncState
	{
		VSYNC_ON, //Framerate is capped to monitor refresh rate.
		VSYNC_OFF //Framerate is uncapped, runs as fast as render loop allows, introduces tearing.
	};

	//Hardware accelerated antialiasing
	enum class MultiSampling
	{
		MSAA_1X = 1, //Same as multisampling disabled
		MSAA_2X = 2,
		MSAA_4X = 4, //default
		MSAA_8X = 8,
		MSAA_16X = 16
	};
	enum class SRGBMode
	{
		SRGB_ENABLED, //Enable color-correct gamma rendering (default)
		SRGB_DISABLED //Colors will look washed out when using linear-space lighting
	};
	enum class ColorBufferBits
	{
		COLOR_RGBA8,   //8 bits per channel (default)
		COLOR_RGB10_A2 //10 bits color, 2 bits alpha (better color precision, sacrifices alpha quality)
	};
	enum class DepthBufferBits
	{
		DEPTH_16, //16-bit integer depth (saves VRAM, bad precision over large distances)
		DEPTH_24  //24-bit integer point depth (default)
	};
	enum class StencilBufferBits
	{
		STENCIL_NONE, //Disables stencil completely (default)
		STENCIL_8     //8-bit stencil
	};
	enum class AlphaChannel
	{
		ALPHA_NONE, //Disables alpha channel completely, cannot have transparent meshes or textures
		ALPHA_8     //8-bit alpha channel (default)
	};

	class OpenGL_Context;

	class LIB_API OpenGL_Global
	{
	public:
		//Toggle verbose logging. If true, then usually frequently updated runtime values like
		//GL notifications will dump their logs into the console.
		static void SetVerboseLoggingState(bool newState);
		static bool IsVerboseLoggingEnabled();

		//Global one-time OpenGL 3.3 init, needs to be called before per-window OpenGL init.
		//Pass os and core gl function initializer functions from KalaGraphics if you are using that.
		static void Initialize();
		static bool IsInitialized();
		
		static void SetOpenGLLibrary();
		static uintptr_t GetOpenGLLibrary();

		//Check if this extension is supported by the current context (OpenGL 3.3)
		static bool IsExtensionSupported(const string& name);
		
		//Make the GL context correct for the current window,
		//accepts the context class which stores the context
		static void MakeContextCurrent(
			OpenGL_Context* context,
			uintptr_t handle);

		//Make the GL context correct for the current window,
		//accepts a uintptr_t as the direct context reference
		static void MakeContextCurrent(
			uintptr_t context,
			uintptr_t handle);
			
		//Confirms that the GL context is the same as the stored context for this window,
		//accepts the context class which stores the context
		static bool IsContextValid(OpenGL_Context* context);

		//Confirms that the GL context is the same as the stored context for this window,
		//accepts a uintptr_t as the direct context reference
		static bool IsContextValid(uintptr_t context);

		//Place after any gl call to check if an issue or error has occurred within that point.
		//Loops through all errors so that all errors at that point are printed, not just the first one.
		static string GetError();
	};

	class LIB_API OpenGL_Context
	{
	public:
		static KalaWindowRegistry<OpenGL_Context>& GetRegistry();

		//Initialize a per-window OpenGL context.
		//parentContext determines the ID of the parent context which
		//this context will get shaders, textures and buffers from
		static OpenGL_Context* Initialize(
			u32 windowID,
			u32 parentContext = 0,
			MultiSampling msaa = MultiSampling::MSAA_4X,
			SRGBMode srgb = SRGBMode::SRGB_ENABLED,
			ColorBufferBits cBits = ColorBufferBits::COLOR_RGBA8,
			DepthBufferBits dBits = DepthBufferBits::DEPTH_24,
			StencilBufferBits sBits = StencilBufferBits::STENCIL_NONE,
			AlphaChannel aChannel = AlphaChannel::ALPHA_8);

		bool IsInitialized() const;

		u32 GetID() const;
		u32 GetWindowID() const;
		
		//Called at the end of each frame, requires handle (HDC) from your window
		void SwapOpenGLBuffers(uintptr_t handle);

		const string& GetContextData();
		
		uintptr_t GetContext() const;
		uintptr_t GetParentContext() const;
		
		void SetVSyncState(VSyncState newValue);
		VSyncState GetVSyncState() const;

		//Do not destroy manually, erase from registry instead
		~OpenGL_Context();
	private:
		bool isInitialized{};

		u32 ID{};
		u32 windowID{};

		uintptr_t hglrc{};
		uintptr_t parentHglrc{};

		string contextData{};

		VSyncState vsyncState{};
		MultiSampling msaa{};
		SRGBMode srgb{};
		ColorBufferBits cBits{};
		DepthBufferBits dBits{};
		StencilBufferBits sBits{};
		AlphaChannel aChannel{};
	};
}