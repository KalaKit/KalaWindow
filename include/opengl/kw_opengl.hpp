//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <functional>

#include "KalaHeaders/core_utils.hpp"
#include "KalaHeaders/math_utils.hpp"

#include "core/kw_registry.hpp"

namespace KalaWindow::OpenGL
{
	using std::string;
	using std::function;

	using KalaWindow::Core::KalaWindowRegistry;
	
	enum VSyncState
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
		//Global one-time OpenGL 3.3 init, needs to be called before per-window OpenGL init.
		//Pass os and core gl function initializer functions from KalaGraphics if you are using that.
		static void Initialize();
		static inline bool IsInitialized() { return isInitialized; }
		
		//Toggle verbose logging. If true, then usually frequently updated runtime values like
		//GL notifications will dump their logs into the console.
		static inline void SetVerboseLoggingState(bool newState) { isVerboseLoggingEnabled = newState; }
		static inline bool IsVerboseLoggingEnabled() { return isVerboseLoggingEnabled; }

		static void SetOpenGLLibrary();
		static inline uintptr_t GetOpenGLLibrary()
		{
			if (openGL32Lib == NULL) SetOpenGLLibrary();

			return openGL32Lib;
		}

		//Check if this extension is supported by the current context (OpenGL 3.3)
		static bool IsExtensionSupported(const string& name);
		
		//Make the GL context correct for the current window
		static void MakeContextCurrent(
			OpenGL_Context* context,
			uintptr_t handle);
			
		//Confirms that the GL context is the same as the stored context for this window
		static bool IsContextValid(OpenGL_Context* context);

		//Place after any gl call to check if an issue or error has occurred within that point.
		//Loops through all errors so that all errors at that point are printed, not just the first one.
		static string GetError();
	private:
		static inline bool isInitialized{};
		static inline bool isVerboseLoggingEnabled{};

		static inline uintptr_t openGL32Lib{};
	};

	class LIB_API OpenGL_Context
	{
	public:
		static inline KalaWindowRegistry<OpenGL_Context> registry{};

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

		inline bool IsInitialized() const { return isInitialized; }

		inline u32 GetID() const { return ID; }
		inline u32 GetWindowID() const { return windowID; }
		
		//Called at the end of each frame, requires handle (HDC) from your window
		void SwapOpenGLBuffers(uintptr_t handle);

		inline const string& GetContextData() { return contextData; }
		
		inline uintptr_t GetContext() const { return hglrc; }
		inline uintptr_t GetParentContext() const { return parentHglrc; }
		
		void SetVSyncState(VSyncState newValue);
		inline VSyncState GetVSyncState() const { return vsyncState; }

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